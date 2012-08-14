# paleoserver

old school wms for mapnik


## Overview

Purely a test application at this point to scope various options around concurrent rendering in Mapnik.

Originally written for the FOSS4G 2010 benchmark.

You may also be interested in mod_mapnik_wms which runs within Apache:

  http://wiki.openstreetmap.org/wiki/Mod_mapnik_wms

The main difference is that paleoserver leverages boost::asio for multithreading rather than Apache and can be run from the command line or as a daemon without depending on another webserver for deployment. The draw back is that this lacks the flexibility for custom setup (urls, auth, etc) that comes easily with Apache.

See also another experimental server written in javascript which is slightly faster: https://github.com/mapbox/landspeed.js


## Contact

Dane Springmeyer <dane - at - dbsgeo.com> with any questions.


# Requires

* Mapnik >= 2.0.x
* Boost >= 1.42
* Scons (for build)


## Credits

Base server adapted from boost::asio 'server2', io_service per thread design.

URL key-value parsing adapted from boost:spirit2 examples.


# Building

Install scons:

    sudo easy_install scons


You will also likely need to manually tweak the compile paths and flags in SConstruct. The trick is to get paths to libicuuc, libfreetype, libboost*, and libmapnik in the compile lines
    
Then from this directory do:

    scons

Next edit the config.json to customize to your mapnik setup.

Then run:

    ./paleoserver config.json
 
Or just pass all required options on the command line like:

  ./paleoserver 0.0.0.0 8000 2 . /usr/local/lib/mapnik demo/world_latlon.xml


# Supports

Can mostly handle WMS 1.1.1 requests from Openlayers like:

`http://127.0.0.1:8000/?LAYERS=__all__&STYLES=&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=713309.35838188,4863472.8980582,4863472.8980583,9013636.4377346&WIDTH=256&HEIGHT=256&transparent=false`

Does not attempt to parse SERVICE, VERSION, REQUEST, EXCEPTIONS, OR BGCOLOR


# Caveats

Does not handle values with spaces, even if encoded.

If it hangs, make sure you are loading ABI-compatible datasources.

Only supports GetMap and limited GetMap params - many are currently ignored.

