
typedef struct sample {
    long samprate;
    long nchannels;
    long sampwidth;
    long nsamples;
    short *data;
} sample;

sample *readsample(const char *);
void playsample(sample *);
void playsubsample(sample *s,int start, int finish);
void writesample(sample *s, char *name);
void flushsample(void);
void printsample(sample *s);
void expandsample(sample *s);
void freesample(sample *s);


int hack_sginap(long ticks);
