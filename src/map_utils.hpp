#ifndef PALEOSERVER_MAP_UTILS_HPP
#define PALEOSERVER_MAP_UTILS_HPP

// mapnik
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>

using namespace mapnik;

namespace http {
namespace paleoserver {

void init_mapnik()
{
#if USING_MAPNIK2
    std::string mapnik_dir("/usr/local/lib/mapnik2/");
#else
    std::string mapnik_dir("/usr/local/lib/mapnik/");
#endif
    datasource_cache::instance()->register_datasources(mapnik_dir + "input/");
    freetype_engine::register_fonts(mapnik_dir + "fonts/");
}

}
}

#endif // PALEOSERVER_MAP_UTILS_HPP
