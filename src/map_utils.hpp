#ifndef PALEOSERVER_MAP_UTILS_HPP
#define PALEOSERVER_MAP_UTILS_HPP

// boost
//#include <boost/algorithm/string.hpp>
//#include <boost/filesystem/operations.hpp>

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

#include <boost/optional.hpp>

#if MAPNIK_VERSION >= 800
   #include <mapnik/box2d.hpp>
   #define Envelope box2d
#else
   #include <mapnik/envelope.hpp>
   #define box2d Envelope
#endif

using namespace mapnik;

namespace http {
namespace map_utils {


inline boost::optional<box2d<double> > parse_bbox_from_string(const std::string& bbox_string)
{
  
    boost::optional<Envelope<double> > result;
    boost::char_separator<char> sep(",");
    boost::tokenizer<boost::char_separator<char> > tok(bbox_string,sep);
    unsigned i = 0;
    bool success = false; 
    double d[4];
    for (boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); 
         beg!=tok.end();++beg)
    {
        try 
        {
            d[i] = boost::lexical_cast<double>(boost::trim_copy(*beg));
        }
        catch (boost::bad_lexical_cast & ex)
        {
            std::clog << *beg << " : " << ex.what() << "\nAre your coordinates each separated by commas?\n";
            break;
        }
        if (i==3) 
        {
            success = true;
            break;
        }
        ++i;
    }
          
    if (success)
    {
      Envelope<double> bbox(d[0],d[1],d[2],d[3]);
      result.reset(bbox);
      return result;
    }
    
    return result;
}
  
}
}

#endif // PALEOSERVER_MAP_UTILS_HPP
