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
/* x86 */

#define INIT_TABLE_VERSION                32
#define MPE_EMUTEX  -1
#define MPE_SI_CACHE_FILE_VERSION 0x101u

#define MPE_FS_OPEN_READ                (0x0001)
#define MPE_FS_OPEN_WRITE               (0x0002)
#define MPE_FS_OPEN_READWRITE           (0x0004)
#define MPE_FS_OPEN_CAN_CREATE          (0x0008)
#define MPE_FS_OPEN_MUST_CREATE         (0x0010)
#define MPE_FS_OPEN_TRUNCATE            (0x0020)
#define MPE_FS_OPEN_APPEND              (0x0040)
#define MPE_FS_OPEN_NOLINKS             (0x0080)
#define MPE_FS_OPEN_STREAMONLY          (0x0100)
#define MPE_FS_OPEN_CACHEONLY           (0x0200)

#define        SIMODULE                             "SITP_SI"// module name

#define        MPEG_2_TRANSPORT                     0
#define        NON_MPEG_2_TRANSPORT                 1


uint32_t g_numberOfServices;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0
#define mpe_mutexNew(p) pthread_mutex_init(p, NULL)
#define mpe_mutexAcquire(p) pthread_mutex_lock(p)
#define mpe_mutexRelease(p) pthread_mutex_unlock(p)
typedef int mpe_File;
typedef struct link LINK;
struct link
{
    LINK * nextp;             /* List foreward pointer */
    LINK * prevp;             /* List back pointer */
    struct linkhdr * headerp; /* Pointer to list link is in */
    void * datap;             /* Pointer to arbitrary data */
};

/*
 * Format of a list header.
 */
typedef struct linkhdr LINKHD;
struct linkhdr
{
    LINK * headp;         /* Pointer to first link */
    LINK * tailp;         /* pointer to last link */
    unsigned long nlinks; /* Number of links in the list */
};

LINKHD      * llist_create(void);
void            llist_free(LINKHD *headerp);
LINK        * llist_mklink(void * datap);
void            llist_freelink(LINK *linkp);
void            llist_append(LINKHD *headerp, LINK *linkp);
LINK        * llist_first(LINKHD * headerp);
LINK        * llist_after(LINK * linkp);
LINK        * llist_last(LINKHD * headerp);
void          llist_rmlink(LINK * linkp);
void          llist_rmfirst(LINKHD * headerp);
void          llist_rmafter(LINK * afterp);
void          llist_rmlast(LINKHD * headerp);
LINK        * llist_setdata(LINK * linkp, void * datap);
void        * llist_getdata(LINK * linkp);
unsigned long llist_cnt(LINKHD * headerp);
LINK        * llist_linkof(LINKHD * headerp, void * datap);
LINK        * llist_nextlinkof(void * datap, LINK * afterp);
LINK        * llist_getNodeAtIndex(LINKHD * headerp, unsigned long index);
LINKHD      * llist_hdrof(LINK * lp);

typedef LINKHD * ListSI;

typedef long long os_TimeMillis;
typedef os_TimeMillis mpe_TimeMillis;
typedef uint32_t mpe_SiTransportStreamHandle;
typedef int mpe_Bool;
typedef enum mpe_SiEntryState
{
    SIENTRY_UNSPECIFIED = 0,
    SIENTRY_PRESENT_IN_DCM     = 0x01,        // Present in DCM
    SIENTRY_DEFINED            = 0x02,        // Defined in DCM
    SIENTRY_MAPPED             = 0x04,        // Mapped in VCM
    SIENTRY_DEFINED_MAPPED     = SIENTRY_PRESENT_IN_DCM|SIENTRY_DEFINED|SIENTRY_MAPPED, // Marked defined in SVCT-DCM, mapped in SVCT-VCM
    SIENTRY_DEFINED_UNMAPPED   = SIENTRY_PRESENT_IN_DCM|SIENTRY_DEFINED,  // Marked defined in SVCT-DCM, not mapped in SVCT-VCM
    SIENTRY_UNDEFINED_MAPPED   = SIENTRY_PRESENT_IN_DCM|SIENTRY_MAPPED,   // Marked undefined in SVCT-DCM, mapped in SVCT-VCM
    SIENTRY_UNDEFINED_UNMAPPED = SIENTRY_PRESENT_IN_DCM                  // Marked undefined SVCT-DCM, not mapped in SVCT-VCM
} mpe_SiEntryState;
typedef enum mpe_SiChannelType
{
    CHANNEL_TYPE_NORMAL      = 0x00,
    CHANNEL_TYPE_HIDDEN      = 0x01,
    CHANNEL_TYPE_RESERVED_2  = 0x02,
    CHANNEL_TYPE_RESERVED_3  = 0x03,
    CHANNEL_TYPE_RESERVED_4  = 0x04,
    CHANNEL_TYPE_RESERVED_5  = 0x05,
    CHANNEL_TYPE_RESERVED_6  = 0x06,
    CHANNEL_TYPE_RESERVED_7  = 0x07,
    CHANNEL_TYPE_RESERVED_8  = 0x08,
    CHANNEL_TYPE_RESERVED_9  = 0x09,
    CHANNEL_TYPE_RESERVED_10 = 0x0A,
    CHANNEL_TYPE_RESERVED_11 = 0x0B,
    CHANNEL_TYPE_RESERVED_12 = 0x0C,
    CHANNEL_TYPE_RESERVED_13 = 0x0D,
    CHANNEL_TYPE_RESERVED_14 = 0x0E,
    CHANNEL_TYPE_RESERVED_15 = 0x0F,
} mpe_SiChannelType;
typedef enum mpe_SiVideoStandard
{
    VIDEO_STANDARD_NTSC = 0x00,
    VIDEO_STANDARD_PAL625 = 0x01,
    VIDEO_STANDARD_PAL525 = 0x02,
    VIDEO_STANDARD_SECAM = 0x03,
    VIDEO_STANDARD_MAC = 0x04,
    VIDEO_STANDARD_UNKNOWN = 0x05
} mpe_SiVideoStandard;
typedef enum mpe_SiServiceType
{
    MPE_SI_SERVICE_TYPE_UNKNOWN = 0x00,
    MPE_SI_SERVICE_ANALOG_TV = 0x01,
    MPE_SI_SERVICE_DIGITAL_TV = 0x02,
    MPE_SI_SERVICE_DIGITAL_RADIO = 0x03,
    MPE_SI_SERVICE_DATA_BROADCAST = 0x04,
    MPE_SI_SERVICE_NVOD_REFERENCE = 0x05,
    MPE_SI_SERVICE_NVOD_TIME_SHIFTED = 0x06,
    MPE_SI_SERVICE_ANALOG_RADIO = 0x07,
    MPE_SI_SERVICE_DATA_APPLICATION = 0x08
} mpe_SiServiceType;
typedef struct mpe_SiLangSpecificStringList
{
    char       language[4];      /* 3-character ISO 639 language code + null terminator */
    char      *string;
    struct mpe_SiLangSpecificStringList  *next;
} mpe_SiLangSpecificStringList;

typedef enum mpe_SiPMTStatus
{
    PMT_NOT_AVAILABLE = 0x00,
    PMT_AVAILABLE_SHORTLY = 0x01,
    PMT_AVAILABLE = 0x02
} mpe_SiPMTStatus;
typedef struct mpe_SiPMTReference
{
    uint32_t                            pcr_pid;
    uint32_t                            number_outer_desc;
    uint32_t                            outer_descriptors_ref;
    uint32_t                            number_elem_streams;
    uint32_t                            elementary_streams_ref;
} mpe_SiPMTReference;

typedef enum mpe_SiElemStreamType
{
    MPE_SI_ELEM_MPEG_1_VIDEO = 0x01,
    MPE_SI_ELEM_MPEG_2_VIDEO = 0x02,
    MPE_SI_ELEM_MPEG_1_AUDIO = 0x03,
    MPE_SI_ELEM_MPEG_2_AUDIO = 0x04,
    MPE_SI_ELEM_MPEG_PRIVATE_SECTION = 0x05,
    MPE_SI_ELEM_MPEG_PRIVATE_DATA = 0x06,
    MPE_SI_ELEM_MHEG = 0x07,
    MPE_SI_ELEM_DSM_CC = 0x08,
    MPE_SI_ELEM_H_222 = 0x09,
    MPE_SI_ELEM_DSM_CC_MPE = 0x0A,
    MPE_SI_ELEM_DSM_CC_UN = 0x0B,
    MPE_SI_ELEM_DSM_CC_STREAM_DESCRIPTORS = 0x0C,
    MPE_SI_ELEM_DSM_CC_SECTIONS = 0x0D,
    MPE_SI_ELEM_AUXILIARY = 0x0E,
    MPE_SI_ELEM_AAC_ADTS_AUDIO = 0x0F,
    MPE_SI_ELEM_ISO_14496_VISUAL = 0x10,
    MPE_SI_ELEM_AAC_AUDIO_LATM = 0x11,
    MPE_SI_ELEM_FLEXMUX_PES = 0x12,
    MPE_SI_ELEM_FLEXMUX_SECTIONS = 0x13,
    MPE_SI_ELEM_SYNCHRONIZED_DOWNLOAD = 0x14,
    MPE_SI_ELEM_METADATA_PES = 0x15,
    MPE_SI_ELEM_METADATA_SECTIONS = 0x16,
    MPE_SI_ELEM_METADATA_DATA_CAROUSEL = 0x17,
    MPE_SI_ELEM_METADATA_OBJECT_CAROUSEL = 0x18,
    MPE_SI_ELEM_METADATA_SYNCH_DOWNLOAD = 0x19,
    MPE_SI_ELEM_MPEG_2_IPMP = 0x1A,
    MPE_SI_ELEM_AVC_VIDEO = 0x1B,
    MPE_SI_ELEM_VIDEO_DCII = 0x80,
    MPE_SI_ELEM_ATSC_AUDIO = 0x81,
    MPE_SI_ELEM_STD_SUBTITLE = 0x82,
    MPE_SI_ELEM_ISOCHRONOUS_DATA = 0x83,
    MPE_SI_ELEM_ASYNCHRONOUS_DATA = 0x84,
    MPE_SI_ELEM_ENHANCED_ATSC_AUDIO = 0x87
} mpe_SiElemStreamType;
typedef enum mpe_SiServiceComponentType
{
    MPE_SI_COMP_TYPE_UNKNOWN = 0x00,
    MPE_SI_COMP_TYPE_VIDEO = 0x01,
    MPE_SI_COMP_TYPE_AUDIO = 0x02,
    MPE_SI_COMP_TYPE_SUBTITLES = 0x03,
    MPE_SI_COMP_TYPE_DATA = 0x04,
    MPE_SI_COMP_TYPE_SECTIONS = 0x05
} mpe_SiServiceComponentType;
typedef enum mpe_SiDescriptorTag
{
    MPE_SI_DESC_VIDEO_STREAM = 0x02,
    MPE_SI_DESC_AUDIO_STREAM = 0x03,
    MPE_SI_DESC_HIERARCHY = 0x04,
    MPE_SI_DESC_REGISTRATION = 0x05,
    MPE_SI_DESC_DATA_STREAM_ALIGNMENT = 0x06,
    MPE_SI_DESC_TARGET_BACKGROUND_GRID = 0x07,
    MPE_SI_DESC_VIDEO_WINDOW = 0x08,
    MPE_SI_DESC_CA_DESCRIPTOR = 0x09,
    MPE_SI_DESC_ISO_639_LANGUAGE = 0x0A,
    MPE_SI_DESC_SYSTEM_CLOCK = 0x0B,
    MPE_SI_DESC_MULTIPLEX_UTILIZATION_BUFFER = 0x0C,
    MPE_SI_DESC_COPYRIGHT = 0x0D,
    MPE_SI_DESC_MAXIMUM_BITRATE = 0x0E,
    MPE_SI_DESC_PRIVATE_DATA_INDICATOR = 0x0F,
    MPE_SI_DESC_SMOOTHING_BUFFER = 0x10,
    MPE_SI_DESC_STD = 0x11,
    MPE_SI_DESC_IBP = 0x12,
    /* Current spec doesn't define MPE_SI_DESC_CAROUSEL_ID descriptor tag */
    /* Needs to be added to DescriptorTagImpl */
    MPE_SI_DESC_CAROUSEL_ID = 0x13,
    MPE_SI_DESC_ASSOCIATION_TAG = 0x14,
    MPE_SI_DESC_DEFERRED_ASSOCIATION_TAG = 0x15,
    MPE_SI_DESC_APPLICATION = 0x00,
    MPE_SI_DESC_APPLICATION_NAME = 0x01,
    MPE_SI_DESC_TRANSPORT_PROTOCOL = 0x02,
    MPE_SI_DESC_DVB_J_APPLICATION = 0x03,
    MPE_SI_DESC_DVB_J_APPLICATION_LOCATION = 0x04,
    MPE_SI_DESC_EXTERNAL_APPLICATION_AUTHORISATION = 0x05,
    MPE_SI_DESC_IPV4_ROUTING = 0x06,
    MPE_SI_DESC_IPV6_ROUTING = 0x07,
    MPE_SI_DESC_APPLICATION_ICONS = 0x0B,
    MPE_SI_DESC_PRE_FETCH = 0x0C,
    MPE_SI_DESC_DII_LOCATION = 0x0D,
    MPE_SI_DESC_COMPONENT_DESCRIPTOR = 0x50,
    MPE_SI_DESC_STREAM_IDENTIFIER_DESCRIPTOR = 0x52,
    MPE_SI_DESC_PRIVATE_DATA_SPECIFIER = 0x5F,
    MPE_SI_DESC_DATA_BROADCAST_ID = 0x66,
    MPE_SI_DESC_APPLICATION_SIGNALING = 0x6F,
    MPE_SI_DESC_SERVICE_IDENTIFIER = 0x0D,
    MPE_SI_DESC_LABEL = 0x70,
    MPE_SI_DESC_CACHING_PRIORITY = 0x71,
    MPE_SI_DESC_CONTENT_TYPE = 0x72,
    MPE_SI_DESC_STUFFING = 0x80,
    MPE_SI_DESC_AC3_AUDIO = 0x81,
    MPE_SI_DESC_CAPTION_SERVICE = 0x86,
    MPE_SI_DESC_CONTENT_ADVISORY = 0x87,
    MPE_SI_DESC_REVISION_DETECTION = 0x93,
    MPE_SI_DESC_TWO_PART_CHANNEL_NUMBER = 0x94,
    MPE_SI_DESC_CHANNEL_PROPERTIES = 0x95,
    MPE_SI_DESC_DAYLIGHT_SAVINGS_TIME = 0x96,
    MPE_SI_DESC_EXTENDED_CHANNEL_NAME_DESCRIPTION = 0xA0,
    MPE_SI_DESC_SERVICE_LOCATION = 0xA1,
    MPE_SI_DESC_TIME_SHIFTED_SERVICE = 0xA2,
    MPE_SI_DESC_COMPONENT_NAME = 0xA3,
    MPE_SI_DESC_MAC_ADDRESS_LIST = 0xA4,
    MPE_SI_DESC_ATSC_PRIVATE_INFORMATION_DESCRIPTOR = 0xAD
} mpe_SiDescriptorTag;
typedef struct mpe_SiMpeg2DescriptorList
{
    mpe_SiDescriptorTag                 tag;
    uint8_t                             descriptor_length;
    void*                               descriptor_content;
    struct  mpe_SiMpeg2DescriptorList*  next;
} mpe_SiMpeg2DescriptorList;

typedef struct mpe_SiElementaryStreamList
{
    mpe_SiElemStreamType                    elementary_stream_type; // PMT
    /* If the PMT contains a component name descriptor then this
    should be the string extracted from the component_name_string () MSS
    string in this descriptor. Otherwise, 'service_comp_type' is the
    generic name associated with this stream type */
    mpe_SiServiceComponentType              service_comp_type;      //
    mpe_SiLangSpecificStringList*           service_component_names;    // PMT (desc)
    uint32_t                                elementary_PID;         // PMT
    char                                    associated_language[4]; // PMT (desc)
    uint32_t                                es_info_length;         // PMT
    uint32_t                                number_desc;            // PMT
    uint32_t                                ref_count;
    mpe_Bool                                valid;
    // refers to the service this component belongs to
    uint32_t                                service_reference;
    mpe_SiMpeg2DescriptorList*              descriptors;            // PMT
    mpe_TimeMillis                          ptime_service_component;
    struct mpe_SiElementaryStreamList*      next;
} mpe_SiElementaryStreamList;

struct mpe_SiTransportStreamEntry;

typedef struct mpe_SiProgramInfo
{
    // These will only be valid for digital services.
    mpe_SiPMTStatus                 pmt_status;             // move to program_info structure.
    uint32_t                        program_number;         // PAT, PMT, SVCT
    uint32_t                        pmt_pid;                // PAT
    uint32_t                        pmt_version;            // PMT
    mpe_Bool                        crc_check;
    uint32_t                        pmt_crc;                // PMT
    mpe_SiPMTReference*             saved_pmt_ref;
    uint8_t*                        pmt_byte_array;         // PMT
    // Duplicated in saved_pmt_ref, why 2 copies?
    uint32_t                        number_outer_desc;      // PMT
    mpe_SiMpeg2DescriptorList*      outer_descriptors;      // PMT
    uint32_t                        number_elem_streams;
    mpe_SiElementaryStreamList*     elementary_streams;     // PMT
    uint32_t                        pcr_pid;                // PMT
    ListSI                          services;               // Add. List of pointers to mpe_SiTableEntry structures.
                                                            // Can be Empty.
    uint32_t                        service_count;
    struct mpe_SiTransportStreamEntry      *stream;                // Transport stream this program is part of.
                                                            // Should never be NULL.
} mpe_SiProgramInfo;
typedef struct mpe_SiTableEntry114
{
    uint32_t                        ref_count;
    uint32_t                        activation_time;    // SVCT
    mpe_TimeMillis                  ptime_service;
    mpe_SiTransportStreamHandle     ts_handle;
    mpe_SiProgramInfo               *program;           // Program info for this service. Can be shared with other mpe_SiTableEntry elements,
                                                        // and can be null (analog channels)
    uint32_t                        tuner_id;           // '0' for OOB, start from '1' for IB
    mpe_Bool                        valid;
    uint16_t                        virtual_channel_number;
    mpe_Bool                        isAppType;          // for DSG app
    uint32_t                        source_id;          // SVCT
    uint32_t                        app_id;             //
    mpe_Bool                        dsgAttached;
    uint32_t                        dsg_attach_count;   // DSG attach count
    mpe_SiEntryState                state;
    mpe_SiChannelType               channel_type;       // SVCT
    mpe_SiVideoStandard             video_standard;     // SVCT (NTSC, PAL etc.)
    mpe_SiServiceType               service_type;       // SVCT (desc)

    mpe_SiLangSpecificStringList   *source_names;           // NTT_SNS, CVCT
    mpe_SiLangSpecificStringList   *source_long_names;      // NTT_SNS
    mpe_SiLangSpecificStringList   *descriptions;           // LVCT/CVCT (desc)
    uint8_t                         freq_index;         // SVCT
    uint8_t                         mode_index;         // SVCT
    uint32_t                        major_channel_number;
    uint32_t                        minor_channel_number;   // SVCT (desc)

    struct mpe_SiTableEntry114*        next;               // next service entry
} mpe_SiTableEntry114;

typedef struct mpe_siSourceNameEntry
{
    mpe_Bool appType;
    uint32_t id;              // if appType=FALSE, sourceID otherwise appID
    mpe_Bool mapped;          // true if mapped by a virtual channel false for DSG service name
    mpe_SiLangSpecificStringList *source_names;      // NTT_SNS, CVCT
    mpe_SiLangSpecificStringList *source_long_names; // NTT_SNS
    struct mpe_siSourceNameEntry* next;              // next source name entry
} mpe_siSourceNameEntry;

    
typedef struct _mpe_HnStreamSessionH
{   
        int unused1;
}   
*mpe_HnStreamSession; /* Opaque streaming session handle. */

typedef struct mpe_SiTableEntry
{
    uint32_t ref_count;
    uint32_t activation_time; // SVCT
    mpe_TimeMillis ptime_service;
    mpe_SiTransportStreamHandle ts_handle;
    mpe_SiProgramInfo *program; // Program info for this service. Can be shared with other mpe_SiTableEntry elements,
    // and can be null (analog channels)
    uint32_t tuner_id; // '0' for OOB, start from '1' for IB
    mpe_Bool valid;
    uint16_t virtual_channel_number;
    mpe_Bool isAppType; // for DSG app
    uint32_t source_id; // SVCT
    uint32_t app_id; //
    mpe_Bool dsgAttached;
    uint32_t dsg_attach_count; // DSG attach count
    mpe_SiEntryState state;
    mpe_SiChannelType channel_type; // SVCT
    mpe_SiVideoStandard video_standard; // SVCT (NTSC, PAL etc.)
    mpe_SiServiceType service_type; // SVCT (desc)

    mpe_siSourceNameEntry *source_name_entry;      // Reference to the corresponding NTT_SNS entry
    mpe_SiLangSpecificStringList *descriptions; // LVCT/CVCT (desc)
    uint8_t freq_index; // SVCT
    uint8_t mode_index; // SVCT
    uint32_t major_channel_number;
    uint32_t minor_channel_number; // SVCT (desc)

    uint16_t program_number;       // SVCT
    uint8_t  transport_type;       // SVCT (0==MPEG2, 1==non-MPEG2)
    mpe_Bool scrambled;            // SVCT
    mpe_HnStreamSession hn_stream_session;  // HN stream session handle
    uint32_t hn_attach_count; // HN stream session PSI attach / registration count
    struct mpe_SiTableEntry* next; // next service entry
} mpe_SiTableEntry;

typedef mpe_SiTableEntry mpe_SiTableEntry121;

typedef enum mpe_SiModulationMode
{
    MPE_SI_MODULATION_UNKNOWN=0,
    MPE_SI_MODULATION_QPSK,
    MPE_SI_MODULATION_BPSK,
    MPE_SI_MODULATION_OQPSK,
    MPE_SI_MODULATION_VSB8,
    MPE_SI_MODULATION_VSB16,
    MPE_SI_MODULATION_QAM16,
    MPE_SI_MODULATION_QAM32,
    MPE_SI_MODULATION_QAM64,
    MPE_SI_MODULATION_QAM80,
    MPE_SI_MODULATION_QAM96,
    MPE_SI_MODULATION_QAM112,
    MPE_SI_MODULATION_QAM128,
    MPE_SI_MODULATION_QAM160,
    MPE_SI_MODULATION_QAM192,
    MPE_SI_MODULATION_QAM224,
    MPE_SI_MODULATION_QAM256,
    MPE_SI_MODULATION_QAM320,
    MPE_SI_MODULATION_QAM384,
    MPE_SI_MODULATION_QAM448,
    MPE_SI_MODULATION_QAM512,
    MPE_SI_MODULATION_QAM640,
    MPE_SI_MODULATION_QAM768,
    MPE_SI_MODULATION_QAM896,
    MPE_SI_MODULATION_QAM1024,
    MPE_SI_MODULATION_QAM_NTSC = 255 // for analog mode
} mpe_SiModulationMode;

typedef enum mpe_SiGlobalState
{
    SI_ACQUIRING = 0x00,
    SI_NOT_AVAILABLE_YET = 0x01,
    SI_PARTIALLY_ACQUIRED = 0x02,
    SI_FULLY_ACQUIRED = 0x03,
    SI_DISABLED = 0x04 // when OOB SI is disabled
} mpe_SiGlobalState;

#define MAX_FREQUENCIES 255
static uint32_t g_frequency[ MAX_FREQUENCIES + 1 ];
static mpe_SiModulationMode g_mode[ MAX_FREQUENCIES + 1 ];
static mpe_Bool g_SITPConditionSet = FALSE;
static mpe_SiGlobalState g_si_state = SI_ACQUIRING;
static mpe_SiTableEntry *g_si_entry = NULL;

#define MPE_MEM_SI 0
#define MPE_SUCCESS 0
#define MPE_EINVAL -1 
typedef int mpe_Error;
static int mpeos_memAllocP(int not_used, size_t len, void ** buf)
{
    *buf = malloc(len);
    return (*buf != NULL)? 0 : -1;
}
static  int   mpeos_memFreeP(int not_used, void *p)
{
    free(p);
    return 0;
}

static void walk_names(char *prefix, mpe_SiLangSpecificStringList *names, int channel, uint32_t source_id) 
{
    if (names) {
        char buf[256] = {0};
        int ret = 0;
        ret += sprintf(&buf[ret], "[%s]####%06d####0x%08x::", prefix, channel, source_id);
        while(names) {
            ret += sprintf(&buf[ret], "%c%c%c%c-->[%s]::", 
                    names->language[0], names->language[1], names->language[2], ' ', names->string ? names->string : "----");
            names = names->next;
        }

        ret += sprintf(&buf[ret], "\r\n");
        printf("%s", buf);
    }
}


static mpe_Error  mpeos_timeGetMillis(mpe_TimeMillis *ptime)
{
    int ret;
    struct timeval tv;
    //struct timezone tz;
    mpe_TimeMillis msec;

    if (NULL == ptime)
    {
        return MPE_EINVAL;
    }

    ret = gettimeofday(&tv,NULL);
    if (0 != ret)
    {
        // TODO - should there be an MPE_MOD_TIME ???
        (void) fprintf(stderr,
                  "mpeos_timeGetMillis(): gettimeofday(): %d.\n", ret);
        return MPE_EINVAL;
    }

    msec = tv.tv_sec;
    msec *= 1000;
    msec += (tv.tv_usec / 1000); 
    *ptime = msec;
    return MPE_SUCCESS;
}

#define MPE_LOG(type, mod, ...) do{}while(0) 
//#define MPE_LOG(type, mod, ...) printf(__VA_ARGS__)
#define MPE_SI_INVALID_HANDLE  0x01 
#define MPE_SI_INVALID_PARAMETER 0x01 
#define MPE_SI_OUT_OF_MEM 0x01 
#define MPE_SI_NOT_FOUND 0x02 
#define MPE_SUCCESS 0
#define MPE_SI_SUCCESS 0
#define MPE_SI_DEFAULT_PROGRAM_NUMBER           (0xFFFFFFFF)
#define PROGRAM_NUMBER_UNKNOWN 0xFFFF
#define MPE_SI_DEFAULT_CHANNEL_NUMBER           0xFFFFFFFF

typedef uint32_t mpe_SiTransportHandle;
typedef uint32_t mpe_SiNetworkHandle;
typedef uint32_t mpe_SiServiceHandle;
typedef uint32_t mpe_SiProgramHandle;
typedef uint32_t mpe_SiServiceDetailsHandle;
typedef uint32_t mpe_SiServiceComponentHandle;
typedef uint32_t mpe_SiRatingDimensionHandle;
typedef uint32_t mpe_SiGenericHandle;               // Cast before use...

#define mpe_siSetSourceId(x, y)         (((mpe_SiTableEntry *)(x))->source_id =y )
#define mpe_siSetAppId(x, y)            (((mpe_SiTableEntry *)(x))->app_id=y )
#define mpe_siSetAppType(x,y)           (((mpe_SiTableEntry *)(x))->isAppType =y )
#define mpe_siSetActivationTime(x,y)    (((mpe_SiTableEntry *)(x))->activation_time =y )
#define mpe_siSetCDSRef(x,y)            (((mpe_SiTableEntry *)(x))->freq_index =y )
#define mpe_siSetMMSRef(x,y)            (((mpe_SiTableEntry *)(x))->mode_index =y )
#define mpe_siSetChannelType(x,y)       (((mpe_SiTableEntry *)(x))->channel_type =y ) 
#define mpe_siSetServiceType(x,y)       (((mpe_SiTableEntry *)(x))->service_type =  y)

typedef enum mpe_TsEntryState
{
    TSENTRY_NEW,
    TSENTRY_OLD,
    TSENTRY_UPDATED
} mpe_TsEntryState;

typedef enum mpe_SiStatus
{
    SI_NOT_AVAILABLE = 0x00,
    SI_AVAILABLE_SHORTLY = 0x01,
    SI_AVAILABLE = 0x02
} mpe_SiStatus;

typedef enum mpe_SiSourceType 
{
    MPE_SOURCE_TYPE_UNKNOWN = 0x00, 
    MPE_SOURCE_TYPE_OOB = 0x01,
    MPE_SOURCE_TYPE_IB = 0x02, 
    MPE_SOURCE_TYPE_PVR_FILE = 0x03,
    MPE_SOURCE_TYPE_HN_STREAM = 0x04, 
    MPE_SOURCE_TYPE_DSG = 0x05
} mpe_SiSourceType;


typedef struct mpe_SiPatProgramList
{
    uint32_t                        programNumber;
    uint32_t                        pmt_pid;
    mpe_Bool                        matched;
    struct  mpe_SiPatProgramList*   next;
} mpe_SiPatProgramList;


typedef pthread_mutex_t* os_Mutex;
typedef os_Mutex mpe_Mutex;		/* Mutex type binding. */
typedef struct mpe_SiTransportStreamEntry
{
    mpe_TsEntryState                    state;
    uint32_t                            frequency;
    mpe_SiModulationMode                modulation_mode;
    uint32_t                            ts_id;
    char                                *ts_name;
    char                                *description;
    mpe_TimeMillis                      ptime_transport_stream;
    uint16_t                            service_count;
    uint16_t                            program_count;
    uint16_t                            visible_service_count;
    mpe_SiStatus                        siStatus;
    uint32_t                            pat_version;        // PAT
    uint32_t                            pat_crc;            // PAT
    uint32_t                            cvct_version;       // CVCT
    uint32_t                            cvct_crc;           // CVCT
    mpe_Bool                            check_crc;
    uint32_t                            pat_reference;
    mpe_SiPatProgramList*               pat_program_list;   // PAT
    struct mpe_SiTransportStreamEntry*  next;

    mpe_SiSourceType                    source_type;        // IB/OOB etc.
    uint8_t*                            pat_byte_array;
    ListSI                              services;
    ListSI                              programs;
    mpe_Mutex                           list_lock;
} mpe_SiTransportStreamEntry;

static mpe_SiTransportStreamEntry *g_si_ts_entry = NULL;
static mpe_SiTransportStreamEntry *g_si_dsg_ts_entry = NULL;

LINKHD * llist_create(void)
{
    LINKHD * hp;

    /*
     * Get space.
     */
    if ( MPE_SUCCESS != mpeos_memAllocP(MPE_MEM_SI, sizeof(LINKHD), (void **)&(hp)) )
    {
        return NULL;
    }

    /*
     * Init list to empty and return it.
     */
    hp->headp = NULL;
    hp->tailp = NULL;
    hp->nlinks = 0;

    return hp;
}

/*
 * =========================================================================
 * Destroy (free) a list header and all it's links.
 *  headerp     header of list to destroy.
 * Returns headerp or NULL on error.
 * =========================================================================
 */
void
llist_free(LINKHD *headerp)
{
    LINK * lp;

    while (1)
    {
        lp = llist_first(headerp);
        if (lp == NULL)
            break;

        llist_rmfirst(headerp);
        llist_freelink(lp);
    }

    mpeos_memFreeP(MPE_MEM_SI, headerp);
}

/*
 * =========================================================================
 * Create a new link, and bind the 'user' data.
 *  datap       pointer to arbitrary data.
 * Returns a pointer to the new link, or NULL on error.
 * =========================================================================
 */
LINK * llist_mklink(void * datap)
{
    LINK * lp;

    /*
     * Get a free link.
     */
    if ( MPE_SUCCESS != mpeos_memAllocP(MPE_MEM_SI, sizeof(LINK), (void **)&(lp)) )
    {
        return NULL;
    }

    lp->datap = datap;
    return lp;
}

/*
 * =========================================================================
 * Destroy (free) a link.
 *  linkp       pointer to link to destroy.
 * The link must not be in a list.
 * Returns a pointer to the freed link, or NULL on error.
 * Really oughta return BOOL status, instead of ptr to free memory.
 * =========================================================================
 */
void llist_freelink(LINK *linkp)
{
    mpeos_memFreeP(MPE_MEM_SI, linkp);
}

/*
 * =========================================================================
 * Append a link to the list tail.
 *  headerp     pointer to the list header.
 *  linkp       the link to append.
 * The link must not already be in a list.
 * Returns a pointer to the link, or NULL on error.
 * =========================================================================
 */
void
llist_append(LINKHD *headerp, LINK *linkp)
{
    linkp->nextp = NULL;
    linkp->headerp = headerp;
    linkp->prevp = headerp->tailp;

    if (headerp->tailp)
        headerp->tailp->nextp = linkp;

    headerp->tailp = linkp;

    if (headerp->headp == NULL)
        headerp->headp = linkp;

    ++headerp->nlinks;
}

/*
 * =========================================================================
 * Return a pointer to the first link in the list.
 *  headerp     the list to look in.
 * Returns the link pointer found, or NULL on error.
 * =========================================================================
 */
LINK * llist_first(LINKHD * headerp)
{
    return headerp->headp;
}

/*
 * =========================================================================
 * Return a pointer to the last link in the list.
 *  headerp     the list to look in.
 * Returns the link pointer found, or NULL on error.
 * =========================================================================
 */
LINK * llist_last(LINKHD * headerp)
{
    return headerp->tailp;
}

/*
 * =========================================================================
 * Return a pointer to the link after the given link.
 *  afterp      the link to look after.
 * Returns the link pointer found, or NULL on error.
 * =========================================================================
 */
LINK * llist_after(LINK * afterp)
{
    return afterp->nextp;
}

/*
 * =========================================================================
 * Remove specified link from it's list and return a pointer to it.
 * The link is not destroyed.
 *  linkp       the link to remove.
 * Returns the link removed, or NULL on error.
 * =========================================================================
 */
void llist_rmlink(LINK *linkp)
{
    LINKHD * hp = linkp->headerp;

    if (linkp == hp->headp)
        llist_rmfirst(hp);
    else
        llist_rmafter(linkp->prevp);
}

/*
 * =========================================================================
 * Remove the first link from the list and return a pointer to it.
 * The link is not destroyed.
 *  headerp     the list to look in.
 * Returns the link removed, or NULL on error.
 * =========================================================================
 */
void llist_rmfirst(LINKHD * headerp)
{
    LINK * lp = headerp->headp;
    if (lp == NULL) return;

    headerp->headp = lp->nextp;
    if (lp->nextp != NULL)
        lp->nextp->prevp = NULL;

    if (headerp->tailp == lp)
        headerp->tailp = NULL;

    --headerp->nlinks;

    lp->nextp = NULL;
    lp->prevp = NULL;
    lp->headerp = NULL;
}

/*
 * =========================================================================
 * Remove the link after the given link and return a pointer to it.
 * The link is not destroyed.
 *  afterp      the link to look after.
 * Returns the link removed, or NULL on error.
 * =========================================================================
 */
void llist_rmafter(LINK * afterp)
{
    if (afterp->nextp == NULL) return;

    if (afterp->nextp == afterp->headerp->tailp)
    {
        llist_rmlast(afterp->headerp);
    }
    else
    {
        LINK * lp = afterp->nextp;

        lp->nextp->prevp = lp->prevp;
        afterp->nextp = lp->nextp;

        --afterp->headerp->nlinks;

        lp->nextp = NULL;
        lp->prevp = NULL;
        lp->headerp = NULL;
    }
}

/*
 * =========================================================================
 * Remove the last link from the list and return a pointer to it.
 * The link is not destroyed.
 *  headerp     the list to look in.
 * Returns the link removed, or NULL on error.
 * =========================================================================
 */
void llist_rmlast(LINKHD * headerp)
{
    LINK * lp = headerp->tailp;
    if (lp == NULL) return;

    headerp->tailp = lp->prevp;
    if (lp->prevp != NULL)
        lp->prevp->nextp = NULL;

    if (headerp->headp == lp)
        headerp->headp = NULL;

    --headerp->nlinks;

    lp->nextp = NULL;
    lp->prevp = NULL;
    lp->headerp = NULL;
}

/*
 * =========================================================================
 * Change the arbitrary data pointer bound to a link.
 *  linkp       the link to rebind.
 *  datap       pointer to be bound.
 * Returns linkp, or NULL on error.
 * =========================================================================
 */
LINK * llist_setdata(LINK * linkp, void * datap)
{
    linkp->datap = datap;
    return linkp;

}

/*
 * =========================================================================
 * Return the arbitrary data pointer bound to a link.
 *  linkp       the link to look at.
 * Returns a void pointer, or NULL on error.
 * =========================================================================
 */
void * llist_getdata(LINK * linkp)
{
    return linkp->datap;
}

/*
 * =========================================================================
 * Return the number of links in a list.
 *  headerp     the list to look at.
 * Returns a link count, 0 on error.
 * =========================================================================
 */
unsigned long llist_cnt(LINKHD * headerp)
{
    return headerp->nlinks;
}

/*
 * =========================================================================
 * Return the first link in the list that is bound to the given data
 * pointer. Note that nothing prevents a data pointer from being bound to
 * more than one link.
 *  headerp     the list to look in.
 *  datap       the data pointer to look for.
 * Returns the link found, or NULL on error.
 * =========================================================================
 */
LINK * llist_linkof(LINKHD * headerp, void * datap)
{
    LINK * lp = headerp->headp;

    while (lp != NULL)
        if (lp->datap == datap)
            break;
        else
            lp = lp->nextp;

    return lp;
}

/*
 * =========================================================================
 * Return the next link after the given link that is bound to the given data
 * pointer. Note that nothing prevents a data pointer from being bound to
 * more than one link.
 *  afterp      the link to start from.
 *  datap       the data pointer to look for.
 * Returns the link found, or NULL on error.
 * =========================================================================
 */
LINK * llist_nextlinkof(void * datap, LINK * afterp)
{
    LINK * lp = afterp->nextp;
    while (lp != NULL)
        if (lp->datap == datap)
            break;
        else
            lp = lp->nextp;
    return lp;
}

LINK * llist_getNodeAtIndex(LINKHD * headerp, unsigned long index)
{
    unsigned long i = 0;
    LINK * lp = headerp->headp;

    if (index == 0) return headerp->headp;

    for (i=0; i<headerp->nlinks && lp; i++)
    {
        if (i == index)
        {
            return lp;
        }
        else
        {
            lp = lp->nextp;
        }
    }

    return NULL;
}

/*
 * =========================================================================
 * Return pointer to the header of the list a link is in,
 * or NULL if not in a list, or not a link.
 * =========================================================================
 */
LINKHD * llist_hdrof(LINK * lp)
{
    return lp->headerp;
}

// --------------------------------------------------------------
//                          CRC UTIL
// --------------------------------------------------------------

#define CRC_QUOTIENT 0x04C11DB7
/*
 * This table should be initialized once
 */
static uint32_t crctab[256];

void init_mpeg2_crc(void)
{
    uint16_t i, j;
    uint32_t crc;

    for (i = 0; i < 256; i++)
    {
        crc = i << 24;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ CRC_QUOTIENT;
            else
                crc = crc << 1;
        }
        crctab[i] = crc;
    }
} // END crc32_init()

/*
 *  CRC calculation function
 */
uint32_t calc_mpeg2_crc(uint8_t * data, uint32_t len)
{
    uint32_t  result;
    uint32_t  i;

    if (len < 4) return 0;

    result =  *data++ << 24;
    result |= *data++ << 16;
    result |= *data++ << 8;
    result |= *data++;
    result = ~ result;
    len -=4;

    for (i=0; i<len; i++)
    {
        result = (result << 8 | *data++) ^ crctab[result >> 24];
    }

    return ~result;
} // END calc_mpeg2_crc()

void mpe_siSetVideoStandard(mpe_SiServiceHandle service_handle,
		mpe_SiVideoStandard video_std)
{
	mpe_SiTableEntry * si_entry = (mpe_SiTableEntry *) service_handle;

	/* Parameter check */
    if ((service_handle == MPE_SI_INVALID_HANDLE))
	{
		MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI,
				"<mpe_siSetVideoStandard> MPE_SI_INVALID_PARAMETER\n");
	}
	else
	{
		MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
				"<mpe_siSetVideoStandard> video_std: %d\n", video_std);
		si_entry->video_standard = video_std;
		// We know that an analog service does, and always will, have no components
		//si_entry->number_elem_streams = 0;
		//si_entry->pmt_status = PMT_AVAILABLE;
	}
}

mpe_Error mpe_siSetChannelNumber(mpe_SiServiceHandle service_handle,
		uint32_t major_channel_number, uint32_t minor_channel_number)
{
	mpe_SiTableEntry *si_entry = (mpe_SiTableEntry *) service_handle;

	if (service_handle == MPE_SI_INVALID_HANDLE)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	MPE_LOG(
			MPE_LOG_DEBUG,
			MPE_MOD_SI,
			"<mpe_siSetChannelNumber> Enter - service_handle: 0x%x, major_channel_number: %d, minor_channel_number: %d\n",
			service_handle, major_channel_number, minor_channel_number);

	/*
     * If this is a one part channel number, then check the value and
     * set in in the minor field.
     */
	if (minor_channel_number == MPE_SI_DEFAULT_CHANNEL_NUMBER)
	{
		if (major_channel_number > 4095)
		{
			return MPE_SI_INVALID_PARAMETER;
		}
		si_entry->major_channel_number = major_channel_number;
		si_entry->minor_channel_number = MPE_SI_DEFAULT_CHANNEL_NUMBER;
		return MPE_SI_SUCCESS;
	}

	/*
     * This is a twor part channel number, so check each param and set the values.
     *
     * Major channel number must be between 1 and 99
     */
	if (major_channel_number > 99 || major_channel_number < 1)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	/* Minor channel number must be between 0 and 999 */
	if (minor_channel_number > 999)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	/* Set the channel numbers */
	si_entry->major_channel_number = major_channel_number;
	si_entry->minor_channel_number = minor_channel_number;

	return MPE_SI_SUCCESS;
}

#define MPE_SI_EVENT_UNKNOWN                        (0x00003000)
#define MPE_SI_EVENT_OOB_VCT_ACQUIRED               (MPE_SI_EVENT_UNKNOWN + 1)
#define MPE_SI_EVENT_OOB_NIT_ACQUIRED               (MPE_SI_EVENT_UNKNOWN + 2)
#define MPE_SI_EVENT_OOB_PAT_ACQUIRED               (MPE_SI_EVENT_UNKNOWN + 3)
#define MPE_SI_EVENT_OOB_PMT_ACQUIRED               (MPE_SI_EVENT_UNKNOWN + 4)
#define MPE_SI_EVENT_IB_PAT_ACQUIRED                (MPE_SI_EVENT_UNKNOWN + 5)
#define MPE_SI_EVENT_IB_PMT_ACQUIRED                (MPE_SI_EVENT_UNKNOWN + 6)

#define MPE_SI_EVENT_TRANSPORT_STREAM_UPDATE        (MPE_SI_EVENT_UNKNOWN + 7)
#define MPE_SI_EVENT_NETWORK_UPDATE                 (MPE_SI_EVENT_UNKNOWN + 8)
#define MPE_SI_EVENT_SERVICE_DETAILS_UPDATE         (MPE_SI_EVENT_UNKNOWN + 9)
#define MPE_SI_EVENT_SERVICE_COMPONENT_UPDATE           (MPE_SI_EVENT_UNKNOWN + 10)
#define MPE_SI_EVENT_IB_PAT_UPDATE                      (MPE_SI_EVENT_UNKNOWN + 11)
#define MPE_SI_EVENT_IB_PMT_UPDATE                      (MPE_SI_EVENT_UNKNOWN + 12)
#define MPE_SI_EVENT_OOB_PAT_UPDATE                     (MPE_SI_EVENT_UNKNOWN + 13)
#define MPE_SI_EVENT_OOB_PMT_UPDATE                     (MPE_SI_EVENT_UNKNOWN + 14)

/* This event is used to terminate the ED session and delete the associatd ED handle */
#define MPE_SI_EVENT_SI_ACQUIRING                   (MPE_SI_EVENT_UNKNOWN + 15)


#define MPE_SI_EVENT_SI_NOT_AVAILABLE_YET           (MPE_SI_EVENT_UNKNOWN + 16)
#define MPE_SI_EVENT_SI_FULLY_ACQUIRED              (MPE_SI_EVENT_UNKNOWN + 17)
#define MPE_SI_EVENT_SI_DISABLED                    (MPE_SI_EVENT_UNKNOWN + 18)
#define MPE_SI_EVENT_TUNED_AWAY                     (MPE_SI_EVENT_UNKNOWN + 19)
#define MPE_SI_EVENT_TERMINATE_SESSION              (MPE_SI_EVENT_UNKNOWN + 20)
#define MPE_SI_EVENT_IB_CVCT_ACQUIRED               (MPE_SI_EVENT_UNKNOWN + 21)
#define MPE_SI_EVENT_IB_CVCT_UPDATE                 (MPE_SI_EVENT_UNKNOWN + 22)
#define MPE_SI_EVENT_NIT_SVCT_ACQUIRED              (MPE_SI_EVENT_UNKNOWN + 23)
//COMCAST[ STT event notification
#define MPE_SI_EVENT_STT_ACQUIRED           (MPE_SI_EVENT_UNKNOWN + 24)
//COMCAST]
#define MPE_SI_CHANGE_TYPE_ADD                              0x01
#define MPE_SI_CHANGE_TYPE_REMOVE                           0x02
#define MPE_SI_CHANGE_TYPE_MODIFY                           0x03

/* SI Handles, default values */
#define MPE_SI_NUM_TRANSPORTS                   0x1
#define MPE_SI_DEFAULT_TRANSPORT_ID             0x1
#define MPE_SI_DEFAULT_TRANSPORT_HANDLE         0x1000

#define MPE_SI_NUM_NETWORKS                     0x1
#define MPE_SI_DEFAULT_NETWORK_ID               0x1
#define MPE_SI_DEFAULT_NETWORK_HANDLE           0x2000
#define MPE_SI_DEFAULT_CHANNEL_NUMBER           0xFFFFFFFF


/* Default values for SI fields (OCAP specified) */
/* sourceId and tsId are set by default to -1 */
#define SOURCEID_UNKNOWN                (0xFFFFFFFF)
#define TSID_UNKNOWN                    (0xFFFFFFFF) // ECR 1072
#define MPE_SI_DEFAULT_PROGRAM_NUMBER           (0xFFFFFFFF)
#define VIRTUAL_CHANNEL_UNKNOWN                 (0xFFFF)

#define NUMBER_NON_UNIQUE_SOURCEIDS    10

#define MPE_ED_TERMINATION_ONESHOT   1        /* EDMgr will delete handle after first event */
#define MPE_ED_TERMINATION_EVCODE    2        /* EDMgr will delete handle after receiving
                                                                                                  event code matching terminationCode */
#define MPE_ED_TERMINATION_OPEN      3        /* EDMgr will not delete the handle */



static void send_si_event(uint32_t siEvent, uint32_t optional_data, uint32_t changeType)
{
}

static void clone_si_entry(mpe_SiTableEntry *new_entry,
		mpe_SiTableEntry *si_entry)
{
	// Clone fields from one service entry to the other
	// This method is called in cases where a dynamic entry
	// is created first (using f-p-q) and later OOB tables
	// are acquired.
	new_entry->activation_time = si_entry->activation_time;
	new_entry->isAppType = si_entry->isAppType;
	new_entry->source_id = si_entry->source_id;
	new_entry->app_id = si_entry->app_id;
	new_entry->channel_type = si_entry->channel_type;
	new_entry->video_standard = si_entry->video_standard;
    //new_entry->source_names = si_entry->source_names;
    //new_entry->source_long_names = si_entry->source_long_names;
    new_entry->source_name_entry = si_entry->source_name_entry;
	new_entry->descriptions = si_entry->descriptions;
	new_entry->major_channel_number = si_entry->major_channel_number;
	new_entry->minor_channel_number = si_entry->minor_channel_number;
	new_entry->service_type = si_entry->service_type;
	new_entry->freq_index = si_entry->freq_index;
	new_entry->mode_index = si_entry->mode_index;
	new_entry->dsgAttached = si_entry->dsgAttached;
	new_entry->dsgAttached = si_entry->dsg_attach_count;
	new_entry->state = si_entry->state;
	new_entry->virtual_channel_number = si_entry->virtual_channel_number;
    new_entry->program_number = si_entry->program_number;
    new_entry->transport_type = si_entry->transport_type;
    new_entry->scrambled = si_entry->scrambled;
    new_entry->hn_stream_session = si_entry->hn_stream_session;
    new_entry->hn_attach_count = si_entry->hn_attach_count;
}
void mpe_siUpdateServiceEntry(mpe_SiServiceHandle service_handle,
		mpe_SiTransportHandle ts_handle, mpe_SiProgramHandle pi_handle)
{
	mpe_SiTableEntry * walker = NULL;
	mpe_SiTableEntry * si_entry = NULL;
	mpe_Bool dupFound = false;

	/* Parameter check */
	if ((service_handle == MPE_SI_INVALID_HANDLE) || (ts_handle
			== MPE_SI_INVALID_HANDLE)
			|| ((((mpe_SiTransportStreamEntry *) ts_handle)->modulation_mode
					!= MPE_SI_MODULATION_QAM_NTSC) && (pi_handle
					== MPE_SI_INVALID_HANDLE)))
	{
		MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI,
				"<mpe_siUpdateServiceEntry> MPE_SI_INVALID_PARAMETER\n");
	}
	else
	{
		walker = (mpe_SiTableEntry*) service_handle;

		if (walker != NULL)
		{
			if (walker->ts_handle != MPE_SI_INVALID_HANDLE)
			{ // exists, or move?
				if (walker->ts_handle != ts_handle)
				{ // In writer locked area, so no extra locking necessary.
                    LINK *lp = llist_linkof(((mpe_SiTransportStreamEntry *) walker->ts_handle)->services,
											(void *) walker);
					if (lp != NULL)
					{
						llist_rmlink(lp);
						((mpe_SiTransportStreamEntry *) walker->ts_handle)->service_count--;
						((mpe_SiTransportStreamEntry *) walker->ts_handle)->visible_service_count--;
					}
					walker->ts_handle = MPE_SI_INVALID_HANDLE;
					if (walker->program)
					{
						LINK *lp1 = llist_linkof(walker->program->services,
								(void *) walker);
						if (lp1 != NULL)
						{
							llist_rmlink(lp1);
							walker->program->service_count--;
						}
						walker->program = NULL;
					}
                    // Signal ServiceDetails update event (MODIFY)
                    {
                        MPE_LOG(
                                MPE_LOG_INFO,
                                MPE_MOD_SI,
                                "<mpe_siUpdateServiceEntry> transport stream changed..: signaling  MPE_SI_EVENT_SERVICE_DETAILS_UPDATE for handle:%p sourceId:0x%x\n", walker, walker->source_id);
                    	send_si_event(MPE_SI_EVENT_SERVICE_DETAILS_UPDATE, (mpe_SiServiceHandle)walker, MPE_SI_CHANGE_TYPE_MODIFY);
                    }
				}
				else
				{ // Same TS, moved programs?
					if (walker->program != NULL)
					{
						if (walker->program != (mpe_SiProgramInfo *) pi_handle)
						{
							LINK *lp = llist_linkof(walker->program->services,
									(void *) walker);
							llist_rmlink(lp);
							walker->program->service_count--;
							walker->program = NULL;
                            // Signal ServiceDetails update event (MODIFY)
                            {
                                MPE_LOG(
                                        MPE_LOG_INFO,
                                        MPE_MOD_SI,
                                        "<mpe_siUpdateServiceEntry> program changed..: signaling  MPE_SI_EVENT_SERVICE_DETAILS_UPDATE for handle:%p sourceId:0x%x pn:%d\n",
                                        walker, walker->source_id, walker->program_number);
                            	send_si_event(MPE_SI_EVENT_SERVICE_DETAILS_UPDATE, (mpe_SiServiceHandle)walker, MPE_SI_CHANGE_TYPE_MODIFY);
                            }						
						}
					}
				}
			}


			// Increment the global number of services
			g_numberOfServices++;

			if (pi_handle != MPE_SI_INVALID_HANDLE)
			{
				LINK *lp, *lp2;
				mpe_SiProgramInfo *pgm = (mpe_SiProgramInfo *) pi_handle;
				lp2 = llist_first(pgm->services);
				while (lp2)
				{
					si_entry = (mpe_SiTableEntry *) llist_getdata(lp2);
					// This may be a dynamic entry created prior to acquiring OOB SI.
					// State would be SIENTRY_UNSPECIFIED
					// clone the fields from current entry to dynamic entry
					if ((si_entry->state == SIENTRY_UNSPECIFIED)
							&& (si_entry->program == pgm)
							&& (si_entry->ts_handle == ts_handle))
					{
						dupFound = true;
						clone_si_entry(si_entry, walker);
						MPE_LOG(
								MPE_LOG_DEBUG,
								MPE_MOD_SI,
								"<mpe_siUpdateServiceEntry> Found a previously created entry...updated state: %d\n",
								si_entry->state);
						// Mark the state of new entry such that it is not navigable
						walker->state = SIENTRY_UNSPECIFIED;
						// If a dup is found, decrement the count
						g_numberOfServices--;
						break;
					}
					lp2 = llist_after(lp2);
				}
                // If this is not a dup and the program handle is not already set
                if (!dupFound && ((mpe_SiProgramHandle)walker->program != pi_handle))
				{
					walker->program = (mpe_SiProgramInfo *) pi_handle;
					lp = llist_mklink((void *) walker);
					llist_append(walker->program->services, lp);
					walker->program->service_count++;
				}
			}

			if (walker->ts_handle == MPE_SI_INVALID_HANDLE && !dupFound)
			{
				LINK *lp;
				walker->ts_handle = ts_handle;
				lp = llist_mklink((void *) walker);
				llist_append(
						((mpe_SiTransportStreamEntry *) ts_handle)->services,
						lp);
				((mpe_SiTransportStreamEntry *) walker->ts_handle)->service_count++;
				((mpe_SiTransportStreamEntry *) walker->ts_handle)->visible_service_count++;
			}
		}
	}
}

#define SOURCEID_UNKNOWN                (0xFFFFFFFF)
#define TSID_UNKNOWN                    (0xFFFFFFFF) // ECR 1072
#define MPE_SI_DEFAULT_PROGRAM_NUMBER           (0xFFFFFFFF)
static void init_si_entry(mpe_SiTableEntry *si_entry)
{
	// TODO: use static struct initialization

	/* set default values as defined in OCAP spec (annex T) */
	/* These are in the order they appear in the struct, to make it
     easier to verify all have been initialized. --mlb */
	si_entry->ref_count = 1;
	si_entry->activation_time = 0;
	mpeos_timeGetMillis(&(si_entry->ptime_service));
	si_entry->ts_handle = MPE_SI_INVALID_HANDLE;
	si_entry->program = NULL;
	si_entry->next = NULL;
	si_entry->isAppType = FALSE;
	si_entry->source_id = SOURCEID_UNKNOWN;
	si_entry->app_id = 0;
	si_entry->channel_type = CHANNEL_TYPE_HIDDEN; /* */
	si_entry->video_standard = VIDEO_STANDARD_UNKNOWN;
    si_entry->source_name_entry = NULL; /* default value */
	si_entry->descriptions = NULL; /* default value */
	si_entry->major_channel_number = MPE_SI_DEFAULT_CHANNEL_NUMBER; /* default value */
	si_entry->minor_channel_number = MPE_SI_DEFAULT_CHANNEL_NUMBER; /* default value */
	si_entry->service_type = MPE_SI_SERVICE_TYPE_UNKNOWN; /* default value */
	si_entry->freq_index = 0;
	si_entry->mode_index = 0;
	si_entry->dsgAttached = FALSE;
	si_entry->dsg_attach_count = 0; // OC over DSG attach count
	si_entry->state = SIENTRY_UNSPECIFIED;
	si_entry->virtual_channel_number = VIRTUAL_CHANNEL_UNKNOWN;
    si_entry->program_number = 0;
    si_entry->transport_type = 0; // Default is MPEG2
    si_entry->scrambled = FALSE;
    si_entry->hn_stream_session = 0;
    si_entry->hn_attach_count = 0;
}
void mpe_siGetServiceEntryFromChannelNumber(uint16_t channel_number,
		mpe_SiServiceHandle *service_handle)
{
	mpe_SiTableEntry *prevWalker = NULL;
	mpe_SiTableEntry *walker = NULL;
	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "<mpe_siGetServiceEntryFromChannelNumber> channel_number: %d\n", channel_number);

	if ((service_handle == NULL) || (channel_number > 4095))
	{
		MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI, "<mpe_siGetServiceEntryFromChannelNumber> MPE_SI_INVALID_PARAMETER channel_number: %d\n", channel_number);
	}
	else
	{
		*service_handle = MPE_SI_INVALID_HANDLE;

		walker = prevWalker = g_si_entry;

		while (walker)
		{
			// If a service entry exists all the members are set to default values
			if (walker->virtual_channel_number == channel_number)
			{
				*service_handle = (mpe_SiServiceHandle) walker;
                //MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "<mpe_siGetServiceEntryFromChannelNumber> found entry for channel_number: %d\n", channel_number);
				break;
			}
			prevWalker = walker;
			walker = walker->next;
		}

		// Create a new entry and append to list if one is not found
		if (walker == NULL)
		{
			mpe_Error tRetVal = mpeos_memAllocP(MPE_MEM_SI, sizeof(mpe_SiTableEntry),
					(void **) &(walker));
			if ((MPE_SI_SUCCESS != tRetVal) || (walker == NULL))
			{
				MPE_LOG(MPE_LOG_FATAL, MPE_MOD_SI,
						"<mpe_siGetServiceEntryFromChannelNumber> MPE_SI_OUT_OF_MEM\n");
			}
			else
			{
                //MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI, "<mpe_siGetServiceEntryFromChannelNumber> creating new entry for channel_number: %d\n", channel_number);

				init_si_entry(walker);
				// Set the new channel number
				walker->virtual_channel_number = channel_number;
				*service_handle = (mpe_SiServiceHandle) walker;

				if (g_si_entry == NULL)
				{
					g_si_entry = walker;
				}
				else
				{
					prevWalker->next = walker;
				}
			}
		}
	}
}

static char *modulationModes[] =
{
"UNKNOWN",
"QPSK",
"BPSK",
"OQPSK",
"VSB8",
"VSB16",
"QAM16",
"QAM32",
"QAM64",
"QAM80",
"QAM96",
"QAM112",
"QAM128",
"QAM160",
"QAM192",
"QAM224",
"QAM256",
"QAM320",
"QAM384",
"QAM448",
"QAM512",
"QAM640",
"QAM768",
"QAM896",
"QAM1024"
};

#define MOD_STRING(mode) ((mode == MPE_SI_MODULATION_QAM_NTSC) ? "NTSC" : (((mode <= 0) || (mode > MPE_SI_MODULATION_QAM1024)) ? "UNKNOWN" : modulationModes[mode]))
#define MPE_SI_OOB_FREQUENCY                0xFFFFFFFF
#define MPE_SI_HN_FREQUENCY                 0xFFFFFFFE
/* Separate OOB and DSG such that they have different
    * frequencies. We may need to support legacy OOB as well
     * as DSG at which point it will become necessary that
      * they have different internal frequency values
       * DSG frequency -3
        */
#define MPE_SI_DSG_FREQUENCY                 0xFFFFFFFD

static mpe_TimeMillis gtime_transport_stream;
static mpe_Error init_transport_stream_entry(
		mpe_SiTransportStreamEntry *ts_entry, uint32_t Frequency)
{
	mpe_Error err = MPE_SUCCESS;

	if (ts_entry == NULL)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	ts_entry->state = TSENTRY_NEW;
	ts_entry->frequency = Frequency;
	ts_entry->modulation_mode = MPE_SI_MODULATION_UNKNOWN;
	ts_entry->ts_id = TSID_UNKNOWN;
	ts_entry->ts_name = NULL;
	ts_entry->description = NULL;
	ts_entry->ptime_transport_stream = gtime_transport_stream;
	ts_entry->service_count = 0;
	ts_entry->program_count = 0;
	ts_entry->visible_service_count = 0;
	ts_entry->siStatus = SI_NOT_AVAILABLE;
	ts_entry->pat_version = INIT_TABLE_VERSION; // default version
	ts_entry->pat_crc = 0;
	ts_entry->cvct_version = INIT_TABLE_VERSION; // default version
	ts_entry->cvct_crc = 0;
	ts_entry->check_crc = FALSE;
	ts_entry->pat_reference = 0;
	ts_entry->pat_program_list = NULL;
	ts_entry->next = NULL;
	ts_entry->source_type = MPE_SOURCE_TYPE_UNKNOWN;
	ts_entry->pat_byte_array = NULL;
	ts_entry->services = NULL;
	ts_entry->programs = NULL;
	ts_entry->list_lock = NULL;

	if (Frequency == MPE_SI_OOB_FREQUENCY)
	{
		ts_entry->source_type = MPE_SOURCE_TYPE_OOB;
	}
    else if (Frequency == MPE_SI_HN_FREQUENCY)
    {
        ts_entry->source_type = MPE_SOURCE_TYPE_HN_STREAM;
    }
    else if (Frequency == MPE_SI_DSG_FREQUENCY)
    {
        ts_entry->source_type = MPE_SOURCE_TYPE_DSG;
    }
	else
	{
		ts_entry->source_type = MPE_SOURCE_TYPE_IB;
	}
	ts_entry->services = llist_create();
	ts_entry->programs = llist_create();

	err = mpe_mutexNew(&ts_entry->list_lock);
	if (err != MPE_SUCCESS)
	{
		MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
				"init_transport_stream_entry() Mutex creation failed, returning MPE_EMUTEX\n");

		if (ts_entry->services)
			llist_free(ts_entry->services);
		if (ts_entry->programs)
			llist_free(ts_entry->programs);

		return MPE_EMUTEX;
	}

	return MPE_SI_SUCCESS;
}
static mpe_SiTransportStreamEntry *g_si_oob_ts_entry = NULL;
static uint32_t get_oob_transport_stream_handle(void)
{
	uint32_t retVal = 0;

	if (g_si_oob_ts_entry == NULL)
	{
		/* Create a new mpe_SiTransportStreamEntry node */
		if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
				sizeof(mpe_SiTransportStreamEntry),
				(void **) &(g_si_oob_ts_entry)))
		{
			MPE_LOG(
					MPE_LOG_WARN,
					MPE_MOD_SI,
					"<get_oob_transport_stream_handle> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");

			return retVal;
		}

		if (MPE_SI_SUCCESS != init_transport_stream_entry(g_si_oob_ts_entry,
				MPE_SI_OOB_FREQUENCY))
		{
			mpeos_memFreeP(MPE_MEM_SI, g_si_oob_ts_entry);
			g_si_oob_ts_entry = NULL;
			return 0;
		}

	}

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
            "<get_oob_transport_stream_handle> g_si_oob_ts_entry:0x%p\n",
			g_si_oob_ts_entry);
	return (uint32_t) g_si_oob_ts_entry;
}

static mpe_SiTransportStreamEntry *g_si_HN_ts_entry = NULL;
static uint32_t get_hn_transport_stream_handle(void)
{
    uint32_t retVal = 0;

    if (g_si_HN_ts_entry == NULL)
    {
        /* Create a new mpe_SiTransportStreamEntry node */
        if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
                sizeof(mpe_SiTransportStreamEntry),
                (void **) &(g_si_HN_ts_entry)))
        {
            MPE_LOG(
                    MPE_LOG_WARN,
                    MPE_MOD_SI,
                    "<get_hn_transport_stream_handle> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");

            return retVal;
        }

        if (MPE_SI_SUCCESS != init_transport_stream_entry(g_si_HN_ts_entry,
                MPE_SI_HN_FREQUENCY))
        {
            mpeos_memFreeP(MPE_MEM_SI, g_si_HN_ts_entry);
            g_si_HN_ts_entry = NULL;
            return 0;
        }

        /* update the source type */
        g_si_HN_ts_entry->source_type = MPE_SOURCE_TYPE_HN_STREAM;

    }

    MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
            "<get_hn_transport_stream_handle> g_si_HN_ts_entry:%p\n",
            g_si_HN_ts_entry);
    return (uint32_t) g_si_HN_ts_entry;
}

static uint32_t get_dsg_transport_stream_handle(void)
{
    uint32_t retVal = 0;
    if (g_si_dsg_ts_entry == NULL)
    {
        /* Create a new mpe_SiTransportStreamEntry node */
        if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
                sizeof(mpe_SiTransportStreamEntry),
                (void **) &(g_si_dsg_ts_entry)))
        {
            MPE_LOG(
                    MPE_LOG_WARN,
                    MPE_MOD_SI,
                    "<get_dsg_transport_stream_handle> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
            return retVal;
        }
        if (MPE_SI_SUCCESS != init_transport_stream_entry(g_si_dsg_ts_entry,
                MPE_SI_DSG_FREQUENCY))
        {
            mpeos_memFreeP(MPE_MEM_SI, g_si_dsg_ts_entry);
            g_si_dsg_ts_entry = NULL;
            return 0;
        }
    }
    MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
            "<get_dsg_transport_stream_handle> g_si_dsg_ts_entry:0x%p\n",
            g_si_dsg_ts_entry);
    return (uint32_t) g_si_dsg_ts_entry;
}
static uint32_t get_transport_stream_handle(uint32_t freq,
		mpe_SiModulationMode mode)
{
	mpe_SiTransportStreamEntry *walker = NULL;
	uint32_t retVal = MPE_SI_INVALID_HANDLE;

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<get_transport_stream_handle> freq:0x%x\n", freq);
	if (freq == MPE_SI_OOB_FREQUENCY)
	{
		return get_oob_transport_stream_handle();
	}
    else if (freq == MPE_SI_HN_FREQUENCY)
    {
        return get_hn_transport_stream_handle();
    }
    else if (freq == MPE_SI_DSG_FREQUENCY)
    {
        return get_dsg_transport_stream_handle();
    }

	walker = g_si_ts_entry;
	while (walker)
	{
		if (walker->frequency == freq)
		{
			if (mode == MPE_SI_MODULATION_UNKNOWN || walker->modulation_mode
					== mode)
			{
				retVal = (uint32_t) walker;
				break;
			}
		}
		walker = walker->next;
	}

	return retVal;
}
static mpe_SiTransportStreamHandle create_transport_stream_handle(
		uint32_t freq, mpe_SiModulationMode mode)
{
	mpe_SiTransportStreamEntry *ts_entry = NULL;
	mpe_SiTransportStreamEntry *ts_walker = NULL;
	uint32_t retVal = MPE_SI_INVALID_HANDLE;

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<create_transport_stream_handle> freq: %d mode:%s\n", freq,
			MOD_STRING(mode));

	if ((retVal = get_transport_stream_handle(freq, mode))
			== MPE_SI_INVALID_HANDLE)
	{
		//MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "create_transport_stream_handle... \n");
		/* Create a new mpe_SiTransportStreamEntry node */
		if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
				sizeof(mpe_SiTransportStreamEntry), (void **) &(ts_entry)))
		{
			MPE_LOG(
					MPE_LOG_WARN,
					MPE_MOD_SI,
					"<create_transport_stream_handle> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");

			return retVal;
		}
		else
		{
			MPE_LOG(MPE_LOG_TRACE7, MPE_MOD_SI, "ts created @ %p\n", ts_entry);
		}

		if (MPE_SI_SUCCESS != init_transport_stream_entry(ts_entry, freq))
		{
			MPE_LOG(MPE_LOG_TRACE7, MPE_MOD_SI, "ts freed @ %p\n", ts_entry);
			mpeos_memFreeP(MPE_MEM_SI, ts_entry);
			return (uint32_t) NULL;
		}
		ts_entry->modulation_mode = mode;

		/* append new entry to the list */
		if ((g_si_ts_entry == NULL) || (g_si_ts_entry->frequency
				> ts_entry->frequency))
		{
			ts_entry->next = g_si_ts_entry;
			g_si_ts_entry = ts_entry;
		}
		else
		{
			ts_walker = g_si_ts_entry;
			while (ts_walker)
			{
				if ((ts_walker->next == NULL) || (ts_walker->next->frequency
						> ts_entry->frequency))
				{
					ts_entry->next = ts_walker->next;
					ts_walker->next = ts_entry;
					break;
				}
				ts_walker = ts_walker->next;
			}
		}

		retVal = (uint32_t) ts_entry;
	}
	else
	{ // Dynamic entry created by tuning.
		if (mode != MPE_SI_MODULATION_UNKNOWN)
		{
			if (((mpe_SiTransportStreamEntry *) retVal)->modulation_mode
					== MPE_SI_MODULATION_UNKNOWN)
			{
				((mpe_SiTransportStreamEntry *) retVal)->modulation_mode = mode;
			}
		}
	}
	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<create_transport_stream_handle> exit 0x%x\n", retVal);

	return retVal;
}
mpe_Error mpe_siGetTransportStreamEntryFromFrequencyModulation(uint32_t freq,
		mpe_SiModulationMode mode, mpe_SiTransportStreamHandle *ts_handle)
{
	mpe_SiTransportStreamEntry *ts = NULL;

	/* Parameter check */
	if (ts_handle == NULL)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	// create_transport_stream_handle() will return an existing ts, if it can, create new if it can't.
	ts = (mpe_SiTransportStreamEntry *) create_transport_stream_handle(freq,
			mode);
	if (ts == NULL)
	{
		MPE_LOG(
				MPE_LOG_WARN,
				MPE_MOD_SI,
				"<mpe_siGetTransportStreamEntryFromFrequencyModulation> Mem allocation (ts) failed, returning MPE_SI_OUT_OF_MEM...\n");
		return MPE_SI_OUT_OF_MEM;
	}
	*ts_handle = (mpe_SiTransportStreamHandle) ts;
	return MPE_SI_SUCCESS;
}

#define false 0
static mpe_Error init_program_info(mpe_SiProgramInfo *pi)
{
	volatile mpe_Error retVal = MPE_SI_SUCCESS;

	if (pi == NULL)
	{
		retVal = MPE_SI_INVALID_PARAMETER;
	}
	else
	{
		// Start struct in a known state.
		pi->pmt_status = PMT_NOT_AVAILABLE;
		pi->program_number = (uint32_t) - 1;
		pi->pmt_pid = (uint32_t) - 1;
		pi->pmt_version = INIT_TABLE_VERSION; // default version
		pi->crc_check = false;
		pi->pmt_crc = 0;
		pi->saved_pmt_ref = NULL;
		pi->pmt_byte_array = NULL;
		pi->number_outer_desc = 0;
		pi->outer_descriptors = NULL;
		pi->number_elem_streams = 0;
		pi->elementary_streams = NULL;
		pi->pcr_pid = (uint32_t) - 1;
		pi->services = llist_create();
		pi->service_count = 0;
		pi->stream = NULL;
	}
	return retVal;
}

static mpe_SiProgramInfo *create_new_program_info(
		mpe_SiTransportStreamEntry *ts_entry, uint32_t prog_num)
{
	mpe_SiProgramInfo *pi = NULL;

	if (ts_entry == NULL)
	{
		MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
				"<create_new_program_info> Cannot create program for NULL transport stream!\n");
	}
	else
	{
		MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
				"<create_new_program_info> Program number: %d for TS %d!\n",
				prog_num, ts_entry->frequency);

		if (prog_num > 65535)
		{
			MPE_LOG(
					MPE_LOG_WARN,
					MPE_MOD_SI,
					"<create_new_program_info> %d is not a valid program number!!\n",
					prog_num);
		}
		else
		{
			if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
					sizeof(mpe_SiProgramInfo), (void **) &(pi)))
			{
				MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
						"<create_new_program_info> Mem allocation failed, returning NULL...\n");
			}
			else
			{
				if (init_program_info(pi) != MPE_SI_SUCCESS)
				{
					MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
							"<create_new_program_info> Initialization failed, returning NULL...\n");
					mpeos_memFreeP(MPE_MEM_SI, pi);
					pi = NULL;
				}
				else
				{
					LINK *lp;
					pi->program_number = prog_num;
					pi->stream = ts_entry;
					lp = llist_mklink((void *) pi);
					llist_append(ts_entry->programs, lp);
					ts_entry->program_count++;
					MPE_LOG(
							MPE_LOG_DEBUG,
							MPE_MOD_SI,
							"<create_new_program_info> ts_entry->program_count %d\n",
							ts_entry->program_count);
				}
			}
		}
	}
	return pi;
}
mpe_Error mpe_siGetProgramEntryFromTransportStreamEntry(
		mpe_SiTransportStreamHandle ts_handle, uint32_t program_number,
		mpe_SiProgramHandle *prog_handle)
{
	mpe_SiTransportStreamEntry *ts = (mpe_SiTransportStreamEntry *) ts_handle;
	mpe_SiProgramInfo *pi = NULL;
	mpe_Error retVal = MPE_SI_SUCCESS;
	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<mpe_siGetProgramEntryFromTransportStreamEntry> 0x%x %d ...\n",
			ts_handle, program_number);

	if (prog_handle == NULL)
	{
		retVal = MPE_SI_INVALID_PARAMETER;
	}
	else
	{
		*prog_handle = MPE_SI_INVALID_HANDLE;
		/* other Parameter check */
		if ((ts_handle == MPE_SI_INVALID_HANDLE) || (program_number > 65535))
		{
			MPE_LOG(
					MPE_LOG_WARN,
					MPE_MOD_SI,
					"<mpe_siGetProgramEntryFromTransportStreamEntry> Bad parameter! %d %d...\n",
					ts_handle, program_number);
			retVal = MPE_SI_INVALID_PARAMETER;
		}
		else
		{
			if (ts->modulation_mode != MPE_SI_MODULATION_QAM_NTSC)
			{
				LINK *lp = llist_first(ts->programs);
				while (lp)
				{
					pi = (mpe_SiProgramInfo *) llist_getdata(lp);
					// We have the writelock, so list manipulation is safe.
					if (pi)
					{
						//MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "<mpe_siGetProgramEntryFromTransportStreamEntry> pi: 0x%x pi->program_number: %d...\n", pi, pi->program_number);
						if (pi->program_number == program_number)
						{
							break;
						}
					}
					lp = llist_after(lp);
				}

				if (lp == NULL)
				{
					pi = create_new_program_info(ts, program_number);
				}

				if (pi != NULL)
				{
					*prog_handle = (mpe_SiProgramHandle) pi;
					MPE_LOG(
							MPE_LOG_DEBUG,
							MPE_MOD_SI,
                            "<mpe_siGetProgramEntryFromTransportStreamEntry> prog_handle: 0x%p ...\n",
							pi);
				}
				else
				{
					MPE_LOG(
							MPE_LOG_WARN,
							MPE_MOD_SI,
							"<mpe_siGetProgramEntryFromTransportStreamEntry> Mem allocation (pi) failed, returning MPE_SI_OUT_OF_MEM...\n");
					retVal = MPE_SI_OUT_OF_MEM;
				}
			}
		}
	}
	return retVal;
}
mpe_Error mpe_siGetServiceEntryFromAppIdProgramNumber(uint32_t appId,
		uint32_t prog_num, mpe_SiServiceHandle *service_handle)
{
	mpe_SiTableEntry *walker = NULL;
	mpe_Error retVal = MPE_SI_SUCCESS;
	int found = 0;

	//MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI, "<mpe_siGetServiceEntryFromAppIdProgramNumber> appId:0x%x\n", appId );

	if (service_handle == NULL || appId == 0)
	{
		retVal = MPE_SI_INVALID_PARAMETER;
	}
	else
	{
		*service_handle = MPE_SI_INVALID_HANDLE;
		walker = g_si_entry;
		while (walker)
		{
			if ((walker->isAppType) && (walker->app_id == appId))
			{
				if ((walker->program != NULL)
						&& ((walker->program->program_number
								== PROGRAM_NUMBER_UNKNOWN)
								|| (walker->program->program_number == prog_num)))
				{
					walker->program->program_number = prog_num;
					*service_handle = (mpe_SiServiceHandle) walker;
					found = 1;
					break;
				}
			}
			walker = walker->next;
		}
	}

	/* Matching sourceId is not found, return error */
	if (!found)
	{
		retVal = MPE_SI_NOT_FOUND;
	}

	return retVal;
}
uint32_t mpe_siGetDSGTransportStreamHandle(void)
{
    return get_dsg_transport_stream_handle();
}

mpe_Error mpe_siSetSourceName(mpe_siSourceNameEntry *source_name_entry,
		char *sourceName, char *language, mpe_Bool override_existing)
{
	mpe_SiLangSpecificStringList *entry = NULL;
	char *source_name_language = NULL;

	// Parameter check
    if (sourceName == NULL)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	// If the language is NULL, this source name must have come from the LVCT/CVCT which
	// does not specify the channel name in a language specific way like the NTT_SNS
	if (language == NULL)
		source_name_language = "";
	else
		source_name_language = language;


    entry = source_name_entry->source_names;

	// Search for an existing entry for the given language
	while (entry != NULL)
	{
		// If a previous entry is found (and we are allowed to override the existing entry),
		// free the old string and allocate new space for the given string
		//COMCAST[ SI change - handle already existing source name - JIRA 1450
		if ((strcmp(entry->language,source_name_language) == 0) )
		{
			if (override_existing)
			{
				mpeos_memFreeP(MPE_MEM_SI, entry->string);

            if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
                    strlen(sourceName) + 1, (void **) &(entry->string)))
				{
                MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
                        "<mpe_siSetSourceName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
					return MPE_SI_OUT_OF_MEM;
				}
				strcpy(entry->string,sourceName);

            MPE_LOG(
                    MPE_LOG_DEBUG,
                    MPE_MOD_SI,
                    "<mpe_siSetSourceName> Replaced previous entry! -- source_name:%s, language:%s\n",
                    entry->string, entry->language);

				return MPE_SI_SUCCESS;
			}
			else
			{
				
				if (strcmp(entry->string, sourceName) == 0)
				{
					return MPE_SI_SUCCESS;
				}
			}
		//COMCAST]
		}

		entry = entry->next;
	}

	// MPE_LOG(
	// 		MPE_LOG_DEBUG,
	// 		MPE_MOD_SI,
	// 		"<mpe_siSetSourceNameForServiceHandle> source_id: 0x%x sourceName: %s\n",
	// 		si_entry->source_id, sourceName);

	// No previous entry found, so allocate a new list entry
	if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
			sizeof(struct mpe_SiLangSpecificStringList), (void **) &entry))
	{
		MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
				"<mpe_siSetSourceName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
		return MPE_SI_OUT_OF_MEM;
	}

	// Set the source_name
	if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI, strlen(sourceName) + 1,
			(void **) &(entry->string)))
	{
		MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
				"<mpe_siSetSourceName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
		mpeos_memFreeP(MPE_MEM_SI,(void *)entry);
		return MPE_SI_OUT_OF_MEM;
	}
	strcpy(entry->string, sourceName);
	strncpy(entry->language, source_name_language, 4);

	// Add new list entry to the front of list for this service
    entry->next = source_name_entry->source_names;
    source_name_entry->source_names = entry;

	MPE_LOG(
            MPE_LOG_TRACE6,
			MPE_MOD_SI,
			"<mpe_siSetSourceName> Created new entry! -- source_name:%s, language:%s\n",
			entry->string, entry->language);

	return MPE_SI_SUCCESS;
}


mpe_Error mpe_siSetSourceLongName(mpe_siSourceNameEntry *source_name_entry,
		char *sourceLongName, char *language)
{
	mpe_SiLangSpecificStringList *entry = NULL;

	// Parameter check
    if ((sourceLongName == NULL)
			|| (language == NULL))
	{
		return MPE_SI_INVALID_PARAMETER;
	}


    entry = source_name_entry->source_long_names;

	// Search for an existing entry for the given language
	while (entry != NULL)
	{
		// If a previous entry is found, free the old string and allocate new space for the
		// given string
		if (strcmp(entry->language, language) == 0)
		{
			mpeos_memFreeP(MPE_MEM_SI, entry->string);

			if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI, strlen(
					sourceLongName) + 1, (void **) &(entry->string)))
			{
				MPE_LOG(
						MPE_LOG_WARN,
						MPE_MOD_SI,
						"<mpe_siSetSourceLongName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
				return MPE_SI_OUT_OF_MEM;
			}
			strcpy(entry->string, sourceLongName);

			MPE_LOG(
					MPE_LOG_DEBUG,
					MPE_MOD_SI,
					"<mpe_siSetSourceLongName> Replaced previous entry! -- source_name:%s, language:%s\n",
					entry->string, entry->language);

			return MPE_SI_SUCCESS;
		}
		entry = entry->next;
	}

	// No previous entry found, so allocate a new list entry
	if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI,
			sizeof(struct mpe_SiLangSpecificStringList), (void **) &entry))
	{
		MPE_LOG(
				MPE_LOG_WARN,
				MPE_MOD_SI,
				"<mpe_siSetSourceLongName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
		return MPE_SI_OUT_OF_MEM;
	}

	// Set the source_name
	if (MPE_SI_SUCCESS != mpeos_memAllocP(MPE_MEM_SI, strlen(sourceLongName)
			+ 1, (void **) &(entry->string)))
	{
		MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
				"<mpe_siSetSourceName> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
		mpeos_memFreeP(MPE_MEM_SI, (void *)entry);
		return MPE_SI_OUT_OF_MEM;
	}
	strcpy(entry->string, sourceLongName);
	strncpy(entry->language, language, 4);

	// Add new list entry to the front of list for this service
    entry->next = source_name_entry->source_long_names;
    source_name_entry->source_long_names = entry;

	MPE_LOG(
			MPE_LOG_DEBUG,
			MPE_MOD_SI,
			"<mpe_siSetSourceLongName> Created new entry! -- source_name:%s, language:%s\n",
			entry->string, entry->language);

	return MPE_SI_SUCCESS;
}
mpe_siSourceNameEntry *g_si_sourcename_entry = NULL;
mpe_Error mpe_siGetSourceNameEntry(uint32_t id, mpe_Bool isAppId,  mpe_siSourceNameEntry **source_name_entry, mpe_Bool create)
{
    mpe_siSourceNameEntry *sn_walker;
    mpe_Error retVal = MPE_SI_SUCCESS;
    MPE_LOG(MPE_LOG_TRACE6, MPE_MOD_SI, "<mpe_siGetSourceNameEntry> id:0x%x (%s)\n",
            id, isAppId ? "AppID" : "SourceID");
    if ((source_name_entry == NULL) || ((id == 0) || (id > 65535)))
    {
        printf("Error in %s, %p, %d\r\n", __FUNCTION__, source_name_entry, id);
        retVal = MPE_SI_INVALID_PARAMETER;
    }
    else
    {
        // FOR NOW Assume write lock already obtained!!!!
        *source_name_entry = NULL;
        sn_walker = g_si_sourcename_entry;
        while (sn_walker != NULL)
        {
            if (sn_walker->id == id && (sn_walker->appType == isAppId))
            {
                *source_name_entry = sn_walker;
                break;
            }
            else
            {
              sn_walker = sn_walker->next;
            }
        }
        //
        // If walker is NULL, entry not found
        if (sn_walker==NULL && create)
        {
            mpe_Error tRetVal = mpeos_memAllocP(MPE_MEM_SI, sizeof(mpe_siSourceNameEntry), (void **) &(sn_walker));
            if ((MPE_SI_SUCCESS != tRetVal) || (sn_walker == NULL))
            {
                MPE_LOG(MPE_LOG_WARN, MPE_MOD_SI,
                        "<mpe_siGetSourceNameEntry> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
                retVal = MPE_SI_OUT_OF_MEM;
            }
            else
            {
                sn_walker->appType = isAppId;
                sn_walker->mapped = false;
                sn_walker->id = id;
                sn_walker->source_long_names = NULL;
                sn_walker->source_names = NULL;
                // just put it at the front.
                sn_walker->next = g_si_sourcename_entry;
                g_si_sourcename_entry = sn_walker;
                *source_name_entry = sn_walker;
            }
        }
    }
    MPE_LOG(MPE_LOG_TRACE6, MPE_MOD_SI,  "<mpe_siGetSourceNameEntry> id:0x%x (%s), *sn_entry=%p\n",
            id, isAppId ? "AppID" : "SourceID", *source_name_entry);
    return retVal;
}
mpe_Error mpe_siCreateDSGServiceHandle(uint32_t appID, uint32_t prog_num,
		char* service_name, char* language, mpe_SiServiceHandle *service_handle)
{
	mpe_SiTableEntry *walker = NULL;
	mpe_SiTableEntry *new_si_entry = NULL;
	mpe_SiTransportStreamEntry *ts = NULL;
	int len = 0;
	int found = 0;
	int inTheList = 0;
	mpe_SiProgramInfo *pgm = NULL;
	mpe_Error tRetVal = MPE_SI_SUCCESS;

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<mpe_siCreateDSGServiceHandle> appID= %d\n", appID);

	// Parameter check
	if (service_handle == NULL)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	// Clear out the value
	*service_handle = MPE_SI_INVALID_HANDLE;

	// Program number for application tunnels?
    ts = (mpe_SiTransportStreamEntry *) get_dsg_transport_stream_handle();

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
            "<mpe_siCreateDSGServiceHandle> transport stream entry...0x%p\n",
			ts);
      if(ts->list_lock) {
	mpe_mutexAcquire(ts->list_lock);
	{
		LINK *lp = llist_first(ts->programs);
		while (lp && !found)
		{
			pgm = (mpe_SiProgramInfo *) llist_getdata(lp);
			if (pgm)
			{
				LINK *lp1 = llist_first(pgm->services);
				while (lp1)
				{
					walker = (mpe_SiTableEntry *) llist_getdata(lp1);
					if (walker != NULL && walker->app_id == appID)
					{
						new_si_entry = walker;
						inTheList = true;
						found = true;
						break;
					}
					lp1 = llist_after(lp1);
				}
			}
			lp = llist_after(lp);
		}

		if (lp == NULL)
		{
			// Create a place holder to store program info
			pgm = create_new_program_info(ts, prog_num);
			MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
                    "<mpe_siCreateDSGServiceHandle> program Entry...0x%p\n",
					pgm);
			if (pgm == NULL)
			{
				mpe_mutexRelease(ts->list_lock);
				return MPE_SI_OUT_OF_MEM;
			}
			// newly created pgm is attached to ts by create_new_program_info.
		}
	}
     }

	if (new_si_entry == NULL)
	{
		tRetVal = mpeos_memAllocP(MPE_MEM_SI, sizeof(mpe_SiTableEntry),
				(void **) &(new_si_entry));
		if ((MPE_SI_SUCCESS != tRetVal) || (new_si_entry == NULL))
		{
			MPE_LOG(
					MPE_LOG_WARN,
					MPE_MOD_SI,
					"<mpe_siCreateDSGServiceHandle> Mem allocation failed, returning MPE_SI_OUT_OF_MEM...\n");
			mpe_mutexRelease(ts->list_lock);
			return MPE_SI_OUT_OF_MEM;
		}

		MPE_LOG(
				MPE_LOG_DEBUG,
				MPE_MOD_SI,
                "<mpe_siCreateDSGServiceHandle> creating new SI Entry...0x%p\n",
				new_si_entry);

		/* Initialize all the fields (some fields are set to default values as defined in spec) */
		init_si_entry(new_si_entry);
		new_si_entry->isAppType = TRUE;
		new_si_entry->app_id = appID;
		found = true;

		// Link into other structs.
		if (pgm != NULL)
		{
			LINK *lp = llist_mklink((void *) new_si_entry);
			MPE_LOG(MPE_LOG_TRACE7, MPE_MOD_SI,
					"Add service %p to program %p...\n", new_si_entry, pgm);
			llist_append(pgm->services, lp);
			pgm->service_count++;
		}
		{
			LINK *lp = llist_mklink((void *) new_si_entry);
			llist_append(ts->services, lp);
			ts->service_count++;
			MPE_LOG(MPE_LOG_TRACE7, MPE_MOD_SI,
					"Add service %p to transport %p...\n", new_si_entry, ts);
		}
	}
	else
	{
		found = true;
	}

	if (service_name != NULL)
		len = strlen(service_name);

	if (len > 0)
	{
        //
        // Code modified to look up source name entry as this information moved to another table.
        mpe_siSourceNameEntry *source_name_entry = NULL;

        //
        // Look up, but allow a create, the source name by appID
        mpe_siGetSourceNameEntry(appID, TRUE, &source_name_entry, TRUE);
        if (source_name_entry != NULL)
			{
          mpe_siSetSourceName(new_si_entry->source_name_entry, service_name, language, FALSE);
          mpe_siSetSourceLongName(new_si_entry->source_name_entry, service_name, language);
		}
	}
        if (ts->list_lock) { 
	   mpe_mutexRelease(ts->list_lock);
        }

	if (found)
	{
		// ServiceEntry pointers
		new_si_entry->ts_handle = (mpe_SiTransportStreamHandle) ts;
		new_si_entry->program = pgm;
		new_si_entry->source_id = SOURCEID_UNKNOWN;

		MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
				"<mpe_siCreateDSGServiceHandle> Inserting service entry in the list \n");

		if (g_SITPConditionSet == TRUE)
		{
			(ts->service_count)++;
			(ts->visible_service_count)++;
		}
		new_si_entry->ref_count++;

		*service_handle = (mpe_SiServiceHandle) new_si_entry;

		/* append new entry to the dynamic list */
		if(!inTheList)
		{
			if (g_si_entry == NULL)
			{
				g_si_entry = new_si_entry;
			}
			else
			{
				walker = g_si_entry;
				while (walker)
				{
					if (walker->next == NULL)
					{
						walker->next = new_si_entry;
						break;
					}
					walker = walker->next;
				}
			}
		}
	}

	MPE_LOG(MPE_LOG_DEBUG, MPE_MOD_SI,
			"<mpe_siCreateDSGServiceHandle> done, returning MPE_SI_SUCCESS\n");
	return MPE_SI_SUCCESS;
}

void mpe_siSetServiceEntryStateMapped(mpe_SiServiceHandle service_handle)
{
	mpe_SiTableEntry * si_entry = (mpe_SiTableEntry *) service_handle;

	MPE_LOG(MPE_LOG_TRACE7, MPE_MOD_SI,
			"<mpe_siSetServiceEntryStateMapped> ...service_handle:0x%x\n",
			service_handle);

	/* Parameter check */
	if (service_handle == MPE_SI_INVALID_HANDLE)
	{
		MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI,
				"<mpe_siSetServiceEntryStateMapped> MPE_SI_INVALID_PARAMETER\n");
	}
	else
	{
		si_entry->state |= SIENTRY_MAPPED;
	}
}

static mpe_Error write_name_string(mpe_File cache_fd, mpe_SiLangSpecificStringList *name, uint32_t *write_count);
static mpe_Error read_name_string(mpe_File cache_fd, mpe_SiLangSpecificStringList *name, uint32_t *read_count, int total_bytes);;
uint32_t g_maxFrequency = 0;
uint32_t g_minFrequency = 0;

void get_frequency_range(uint32_t *freqArr, int count, uint32_t *minFreq, uint32_t *maxFreq )
{
    int i = 1;
    uint32_t minVal = 0, maxVal = 0;

    if(count && freqArr[0])
    {

    	minVal = freqArr[0];
        maxVal = freqArr[0];;
        while ((i < count) && freqArr[i])
        {
            if (maxVal < freqArr[i])
            {
                maxVal = freqArr[i];
            }
            else if(minVal > freqArr[i])
            {
            	minVal = freqArr[i];
            }
            i++;
        }
    }

    *minFreq = minVal;
    *maxFreq = maxVal;
    MPE_LOG(MPE_LOG_INFO, MPE_MOD_SI,"<get_frequency_range> g_minFrequency: %d, g_maxFrequency: %d count:%d \n", *minFreq, *maxFreq, count);
    
    return;
}

mpe_Error mpe_siSetTransportType(mpe_SiServiceHandle service_handle, uint8_t transportType)
{
    mpe_SiTableEntry * si_entry = (mpe_SiTableEntry *) service_handle;
    /* Parameter check */
    if (service_handle == MPE_SI_INVALID_HANDLE)
    {
        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI,
                "<mpe_siSetTransportType> MPE_SI_INVALID_PARAMETER\n");
    }
    else
    {
        si_entry->transport_type = transportType;
    }
    return MPE_SI_SUCCESS;
}

mpe_Error mpe_siSetScrambled(mpe_SiServiceHandle service_handle, mpe_Bool scrambled)
{
    mpe_SiTableEntry * si_entry = (mpe_SiTableEntry *) service_handle;
    /* Parameter check */
    if (service_handle == MPE_SI_INVALID_HANDLE)
    {
        MPE_LOG(MPE_LOG_ERROR, MPE_MOD_SI,
                "<mpe_siSetScrambled> MPE_SI_INVALID_PARAMETER\n");
    }
    else
    {
        si_entry->scrambled = scrambled;
    }
    return MPE_SI_SUCCESS;
}
mpe_Error mpe_siSetProgramNumber(mpe_SiServiceHandle service_handle,
		uint32_t prog_num)
{
	mpe_SiTableEntry *si_entry = (mpe_SiTableEntry *) service_handle;

	/* Parameter check */
	if (service_handle == MPE_SI_INVALID_HANDLE)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	/* check for invalid Progam number (max 16 bits) */
	if (prog_num > 0xffff)
	{
		return MPE_SI_INVALID_PARAMETER;
	}

	/* Set the program number */
    si_entry->program_number = prog_num;

	return MPE_SI_SUCCESS;
}

void mpe_siUpdateServiceEntries()
{
    mpe_SiTransportStreamHandle ts_handle = MPE_SI_INVALID_HANDLE;
    mpe_SiProgramHandle prog_handle = MPE_SI_INVALID_HANDLE;
    uint32_t frequency_by_cds_ref = 0;
    mpe_SiModulationMode mode_by_mms_ref = MPE_SI_MODULATION_UNKNOWN;
    mpe_SiTableEntry *walker = NULL;
    mpe_siSourceNameEntry *source_name_entry = NULL;
    // Lock should be acquired by the caller
    walker = g_si_entry;
    while (walker)
    {
        frequency_by_cds_ref = 0;
        mode_by_mms_ref = MPE_SI_MODULATION_UNKNOWN;
        // For each service entry
        frequency_by_cds_ref = g_frequency[walker->freq_index];
        if (walker->transport_type == NON_MPEG_2_TRANSPORT)
        {
            mode_by_mms_ref = MPE_SI_MODULATION_QAM_NTSC;
        }
        else
        {
            mode_by_mms_ref = g_mode[walker->mode_index];
        }

        if((frequency_by_cds_ref != 0)  && (mode_by_mms_ref != MPE_SI_MODULATION_UNKNOWN))
        {
            mpe_siGetTransportStreamEntryFromFrequencyModulation(frequency_by_cds_ref, mode_by_mms_ref, &ts_handle);
            (void) mpe_siGetProgramEntryFromTransportStreamEntry(ts_handle, walker->program_number, &prog_handle);
            // Update the service entry with transport stream and program info
            mpe_siUpdateServiceEntry((mpe_SiServiceHandle)walker, ts_handle, prog_handle);
        }
        MPE_LOG(MPE_LOG_TRACE6,
                MPE_MOD_SI,
                "<mpe_siUpdateServiceEntries> - frequency(%d) = (%d) modulation(%d) = %d, state %d.\n",
                frequency_by_cds_ref, g_frequency[walker->freq_index], mode_by_mms_ref, g_mode[walker->mode_index], walker->state);
        //
        // Now, find the source name entry and update
        source_name_entry = NULL;
        mpe_siGetSourceNameEntry(walker->isAppType ? walker->app_id : walker->source_id, walker->isAppType,
                                 &source_name_entry, FALSE);
        if (source_name_entry != NULL)
        {
          // If there is a pre-existing source_name_entry for this walker that means the name changed?
          // If source name info changed, signal MPE_SI_EVENT_SERVICE_DETAILS_UPDATE (MODIFY) event..??
          walker->source_name_entry = source_name_entry;
          source_name_entry->mapped = TRUE;
        }
        else
        {
          MPE_LOG(MPE_LOG_TRACE9, MPE_MOD_SI,
                  "<%s::si_FinalizeSIAfterAcquisition> - ERROR SourceName not found for id %d of type %s\n",
                  SIMODULE, walker->isAppType ? walker->app_id : walker->source_id, walker->isAppType ? "APP" : "SRC");
        }
        walker = walker->next;
    }
    //
    // Go through the source name table and find all entries that have not been mapped
    source_name_entry = g_si_sourcename_entry;

    while(source_name_entry != NULL)
    {
        if (!source_name_entry->mapped)
        {
            mpe_SiServiceHandle si_database_handle;
            // Call the create DSG Service handle, but no need to pass the soure name stuff, we already have it.
            mpe_siCreateDSGServiceHandle((uint32_t)source_name_entry->id, PROGRAM_NUMBER_UNKNOWN, NULL, NULL, &si_database_handle);
            // Set the newly returned database handle entry's source name entry
            if (si_database_handle != MPE_SI_INVALID_HANDLE)
            {
                ((mpe_SiTableEntry *)si_database_handle)->source_name_entry = source_name_entry;
                source_name_entry->mapped = TRUE;   // this source name now mapped to an si db entry
                
            }
        }
        source_name_entry = source_name_entry->next;
    }
}

mpe_Error mpe_fileOpen(const char *location, int flag, int *fd)
{
    if (flag == MPE_FS_OPEN_READ) {
        *fd = open(location, O_RDONLY);
    }
    else if ((flag & MPE_FS_OPEN_WRITE) || (flag & MPE_FS_OPEN_MUST_CREATE)) {
        *fd = open(location, O_RDWR|O_CREAT, 0666);
    }
    else
    {
        *fd = -1;
    }

    if(*fd >=0 )
    {
        lseek(*fd, 0, SEEK_SET);
    }
    return *fd >= 0 ? MPE_SUCCESS : MPE_SI_NOT_FOUND;
}

mpe_Error mpe_fileRead(int fd, int *size, char *buf)
{
    int count = 0;
    count = read(fd, buf, *size);
    if (count >= 0) {
        *size = count;
        return MPE_SUCCESS;
    }
    else {
        return MPE_SI_INVALID_HANDLE;
    }
    
}

mpe_Error mpe_fileDelete(const char *location)
{
    unlink(location);
    return MPE_SUCCESS;
}

mpe_Error mpe_fileClose(int fd) 
{
    //All error codes for close are not mapped for MPE_* errors, hence returning INVALID HANDLE for error case.
    return close(fd) == 0 ? MPE_SUCCESS : MPE_SI_INVALID_HANDLE;
}


mpe_Error mpe_fileWrite(int fd, int *count, char *buf) 
{
    *count = write(fd, buf, *count);
    return *count >= 0 ? MPE_SUCCESS : MPE_SI_INVALID_HANDLE;
}
