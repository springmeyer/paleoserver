paleoserver
~~~~~~~~~~~

old school wms for mapnik.


Goals
-----

Purely a test application at this point to scope various options around concurrent rendering in Mapnik.

Originally written for the FOSS4G 2010 benchmark.

You may also be interested in mod_mapnik_wms which runs within Apache:

  http://wiki.openstreetmap.org/wiki/Mod_mapnik_wms

The main difference is that paleoserver leverages boost::asio for multithreading rather than Apache and can be run from the command line or as a daemon without depending on another webserver for deployment. This has the benefit of avoiding Apaches concurrence model and overhead but has the draw back of lacking all the nice flexibility for custom setup (urls, auth, etc) that comes easily with Apache.


Contact
-------
Dane Springmeyer <dane - at - dbsgeo.com> with any questions.


Requires
--------

* Mapnik 0.7.2 (http://svn.mapnik.org/branches/0.7.2-dev/)  or Mapnik2 (http://svn.mapnik.org/trunk)
* Boost 1.42 (for spirit2)
* Scons (for build)


Credits
-------

Base server adapted from boost::asio 'server2', io_service per thread design.
URL key-value parsing adapted from boost:spirit2 examples


Building
--------

* Install scons ($sudo easy_install scons)

* Set your mapnik version in SConstruct (mapnik or mapnik2)

* Note: you will also likely need to manually tweak the compile paths and flags in SConstruct
    * Trick is to get paths to libicuuc, libfreetype, libboost*, and libmapnik in the compile lines
    
* Then from this directory do:

    $ scons

* Next edit the config.json to customize to your mapnik setup.

* Then run:

   (for mapnik2):

    $ ./paleoserver2 config.json

   (for mapnik 0.7.2):

    $ ./paleoserver config.json
 
* Or just pass all required options on the command line...

   (for mapnik2):

  ./paleoserver2 0.0.0.0 8000 2 . /usr/local/lib/mapnik2 demo/world_latlon.xml

   (for mapnik 0.7.2):
   
  ./paleoserver 0.0.0.0 8000 2 . /usr/local/lib/mapnik demo/world_latlon.xml


Supports
--------

Can mostly handle WMS 1.1.1 requests from Openlayers like:

http://127.0.0.1:8000/?LAYERS=countries&STYLES=&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=713309.35838188,4863472.8980582,4863472.8980583,9013636.4377346&WIDTH=256&HEIGHT=256

but does not parse SERVICE, VERSION, REQUEST, EXCEPTIONS, OR BGCOLOR


Caveats
-------

* Does not handle values with spaces, even if encoded
* If it hangs, make sure you are loading ABI-compatible datasources
* Only supports GetMap and limited GetMap params - many are currently ignored


ToDo
----
* Automated builds using different options (mapnik vs. mapnik2)
* Support params?
  - service
  - request
  - bgcolor
  - exceptions
  - version
* Benchmark request/sec vs. # of threads
* Benchmark single shared map vs map per thread (io_service)
* Benchmark no-proj branch (no mutexes)
* Modify mapnik to be able to send a query bbox more refined than map extent

