paleoserver
~~~~~~~~~~~

old school wms for mapnik.


Credits
-------

Base server adapted from boost::asio 'server2', io_service per thread design.
URL key-value parsing adapted from boost:spirit2 examples


Contact
-------
Dane Springmeyer <dane - at - dbsgeo.com> with any questions.


Requires
--------

* Mapnik 0.7.2 (http://svn.mapnik.org/branches/0.7.2-dev/)  or Mapnik2 (http://svn.mapnik.org/trunk)
* Boost 1.42 (for spirit2)
* Scons (for build)


Building
--------
* Install scons ($sudo easy_install scons)

* Set your mapnik version in SConstruct (mapnik or mapnik2)

* Note: you will also likely need to manually tweak the compile paths and flags in SConstruct
    * Trick is to get libicuuc, libfreetype, libboost*, and libmapnik in the compile lines
    
* Then from this directory do:

    $ scons

* Next edit the config.json to customize to your mapnik setup.

* Then run:

   (for mapnik2):

    $ ./paleoserver2 config.json

   (for mapnik 0.7.2):

    $ ./paleoserver config.json
 
* Or pass all required options on the command line...

   (for mapnik2):

  ./paleoserver2 0.0.0.0 8000 2 . /usr/local/lib/mapnik2 demo/world_latlon.xml

   (for mapnik 0.7.2):
   
  ./paleoserver 0.0.0.0 8000 2 . /usr/local/lib/mapnik demo/world_latlon.xml


Supports
--------

Very little.

Can mostly handle WMS 1.1.1 requests from Openlayers like:

http://127.0.0.1:8000/?LAYERS=countries&STYLES=&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A900913&BBOX=713309.35838188,4863472.8980582,4863472.8980583,9013636.4377346&WIDTH=256&HEIGHT=256

but only knows about bbox and dimensions.


Caveats
-------

* Does not handle values with spaces, even if encoded
* Likely to crash, requires kill -9 to stop all threads
* If it hangs, make sure you are loading ABI-compatible datasources
* Only supports BBOX, WIDTH, HEIGHT - all other WMS params ignored


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
* Modify mapnik to be able to send a query bbox more refined than map extent

