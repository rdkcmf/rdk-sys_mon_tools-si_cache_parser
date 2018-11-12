#!/bin/sh
##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
#
set -x
echo "Building SI Parser for XG1..."
#source $WORK_DIR/build_scripts/setBCMenv.sh
export BCM_TOOLCHAIN=$SCRIPT_DIR/workXG1/tools/stbgcc-4.5.4-2.8/bin
export GCC=$BCM_TOOLCHAIN/mipsel-linux-gcc

cd src/
make clean all
cd -
