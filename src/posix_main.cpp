//
// posix_main.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


// stl
#include <iostream>
#include <string>

// boost
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#include <libxml/parser.h>

// paleoserver
#include "server.hpp"
#include "map_utils.hpp"

// mapnik
#include <mapnik/datasource_cache.hpp>

#if MAPNIK_VERSION >= 800
   #include <mapnik/box2d.hpp>
   #define Envelope box2d
#else
   #include <mapnik/envelope.hpp>
   #define box2d Envelope
#endif

#define PALEO_VERSION 100

#if !defined(_WIN32)

#include <pthread.h>
#include <signal.h>

#include <mapnik/font_engine_freetype.hpp>

using namespace mapnik;

#if MAPNIK_VERSION <= 701
#include "compatibility.hpp"
using http::paleoserver::register_fonts;
#endif

using boost::optional;
using boost::property_tree::ptree;

std::string format_version(int version)
{
    std::ostringstream s;
    s << version/100000 << "." << version/100 % 1000  << "." << version % 100 << "\n";
    return s.str();
}

int main(int argc, char* argv[])
{
  try
  {
    bool help_requested = false;
    bool setup_with_json_config = (argc == 2);
    bool setup_with_command_options = (argc > 2 && argc == 7);
    bool config_errors = false;

    // core options, exposed on command line and json config file
    optional<std::string> address;
    optional<std::string> port;
    optional<std::size_t> threads;
    optional<std::string> doc_root;
    optional<std::string> stylesheet;
    
    // command line only option of root path to mapnik dir
    // containing fonts/input dirs
    optional<std::string> mapnik_dir;
    
    // options only exposed in json config
    optional<std::string> plugins;
    optional<std::string> fonts;
    
    // custom max_extent_string and max_extent
    optional<std::string> max_extent_string;
    box2d<double> max_extent;

    // font list we register
    std::vector<std::string> face_names;
    
    for (int i = 0; i < argc; ++i)
    {
        if (boost::algorithm::iequals(argv[i],"--help") 
            || boost::algorithm::iequals(argv[i],"-h"))
            help_requested = true;

        else if (boost::algorithm::iequals(argv[i],"--version") 
            || boost::algorithm::iequals(argv[i],"-v"))
        {
            std::cerr << "Paleoserver version: " << format_version(PALEO_VERSION);
            std::cerr << "Mapnik version: " << format_version(MAPNIK_VERSION);
            std::cerr << "Boost version: " << format_version(BOOST_VERSION);
            return 1;
        }
    }
    
    if ((!setup_with_json_config) && (!setup_with_command_options || help_requested) )
    {
      std::cerr << "Usage: " << argv[0] << " <config.json>\n";
      std::cerr << "  or\n";
      std::cerr << "Usage: " << argv[0] << " <address> <port> <threads> <doc_root> <mapnik_dir> <stylesheet>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 8000 2 . /usr/local/lib/mapnik stylesheet.xml\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 8000 2 . /usr/local/lib/mapnik stylesheet.xml\n";
      return 1;
    }
    
    if (setup_with_command_options)
    {

        address.reset(argv[1]);
        port.reset(argv[2]);
        threads.reset(boost::lexical_cast<std::size_t>(argv[3]));
        doc_root.reset(argv[4]);
        stylesheet.reset(argv[6]);

        mapnik_dir.reset(argv[5]);
        if (mapnik_dir)
        {

            #if MAPNIK_VERSION >= 702
            face_names = mapnik::freetype_engine::face_names();
            freetype_engine::register_fonts(*mapnik_dir + "/fonts",false);
            if (!mapnik::freetype_engine::face_names().size() > face_names.size())
            {
                std::cerr << "### Warning: Did not sucessfully register any new fonts!\n";
            }
            #else
            register_fonts(*mapnik_dir + "/fonts",false);            
            #endif
            datasource_cache::instance()->register_datasources(*mapnik_dir + "/input");
        }
      
    }
    
    else if (setup_with_json_config)
    {

        ptree pt;
        std::string json_config = argv[1];

        try
        {
            read_json(json_config, pt);
        }
        catch (std::exception& e)
        {
          std::cerr << "Error attempting to read json config: \"" << e.what() << "\"\n";
          return 1;
        }

        address = pt.get_optional<std::string>("address");
        if (!address)
        {
          std::cerr << "Config error: please provide a server 'address' (e.g. 0.0.0.0) in "
              << json_config << std::endl;
          config_errors = true;
        }
    
        port = pt.get_optional<std::string>("port");
        if (!port)
        {
          std::cerr << "Config error: please provide a server 'port' to mount on in "
              << json_config << std::endl;
          config_errors = true;
        }
    
        threads = pt.get_optional<std::size_t>("threads");
        if (!threads)
        {
          std::cerr << "Config error: please provide a number of 'threads' to support in "
              << json_config << std::endl;
          config_errors = true;
        }

        doc_root = pt.get_optional<std::string>("doc_root");
        if (!doc_root)
        {
          std::cerr << "Config error: please provide a document root ('doc_root') in "
              << json_config << std::endl;
          config_errors = true;
        }

        /*
        // experimental nested layers support from json config
        ptree const & styles = pt.get_child("layers");
        ptree::const_iterator itr = styles.begin();
        ptree::const_iterator end = styles.end();
    
        for (; itr != end; ++itr)
        {
            ptree::value_type const& v = *itr;
            std::clog << "found style: " << v.first << std::endl;
            std::clog << "slice: " << styles.get(v.first,"nope") << std::endl;
            
            ptree::const_iterator paramIter = v.second.begin();
            ptree::const_iterator endParam = v.second.end();
    
            for (; paramIter != endParam; ++paramIter)
            {
                //ptree const& param = paramIter->second;
    
                if (paramIter->first == "stylesheet")
                {
                    std::clog << "tricky: " << v.second.get("stylesheet","none") << std::endl;
                    //std::string name = get_attr<string>(param, "name");
                }
                else
                {
                    std::clog << "we should throw, this is invalid: " << paramIter->first << std::endl;
                }
            }
    
        }
        */
        
        // todo - support more than one stylesheet with above method
        stylesheet = pt.get_optional<std::string>("stylesheet");
        if (!stylesheet)
        {
          std::cerr << "Config error: please provide a the path to a 'stylesheet' to load in "
              << json_config << std::endl;
          config_errors = true;
        }
    
        // font config
        fonts = pt.get_optional<std::string>("fonts");
        
        if (!fonts)
        {
          std::cerr << "Config error: please provide a mapnik 'fonts' directory in "
              << json_config << std::endl;
          config_errors = true;
        }
    
        // register font faces
        #if MAPNIK_VERSION >= 702
        face_names = mapnik::freetype_engine::face_names();
        freetype_engine::register_fonts(*fonts,true);
        if (!mapnik::freetype_engine::face_names().size() > face_names.size())
        {
            std::cerr << "### Warning: Did not sucessfully register any new fonts!\n";
        }
        #else
        register_fonts(*fonts,true);
        #endif
    
        // plugin config
        plugins = pt.get_optional<std::string>("plugins");
        if (!plugins)
        {
          std::cerr << "Config error: please provide a mapnik 'plugins' directory in "
              << json_config << std::endl;
          config_errors = true;
        }
    
        // register datasource plugins
        datasource_cache::instance()->register_datasources(*plugins);
        
        // handle optional max_extent parameter
        max_extent_string = pt.get_optional<std::string>("max_extent");
        if (max_extent_string)
        {
            if (!http::map_utils::parse_bbox_from_string(max_extent,*max_extent_string))
                config_errors = true;
        }

    }
    
    if (config_errors)
    {
        return 1;
    }
    else
    {
        std::clog << "ready: listening on " << *address << ":" << *port << std::endl;
    }
    
    // Block all signals for background thread.
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigset_t old_mask;
    pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

    // create server
    http::paleoserver::server s(*address, *port, *doc_root, *threads, *stylesheet, max_extent);
    boost::thread t(boost::bind(&http::paleoserver::server::run, &s));

    // Restore previous signals.
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);

    // Wait for signal indicating time to shut down.
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGINT);
    sigaddset(&wait_mask, SIGQUIT);
    sigaddset(&wait_mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
    int sig = 0;
    sigwait(&wait_mask, &sig);

    // Stop the server.
    s.stop();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
  }

  // only call this once, on exit
  // to make sure valgrind output is clean
  // http://lists.fedoraproject.org/pipermail/devel/2010-January/129117.html
  xmlCleanupParser();
  return 0;
}

#endif // !defined(_WIN32)
