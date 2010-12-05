//
// io_service_pool.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

#ifdef MAP_PER_IO
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#else
#endif

namespace http {
namespace paleoserver {

#ifdef MAP_PER_IO
io_service_pool::io_service_pool(std::size_t pool_size, std::string stylesheet)
  : next_io_service_(0),
    next_map_service_(0),
    stylesheet_(stylesheet)
{
  if (pool_size == 0)
    throw std::runtime_error("io_service_pool size is 0");

  for (std::size_t i = 0; i < pool_size; ++i)
  {
#ifdef PALEO_DEBUG
    std::clog << "setting up io service #" << i <<" \n";
#endif
    map_ptr map_service(new mapnik::Map(256,256));
#ifdef PALEO_DEBUG
  std::clog << "loading map: " << stylesheet_ << "\n";
#endif
    mapnik::load_map(*map_service,stylesheet_);
#ifdef PALEO_DEBUG
  std::clog << "map loaded..\n";
#endif
    map_services_.push_back(map_service);
  }
  
#else
io_service_pool::io_service_pool(std::size_t pool_size)
  : next_io_service_(0)
{
  if (pool_size == 0)
    throw std::runtime_error("io_service_pool size is 0");
#endif

  // Give all the io_services work to do so that their run() functions will not
  // exit until they are explicitly stopped.
  for (std::size_t i = 0; i < pool_size; ++i)
  {
#ifdef PALEO_DEBUG
    std::clog << "setting up io service #" << i <<" \n";
#endif
    io_service_ptr io_service(new boost::asio::io_service);
    //io_service_ptr io_service(new boost::asio::io_service(2 /* concurrency hint*/));
    work_ptr work(new boost::asio::io_service::work(*io_service));
    io_services_.push_back(io_service);
    work_.push_back(work);
  }
}

void io_service_pool::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < io_services_.size(); ++i)
  {
#ifdef PALEO_DEBUG
    std::clog << "running io service #" << i <<" in thread \n";
#endif
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, io_services_[i])));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void io_service_pool::stop()
{
  // Explicitly stop all io_services.
  for (std::size_t i = 0; i < io_services_.size(); ++i)
    io_services_[i]->stop();
}

boost::asio::io_service& io_service_pool::get_io_service()
{
  // Use a round-robin scheme to choose the next io_service to use.
#ifdef PALEO_DEBUG
  std::clog << "grabbing io service #" << next_io_service_ << "\n";
#endif

  boost::asio::io_service& io_service = *io_services_[next_io_service_];
  ++next_io_service_;
  if (next_io_service_ == io_services_.size())
    next_io_service_ = 0;
  return io_service;
}

#ifdef MAP_PER_IO
map_ptr io_service_pool::get_map_service()
{
  // Use a round-robin scheme to choose the next io_service to use.
#ifdef PALEO_DEBUG
  std::clog << "grabbing map service #" << next_map_service_ << "\n";
#endif
  map_ptr map_ = map_services_[next_map_service_];
  ++next_map_service_;
  if (next_map_service_ == map_services_.size())
    next_map_service_ = 0;
  return map_;
}
#endif

} // namespace paleoserver
} // namespace http
