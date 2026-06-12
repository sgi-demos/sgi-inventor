/*
 * AIFF format header file
 *
 * bytes are stored in 68000 = big endian order
 */ 

/*
 * some of the audio parameters in an AIFF file
 */
typedef struct
{
    long samprate;
    long nchannels;
    long sampwidth;
} audio_params_t;

/*
 * all chunks consist of a chunk header followed by some data
 */
typedef struct
{
    char id[4];
    long size;
} chunk_header_t;

#define CHUNK_ID     4
#define CHUNK_HEADER 8

typedef struct
{
    chunk_header_t header;
    int file_position;  /* not in AIFF file */
    char type[4]; /* should contain 'AIFF' for any audio IFF file */
} form_chunk_t;

#define FORM_CHUNK      12  /* including the header */ 
#define FORM_CHUNK_DATA 4   

#define COMM_CHUNK      26   /* including the header */
#define COMM_CHUNK_DATA 18

typedef struct
{
    chunk_header_t header;
    int file_position;            /* not in AIFF file */
    short nchannels;
    unsigned long nsampframes;
    short sampwidth;
    long samprate;              /* not in AIFF file */
} comm_chunk_t;


#define SSND_CHUNK      16   /* including the header */
#define SSND_CHUNK_DATA 8

typedef struct
{
    chunk_header_t header;
    unsigned long offset;
    unsigned long blocksize;

    long file_position; /* not in AIFF file */
    long sample_bytes; /* not in AIFF file */
} ssnd_chunk_t;
