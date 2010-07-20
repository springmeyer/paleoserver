paleoserver
***********

old school wms for mapnik.


Requires
--------

* Mapnik 0.7.2 / Mapnik2
* Boost 1.42 (for spirit2)
* Scons (for build)


Building
--------

* Consult the cppflags in SConstruct
* Edit the paths to mapnik fonts/datasources in map_utils.hpp
* Then build locally by typing `scons`
* Then run...

   (for mapnik2):

  ./paleoserver2 0.0.0.0 8000 2 . path/to/map.xml

   (for mapnik 0.7.2):
   
  ./paleoserver 0.0.0.0 8000 2 . path/to/map.xml


Supports
--------

Very little.

Can handles WMS 1.1.1 requests from Openlayers like:

http://127.0.0.1:8000/?LAYERS=countries&STYLES=&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=713309.35838188,4863472.8980582,4863472.8980583,9013636.4377346&WIDTH=256&HEIGHT=256

but only knows about bbox and dimensions.


Caveats
-------

* Likely to crash, requires kill -9 to stop
* If it hangs, make sure you are loading ABI-compatible datasources
* Only supports BBOX, WIDTH, HEIGHT, all other WMS params ignored


ToDo
----
* Automated builds using different options (mapnik vs. mapnik2)
* Support params:
  - srs ?
  - image/png8,image/jpeg ?
  - bgcolor ?
* Benchmark request/sec vs. # of threads
* Benchmark single shared map vs map per thread (io_service)
* Benchmark no-proj branch (no mutexes)

