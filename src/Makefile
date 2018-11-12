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
#######################################
##### Makefile to build SI Parser #####
#######################################
all: Makefile si_cache_parser_114  si_cache_114_to_121  si_cache_parser_121 si_cache_parser_diag.so

si_cache_parser_114: si_cache_parser_114.c 
	@echo "Building si_cache_parser_114...."
	@$(GCC) si_cache_parser_114.c -o si_cache_parser_114

si_cache_114_to_121: si_cache_parser_121_impl.c  si_cache_parser_121_lib.c si_cache_114_to_121.c
	@echo "Building si_cache_114_to_121...."
	@$(GCC) si_cache_114_to_121.c -o si_cache_114_to_121

si_cache_parser_121: si_cache_parser_121_impl.c  si_cache_parser_121_lib.c si_cache_parser_121.c
	@echo "Building si_cache_parser_121...."
	@$(GCC) si_cache_parser_121.c -o si_cache_parser_121

si_cache_parser_diag.so: si_cache_parser_diag.c
	@echo "Creating shared library for cache parser"
	@$(GCC) -c -fpic si_cache_parser_diag.c
	@$(GCC) -shared -o libsi_cache_parser_diag.so si_cache_parser_diag.o

clean:
	@echo "Cleaning....!"
	@rm -f si_cache_114_to_121  si_cache_parser_114  si_cache_parser_121 SICache SISNSCache libsi_cache_parser_diag.so
