//
// mime_types.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "mime_types.hpp"

namespace http {
namespace paleoserver {
namespace mime_types {

struct mapping
{
  const char* extension;
  const char* mime_type;
} mappings[] =
{
  { "png", "image/png" },
  { "png8", "image/png8" },
  { "jpg", "image/jpeg" },
  { "jpeg", "image/jpeg" },
  { "gif", "image/gif" },
  { "htm", "text/html" },
  { "html", "text/html" },
  { 0, 0 } // Marks end of list.
};

std::string extension_to_type(const std::string& extension)
{
  for (mapping* m = mappings; m->extension; ++m)
  {
    if (m->extension == extension)
    {
      return m->mime_type;
    }
  }

  return "image/png";
}

struct r_mapping
{
  const char* mime_type;
  const char* extension;
} r_mappings[] =
{
  { "image/png", "png" },
  { "image/png8", "png8" },
  { "image/jpeg", "jpeg" },
  { "image/jpg", "jpeg" },
  { 0, 0 } // Marks end of list.
};

std::string type_to_extension(std::string mime_type)
{
  for (r_mapping* m = r_mappings; m->mime_type; ++m)
  {
    if (m->mime_type == mime_type)
    {
      return m->extension;
    }
  }

  return "png";
}

} // namespace mime_types
} // namespace paleoserver
} // namespace http
