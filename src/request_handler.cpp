//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// paleoserver
#include "uri_parser.hpp"
#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

//stl
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// mapnik

#include <mapnik/version.hpp>
#include <mapnik/map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/filter_factory.hpp>
#include <mapnik/color_factory.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>

#if MAPNIK_VERSION >= 800
   #include <mapnik/box2d.hpp>
   #define Image32 image_32
   #define ImageData32 image_data_32
   #define Envelope box2d
#else
   #include <mapnik/envelope.hpp>
   #define zoom_to_box zoomToBox
   #define image_32 Image32
   #define image_data_32 ImageData32
   #define box2d Envelope
   #define layer Layer
   #define set_background setBackground
#endif


// boost
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>


using namespace mapnik;

namespace http {
namespace paleoserver {

#ifdef MAP_PER_IO
request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root) {}
#else
request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root),
    map_() {}
#endif

#ifdef MAP_PER_IO
#else
void request_handler::set_map(mapnik::Map mapnik_map)
{
    map_ = mapnik_map;
}
#endif

void request_handler::set_max_extent(const mapnik::box2d<double>& bbox)
{
    max_extent_ = bbox;
}

boost::optional<mapnik::box2d<double> > request_handler::max_extent() const
{
    return max_extent_;
}

#ifdef MAP_PER_IO
void request_handler::handle_request(const request& req, reply& rep, mapnik::Map map_)
#else
void request_handler::handle_request(const request& req, reply& rep)
#endif
{

#ifdef PALEO_DEBUG
  std::clog << "handling request...\n";
#endif

  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }
  
  // Request path must be absolute and not contain "..".
  /*
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos
      || request_path == "/favicon.ico")
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  if (request_path[request_path.size() - 1] == '/')
  {
    std::string msg("Ready to accept Query");
    rep = reply::reply_html(msg);
    return;      
  }
  */

  // If path ends in slash (i.e. is a directory) then add "index.html".
  //if (request_path[request_path.size() - 1] == '/')
  //{
  //  request_path += "index.html";
  //}

  std::string query;
  size_t breakpoint = request_path.find_first_of("?");
  if (breakpoint != string::npos)
  {
      query = request_path.substr(breakpoint+1, request_path.length());
  }
   
  if (query.empty())
  {
    std::string msg("Ready to accept Query");
    rep = reply::reply_html(msg);
    return;      
  }
  
#ifdef PALEO_DEBUG
  std::clog << "query: " << query << "\n";
#endif

  wms_query wms_query(query);

  std::string bbox_string;
  if (!wms_query.get_bbox_string(bbox_string))
  {
      rep = reply::reply_html("missing bbox");
      return;  
  }

  Envelope<double> bbox;
  if (!wms_query.parse_bbox_string(bbox,bbox_string))
  {
      rep = reply::reply_html("failed to parse bbox");
      //rep = reply::stock_reply(reply::bad_request);
      return;  
  }

  unsigned w;
  unsigned h;

  if (!wms_query.get_width(w) || !wms_query.get_height(h))
  {
      std::ostringstream s_error;
      s_error << "missing width or height. got width value of: " << w
          << " got height value of: " << h << "\n"; 
      rep = reply::reply_html(s_error.str());
      //rep = reply::stock_reply(reply::bad_request);
      return;  
  }


  std::string layer_string;
  if (!wms_query.get_layer_string(layer_string))
  {
      rep = reply::reply_html("missing layers");
      return;  
  }

  std::string srs;
  if (!wms_query.get_srs(srs))
  {
      rep = reply::reply_html("missing srs");
      return;
  }

  map_.set_srs("+init=" + srs);
  
  // check for intersection with max/valid extent
  //boost::optional<mapnik::box2d<double> > bounds = max_extent();
  bool intersects = true;
  /*
  if (bounds){
      if (!bounds->intersects(*bbox)) intersects = false;
      // todo write directly to png/jpeg...
  }*/

  // setup transparent response image
  image_32 im(w,h);
  
  if (intersects)
  {
      // handle layers
      if (boost::algorithm::iequals(layer_string,"__all__"))
      {
          BOOST_FOREACH ( layer & lyr, map_.layers() )
          {
              lyr.setActive(true);
          }
      }
      else
      {
          // convert comma separated layers to vector
          std::set<std::string> layer_names;
          wms_query.parse_layer_string(layer_names, layer_string);
          BOOST_FOREACH ( layer & lyr, map_.layers() )
          {
              bool requested = (layer_names.find(lyr.name()) != layer_names.end());
              lyr.setActive(requested);
          }
      }

      map_.resize(w,h);

      #ifdef MAP_PER_IO
      map_.zoom_to_box(bbox);

      //map_.set_aspect_fix_mode(mapnik::Map::ADJUST_CANVAS_HEIGHT);
      agg_renderer<image_32> ren(map_,im);
      
      #else
      
        #ifdef MAP_REQUEST
          // requires ripped apart mapnik:Map object...
          // http://svn.mapnik.org/branches/map_request/
          mapnik::request r_(w,h);
          r_.set_srs(map_.srs());
          r_.set_buffer_size(128);
          boost::optional<color> const& bg = map_.background();
          if (bg) r_.set_background(*bg);
          
          r_.zoom_to_box(bbox);
          // todo, pass only layers and styles?
          // std::vector<layer> & map_.layers()
          // setActive(true)
          agg_renderer<image_32> ren(map_,im,r_);
        #else
          
          map_.zoom_to_box(bbox);
          //map_.set_buffer_size(128);
          agg_renderer<image_32> ren(map_,im);
        
        #endif
      
      #endif
      
      #ifdef PALEO_DEBUG
      std::clog << "rendering... \n";
      #endif
      
      ren.apply();
  }
  else
  {
      boost::optional<color> const& bg = map_.background();
      if (bg) im.set_background(*bg);
  }

  std::string mime;
  if (!wms_query.get_mime(mime))
  {
      mime = "image/png";
  }
  
  rep.status = reply::ok;
  rep.content = save_to_string(im, mime_types::type_to_extension(mime));
  rep.headers.resize(3);
  rep.headers[0].name = "Content-Type";
  rep.headers[0].value = mime;
  rep.headers[1].name = "Server";
  rep.headers[1].value = "PaleoServer/0.1.0";
  rep.headers[2].name = "Content-Length";
  rep.headers[2].value = boost::lexical_cast<std::string>(rep.content.size());
}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace paleoserver
} // namespace http
