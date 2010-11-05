import os
import sys
from SCons.SConf import *

from SCons.Script import *

def main():
    ## SETTINGS ##
    DEBUG = False
    mapnik = 'mapnik2'
    #mapnik = 'mapnik'
    COPY_MAP_PER_THREAD = True
    FRAMEWORK = False
    
    env = Environment(ENV=os.environ)
    
    env['PREFIX'] = '/usr/local'
    
    command_line_args = sys.argv[1:]
    
    HELP_REQUESTED = False
    
    if ('-h' in command_line_args) or ('--help' in command_line_args):
        HELP_REQUESTED = True
    
    #env['CXX'] = 'clang++'
    #env['CXX'] = 'ccache g++'
    env['CXX'] = os.environ.get('CXX') or 'g++'
    
    env['LIBS'] = [mapnik,'icuuc','boost_filesystem','boost_regex','boost_system','boost_thread']
    env['CPPPATH'] = ['/usr/local/include']
    env['LIBPATH'] = ['/usr/local/lib']
    
    if FRAMEWORK:
      fm_path = '/Library/Frameworks/Mapnik.framework/Versions/Current/unix/'
      env['CPPPATH'].insert(0,'%sinclude' % fm_path)
      env['LIBPATH'].insert(0,'%slib' % fm_path)  
    
    # add freetype paths
    env.ParseConfig('freetype-config --libs --cflags')
    env.ParseConfig('xml2-config --cflags --libs')
    
    if mapnik == 'mapnik2':
        pass#env.ParseConfig('mapnik-config --libs --cppflags')
    else:
        if not FRAMEWORK:
            env['CPPPATH'].insert(0,'/Users/dane/projects/mapnik-dev/0.7.2-dev/include/')
    
    cppflags = '-O3 -ansi -Wall'
    
    if 'mapnik2' in env['LIBS']:
        cppflags += ' -DUSING_MAPNIK2'
    
    if COPY_MAP_PER_THREAD:
        # instead of sharing a single map
        # for every request instead load
        # up a map per thread/io_service
        cppflags += ' -DMAP_PER_IO'
    else:
        # to be used when single map is shared
        # but we're building again mapnik lib
        # that supports a mapnik::request object
        # experimental branch at:
        # svn.mapnik.org/branches/map_request
        cppflags += ' -DMAP_REQUEST'
    
    # turn on debug output
    if DEBUG:
        cppflags += ' -g -DDEBUG -DPALEO_DEBUG'
    else:
        cppflags += ' -DNDEBUG'
        
    env['CPPFLAGS'] = cppflags
    
    
    #### Builds ####
    if not HELP_REQUESTED:
        # export env so it is available in Sconscript files
        Export('env')
               
        # Build the core library
        SConscript('src/SConscript')

if __name__ == '__main__':
    main()
