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
#include "si_cache_parser_121_lib.c"
#include "si_cache_parser_121_impl.c"

CHANNEL_MAP_ENTRY* channelMapForDiag( char* persistent_dir )
{
    char SICache[50] = "";
    char SNSCache[50] = "";
    CHANNEL_MAP_ENTRY *channel_map_entry = NULL;

    if ( NULL != persistent_dir )
    {
        init_mpeg2_crc();
        snprintf (SICache, sizeof(SICache) - 1, "%s/SICache", persistent_dir);
        snprintf (SNSCache, sizeof(SNSCache) - 1,  "%s/SISNSCache", persistent_dir);
        printf ("Executing 'si_cache_parser_121 %s %s'\n", SICache, SNSCache);
        if (verify_version_and_crc(SICache, SNSCache))
        {
                load_sns_entries(SNSCache);
                load_si_entries_Post114(SICache);
                channel_map_entry = (CHANNEL_MAP_ENTRY*) dump_si_cache_diagnostics();
        }
        else
                printf ("Version/CRC mismatch \n");
    }
    return channel_map_entry;
}

void freeChannelMapEntryList( void )
{
     freeEntryList();
}
