#include <mapnik/font_engine_freetype.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace mapnik;

namespace http {
namespace paleoserver {


bool is_font_file(std::string const& file_name)
{
    /** only accept files that will be matched by freetype2's `figurefiletype()` */
    std::string const& fn = boost::algorithm::to_lower_copy(file_name);
    return boost::algorithm::ends_with(fn,std::string(".ttf")) ||
        boost::algorithm::ends_with(fn,std::string(".otf")) ||
        boost::algorithm::ends_with(fn,std::string(".ttc")) ||
        boost::algorithm::ends_with(fn,std::string(".pfa")) ||
        boost::algorithm::ends_with(fn,std::string(".pfb")) ||
        boost::algorithm::ends_with(fn,std::string(".ttc")) ||
        /** Plus OSX custom ext */
        boost::algorithm::ends_with(fn,std::string(".dfont"));
}

bool register_fonts(std::string const& dir, bool recurse)
{
    boost::filesystem::path path(dir);
    if (!boost::filesystem::exists(path) || !boost::filesystem::is_directory(path)) return false;
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(dir); itr != end_itr; ++itr)
    {
        if (boost::filesystem::is_directory(*itr) && recurse)
        {
            if (!register_fonts(itr->string(), true)) return false;
        }
        else 
        {
            mapnik::freetype_engine::register_font(itr->string());
        }
    }
    return true;
}

}
}