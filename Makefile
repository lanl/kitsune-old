#
###########################################################################
# Copyright (c) Los Alamos National Security, LLC.
# All rights reserved.
#
#  Copyright 2010. Los Alamos National Security, LLC. This software was
#  produced under U.S. Government contract DE-AC52-06NA25396 for Los
#  Alamos National Laboratory (LANL), which is operated by Los Alamos
#  National Security, LLC for the U.S. Department of Energy. The
#  U.S. Government has rights to use, reproduce, and distribute this
#  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
#  LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
#  FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
#  derivative works, such modified software should be clearly marked,
#  so as not to confuse it with the version available from LANL.
#
#  Additionally, redistribution and use in source and binary forms,
#  with or without modification, are permitted provided that the
#  following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of Los Alamos National Security, LLC, Los
#      Alamos National Laboratory, LANL, the U.S. Government, nor the
#      names of its contributors may be used to endorse or promote
#      products derived from this software without specific prior
#      written permission.
#
#  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
#  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
#  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
#  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#
###########################################################################
#
# This makefile is simply a shortcut for running the steps of a cmake
# configuration, kicking off a build and various other steps
# (e.g. sync'ing up with the llvm repos, cleaning up, etc.).
#
# Summary of targets: 
#
#   'default'         : Run 'config' and 'build' (cmake 'Makefile'-based config 
#                       and build). 
#
#   'config'          : Run cmake to dump a build configuration. The out-of-source 
#                       build location defaults to $(PWD)/build. 
# 
#   'build'           : Build from a cmake build configuration. 
#
#   'ninja'           : Run 'ninja-config' and 'ninja-build' (cmake 'Ninja'-based 
#                       config and build).  For more on Ninja see: 
#
#                         https://ninja-build.org/
# 
#   'ninja-config'    : Run cmake to dump a ninja-based configuration/build. 
#                       The out-of-source build location defaults to $(PWD)/build. 
#             
#   'ninja-build'     : Build from a ninja build configuration.
#
#   'update-subtrees' : Update the llvm subtrees used in the project. 
#
#   'clean'           : Clean up everything. 
# 
########

src_dir     := ${CURDIR}
cmake_dir   := ${src_dir}/cmake 
build_dir   := $(src_dir)/build
log_dir     := $(src_dir)/logs
install_dir := $(build_dir)/kitsune/ 

#cmake_flags := -C $(src_dir)/cmake/DefaultCache.cmake \
#               -DKITSUNE_CLANG=ON -DCMAKE_INSTALL_PREFIX=$(install_dir)

cmake_flags := -C $(src_dir)/cmake/DefaultCache.cmake \
                -DCMAKE_INSTALL_PREFIX=$(install_dir)

.PHONY: default build clean

default: config build 

ninja: ninja-config ninja-build 
	@echo "kitsune: finished..."

$(build_dir):
	@((test -d $(build_dir)) || (mkdir $(build_dir)))

$(build_dir)/Makefile: config 
	@echo "kitsune: cmake makefile configuration finished..."

config: $(build_dir) CMakeLists.txt 
	@echo "kitsune: configuring build...  (grab some coffee, take a walk... :-)"
	@echo "kitsune: [source directory: $(src_dir)]"
	@echo "kitsune: [configuration log: $(log_dir)/kitsune-configure.log]"
	@((test -f $(log_dir)/configure.log) || (rm -f $(log_dir)/kitsune-configure.log))
	@(cd $(build_dir); cmake $(cmake_flags) $(src_dir)) &> $(log_dir)/kitsune-configure.log

build: 
	@(cd $(build_dir); make -j 8)

ninja-config: $(build_dir)
	@echo "kitsune: configuring build...  (grab some coffee, take a walk... :-)"
	@echo "kitsune: [source directory: $(src_dir)]"
	@echo "kitsune: [configuration log: $(log_dir)/kitsune-configure.log]"
	@((test -f $(log_dir)/configure.log) || (rm -f $(log_dir)/kitsune-configure.log))
	@(cd $(build_dir); cmake -G Ninja $(cmake_flags) $(src_dir)) &> $(log_dir)/kitsune-configure.log
	@echo "kitsune: cmake ninja generation finished..."

ninja-build:
	@echo "kitsune: building..."
	@(cd $(build_dir); ninja)

# TODO: Make sure build configuration is done before updating subtrees... 
update-subtrees:
	@echo "kitsune: updating llvm subtrees..."
	@echo "kitsune: [subtree update log: $(log_dir)/llvm-update-subtrees.log]"
	@((test -f $(log_dir)/llvm-update-subtrees.log) || (rm -f $(log_dir)llvm-update-subtrees.log))
	@(cd $(src_dir); ./git-tools/update-llvm > $(log_dir)/llvm-update-subtrees.log 2>&1)

clean:
	-@/bin/rm -rf $(build_dir)
	-@/usr/bin/find . -name '*~' -exec rm -f {} \;
	-@/usr/bin/find . -name '._*' -exec rm -f {} \;
	-@/usr/bin/find . -name '.DS_Store' -exec rm -f {} \;
	-@/bin/rm -f $(log_dir)/*.log
