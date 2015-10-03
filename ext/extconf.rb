require 'mkmf'

extension_name = 'opendmx'

# LIBDIR      = RbConfig::CONFIG['libdir']
# INCLUDEDIR  = RbConfig::CONFIG['includedir']

# HEADER_DIRS = [
#   # Then search /usr/local for people that installed from source
#   '/usr/local/include',

#   # Check the ruby install locations
#   INCLUDEDIR,

#   # Finally fall back to /usr
#   '/usr/include',
# ]

# LIB_DIRS = [

#   # Then search /usr/local for people that installed from source
#   '/usr/local/lib',

#   # Check the ruby install locations
#   LIBDIR,

#   # Finally fall back to /usr
#   '/usr/lib',
# ]

$LIBS << ' -lftd2xx'

$DLDFLAGS << ' -Wl,-rpath /usr/local/lib'

# dir_config(extension_name, HEADER_DIRS, LIB_DIRS)

dir_config(extension_name)

create_makefile(extension_name)
