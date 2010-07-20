import os
import sys

env = Environment(ENV=os.environ)
env['PREFIX'] = '/usr/local'

command_line_args = sys.argv[1:]

HELP_REQUESTED = False

if ('-h' in command_line_args) or ('--help' in command_line_args):
    HELP_REQUESTED = True

#env.ParseConfig('mapnik-config --libs --cppflags')

mapnik = 'mapnik2'
#mapnik = 'mapnik'

env['LIBS'] = [mapnik,'icuuc','boost_filesystem','boost_regex','boost_system','boost_thread']
env['CPPPATH'] = ['/usr/local/include','/usr/local/Cellar/icu4c/4.3.1/include/','/Library/Frameworks/FreeType.framework/unix/include']
env['LIBPATH'] = ['/usr/local/lib','/usr/local/Cellar/icu4c/4.3.1/lib/']

cppflags = '-O3 -ansi -Wall'

if 'mapnik2' in env['LIBS']:
    cppflags += ' -DUSING_MAPNIK2'

# instead of sharing a single map
# for every request instead load
# up a map per thread/io_service
cppflags += ' -DMAP_PER_IO'

# to be used when single map is shared
# but we're building again mapnik lib
# that supports a mapnik::request object
#cppflags += ' -DMAP_REQUEST'

# turn on debug output
#cppflags += ' -DPALEO_DEBUG'

env['CPPFLAGS'] = cppflags


#### Builds ####
if not HELP_REQUESTED:
    # export env so it is available in Sconscript files
    Export('env')
           
    # Build the core library
    SConscript('src/SConscript')