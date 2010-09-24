#include "uri_parser.hpp"
#include "uri_grammer.hpp"
#include "map_utils.hpp"

// clog
#include <iostream>

// atoi
#include <stdio.h>
#include <stdlib.h>

// parsing
// boost
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

#include <boost/optional.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>
#include <map>

namespace qi = boost::spirit::qi;

namespace http {
namespace paleoserver {

typedef boost::unordered_map<std::string, std::string>::const_iterator iterator_type;

wms_query::wms_query(const std::string& query)
  //: bbox_()
  {

    std::string::const_iterator begin = query.begin();
    std::string::const_iterator end = query.end();

    key_value_sequence_empty_value<std::string::const_iterator> p;
    pairs_type m;

    if (!qi::parse(begin, end, p, m))
    {
        std::cout << "Parsing failed\n";
    }
    else
    {
        pairs_type::iterator end = m.end();
        for (pairs_type::iterator it = m.begin(); it != end; ++it)
        {
            //std::cout << (*it).first;
            std::string key = boost::algorithm::to_lower_copy(boost::trim_copy((*it).first));
            if ((*it).second)
            {
                //std::cout << "=" << boost::get<std::string>((*it).second);
                //std::cout << std::endl;
                std::string value = boost::algorithm::to_lower_copy(boost::trim_copy(boost::get<std::string>((*it).second)));
                params_.insert(std::make_pair(key,value));
            }
        }
    }
  }

  bool wms_query::parse_bbox_string(Envelope<double>& box, const std::string& bbox_string)
  {
      return map_utils::parse_bbox_from_string(box, bbox_string);
  }

  void wms_query::parse_layer_string(std::set<std::string>& layers, const std::string& layer_string)
  {

    boost::char_separator<char> sep(",");
    boost::tokenizer<boost::char_separator<char> > tok(layer_string,sep);
    for (boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); 
         beg!=tok.end();++beg)
    {
        try 
        {
            std::string layer_name = boost::trim_copy(*beg);
            if (!layer_name.empty())
            {
                layers.insert(layer_name);
            }
        }
        catch (boost::bad_lexical_cast & ex)
        {
            std::clog << *beg << " : " << ex.what() << "\n";
            break;
        }
    }

  }
  
  /*static boost::optional<color>& bgcolor()
  {
      
  }*/

  bool wms_query::get_bbox_string(std::string& bbox_string)
  {
      iterator_type itr = params_.find("bbox");
      if (itr != params_.end())
      {
          bbox_string = itr->second;
          return true;
      }
      return false;
  }   

  bool wms_query::get_layer_string(std::string& layer_string)
  {
      iterator_type itr = params_.find("layers");
      if (itr != params_.end())
      {
          layer_string = itr->second;
          return true;
      }
      return false;
  }   

  bool wms_query::get_mime(std::string& mime)
  {
      iterator_type itr = params_.find("format");
      if (itr != params_.end())
      {
          mime = itr->second;
          return true;
      }
      return false;
  }   

  bool wms_query::get_srs(std::string& srs)
  {
      iterator_type itr = params_.find("srs");
      if (itr != params_.end())
      {
          srs = itr->second;
          return true;
      }
      return false;
  }   
  
  bool wms_query::get_width(unsigned& w)
  {
      iterator_type itr = params_.find("width");
      if (itr != params_.end())
      {
          w = atoi(itr->second.c_str());
          return true;
      }
      return false;
  }
  
  bool wms_query::get_height(unsigned& h)
  {
      iterator_type itr = params_.find("height");
      if (itr != params_.end())
      {
          h = atoi(itr->second.c_str());
          return true;
      }
      return false;
  }


} // namespace paleoserver
} // namespace http
