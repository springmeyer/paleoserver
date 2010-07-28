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

  boost::optional<Envelope<double> > wms_query::parse_bbox_string(const std::string& bbox_string)
  {
      return map_utils::parse_bbox_from_string(bbox_string);
  }
  
  /*static boost::optional<color>& bgcolor()
  {
      
  }*/

  boost::optional<std::string> wms_query::get_bbox_string()
  {

      boost::optional<std::string> result;
      iterator_type itr = params_.find("bbox");
      if (itr != params_.end())
      {
          result.reset(itr->second);
          return result;
      }
      return result;
  }   
  
  boost::optional<unsigned> wms_query::width()
  {
      boost::optional<unsigned> result;
      //typedef std::map<std::string, std::string>::const_iterator iterator_type;
      iterator_type itr = params_.find("width");
      if (itr != params_.end())
      {
          unsigned w = atoi(itr->second.c_str());
          result.reset(w);
          return result;
      }
      return result;  
  }
  
  boost::optional<unsigned> wms_query::height()
  {
      boost::optional<unsigned> result;
      //typedef std::map<std::string, std::string>::const_iterator iterator_type;
      iterator_type itr = params_.find("height");
      if (itr != params_.end())
      {
          unsigned h = atoi(itr->second.c_str());
          result.reset(h);
          return result;
      }
      return result;  
  }


} // namespace paleoserver
} // namespace http
