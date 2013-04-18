# Copyright 2005-2013 Intel Corporation.  All Rights Reserved.
#
# This file is part of Threading Building Blocks.
#
# Threading Building Blocks is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# Threading Building Blocks is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Threading Building Blocks; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.

ifneq (mic,$(offload))
  $(error File mic.offload.inc should not be included directly. Use offload=mic instead.)
endif
ifneq (icc,$(compiler))
  $(error Only Intel(R) Compiler is supported for MIC offload compilation)
endif

tbb_offload_build_prefix?=mic_offload_icc$(CPF_SUFFIX)
# The same build prefix should be used in mic.linux.inc
export tbb_mic_build_prefix?=mic_icc$(CPF_SUFFIX)
initial_tbb_root:=$(tbb_root)

ifndef BUILDING_PHASE
tbb_native_debug:
	$(MAKE) tbb_debug target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbb_native_release:
	$(MAKE) tbb_release target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbbmalloc_native_debug:
	$(MAKE) tbbmalloc_debug target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbbmalloc_native_release:
	$(MAKE) tbbmalloc_release target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
rml_native_debug:
	$(MAKE) rml_debug target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
rml_native_release:
	$(MAKE) rml_release target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbb_native:
	$(MAKE) tbb target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbbmalloc_native:
	$(MAKE) tbbmalloc target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
rml_native:
	$(MAKE) rml target=mic compiler=icc offload= tbb_root=$(initial_tbb_root)
tbb_host_debug:
	$(MAKE) tbb_debug compiler=icc offload= tbb_root=$(initial_tbb_root)
tbb_host_release:
	$(MAKE) tbb_release compiler=icc offload= tbb_root=$(initial_tbb_root)
tbbmalloc_host_debug:
	$(MAKE) tbbmalloc_debug compiler=icc offload= tbb_root=$(initial_tbb_root)
tbbmalloc_host_release:
	$(MAKE) tbbmalloc_release compiler=icc offload= tbb_root=$(initial_tbb_root)

# The work_dir variable can not be set via target-specific variable mechanism
# since it will affect other targets on which the target depends.
# So work_dir is set via eval function inside the recipe.
prepare_test_offload_dir_debug: initial_tbb_build_dir:=$(tbb_build_dir)
prepare_test_offload_dir_debug: tbb_host_debug tbbmalloc_host_debug tbb_native_debug tbbmalloc_native_debug
	$(eval work_dir=$(initial_tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix))
	$(shell $(MD) "$(work_dir)_debug$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_build_prefix)_debug$(SLASH)lib"* "$(work_dir)_debug" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_mic_build_prefix)_debug$(SLASH)lib"* "$(work_dir)_debug$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(eval export TEST_COMPILATION=1)

prepare_test_offload_dir_release: initial_tbb_build_dir:=$(tbb_build_dir)
prepare_test_offload_dir_release: tbb_host_release tbbmalloc_host_release tbb_native_release tbbmalloc_native_release
	$(eval work_dir=$(initial_tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix))
	$(shell $(MD) "$(work_dir)_release$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_build_prefix)_release$(SLASH)lib"* "$(work_dir)_release" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_mic_build_prefix)_release$(SLASH)lib"* "$(work_dir)_release$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(eval export TEST_COMPILATION=1)

prepare_test_offload_dir: initial_tbb_build_dir:=$(tbb_build_dir)
prepare_test_offload_dir: tbb tbbmalloc tbb_native tbbmalloc_native
	$(eval work_dir=$(initial_tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix))
	$(shell $(MD) "$(work_dir)_debug$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(shell $(MD) "$(work_dir)_release$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_build_prefix)_debug$(SLASH)lib"* "$(work_dir)_debug" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_build_prefix)_release$(SLASH)lib"* "$(work_dir)_release" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_mic_build_prefix)_debug$(SLASH)lib"* "$(work_dir)_debug$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(shell $(CP) "$(initial_tbb_build_dir)$(SLASH)$(tbb_mic_build_prefix)_release$(SLASH)lib"* "$(work_dir)_release$(SLASH)mic" >$(NUL) 2>$(NUL))
	$(eval export TEST_COMPILATION=1)

test: prepare_test_offload_dir
tbb_test_debug: prepare_test_offload_dir_debug
tbb_test_release: prepare_test_offload_dir_release
tbbmalloc_test_debug: prepare_test_offload_dir_debug
tbbmalloc_test_release: prepare_test_offload_dir_release
rml_test_debug: rml_native_debug prepare_test_offload_dir_debug
rml_test_release: rml_native_release prepare_test_offload_dir_release

debug_% test_% stress_% time_% perf_%:: work_dir:=$(tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix)
debug_% test_% stress_% time_% perf_%:: export TEST_COMPILATION=1

clean: work_dir:=$(tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix)
# The _clean* targets begin with an underscore symbol to prevent a collision with the clean_% template
clean: _clean_mic_subdir_debug _clean_mic_subdir_release _clean_host _clean_mic
clean_release: work_dir:=$(tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix)
clean_release: _clean_mic_subdir_release _clean_host_release _clean_mic_release
clean_debug: work_dir:=$(tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix)
clean_debug: _clean_mic_subdir_debug _clean_host_debug _clean_mic_debug
clean_%:: work_dir:=$(tbb_build_dir)$(SLASH)$(tbb_offload_build_prefix)

_clean_mic_subdir_debug:
	$(shell $(RM) $(work_dir)_debug$(SLASH)mic/$(SLASH)*.* >$(NUL) 2>$(NUL))
	$(shell $(RD) $(work_dir)_debug$(SLASH)mic >$(NUL) 2>$(NUL))
_clean_mic_subdir_release:
	$(shell $(RM) $(work_dir)_release$(SLASH)mic/$(SLASH)*.* >$(NUL) 2>$(NUL))
	$(shell $(RD) $(work_dir)_release$(SLASH)mic >$(NUL) 2>$(NUL))
_clean_host:
	$(MAKE) clean tbb_root=$(initial_tbb_root) offload=
_clean_mic:
	$(MAKE) clean target=mic tbb_root=$(initial_tbb_root) offload=
_clean_host_release:
	$(MAKE) clean_release tbb_root=$(initial_tbb_root) offload=
_clean_host_debug:
	$(MAKE) clean_debug tbb_root=$(initial_tbb_root) offload=
_clean_mic_release:
	$(MAKE) clean_release target=mic tbb_root=$(initial_tbb_root) offload=
_clean_mic_debug:
	$(MAKE) clean_debug target=mic tbb_root=$(initial_tbb_root) offload=
endif

ifeq ($(TEST_COMPILATION),1)
  ifdef MIC_LD_LIBRARY_PATH
    export MIC_LD_LIBRARY_PATH := ./mic:$(MIC_LD_LIBRARY_PATH)
  else
    export MIC_LD_LIBRARY_PATH := ./mic
  endif
  LINK_TBB.LIB=-offload-option,mic,ld,"./mic/$(TBB.LIB)" $(TBB.LIB)
  LINK_MALLOC_LIB=-offload-option,mic,ld,"./mic/$(MALLOC.DLL)" $(MALLOC.DLL)
  LINK_MALLOC_PROXY_LIB=-offload-option,mic,ld,"./mic/$(MALLOCPROXY.DLL)" $(MALLOCPROXY.DLL)
  LINK_FLAGS =
  LIB_LINK_FLAGS += $(DYLIB_KEY)
  # Do not use -Werror because it is too strict for the early offload compiler.
  # Need to set anything because WARNING_AS_ERROR_KEY should not be empty.
  # Treat #2426 as a warning. Print errors only.
  tbb_strict=0
  override WARNING_AS_ERROR_KEY = Warning as error
  override WARNING_KEY = -diag-warning 2426 -w0
  # Enable mic-specific stuff.
  CXX_MIC_STUFF = -offload-attribute-target=mic -D__TBB_MIC_OFFLOAD=1 \
          -offload-option,mic,compiler,"-D__TBB_MIC_OFFLOAD=1 -DHARNESS_INCOMPLETE_SOURCES=1 -D__TBB_MIC_NATIVE -DTBB_USE_EXCEPTIONS=0"
  CPLUS_FLAGS += $(CXX_MIC_STUFF)
  export OFFLOAD_EXECUTION = 1
  # Workaround to skip the RML tests in the offload mode
  TBB_DEP_NON_RML_TEST =
  TBB_DEP_RML_TEST =
  RML_TBB_CLIENT.OBJ = 
  RML_OMP_CLIENT.OBJ =
endif

# detects whether examples are being built.
ifeq ($(BUILDING_PHASE),0)
  export UI = con
  export x64 = 64
endif # examples
