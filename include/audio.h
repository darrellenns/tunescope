#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
#include <jack/jack.h>
#include "tone.h"

class Audio
{
    public:
        Audio(int buffer_count=128);
        ~Audio();
        int process(int nframes);

        inline int getRate() {return jack_get_sample_rate(this->client);};

        int setBlockSize(int block_size);//must be public as it's a callback, but don't use it publicly
        inline int getBlockSize() { return this->block_size; };
        int getSampleRate();
        int getBuffer(float** buffer);
        bool dataAvailable();

    private:
        jack_default_audio_sample_t* buffer;
        jack_client_t* client;
        jack_port_t* in_port;
        int current_block;//only write to this in rt thread
        int current_block_read;//only write to this in non-rt thread
        int block_count;
        int block_size;
};
inline int audio_process_cb(jack_nframes_t data_len, void* p) {return static_cast<Audio*>(p)->process(data_len);}
inline int audio_blocksize_cb(jack_nframes_t block_size, void* p) {return static_cast<Audio*>(p)->setBlockSize(block_size);}



#endif // AUDIO_H_INCLUDED
