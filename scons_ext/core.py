import os
import sys
from SCons.SConf import *

from SCons.Script import *

# build with debug output
DEBUG = False

# Create a map object per thread (required, do not change)
COPY_MAP_PER_THREAD = True

def main():
    
    env = Environment(ENV=os.environ)
    
    env['PREFIX'] = '/usr/local'
    
    command_line_args = sys.argv[1:]
    
    HELP_REQUESTED = False
    
    if ('-h' in command_line_args) or ('--help' in command_line_args):
        HELP_REQUESTED = True
    
    env['CXX'] = os.environ.get('CXX') or 'g++'
    
    env.ParseConfig('mapnik-config --ldflags --libs --dep-libs --cflags')
        
    cppflags = ''
        
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
