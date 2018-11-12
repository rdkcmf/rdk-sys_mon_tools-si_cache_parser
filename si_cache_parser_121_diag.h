/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#ifndef _SI_CACHE_DIAG_
#define _SI_CACHE_DIAG__
typedef struct channel_map_entry
{
    uint32_t  virtual_channel_number;
    uint32_t source_id; // SVCT
    uint32_t freq_index; // SVCT
    uint16_t program_number;      
    struct channel_map_entry* next; // next service entry
} CHANNEL_MAP_ENTRY;

/**
 *  Function to return the following details from channel map
 *  channel number (VCN)
 *  source ID
 *  carrier frequency
 *  service/program number
 */
CHANNEL_MAP_ENTRY* channelMapForDiag( char* persistent_dir );

/**
 *  Function to free the global channel map entry list
 */
void freeChannelMapEntryList(void);
#endif
