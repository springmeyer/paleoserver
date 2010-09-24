#ifndef PALEOSERVER_URI_PARSER_HPP
#define PALEOSERVER_URI_PARSER_HPP

#include <boost/noncopyable.hpp>

#include "uri_grammer.hpp"

// new
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

#include <set>

#include <mapnik/version.hpp>

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
#endif

// mapnik
#include <mapnik/color.hpp>

using namespace mapnik;

namespace http {
namespace paleoserver {


/// A pool of io_service objects.
class wms_query
  : private boost::noncopyable
{
public:
  explicit wms_query(const std::string& request_path);
  
  bool get_bbox_string(std::string& bbox_string);
  bool get_layer_string(std::string& layer_string);
  bool get_mime(std::string& mime);
  bool get_srs(std::string& srs);
  //static boost::optional<color>& bgcolor();
  bool parse_bbox_string(Envelope<double>& box, const std::string& bbox_string);
  void parse_layer_string(std::set<std::string>& layers, const std::string& layer_string);
  bool get_width(unsigned& w);
  bool get_height(unsigned& h);
  
protected:
  Envelope<double> bbox_;
  boost::unordered_map<std::string,std::string> params_;
  
};

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_URI_PARSER_HPP
