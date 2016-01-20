from setuptools import setup, find_packages
from setuptools.command.install import install
from setuptools.command.develop import develop
from setuptools.command.egg_info import egg_info
from subprocess import call, STDOUT
import distutils.sysconfig
import os
import os.path
import tempfile
import glob
import shutil
import distutils
print(os.getcwd())


PYTHONINC = distutils.sysconfig.get_python_inc()
PYTHONLIB = distutils.sysconfig.get_python_lib(plat_specific=True, standard_lib=True)
PYTHONLIBDIR = distutils.sysconfig.get_config_var("LIBDIR")
PYTHONLIBS = glob.glob(os.path.join(PYTHONLIBDIR, "*.dylib"))
PYTHONLIBS.extend(glob.glob(os.path.join(PYTHONLIBDIR, "*.so")))
PYTHONLIB = PYTHONLIBS[0]


#print(os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir)))
#print(PYTHONINC)
#print(PYTHONLIB)

d = "setuppy_build"

PROJECT_PYTHON_DIR = "stormpy"
PROJECT_PYTHON_FILES = os.path.join(os.path.dirname(os.path.realpath(__file__)),  PROJECT_PYTHON_DIR)
PROJECT_PYTHON_TMP_DESTINATION = os.path.join(os.path.realpath(d), PROJECT_PYTHON_DIR)

print(d)
if not os.path.exists(d):
    os.makedirs(d)

class MyEggInfo(egg_info):
    def run(self):
        try:
            src = PROJECT_PYTHON_FILES
            print(src)
            dst = PROJECT_PYTHON_TMP_DESTINATION
            print(dst)
            distutils.dir_util.copy_tree(src, dst)
            egg_info.run(self)
        except:
            print("Exception occurred")
            egg_info.run(self)


class MyInstall(install):
    def run(self):
        ret = call(["cmake", "-DSTORM_PYTHON=ON", "-DUSE_BOOST_STATIC_LIBRARIES=OFF",  "-DPYTHON_LIBRARY="+PYTHONLIB, "-DPYTHON_INCLUDE_DIR="+PYTHONINC, os.path.abspath(os.path.dirname(os.path.realpath(__file__)))], cwd=d)
        if ret != 0:
            raise RuntimeError("Cmake exited with return code {}".format(ret))
        ret = call(["make", "stormpy"], cwd=d)
        if ret != 0:
            raise RuntimeError("Make exited with return code {}".format(ret))
        install.run(self)
class MyDevelop(develop):
    def run(self):
       ret = call(["cmake", "-DSTORM_PYTHON=ON","-DUSE_BOOST_STATIC_LIBRARIES=OFF",  "-DPYTHON_LIBRARY="+PYTHONLIB, "-DPYTHON_INCLUDE_DIR="+PYTHONINC, os.path.abspath(os.path.dirname(os.path.realpath(__file__)))], cwd=d)
       if ret != 0:
           raise RuntimeError("Cmake exited with return code {}".format(ret))
       ret = call(["make", "stormpy"], cwd=d)
       if ret != 0:
           raise RuntimeError("Make exited with return code {}".format(ret))
       develop.run(self)


setup(cmdclass={'install': MyInstall, 'develop': MyDevelop, 'egg_info': MyEggInfo},
      name="stormpy",
      version="0.2",
      description="Stormpy - Python Bindings for Storm",
      package_dir={'':d},
      packages=['stormpy', 'stormpy.core', 'stormpy.info', 'stormpy.logic'],
      package_data={'stormpy.core': ['_core.so'],
                    'stormpy.logic': ['_logic.so'],
                    'stormpy.info' : ['_info.so'] ,
                     'stormpy': ['*.so', '*.dylib', '*.a']},

      include_package_data=True)