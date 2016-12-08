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
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
#include <sys/stat.h>
#include "si_cache_parser_121_diag.h"

/* x86 */

static mpe_Error read_name_string(mpe_File cache_fd, mpe_SiLangSpecificStringList *name, uint32_t *read_count, int total_bytes);


CHANNEL_MAP_ENTRY *channel_map_list_for_diag = NULL;

mpe_Error cache_si_entries(const char *siOOBCacheLocation)
{
    mpe_Error err = MPE_SUCCESS;
    mpe_SiTableEntry *walker = NULL;
    mpe_siSourceNameEntry *source_name_entry = NULL;
    mpe_File cache_fd;
    mpe_File fd;
    mpe_SiProgramInfo *program_info = NULL;
    uint32_t num_entry_written = 0;
    uint32_t defined_mapped_count = 0;
    uint32_t count = 0;
    // Parameter check
    {
        if (MPE_SUCCESS != mpe_fileDelete(siOOBCacheLocation))
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "CACHE: Error deleting file...%s\n", siOOBCacheLocation);
        }
        err = mpe_fileOpen(siOOBCacheLocation, MPE_FS_OPEN_WRITE | MPE_FS_OPEN_MUST_CREATE, &fd);
        if (err == MPE_SUCCESS)
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s]  failed with %d\n", siOOBCacheLocation, err);
            err = MPE_SI_INVALID_PARAMETER;
        }
    }
    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: starting writing cache\n");
    if (err == MPE_SUCCESS)
    {
        uint32_t write_count = 0;
        uint32_t loop_count = 0;
        uint32_t version = MPE_SI_CACHE_FILE_VERSION;

        // Write header version
        count = sizeof(version);
        err = mpe_fileWrite(cache_fd, &count, (void *)&version);

        if (err == MPE_SUCCESS && (count == sizeof(version)))
        {
            MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "writing version to [%s] 0x%x count:%d\n", siOOBCacheLocation, MPE_SI_CACHE_FILE_VERSION, sizeof(version));
            write_count += count;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing version to [%s] failed with %d\n", siOOBCacheLocation, err);
            err = MPE_SI_INVALID_PARAMETER;
        }

        if (err == MPE_SUCCESS)
        {
            // Write frequency table
            count = sizeof(g_frequency);
            err = mpe_fileWrite(cache_fd, &count, (void *)g_frequency);
    
            if (err == MPE_SUCCESS && (count == sizeof(g_frequency)))
            {
                MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "writing FREQ to [%s] %d \n", siOOBCacheLocation, count);
                write_count += count;
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing FREQ to [%s] failed with %d\n", siOOBCacheLocation, err);
                err = MPE_SI_INVALID_PARAMETER;
            }
        }

        if (err == MPE_SUCCESS)
        {
            // Write modulation mode table
            count = sizeof(g_mode);
            err = mpe_fileWrite(cache_fd, &count, (void *)g_mode);
            if (err == MPE_SUCCESS && (count == sizeof(g_mode)))
            {
                MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "writing MODE to [%s] %d \n", siOOBCacheLocation, count);
                write_count += count;
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing MODE to [%s] failed with %d\n", siOOBCacheLocation, err);
                err = MPE_SI_INVALID_PARAMETER;
            }
        }

        // Write table entries
        walker = g_si_entry;
        while (walker && err == MPE_SUCCESS)
        {
            mpe_SiLangSpecificStringList *name_list = walker->descriptions;
            int                             name_count = 0;
            mpe_SiTransportStreamHandle     ts_handle;

            // OOB, HN??
            if (!( (walker->state == SIENTRY_DEFINED_MAPPED) ||
                   ((walker->state == SIENTRY_MAPPED) &&
                    (walker->ts_handle == (mpe_SiTransportStreamHandle)g_si_dsg_ts_entry))) )
            {
                walker = walker->next;
                continue;
            }

            loop_count++;
            {
                source_name_entry = walker->source_name_entry;
                if (walker->state == SIENTRY_DEFINED_MAPPED)
                {
                    defined_mapped_count++;
                }
                while(name_list)
                {
                    name_count++;
                    name_list = name_list->next;
                }

                walker->source_name_entry   = (mpe_siSourceNameEntry *)0;
                walker->descriptions        = (mpe_SiLangSpecificStringList *)name_count;
                program_info = walker->program;
                if (walker->program != NULL)
                {
                    walker->program = (mpe_SiProgramInfo *)PROGRAM_NUMBER_UNKNOWN;
                }
                else
                {
                    walker->program = (mpe_SiProgramInfo *)0;
                }
                ts_handle = walker->ts_handle;
                if(walker->ts_handle == (mpe_SiTransportStreamHandle)g_si_dsg_ts_entry)
                {
                    walker->ts_handle = MPE_SI_DSG_FREQUENCY;
                    walker->program = (mpe_SiProgramInfo *)PROGRAM_NUMBER_UNKNOWN;
                }

                count = sizeof(*walker);
                err = mpe_fileWrite(cache_fd, &count, (void *)walker);
                walker->source_name_entry   = source_name_entry;
                walker->descriptions        = name_list;
                walker->program             = program_info;
                walker->ts_handle           = ts_handle;

                if (count == sizeof(*walker))
                {
                    write_count += count;
                    num_entry_written++;
                    // now copy names
                    {
                        // LEN-NAME, LEN-NAME...
                        err = write_name_string(cache_fd, name_list, &write_count);
                    }
                }
                else
                {
                    MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI %s  failed with %d\n", siOOBCacheLocation, err);
                    err = MPE_SI_INVALID_PARAMETER;
                    break;
                }
                if (1)
                MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "WChannelVCN#%06d-SRCID#%06x-Name[%s]-State-[%d]-Freq[%09d]-Mode[%04d]-Prog[%08d]-Vid[%d]-TT[%d]\n",
                        walker->virtual_channel_number,
                        walker->isAppType ? walker->app_id : walker->source_id,
                        (walker->source_name_entry && walker->source_name_entry->source_names && walker->source_name_entry->source_names->string)? walker->source_name_entry->source_names->string : "NULL",
                        walker->state,
                        g_frequency[walker->freq_index],
                        g_mode[walker->mode_index],
                        walker->program_number,
                        walker->video_standard,
                        walker->transport_type);

            }
            walker = walker->next;
        } // For each walker

        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "writing /persistent/si/si_table_cache DONE writing SVCT entries %d bytes %d entries\n", sizeof(*walker), loop_count);

        mpe_fileClose(cache_fd);

        if (defined_mapped_count == 0)
        {
            /* Only OOB channels are cached, discard it */
            MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "CACHE: No DEFINED_MAPPED entries...\n");
            if (MPE_SUCCESS != mpe_fileDelete(siOOBCacheLocation))
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "CACHE: Error deleting file...%s\n", siOOBCacheLocation);
            }
        }
    }
    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: done writing cache %d, %d\n", num_entry_written, defined_mapped_count);

    return err;
}

mpe_Error cache_sns_entries(const char *siOOBSNSCacheLocation)
{
    mpe_Error err = MPE_SUCCESS;
    mpe_siSourceNameEntry *sn_walker = NULL;
    mpe_File cache_fd;
    mpe_File fd;
    uint32_t count = 0;
    // Parameter check
    {
        if (MPE_SUCCESS != mpe_fileDelete(siOOBSNSCacheLocation))
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "CACHE: Error deleting file...%s\n", siOOBSNSCacheLocation);
        }

        err = mpe_fileOpen(siOOBSNSCacheLocation, MPE_FS_OPEN_WRITE | MPE_FS_OPEN_MUST_CREATE, &fd);

        if (err == MPE_SUCCESS)
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s]  failed with %d\n", siOOBSNSCacheLocation, err);
            err = MPE_SI_INVALID_PARAMETER;
        }
    }
    if (err == MPE_SUCCESS)
    {
        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: starting writing SNS cache\n");
        uint32_t write_count = 0;
        uint32_t loop_count = 0;

        // write the source name entry list
        sn_walker = g_si_sourcename_entry;
        while (sn_walker && err == MPE_SUCCESS)
        {
            mpe_SiLangSpecificStringList *name_list[2] = {sn_walker->source_names, sn_walker->source_long_names};
            mpe_SiLangSpecificStringList *name_walker = NULL;
            int                          name_counts[2] = {0};

            if (!(sn_walker->mapped))
            {
                sn_walker = sn_walker->next;
                continue;
            }

            {
                uint8_t list_idx = 0;
                {
                    for (list_idx = 0; list_idx < 2; list_idx++)
                    {
                        uint8_t name_count = 0;
                        name_walker = name_list[list_idx];
                        while(name_walker)
                        {
                            name_count++;
                            name_walker = name_walker->next;
                        }
                        name_counts[list_idx] = name_count;
                    }

                    sn_walker->source_names        = (mpe_SiLangSpecificStringList   *)(name_counts[0]);
                    sn_walker->source_long_names   = (mpe_SiLangSpecificStringList   *)(name_counts[1]);
                }
                count = sizeof(*sn_walker);
                loop_count++;
                err = mpe_fileWrite(cache_fd, &count, (void *)sn_walker);

                sn_walker->source_names        = name_list[0];
                sn_walker->source_long_names   = name_list[1];

                if (count == sizeof(*sn_walker))
                {
                    write_count += count;
                    {
                        // now copy names
                        for (list_idx = 0; list_idx < 2; list_idx++)
                        {
                             // LEN-NAME, LEN-NAME...
                            name_walker = name_list[list_idx];
                            err = write_name_string(cache_fd, name_walker, &write_count);

                        } // For each name list
                    }
                }
                else
                {
                    MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI /persistent/si/si_table_cache failed with %d\n", err);
                    err = MPE_SI_INVALID_PARAMETER;
                    break;
                }
            }

            if (1)
            MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "AppType[%d] SourceId[%d], Name[%s] \n",
                    sn_walker->appType,  sn_walker->id,
                    (sn_walker->source_names && sn_walker->source_names->string) ? sn_walker->source_names->string : "NULL");

            sn_walker = sn_walker->next;
        } // For each sn_walker
        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "writing %s DONE writing SNS entries, %d bytes %d entries\n", siOOBSNSCacheLocation, sizeof(*sn_walker), loop_count);

        mpe_fileClose(cache_fd);
    }
    return err;
}

static mpe_Error generate_new_si_entry(mpe_SiTableEntry *input_si_entry, mpe_SiTableEntry **new_si_entry_out)
{
    mpe_SiTableEntry *new_si_entry = NULL;
    mpe_SiProgramHandle prog_handle = MPE_SI_INVALID_HANDLE;

    mpe_siGetTransportStreamEntryFromFrequencyModulation(g_frequency[input_si_entry->freq_index], g_mode[input_si_entry->mode_index], &input_si_entry->ts_handle);
    mpe_siGetProgramEntryFromTransportStreamEntry(input_si_entry->ts_handle, input_si_entry->program_number, &prog_handle);
    mpe_siGetServiceEntryFromChannelNumber(input_si_entry->virtual_channel_number, (mpe_SiServiceHandle *)(&new_si_entry));
    mpe_siSetSourceId((mpe_SiServiceHandle)new_si_entry, input_si_entry->source_id);
    mpe_siSetAppType((mpe_SiServiceHandle)new_si_entry, input_si_entry->isAppType);
    mpe_siSetAppId((mpe_SiServiceHandle)new_si_entry, input_si_entry->app_id);
    mpe_siSetActivationTime ((mpe_SiServiceHandle)new_si_entry, input_si_entry->activation_time);
    mpe_siSetChannelNumber((mpe_SiServiceHandle)new_si_entry, input_si_entry->virtual_channel_number, MPE_SI_DEFAULT_CHANNEL_NUMBER);
    mpe_siSetCDSRef((mpe_SiServiceHandle)new_si_entry, input_si_entry->freq_index);
    mpe_siSetMMSRef ((mpe_SiServiceHandle)new_si_entry, input_si_entry->mode_index);
    mpe_siSetChannelType((mpe_SiServiceHandle)new_si_entry, input_si_entry->channel_type);
    mpe_siSetVideoStandard((mpe_SiServiceHandle)new_si_entry, input_si_entry->video_standard);
    mpe_siSetServiceType((mpe_SiServiceHandle)new_si_entry, input_si_entry->service_type);
    mpe_siSetProgramNumber((mpe_SiServiceHandle)new_si_entry, input_si_entry->program_number);
    mpe_siSetTransportType((mpe_SiServiceHandle)new_si_entry, input_si_entry->transport_type);
    mpe_siSetScrambled((mpe_SiServiceHandle)new_si_entry, input_si_entry->scrambled);
    mpe_siSetServiceEntryStateMapped((mpe_SiServiceHandle)new_si_entry);
    // This field will be reconciled when updateServiceEntries is called
    new_si_entry->source_name_entry = NULL;
    new_si_entry->descriptions = input_si_entry->descriptions;
    new_si_entry->state = input_si_entry->state;
    mpeos_timeGetMillis(&(new_si_entry->ptime_service));
    *new_si_entry_out = new_si_entry;
    MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "Generating entry for isApp %d with state %d\r\n", new_si_entry->isAppType, new_si_entry->state);

     return MPE_SUCCESS;
}

/**
 * De-serialize SI data from cache file int g_si_entry. The data in g_si_entry is later updated by new tables
 * acquired during SI acquisition.
 */
mpe_Error load_si_entries_114(const char *siOOBCacheLocation, const char *siOOBCachePost114Location, const char *siOOBSNSCacheLocation)
{
    /* For 121 version, we need to load SICache into both SICACHE and SNSCACHE */
    mpe_SiTableEntry114 input_si_entry_buf;
    mpe_SiTableEntry114 *input_si_entry = &input_si_entry_buf;

    mpe_SiTableEntry121 input_si_entry_buf_121;
    mpe_SiTableEntry121 *input_si_entry_121 = &input_si_entry_buf_121;

    mpe_SiTableEntry121 *new_si_entry = NULL;
    int ret = -1;
    mpe_Error err = MPE_SUCCESS;
    mpe_File cache_fd;
    uint32_t program_number = 0;
    int num_entry_read = 0;
    uint32_t count = 0;

    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: start loading SI cache 114\r\n");
    {
        mpe_File fd;
        err = mpe_fileOpen(siOOBCacheLocation, MPE_FS_OPEN_READ, &fd);
        if ((err == MPE_SUCCESS) && (fd != MPE_EINVAL))
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed for [%s]\r\n", siOOBCacheLocation, strerror(errno));
            err = MPE_SI_INVALID_PARAMETER;
        }
    }
    if (err == MPE_SUCCESS)
    {
        int read_count = 0;
        count = sizeof(g_frequency);
        ret = mpe_fileRead(cache_fd, &count, (void *)g_frequency);
        if (ret == MPE_SUCCESS && count == sizeof(g_frequency))
        {
            read_count += count;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading FREQ to [%s] failed %s\r\n", siOOBCacheLocation, strerror(errno));
            err =  MPE_SI_INVALID_PARAMETER;
        }

        /* Assert that the first 4 bytes are all 0s */
        if (g_frequency[0] != 0x0000) {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Invalid Version detected for Cache file at [%s]\r\n", siOOBCacheLocation);
            err = MPE_SI_INVALID_PARAMETER;
            //mpe_fileDelete(siOOBCacheLocation);
        }

        if (err == MPE_SUCCESS)
        {
            count = sizeof(g_mode);
            ret = mpe_fileRead(cache_fd, &count, (void *)g_mode);
            if (ret == MPE_SUCCESS && count == sizeof(g_mode))
            {
                read_count += count;
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading MODE from [%s] failed %s\r\n", siOOBCacheLocation, strerror(errno));
                err = MPE_SI_INVALID_PARAMETER;
            }
        }

        while (err == MPE_SUCCESS)
        {
            if (err == MPE_SUCCESS && input_si_entry != NULL)
            {
                memset(input_si_entry, 0, sizeof(*input_si_entry));
                memset(input_si_entry_121, 0, sizeof(*input_si_entry_121));
                {
                    count = sizeof(*input_si_entry);
                    ret = mpe_fileRead(cache_fd, &count, (void *)input_si_entry);
                    if (ret == MPE_SUCCESS && count == sizeof(*input_si_entry))
                    {
                        num_entry_read++;
                        read_count += count;
                        /* Read description names first */
                        int list_idx = 0;
                        mpe_SiLangSpecificStringList **name_list[3] = {&input_si_entry->source_names,  &input_si_entry->source_long_names, &input_si_entry->descriptions};
                        int name_counts[3] = {(int)(input_si_entry->source_names), (int)(input_si_entry->source_long_names), (int)(input_si_entry->descriptions)};
                        for (list_idx = 0; list_idx < 3; list_idx++)
                        {
                            int i = 0;
                            int tmp_limit = name_counts[list_idx];
                            *name_list[list_idx] = NULL;
                            mpe_SiLangSpecificStringList *name_walker = NULL;
                            for (i = 0; i < tmp_limit; i++)
                            {
                                int name_len = 0;
                                unsigned char buf[4] = {0};
                                count = sizeof(buf);
                                ret = mpe_fileRead(cache_fd, &count, (void *)buf);
                                read_count += count;
                                name_len = (buf[0]) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
                                ret = mpeos_memAllocP(MPE_MEM_SI, sizeof(*name_walker), (void **)&(name_walker));
                                memset(name_walker, 0, sizeof(*name_walker));
                                if (ret == MPE_SUCCESS && name_walker)
                                {
                                    count = sizeof(name_walker->language);
                                    ret = mpe_fileRead(cache_fd, &count, (void *)name_walker->language);
                                    read_count += count;
                                    ret = mpeos_memAllocP(MPE_MEM_SI, name_len - sizeof(name_walker->language) + 1, (void **)&(name_walker->string));
                                    memset(name_walker->string, 0, name_len - sizeof(name_walker->language) + 1);

                                    count = name_len - sizeof(name_walker->language);
                                    ret = mpe_fileRead(cache_fd, &count, (void *)name_walker->string);
                                    read_count += count;
                                    name_walker->next = *name_list[list_idx];
                                    *name_list[list_idx] = name_walker;
                                }
                            }
                        }

                        /* Done reading the entry. Convert it to 121 Struct */
                        {
                            input_si_entry_121->ref_count = input_si_entry->ref_count;
                            input_si_entry_121->activation_time = input_si_entry->activation_time; // SVCT
                            input_si_entry_121->ptime_service = input_si_entry->ptime_service;
                            input_si_entry_121->ts_handle =  input_si_entry->ts_handle == MPE_SI_OOB_FREQUENCY ? MPE_SI_DSG_FREQUENCY : input_si_entry->ts_handle;
                            input_si_entry_121->program = input_si_entry->program;
                            input_si_entry_121->tuner_id= input_si_entry->tuner_id;
                            input_si_entry_121->valid= input_si_entry->valid;
                            input_si_entry_121->virtual_channel_number= input_si_entry->virtual_channel_number;
                            input_si_entry_121->isAppType= input_si_entry->isAppType;
                            input_si_entry_121->source_id= input_si_entry->source_id;
                            input_si_entry_121->app_id= input_si_entry->app_id;
                            input_si_entry_121->dsgAttached= input_si_entry->dsgAttached;
                            input_si_entry_121->dsg_attach_count= input_si_entry->dsg_attach_count;
                            input_si_entry_121->state= input_si_entry->state;
                            input_si_entry_121->channel_type= input_si_entry->channel_type;
                            input_si_entry_121->video_standard= input_si_entry->video_standard;
                            input_si_entry_121->service_type= input_si_entry->service_type;

                            //mpe_SiLangSpecificStringList *source_names;  assigned to SNScache 
                            //mpe_SiLangSpecificStringList *source_long_names;  assigned to SNScache 
                            {
                                mpe_siSourceNameEntry input_sn_entry_buf;
                                mpe_siSourceNameEntry *input_sn_entry = &input_sn_entry_buf;
                                mpe_siSourceNameEntry *new_sn_entry = NULL;
                                memset(input_sn_entry, 0, sizeof(*input_sn_entry));
                                input_sn_entry->id = (input_si_entry_121->isAppType ? input_si_entry_121->app_id : input_si_entry_121->source_id);
                                input_sn_entry->appType = (input_si_entry_121->isAppType ? true : false);
                                input_sn_entry->source_names = input_si_entry->source_names;
                                input_sn_entry->source_long_names = input_si_entry->source_long_names;
                                mpe_siGetSourceNameEntry(input_sn_entry->id, input_sn_entry->appType, &new_sn_entry, TRUE);
                                // 'Mapped' field is set when updateServiceEntries is called
                                new_sn_entry->source_names = input_sn_entry->source_names;
                                new_sn_entry->source_long_names = input_sn_entry->source_long_names;
                                if (1) {
                                    MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "SI114 AppType[%d] SourceId[%d], Name[%s]\n",
                                            new_sn_entry->appType,  new_sn_entry->id,
                                            (new_sn_entry->source_names && new_sn_entry->source_names->string) ? new_sn_entry->source_names->string : "NULL");
                                }
                            }

                            input_si_entry_121->descriptions= input_si_entry->descriptions;
                            input_si_entry_121->freq_index= input_si_entry->freq_index;
                            input_si_entry_121->mode_index= input_si_entry->mode_index;
                            input_si_entry_121->major_channel_number= input_si_entry->major_channel_number;
                            input_si_entry_121->minor_channel_number= input_si_entry->minor_channel_number;
                            //input_si_entry_121->program_number; find out below 
                            input_si_entry_121->transport_type = 0; //only mpeg2, no analog supported
                            input_si_entry_121->scrambled = 0;          //only mpeg2, no analog supported
                        }

                        {
                            if (input_si_entry_121->ts_handle == MPE_SI_DSG_FREQUENCY)
                            {
                                /* 121 defer handling of this to mpe_siUpdateServiceEntries */

                            }
                            else
                            {
                                input_si_entry_121->program_number = (uint32_t)(input_si_entry->program);
                                generate_new_si_entry(input_si_entry_121, &new_si_entry);
                            }
                        }

                        if (1)
                            MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,"\nSI114 RChannelVCN#%06d-%s#%06x--State-[%d]-Freq[%09d]-Mode[%04d]-Prog[%08d]\n",
                                    new_si_entry->virtual_channel_number,
                                    new_si_entry->isAppType ? "APPID" : "SRCID",
                                    new_si_entry->isAppType ? new_si_entry->app_id : new_si_entry->source_id,
                                    new_si_entry->state,
                                    g_frequency[new_si_entry->freq_index],
                                    g_mode[new_si_entry->mode_index],
                                    new_si_entry->program ? new_si_entry->program->program_number : 87654321);
                    }
                    else if (ret < 0)
                    {
                        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Reading [%s] failed %s\r\n", siOOBCacheLocation, strerror(errno));
                        return MPE_SI_NOT_FOUND;
                        break;
                    }
                    else
                    {
                        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Reading [%s] reaches EOF%s\r\n", siOOBCacheLocation, strerror(errno));
                        break;
                    }
                }
            }
        }
        MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "reading [%s] DONE %d bytes\r\n", siOOBCacheLocation, read_count);
        mpe_fileClose(cache_fd);

        mpe_siUpdateServiceEntries();


        if (err == MPE_SUCCESS)
        {
            if (num_entry_read > 0)
            {
                g_si_state = SI_FULLY_ACQUIRED;
                g_SITPConditionSet = TRUE;
                MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "SI_FULLY_ACQUIRED FULLY via 114 CACHING\r\n");
                get_frequency_range(&g_frequency[1], MAX_FREQUENCIES, &g_minFrequency, &g_maxFrequency);
                /* Now cache file to Post114 format */
                if (0 && siOOBCachePost114Location && siOOBSNSCacheLocation)
                {
                    MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,"Writing 114 Cache to 121 Format NOW\n");
                    // Cache NTT-SNS entries
                    if (cache_sns_entries(siOOBSNSCacheLocation) == MPE_SUCCESS)
                    {
                        // Cache SVCT entries
                        if(cache_si_entries(siOOBCachePost114Location) == MPE_SUCCESS)
                        {
                            MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,"NEW CACHING DONE.\n");
                        }
                    }
                }
            }
            else
            {
                err = MPE_SI_NOT_FOUND;
            }
        }
    }
    else
    {
    }
    // Reconcile service entry's program handle, transport stream handle and service names etc.
    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: done loading 114 cache %d\r\n", num_entry_read);
    return err;
}

mpe_Error load_si_entries_Post114(const char *siOOBCacheLocation)
{
    mpe_Error err = MPE_SUCCESS;
    mpe_SiTableEntry input_si_entry_buf;
    mpe_SiTableEntry *input_si_entry = &input_si_entry_buf;
    mpe_SiTableEntry *new_si_entry = NULL;
    int ret = -1;
    mpe_File cache_fd;
    uint32_t program_number = 0;
    uint32_t num_entry_read = 0;
    uint32_t count = 0;

    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: start loading cache 121\n");
    {
        mpe_File fd;
        err = mpe_fileOpen(siOOBCacheLocation, MPE_FS_OPEN_READ, &fd);
        if ((err == MPE_SUCCESS) && (fd != MPE_EINVAL))
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", siOOBCacheLocation, err);
            return MPE_SI_INVALID_PARAMETER;
        }
    }
    // SI DB write lock is acquired by the caller!
    if (err == MPE_SUCCESS)
    {
        uint32_t read_count = 0;
        uint32_t version;

        // Read header version
        count = sizeof(version);
        err = mpe_fileRead(cache_fd, &count, (void *)&version);

        if (err == MPE_SUCCESS && (count == sizeof(version)))
        {
            read_count += count;
            if (version != MPE_SI_CACHE_FILE_VERSION)
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading Version failed with %x, expected %x\n", version, MPE_SI_CACHE_FILE_VERSION);
                err =  MPE_SI_INVALID_PARAMETER;
            }
            else
            {
                MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Read version 0x%x\n", version);
            }
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading Version [%s] failed with err:%d count:%d version:0x%x\n", siOOBCacheLocation, err, count, version);
            err =  MPE_SI_INVALID_PARAMETER;
        }
        if (err == MPE_SUCCESS)
        {
            count = sizeof(g_frequency);
            ret = mpe_fileRead(cache_fd, &count, (void *)g_frequency);
            if (ret == MPE_SUCCESS && (count == sizeof(g_frequency)))
            {
                read_count += count;
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading FREQ to [%s] failed with %d\n", siOOBCacheLocation, err);
                err =  MPE_SI_INVALID_PARAMETER;
            }
        }

        if (err == MPE_SUCCESS)
        {
            count = sizeof(g_mode);
            ret = mpe_fileRead(cache_fd, &count, (void *)g_mode);
            if (ret == MPE_SUCCESS && count == sizeof(g_mode))
            {
                read_count += count;
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading MODE from [%s] failed with %d\n", siOOBCacheLocation, err);
                err = MPE_SI_INVALID_PARAMETER;
            }
        }

        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Reading SVCT entries..\n");
        // Read SVCT entries
        while (err == MPE_SUCCESS)
        {
            // Now read the table entries
            if (err == MPE_SUCCESS && input_si_entry != NULL)
            {
                memset(input_si_entry, 0, sizeof(*input_si_entry));
                {
                    count = sizeof(*input_si_entry);
                    ret = mpe_fileRead(cache_fd, &count, (void *)input_si_entry);
                    if (ret == MPE_SUCCESS && count == sizeof(*input_si_entry))
                    {
                        num_entry_read++;
                        read_count += count;
                        /* Read description names first */
                        mpe_SiLangSpecificStringList **name_list = &input_si_entry->descriptions;
                        int name_count = (int)(input_si_entry->descriptions);
                        {
                            int i = 0;
                            mpe_SiLangSpecificStringList *name_walker = NULL;
                            for (i = 0; i < name_count; i++)
                            {
                                unsigned char buf[4] = {0};

                                count = sizeof(buf);
                                ret = mpe_fileRead(cache_fd, &count, (void *)buf);

                                read_count += count;
                                ret = mpeos_memAllocP(MPE_MEM_SI, sizeof(*name_walker), (void **)&(name_walker));
                                memset(name_walker, 0, sizeof(*name_walker));
                                if (ret == MPE_SUCCESS && name_walker)
                                {
                                    int name_len = (buf[0]) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
                                    ret = read_name_string(cache_fd, name_walker, &read_count, name_len);
                                    *name_list = name_walker;
                                }
                            }
                        }

                        {
                            program_number = (uint32_t)(input_si_entry->program_number);
                            generate_new_si_entry(input_si_entry, &new_si_entry);
                        }

                        if (1)
                        MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "Post114 RChannelVCN#%06d-%s#%06x-State-[%d]-Freq[%09d]-Mode[%04d]-Prog[%08d]\n",
                                new_si_entry->virtual_channel_number,
                                new_si_entry->isAppType ? "APPID" : "SRCID",
                                new_si_entry->isAppType ? new_si_entry->app_id : new_si_entry->source_id,
                                new_si_entry->state,
                                g_frequency[new_si_entry->freq_index],
                                g_mode[new_si_entry->mode_index],
                                new_si_entry->program_number);
                    }
                    else
                    {
                        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Reading [%s] reaches EOF with %d\n", siOOBCacheLocation, err);
                        break;
                    }
                }
            }
        }
        MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "reading [%s] DONE %d bytes\n", siOOBCacheLocation, read_count);
        mpe_fileClose(cache_fd);
        if (err == MPE_SUCCESS)
        {
            if (num_entry_read > 0)
            {
                g_si_state = SI_FULLY_ACQUIRED;
                g_SITPConditionSet = TRUE;
                MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "SI_FULLY_ACQUIRED FULLY via 121 CACHING\n");
                get_frequency_range(&g_frequency[1], 255, &g_minFrequency, &g_maxFrequency);
            }
            else
            {
                err = MPE_SI_NOT_FOUND;
            }
        }
    }

    // Reconcile service entry's program handle, transport stream handle and service names etc.
    mpe_siUpdateServiceEntries();

    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: done loading 121 SI cache %d\n", num_entry_read);

    return err;
}

mpe_Error load_sns_entries(const char *siOOBSNSCacheLocation)
{
    /*TODO: Recognize illegal SNS files */
    mpe_Error err = MPE_SUCCESS;
    mpe_siSourceNameEntry input_sn_entry_buf;
    mpe_siSourceNameEntry *input_sn_entry = &input_sn_entry_buf;
    mpe_siSourceNameEntry *new_sn_entry = NULL;

    int ret = -1;
    mpe_File cache_fd;
    uint32_t count = 0;

    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: start loading SNS cache\n");
    {
        mpe_File fd;
        err = mpe_fileOpen(siOOBSNSCacheLocation, MPE_FS_OPEN_READ, &fd);
        if (err == MPE_SUCCESS && fd != 0)
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", siOOBSNSCacheLocation, err);
            return MPE_SI_INVALID_PARAMETER;
        }
    }

    // SI DB write lock is acquired by the caller!
    if (err == MPE_SUCCESS)
    {
        uint32_t read_count = 0;

        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Reading SNS entries..\n");
        // Read SNS entries
        while (err == MPE_SUCCESS)
        {
            if (err == MPE_SUCCESS && input_sn_entry != NULL)
            {
                // Read source name list first
                memset(input_sn_entry, 0, sizeof(*input_sn_entry));
                {
                    count = sizeof(*input_sn_entry);
                    ret = mpe_fileRead(cache_fd, &count, (void *)input_sn_entry);
                    if (ret == MPE_SUCCESS && count == sizeof(*input_sn_entry))
                    {
                        read_count += count;
                        /* Read names */
                        int list_idx = 0;
                        mpe_SiLangSpecificStringList **name_list[2] = {&input_sn_entry->source_names,  &input_sn_entry->source_long_names};
                        int name_counts[2] = {(int)(input_sn_entry->source_names), (int)(input_sn_entry->source_long_names)};
                        for (list_idx = 0; list_idx < 2; list_idx++)
                        {
                            int i = 0;
                            int tmp_limit = name_counts[list_idx];
                            *name_list[list_idx] = NULL;
                            mpe_SiLangSpecificStringList *name_walker = NULL;
                            for (i = 0; i < tmp_limit; i++)
                            {
                                unsigned char buf[4] = {0};

                                count = sizeof(buf);
                                ret = mpe_fileRead(cache_fd, &count, (void *)buf);
                                if (ret == MPE_SUCCESS && count == sizeof(buf))
                                {
                                    read_count += count;
                                    ret = mpeos_memAllocP(MPE_MEM_SI, sizeof(*name_walker), (void **)&(name_walker));
                                    memset(name_walker, 0, sizeof(*name_walker));
                                    if (ret == MPE_SUCCESS && name_walker)
                                    {
                                        int name_len = (buf[0]) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
                                        ret = read_name_string(cache_fd, name_walker, &read_count, name_len);
                                        name_walker->next = *name_list[list_idx];
                                        *name_list[list_idx] = name_walker;
                                    }
                                    else {
                                        break;
                                    }
                                }
                                else 
                                {
                                    break;
                                }
                            }
                        }

                        {
                            mpe_siGetSourceNameEntry(input_sn_entry->id, input_sn_entry->appType, &new_sn_entry, TRUE);
                            // 'Mapped' field is set when updateServiceEntries is called
                            new_sn_entry->source_names = input_sn_entry->source_names;
                            new_sn_entry->source_long_names = input_sn_entry->source_long_names;
                        }

                        if (1)
                        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "AppType[%d] SourceId[%d], Name[%s]\n",
                                new_sn_entry->appType,  new_sn_entry->id,
                                (new_sn_entry->source_names && new_sn_entry->source_names->string) ? new_sn_entry->source_names->string : "NULL");

                    }
                    else
                    {
                        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Reading reaches EOF with %d\n", err);
                        break;
                    }
                }
            }

        }

        MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "reading DONE %d bytes\n", read_count);
        mpe_fileClose(cache_fd);
    }
    return err;
}


static mpe_Error write_name_string(mpe_File cache_fd, mpe_SiLangSpecificStringList *name, uint32_t *write_count)
{
    mpe_Error err = MPE_SUCCESS;
    int name_len = 0;
    uint32_t count = 0;
    mpe_SiLangSpecificStringList *name_walker = name;
    while(name_walker)
    {
        unsigned char buf[4] = {0};
        /*
         * 0) write total_len = 1)+2)+3)
         * 1)write lanaguage[4]
         * 2)write string_len
         * 3)write string
         */
        name_len = sizeof(name_walker->language) + (name_walker->string ? (strlen(name_walker->string) + sizeof(uint32_t)) : 0);
        buf[0] = name_len     & 0xFF;
        buf[1] =(name_len>>8) & 0xFF;
        buf[2] =(name_len>>16)& 0xFF;
        buf[3] =(name_len>>24)& 0xFF;

        count = sizeof(buf);
        err = mpe_fileWrite(cache_fd, &count, (void *)&buf);
        if (count == sizeof(buf))
        {
            *write_count += count;

            count = sizeof(name_walker->language);

            err = mpe_fileWrite(cache_fd, &count, (void *)name_walker->language);

            if (count == sizeof(name_walker->language))
            {
                *write_count += count;

                if (name_walker->string)
                {
                    uint32_t size;
                    size = strlen(name_walker->string);
                    count = sizeof(uint32_t);

                    err = mpe_fileWrite(cache_fd, &count, (void *)&size);
                    if (count == sizeof(uint32_t))
                    {
                        *write_count += count;
                        if (size > 0)
                        {
                          count = strlen(name_walker->string);

                          err = mpe_fileWrite(cache_fd, &count, (void *)name_walker->string);
                          if (count != size)
                          {
                            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI name failed with %d\n", err);
                            err = MPE_SI_INVALID_PARAMETER;
                          }
                        }
                    }
                    else
                    {
                        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI name length failed with %d\n", err);
                        err = MPE_SI_INVALID_PARAMETER;
                    }
                }
            }
            else
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI name language failed with %d\n", err);
                err = MPE_SI_INVALID_PARAMETER;
            }
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "writing SI name total failed with %d\n", err);
            err = MPE_SI_INVALID_PARAMETER;
        }
        name_walker = name_walker->next;
    } // For each name

    return err;
}

static mpe_Error read_name_string(mpe_File cache_fd, mpe_SiLangSpecificStringList *name, uint32_t *read_count, int total_bytes)
{
    uint32_t count = 0;
    mpe_Error ret = MPE_SUCCESS;
    mpe_SiLangSpecificStringList *name_walker = name;

    if (name_walker && total_bytes > 0)
    {
        uint32_t size;
        count = sizeof(name_walker->language);
        ret = mpe_fileRead(cache_fd, &count, (void *)name_walker->language);

        if (ret == MPE_SUCCESS && count == sizeof(name_walker->language)) {
            *read_count += count;
            total_bytes -= count;

            if (total_bytes > 0) {

                count = sizeof(uint32_t);
                ret = mpe_fileRead(cache_fd, &count, (void *)&size);
                if (ret == MPE_SUCCESS && count == sizeof(uint32_t)) {
                    *read_count += count;
                    total_bytes -= count;
                    ret = mpeos_memAllocP(MPE_MEM_SI, size+1, (void **)&(name_walker->string));
                    memset(name_walker->string, 0, size + 1);
                    count = size;

                    ret = mpe_fileRead(cache_fd, &count, (void *)name_walker->string);
                    *read_count += count;
                    total_bytes -= count;
                }
            }
        }
    }

    if (total_bytes != 0)  {
        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "ERROR: totalBytes %d != 0 \r\n", total_bytes);
        printf("ERROR: totalBytes %d != 0 \r\n", total_bytes);
    }

    return ret;
}

void dump_si_cache()
{
	mpe_SiTableEntry *walker =  g_si_entry;

    printf("\n");
    while (walker)
    {
        printf("RChannelVCN#%06d-%s#%06x-Name[%s]-State-[%d]-Freq[%09d]-Mode[%04d]-Prog[%08d]\n",
                walker->virtual_channel_number,
                walker->isAppType ? "APPID" : "SRCID",
                walker->isAppType ? walker->app_id : walker->source_id,
                walker->source_name_entry ? (walker->source_name_entry->source_names ? walker->source_name_entry->source_names->string : NULL) : NULL,
                walker->state,
                g_frequency[walker->freq_index],
                g_mode[walker->mode_index],
                walker->program_number);
        walker = walker->next;
    }
}

/**
 * Function to return pointer to Linked List containing entries for VCN number, source id,
 * frequency, program number. 
 **/

CHANNEL_MAP_ENTRY* dump_si_cache_diagnostics()
{
    mpe_SiTableEntry *walker =  g_si_entry;
    CHANNEL_MAP_ENTRY  *current = NULL;
    while (walker)
    {
        if( NULL == channel_map_list_for_diag )
        {
            channel_map_list_for_diag = (CHANNEL_MAP_ENTRY*) malloc(sizeof(CHANNEL_MAP_ENTRY));
            if (channel_map_list_for_diag) {
		    channel_map_list_for_diag->virtual_channel_number = walker->virtual_channel_number ;
		    channel_map_list_for_diag->source_id = walker->isAppType ? walker->app_id : walker->source_id ;
		    channel_map_list_for_diag->freq_index = g_frequency[walker->freq_index];
		    channel_map_list_for_diag->program_number = walker->program_number;
		    channel_map_list_for_diag->next = NULL;
                    current = channel_map_list_for_diag ;
            }

        } else {
            CHANNEL_MAP_ENTRY *temp = (CHANNEL_MAP_ENTRY *) malloc(sizeof(CHANNEL_MAP_ENTRY));
            if (temp) 
            {
		    temp->virtual_channel_number = walker->virtual_channel_number ;
		    temp->source_id = walker->isAppType ? walker->app_id : walker->source_id ;
		    temp->freq_index = g_frequency[walker->freq_index] ;
		    temp->program_number = walker->program_number;
                    temp->next  = NULL;
		    current->next = temp;
		    current = temp;
            }
        }
        walker = walker->next;
    }
    return channel_map_list_for_diag;
}

void freeEntryList( )
{
    while(channel_map_list_for_diag) 
    {
        CHANNEL_MAP_ENTRY *temp = channel_map_list_for_diag->next;
        free(channel_map_list_for_diag);
        channel_map_list_for_diag = temp;
    }
}


/**
 * The <i>si_getFileSize()</i> function will get the size the specified file.
 *
 * @param fileName Is a pointer for the name of the file
 * @param size Is the size to file to be checked.
 * @return The size of the given file. -1 if the file not present or any other failure.
 *          is returned.
 */
unsigned int si_getFileSize(const char * location, unsigned int *size)
{
    struct stat buff;
    if (0 == stat(location, &buff)) 
    {   
        *size = (unsigned int) buff.st_size;
        return *size;
    }       
    return -1;
}

mpe_Error write_crc (const char * pSICacheFileName, unsigned int crcValue)
{
    mpe_Error err = MPE_SUCCESS;
    uint32_t sizeOfSICache = 0;
    unsigned char *pActualData = NULL;
	uint32_t readDataSize = 0;
    mpe_File cache_fd;
    mpe_File fd;
    
    si_getFileSize(pSICacheFileName, &sizeOfSICache);

    if (sizeOfSICache > 0)
    {
        err = mpeos_memAllocP(MPE_MEM_SI, sizeOfSICache, (void**) &pActualData);
    }

    if ((err == MPE_SUCCESS) && (pActualData != NULL))
    {
        err = mpe_fileOpen(pSICacheFileName, MPE_FS_OPEN_WRITE | MPE_FS_OPEN_MUST_CREATE, &fd);
        if (err == MPE_SUCCESS)
        {
            cache_fd = fd;
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s]  failed with %d\n", pSICacheFileName, err);
            err = MPE_SI_INVALID_PARAMETER;
        }
	    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"CACHE: starting writing cache\n");
	    if (err == MPE_SUCCESS)
	    {
			readDataSize = sizeOfSICache;
/*			err = mpe_fileRead(cache_fd, &readDataSize, pActualData);
			if (err == MPE_SUCCESS)
			{
                // The file pointer is moved bcoz of read operations, so close and reopen the file to write the data again.
            mpe_fileClose(cache_fd);
        err = mpe_fileOpen(pSICacheFileName, MPE_FS_OPEN_WRITE | MPE_FS_OPEN_MUST_CREATE, &fd);
	        	err = mpe_fileWrite(cache_fd, &readDataSize, (void *)pActualData);

	        	if (err == MPE_SUCCESS)
	        	{
	        	    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Original Data Written once again. Now CRC to be written.");
	        	}
	        	else
	        	{
	            	MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Writing original data failed\n");
	            	err = MPE_SI_INVALID_PARAMETER;
	        	}
			}*/
			if(lseek (cache_fd, readDataSize, SEEK_SET) < 0)
			{
				printf("lseek failed in %s\n",__func__);
				err = MPE_EINVAL;
			}	
			if (err == MPE_SUCCESS)
			{
                /* Write the CRC value */
				uint32_t crcValueReceived = crcValue;
				readDataSize = sizeof(crcValue);
                printf ("Received for writting CRC is [%u]\n", crcValue);
 	       		err = mpe_fileWrite(cache_fd, &readDataSize, (void *)&crcValueReceived);
			}

            /* Close the file */
            mpe_fileClose(cache_fd);
		}
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Failed to open a file");
        }
        mpeos_memFreeP (MPE_MEM_SI, pActualData);
    }
    else
    {
        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Failed to allocate memory");
    }

    return err;
}

/**
 * The <i>write_crc_for_si_and_sns_cache()</i> function will verify the
 * CRC value that is written in the size with calculated CRC.
 *
 * @param pSICache Is a pointer for the name of the SI Cache file
 * @param pSISNSCache Is a pointer for the name of the SISNS Cache file
 * @return The MPE error code if the create fails, otherwise <i>MPE_SUCCESS<i/>
 *          is returned.
 */
mpe_Error write_crc_for_si_and_sns_cache(const char* pSICache, const char* pSISNSCache)
{
    mpe_Error err = MPE_SUCCESS;
    mpe_File fdSNSCache = 0;
    mpe_File fdSICache = 0;
    unsigned char *pCRCData = NULL;
    uint32_t sizeOfSICache = 0;
    uint32_t sizeOfSISNSCache = 0;
    uint32_t crcFileSize = 0;
    uint32_t readSNSSize = 0;
    uint32_t readSISize = 0;
    uint32_t crcValue = 0xFFFFFFFF;
    uint32_t crcRead = 0xFFFFFFFF;

    /* Init should be done only once.. The main application will do init in actual product. in this test app we must call */
    //init_mpeg2_crc();

    si_getFileSize(pSICache, &sizeOfSICache);
    si_getFileSize(pSISNSCache, &sizeOfSISNSCache);

    readSNSSize = sizeOfSISNSCache;
    readSISize = sizeOfSICache;

    crcFileSize = sizeOfSISNSCache + sizeOfSICache;
    err = mpeos_memAllocP(MPE_MEM_SI, crcFileSize,(void**) &pCRCData);
    if (err == MPE_SUCCESS && pCRCData != NULL)
    {

        err = mpe_fileOpen(pSISNSCache, MPE_FS_OPEN_READ, &fdSNSCache);
        if ((err == MPE_SUCCESS) && (fdSNSCache != MPE_EINVAL))
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] success\n", pSISNSCache);
            err = mpe_fileRead(fdSNSCache, &readSNSSize, pCRCData);
            if ((err != MPE_SUCCESS))
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Read less than the size of the file\n");
            }
            mpe_fileClose(fdSNSCache);
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", pSISNSCache, err);
            mpeos_memFreeP (MPE_MEM_SI, pCRCData);
            return err;
        }

        /* Read the SICache file, even if readSNSSize != sizeOfSISNSCache */
        err = mpe_fileOpen(pSICache, MPE_FS_OPEN_READ, &fdSICache);
        if ((err == MPE_SUCCESS) && (fdSICache != MPE_EINVAL))
        {
            /* Read from SI cache file. the buffer shd be offsetted to SNScache size */
            err = mpe_fileRead(fdSICache, &readSISize, (pCRCData + readSNSSize));
            if ((err != MPE_SUCCESS))
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Read less than the size of the file\n");
            }
            mpe_fileClose(fdSICache);
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", pSICache, err);
            mpeos_memFreeP (MPE_MEM_SI, pCRCData);
            return err;
        }

        /* Find the CRC and write it at the end of SICache file */
        readSISize = sizeof(crcValue);
        crcValue = calc_mpeg2_crc (pCRCData, crcFileSize);
        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Calculated CRC is [%u]\n", crcValue);
        printf ("Calculated CRC is [%u]\n", crcValue);
        mpeos_memFreeP (MPE_MEM_SI, pCRCData);

		return write_crc(pSICache, crcValue);
    }
    return err;
}

/**
 * The <i>verify_crc_for_si_and_sns_cache()</i> function will verify the
 * CRC value that is written in the size with calculated CRC.
 *
 * @param pSICache Is a pointer for the name of the SI Cache file
 * @param pSISNSCache Is a pointer for the name of the SISNS Cache file
 * @param pStatus Is the result of CRC comparison.
 * @return The MPE error code if the create fails, otherwise <i>MPE_SUCCESS<i/>
 *          is returned.
 */
mpe_Error verify_crc_for_si_and_sns_cache(const char* pSICache, const char* pSISNSCache, int *pStatus)
{
    mpe_Error err = MPE_SUCCESS;
    mpe_File fdSNSCache = 0;
    mpe_File fdSICache = 0;
    unsigned char *pCRCData = NULL;
    uint32_t sizeOfSICache = 0;
    uint32_t sizeOfSISNSCache = 0;
    uint32_t crcFileSize = 0;
    uint32_t readSNSSize = 0;
    uint32_t readSISize = 0;
    uint32_t crcValue = 0xFFFFFFFF;
    uint32_t crcRead = 0xFFFFFFFF;

    *pStatus = 0;
    /* Init should be done only once.. The main application will do init in actual product. in this test app we must call */
    //init_mpeg2_crc();

    si_getFileSize(pSICache, &sizeOfSICache);
    si_getFileSize(pSISNSCache, &sizeOfSISNSCache);

    readSNSSize = sizeOfSISNSCache;
    readSISize = sizeOfSICache;

    crcFileSize = sizeOfSISNSCache + sizeOfSICache;
    printf("Recalculating cache with size %d %d\r\n", sizeOfSISNSCache , sizeOfSICache);
    err = mpeos_memAllocP(MPE_MEM_SI, crcFileSize,(void**) &pCRCData);
    if (err == MPE_SUCCESS && pCRCData != NULL)
    {

        err = mpe_fileOpen(pSISNSCache, MPE_FS_OPEN_READ, &fdSNSCache);
        if ((err == MPE_SUCCESS) && (fdSNSCache != MPE_EINVAL))
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] success\n", pSISNSCache);
            err = mpe_fileRead(fdSNSCache, &readSNSSize, pCRCData);
            if ((err != MPE_SUCCESS))
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Read less than the size of the file\n");
            }
            printf("SISNS Cache read with size %d\r\n", readSNSSize);
            mpe_fileClose(fdSNSCache);
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", pSISNSCache, err);
            mpeos_memFreeP (MPE_MEM_SI, pCRCData);
            return err;
        }

        /* Read the SICache file, even if readSNSSize != sizeOfSISNSCache */
        err = mpe_fileOpen(pSICache, MPE_FS_OPEN_READ, &fdSICache);
        if ((err == MPE_SUCCESS) && (fdSICache != MPE_EINVAL))
        {
            /* Read from SI cache file. the buffer shd be offsetted to SNScache size */
            err = mpe_fileRead(fdSICache, &readSISize, (pCRCData + readSNSSize));
            printf("SICache Cache read with size %d\r\n", readSISize);
            if ((err != MPE_SUCCESS))
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "Read less than the size of the file\n");
            }
            mpe_fileClose(fdSICache);
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", pSICache, err);
            mpeos_memFreeP (MPE_MEM_SI, pCRCData);
            return err;
        }

        /* Find the CRC and write it at the end of SICache file */
        readSISize = sizeof(crcValue);
        crcValue = calc_mpeg2_crc (pCRCData, (crcFileSize - readSISize));
        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Calculated CRC is [%u]\n", crcValue);
        
        memcpy (&crcRead, &pCRCData[crcFileSize - readSISize], readSISize);
        MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "CRC Read from file [%u]\n", crcRead);

        if(crcValue == crcRead)
        {
                *pStatus = 1;
                MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "CRC Matching.....\n");
        }
        mpeos_memFreeP (MPE_MEM_SI, pCRCData);
    }
    return err;
}

/**
 * The <i>verify_version_and_crc()</i> function will verify the version of cache and
 * CRC value that is written in the size with calculated CRC.
 *
 * @param siOOBCacheLocation Is a pointer for the name of the SI Cache file
 * @param siOOBSNSCacheLocation Is a pointer for the name of the SISNS Cache file
 * @return The result of verification of version and crc. 0 when fails and 1 when success is returned.
 */
int verify_version_and_crc (const char *siOOBCacheLocation, const char *siOOBSNSCacheLocation)
{
    mpe_File cache_fd;
    mpe_Error err = MPE_SUCCESS;
    int proceed = 0;
    int isCRCMatch = 0;

    mpe_File fd;
    err = mpe_fileOpen(siOOBCacheLocation, MPE_FS_OPEN_READ, &fd);
    if (err == MPE_SUCCESS && fd != 0)
    {
        cache_fd = fd;
    }
    else
    {
        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "open [%s] failed with %d\n", siOOBCacheLocation, err);
    }
    
    // SI DB write lock is acquired by the caller!
    if (err == MPE_SUCCESS)
    {
        uint32_t read_count = 0;
        uint32_t version;
        uint32_t count = 0;

        // Read header version
        count = sizeof(version);
        err = mpe_fileRead(cache_fd, &count, (void *)&version);

        if (err == MPE_SUCCESS && (count == sizeof(version)))
        {
            read_count += count;
            if (version < MPE_SI_CACHE_FILE_VERSION)
            {
                MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading Version failed with %x, expected %x\n", version, MPE_SI_CACHE_FILE_VERSION);
                proceed = 0;
            }
            else
            {
                MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "Read version 0x%x\n", version);
                proceed = 1;
            }
        }
        else
        {
            MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "reading Version [%s] failed with err:%d count:%d version:0x%x\n", siOOBCacheLocation, err, count, version);
        }
 
        mpe_fileClose(cache_fd);
    }

    /* check CRC only when version matches.. */
    if ((err == MPE_SUCCESS) && (proceed == 1))
    {
        err = verify_crc_for_si_and_sns_cache (siOOBCacheLocation, siOOBSNSCacheLocation, &isCRCMatch);
    }
    
    if ((err != MPE_SUCCESS) || (isCRCMatch == 0) || (proceed == 0))
    {
        return 0;
    }
   
    /* Return SUCCESS, so that the calling method can proceed */
    return 1;
}

