//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PALEOSERVER_REQUEST_HANDLER_HPP
#define PALEOSERVER_REQUEST_HANDLER_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// mapnik
#include <mapnik/map.hpp>
#include <mapnik/version.hpp>

#if MAPNIK_VERSION >= 800
   #include <mapnik/box2d.hpp>
   #define Envelope box2d
#else
   #include <mapnik/envelope.hpp>
   #define box2d Envelope
#endif

namespace http {
namespace paleoserver {

struct reply;
struct request;

typedef boost::shared_ptr<mapnik::Map> map_ptr;
//typedef mapnik::Map * map_ptr;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& doc_root);

#ifdef MAP_PER_IO  
  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep, map_ptr map_);

#else
  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

  void set_map(map_ptr mapnik_map);
#endif

  void set_max_extent(const mapnik::box2d<double>& bbox);

  boost::optional<mapnik::box2d<double> > max_extent() const;
private:
  /// The directory containing the files to be served.
  std::string doc_root_;

#ifdef MAP_PER_IO
#else
  map_ptr map_;
#endif

  boost::optional<mapnik::box2d<double> > max_extent_;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_REQUEST_HANDLER_HPP
