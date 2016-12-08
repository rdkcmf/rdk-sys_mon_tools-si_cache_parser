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
    printf ("This executable will parse 121 cache files and prints on console.\n");
    printf ("si_cache_parser_121   < Folder >\n");
    printf ("\t- Executing this binary without single input will take SICache & SISNSCache from given directory and parses it...!\n\n");
    printf ("si_cache_parser_121   <SICache>   <SISNSCache> \n");
    printf ("\t- This will parse the given SI cache files...!\n\n");
    return;
}


main(int argc, char *argv[])
{
    char SICache[50] = "";
    char SNSCache[50] = "";

    if (argc == 2)
    {
        init_mpeg2_crc();
        snprintf (SICache, sizeof(SICache) - 1, "%s/SICache", argv[1]);
        snprintf (SNSCache, sizeof(SNSCache) - 1,  "%s/SISNSCache", argv[1]);
        printf ("Executing 'si_cache_parser_121 %s %s'\n", SICache, SNSCache);
        if (verify_version_and_crc(SICache, SNSCache))
        {
                load_sns_entries(SNSCache);
                load_si_entries_Post114(SICache);
                dump_si_cache();
        }
        else
                printf ("Version/CRC mismatch \n");
    }
    else if (argc == 3)
    {
        init_mpeg2_crc();
        if (verify_version_and_crc(argv[1], argv[2]))
        {
            load_sns_entries(argv[2]);
            load_si_entries_Post114(argv[1]);
            dump_si_cache();
        }
        else
                printf ("Version/CRC mismatch \n");
    }
    else 
    {
        usuage();
    }
    return 0;
}
