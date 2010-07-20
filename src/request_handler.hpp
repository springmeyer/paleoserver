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

// mapnik
#include <mapnik/map.hpp>

namespace http {
namespace paleoserver {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& doc_root);

#if MAP_PER_IO  
  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep, mapnik::Map map_);

#else
  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

  void set_map(mapnik::Map mapnik_map);
#endif

private:
  /// The directory containing the files to be served.
  std::string doc_root_;

#if MAP_PER_IO
#else
  mapnik::Map map_;
#endif

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_REQUEST_HANDLER_HPP
