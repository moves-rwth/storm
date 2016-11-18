#!/usr/bin/env python
from setuptools import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext as orig_build_ext
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
include_dirs = [PROJECT_DIR, os.path.join(PROJECT_DIR, 'src'),
                os.path.join(PROJECT_DIR, 'resources', 'pybind11', 'include')]
libraries = ['storm']
library_dirs = []
extra_compile_args = []
define_macros = []
extra_link_args = []

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

class build_ext(orig_build_ext):
    """Extend build_ext to provide CLN toggle option
    """
    user_options = orig_build_ext.user_options + [
        ('use-cln', None,
         "use cln numbers instead of gmpxx"),
        ('compile-flags', None,
         "compile flags for C++"),
        ]

    boolean_options = orig_build_ext.boolean_options + ['use-cln']

    def initialize_options (self):
        super(build_ext, self).initialize_options()
        self.use_cln = None
        self.compile_flags = None

    def finalize_options(self):
        super(build_ext, self).finalize_options()

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

        if self.library_dirs:
            # Makes local storm library lookup that much easier
            self.rpath += self.library_dirs

        if platform.system() == 'Darwin' and len(self.rpath) > 0:
            for e in self.extensions:
                # If rpath is used on OS X, set this option
                e.extra_link_args.append('-Wl,-rpath,'+self.rpath[0])

        for e in self.extensions:
            e.extra_compile_args += self.compile_flags.split()

setup(name="stormpy",
      version="0.9",
      author="M. Volk",
      author_email="matthias.volk@cs.rwth-aachen.de",
      maintainer="S. Junges",
      maintainer_email="sebastian.junges@cs.rwth-aachen.de",
      url="http://moves.rwth-aachen.de",
      description="stormpy - Python Bindings for Storm",
      packages=['stormpy', 'stormpy.info', 'stormpy.expressions', 'stormpy.logic', 'stormpy.storage'],
      package_dir={'':'lib'},
      ext_package='stormpy',
      ext_modules=[ext_core, ext_info, ext_expressions, ext_logic, ext_storage
                   ],
      cmdclass={
        'build_ext': build_ext,
      }
)
