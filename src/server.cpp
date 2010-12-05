//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
//#include "map_utils.hpp"
#include <boost/bind.hpp>


#include <boost/shared_ptr.hpp>

// mapnik
#ifdef MAP_PER_IO
#else
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#endif

#if MAPNIK_VERSION >= 800
   #include <mapnik/box2d.hpp>
   #define Envelope box2d
#else
   #include <mapnik/envelope.hpp>
   #define box2d Envelope
#endif

namespace http {
namespace paleoserver {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root, std::size_t io_service_pool_size,
    const std::string& stylesheet,
    boost::optional<mapnik::box2d<double> > max_extent
    )
#ifdef MAP_PER_IO
  : io_service_pool_(io_service_pool_size,stylesheet),
    acceptor_(io_service_pool_.get_io_service()),
    new_connection_(new connection(
          io_service_pool_.get_io_service(), request_handler_, io_service_pool_.get_map_service())),
#else
  : io_service_pool_(io_service_pool_size),
    acceptor_(io_service_pool_.get_io_service()),
    new_connection_(new connection(
          io_service_pool_.get_io_service(), request_handler_)),
#endif
    request_handler_(doc_root),
    stylesheet_(stylesheet)
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(acceptor_.io_service());
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
  
  if (max_extent) request_handler_.set_max_extent(*max_extent);
  
#ifdef MAP_PER_IO
  // maps will be loaded per thread later
#else
  boost::shared_ptr<mapnik::Map> map_(new mapnik::Map(256,256));
  mapnik::load_map(*map_,stylesheet_);
  request_handler_.set_map(map_);
#endif  
}

void server::run()
{
  io_service_pool_.run();
}

void server::stop()
{
  io_service_pool_.stop();
}

void server::handle_accept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_connection_->start();
#ifdef MAP_PER_IO
    new_connection_.reset(new connection(
          io_service_pool_.get_io_service(), request_handler_, io_service_pool_.get_map_service()));
#else
    new_connection_.reset(new connection(
          io_service_pool_.get_io_service(), request_handler_));
#endif
    acceptor_.async_accept(new_connection_->socket(),
        boost::bind(&server::handle_accept, this,
          boost::asio::placeholders::error));
  }
}

} // namespace paleoserver
} // namespace http
