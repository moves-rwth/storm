#!/usr/bin/env python
from setuptools import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext
import os.path
import platform
from glob import glob

PROJECT_DIR = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))

# Glob source files for modules
core_sources = glob(os.path.join('src', 'core', '*.cpp'))
expressions_sources = glob(os.path.join('src', 'expressions', '*.cpp'))
logic_sources = glob(os.path.join('src', 'logic', '*.cpp'))
storage_sources = glob(os.path.join('src', 'storage', '*.cpp'))

# Configuration shared between external modules follows

# To help along, if storm and/or pybind is not system installed, retrieve from storm distribution
include_dirs = ['.', 'src', 'resources/pybind11/include/']
# Add more include dirs
# TODO handle by cmake
include_dirs.extend(['../build/include/', '../resources/3rdparty/sylvan/src/', '../resources/3rdparty/exprtk/', '../resources/3rdparty/gmm-5.0/include/'])
boost_dir = '/usr/local/include/'
include_dirs.append(boost_dir)
cudd_dirs = ['../resources/3rdparty/cudd-3.0.0/cplusplus/', '../resources/3rdparty/cudd-3.0.0/mtr/', '../resources/3rdparty/cudd-3.0.0/cudd/']
include_dirs.extend(cudd_dirs)
log4cplus_dirs = ['../resources/3rdparty/log4cplus-1.1.3-rc1/include/', '../build/resources/3rdparty/log4cplus-1.1.3-rc1/include/']
include_dirs.extend(log4cplus_dirs)

local_storm_path = os.path.join(PROJECT_DIR, '..')
if os.path.exists(local_storm_path):
    include_dirs.append(local_storm_path)

# Like includes, also add local path for library, assuming made in 'build'
library_dirs = []
local_storm_lib_path = os.path.join(PROJECT_DIR, '..', 'build/src')
if os.path.exists(local_storm_lib_path):
    library_dirs.append(local_storm_lib_path)

libraries = ['storm']
extra_compile_args = ['-std=c++11']
define_macros = []

extra_link_args = []
if platform.system() == 'Darwin':
    extra_link_args.append('-Wl,-rpath,'+library_dirs[0])

ext_core = Extension(
    name='core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros,
    extra_link_args=extra_link_args
)

ext_info = Extension(
    name='info.info',
    sources=['src/mod_info.cpp'],
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros,
    extra_link_args=extra_link_args
)

ext_expressions = Extension(
    name='expressions.expressions',
    sources=['src/mod_expressions.cpp'] + expressions_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros,
    extra_link_args=extra_link_args
)

ext_logic = Extension(
    name='logic.logic',
    sources=['src/mod_logic.cpp'] + logic_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros,
    extra_link_args=extra_link_args
)

ext_storage = Extension(
    name='storage.storage',
    sources=['src/mod_storage.cpp'] + storage_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros,
    extra_link_args=extra_link_args
)

class stormpy_build_ext(build_ext):
    """Extend build_ext to provide CLN toggle option
    """
    user_options = build_ext.user_options + [
        ('use-cln', None,
         "use cln numbers instead of gmpxx"),
        ('carl_src', None,
         "path to src directory of CaRL"),

        ]

    def __init__(self, *args, **kwargs):
        build_ext.__init__(self, *args, **kwargs)

    def initialize_options (self):
        build_ext.initialize_options(self)
        self.use_cln = None
        self.carl_src = None

    def finalize_options(self):
        build_ext.finalize_options(self)

        if self.use_cln:
            self.libraries += ['cln']
            if not self.define:
                self.define = []
            else:
                self.define = list(self.define)
            self.define += [('STORMPY_USE_CLN', 1)]
        else:
            self.libraries += ['gmp', 'gmpxx']
            if not self.undef:
                self.undef = []
            self.undef += ['STORMPY_USE_CLN']

        if library_dirs:
            # Makes local storm library lookup that much easier
            self.rpath += library_dirs

        print("Add carl_src: {}".format(self.carl_src))
        include_dirs.append(self.carl_src)

setup(name="stormpy",
      version="0.9",
      author="M. Volk",
      author_email="matthias.volk@cs.rwth-aachen.de",
      maintainer="S. Junges",
      maintainer_email="sebastian.junges@cs.rwth-aachen.de",
      url="http://moves.rwth-aachen.de",
      description="stormpy - Python Bindings for Storm",
      packages=['stormpy', 'stormpy.info', 'stormpy.expressions', 'stormpy.logic'],
      package_dir={'':'lib'},
      ext_package='stormpy',
      ext_modules=[ext_core, ext_info, ext_expressions, ext_logic, ext_storage
                   ],
      cmdclass={
        'build_ext': stormpy_build_ext,
      }
)
