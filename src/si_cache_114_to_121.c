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

void usuage()
{
    printf ("\n\tSI Cache Converter\n");
    printf (  "\t------------------\n");
    printf ("This executable will convert the 114 SI cache into 121 cache.\n");
    printf ("si_cache_114_to_121    <114 - SICache> \n");
    printf ("\t- This will create the 121 SI cache files in the name of SICache & SNSCache in current directory...!\n\n");
    printf ("si_cache_114_to_121    <114 - SICache>   <121 SI Cache>   <121 - SNSCache> \n");
    printf ("\t- This will create the 121 SI cache files with given name...!\n\n");
    return;
}

main(int argc, char *argv[])
{
    char *pSICache = "SICache";
    char *pSISNSCache = "SISNSCache";

    if (argc == 2)
    {
        printf("NON CRC version\r\n");
        init_mpeg2_crc();
        load_si_entries_114(argv[1], pSICache, pSISNSCache);
        cache_si_entries(pSICache);
        cache_sns_entries(pSISNSCache);
        write_crc_for_si_and_sns_cache (pSICache, pSISNSCache);
        dump_si_cache();

    }
    else if (argc == 4) {
        init_mpeg2_crc();
        load_si_entries_114(argv[1], argv[2], argv[3]);
        cache_si_entries(argv[2]);
        cache_sns_entries(argv[3]);
        write_crc_for_si_and_sns_cache (argv[2], argv[3]);
        dump_si_cache();
    }
    else
        usuage();

    return 0;
}
