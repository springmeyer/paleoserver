#ifndef PALEOSERVER_URI_PARSER_HPP
#define PALEOSERVER_URI_PARSER_HPP

#include <boost/noncopyable.hpp>

#include "uri_grammer.hpp"

// new
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

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
  
  boost::optional<std::string>  get_bbox_string();
  //static boost::optional<color>& bgcolor();
  boost::optional<Envelope<double> > parse_bbox_string(const std::string& bbox_string);
  boost::optional<unsigned> width();
  boost::optional<unsigned> height();
  
protected:
  Envelope<double> bbox_;
  boost::unordered_map<std::string,std::string> params_;
  
};

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_URI_PARSER_HPP
