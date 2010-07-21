//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// The purpose of this example is to show how to parse arbitrary key/value 
// pairs delimited by some separator into a std::vector. The difference to 
// the example 'key_value_sequence.cpp' is that we preserve the order of the
// elements in the parsed sequence as well as possibly existing duplicates.
// In addition to the example 'key_value_sequence_ordered.cpp' we allow for 
// empty values, i.e. the grammar allows to distinguish between 'key=;' and
// 'key;", where the first stores an empty string as the value, while the 
// second does not initialize the optional holding the value.
//
// For a more elaborate explanation see here: http://spirit.sourceforge.net/home/?p=371

#ifndef PALEOSERVER_GRAMMER_HPP
#define PALEOSERVER_GRAMMER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>
#include <map>

namespace http {
namespace paleoserver {

    namespace qi = boost::spirit::qi;

    typedef std::pair<std::string, boost::optional<std::string> > pair_type;
    typedef std::vector<pair_type> pairs_type;

    template <typename Iterator>
    struct key_value_sequence_empty_value
      : qi::grammar<Iterator, pairs_type()>
    {
        key_value_sequence_empty_value()
          : key_value_sequence_empty_value::base_type(query)
        {
            query =  pair >> *((qi::lit('&')) >> pair);
            pair  =  key >> -('=' >> -value);
            key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
            value = +qi::char_("a-zA-Z_0-9,/.:-");
        }

        qi::rule<Iterator, pairs_type()> query;
        qi::rule<Iterator, pair_type()> pair;
        qi::rule<Iterator, std::string()> key, value;
    };

} // namespace paleoserver
} // namespace http

#endif // PALEOSERVER_GRAMMER_HPP
