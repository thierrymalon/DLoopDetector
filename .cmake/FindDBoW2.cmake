#.rst:
# FindDBoW2
# --------
#
# Find the native DBoW2 includes and libraries.
#
# DBoW2 is a collection of C++ classes to solve common tasks in C++ programs, as well as to 
# offer additional functionality to use OpenCV data and to solve computer vision problems.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project::
#
#  DBOW2_FOUND          - True if DBoW2 found on the local system
#  DBOW2_INCLUDE_DIRS   - Location of DBoW2 header files.
#  DBOW2_LIBRARIES      - The DBoW2 libraries.
#
# Hints
# ^^^^^
#
# Set ``DBOW2_ROOT_DIR`` to a directory that contains a DBoW2 installation.
#
# This script expects to find libraries at ``$DBOW2_ROOT_DIR/`` and the DBoW2
# headers at ``$DBOW2_ROOT_DIR``.  

# =============================================================================
# Copyright (c) 2015, Simone Gasparini <simone.gasparini@gmail.com> All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.

# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# =============================================================================

FIND_PATH(DBOW2_INCLUDE_DIRS
	NAMES DBoW2/DBoW2.h
	HINTS
    ${DBOW2_ROOT_DIR}/include
	PATHS
    /usr/include
    /usr/local/include
    /sw/include
    /opt/local/include
    DOC "The directory where DBoW2 headers reside")

 message(STATUS  "DBOW2_INCLUDE_DIRS = ${DBOW2_INCLUDE_DIRS}")


FIND_LIBRARY(DBOW2_LIBRARIES 
	NAMES DBoW2
	PATHS ${DBOW2_ROOT_DIR}/lib
	DOC "The DBoW2 library")

message(STATUS  "DBOW2_LIBRARIES = ${DBOW2_LIBRARIES}")


include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBDBOW2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( libDBOW2  DEFAULT_MSG
                                  DBOW2_LIBRARIES DBOW2_INCLUDE_DIRS )

