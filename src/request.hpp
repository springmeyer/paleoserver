//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PALEOSERVER_REQUEST_HPP
#define PALEOSERVER_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
namespace paleoserver {

/// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
};

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_REQUEST_HPP
