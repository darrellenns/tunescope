#include "audio.h"
#include "tone.h"
#include <iostream>
using namespace std;

Audio::Audio(int block_count)
{
    this->block_count=block_count;
    this->buffer=NULL;

    if ((this->client = jack_client_open ("tunerscope",JackNoStartServer,NULL)) == 0) {
        cerr<<"jack server not running?"<<endl;
        throw;
    }
    this->in_port=jack_port_register(this->client,"input",JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
    jack_set_process_callback(client,audio_process_cb,this);
    jack_set_buffer_size_callback(client,audio_blocksize_cb,this);
    if (jack_activate(client)) {
        cerr<<"cannot activate client"<<endl;
        throw;
    }

    //DEBUG
    //jack_connect(this->client,"system:capture_1","tunerscope:input");
    //jack_connect(this->client,"jaaa:out_1","tunerscope:input");
    //jack_connect(this->client,"system:capture_2","tunerscope:input");

}
Audio::~Audio()
{
    if (this->buffer) delete this->buffer;
}

int Audio::setBlockSize(int block_size)
{
    if(this->buffer) delete this->buffer;
    this->buffer=new float[block_size*this->block_count];
    this->block_size=block_size;
    this->current_block=0;
    this->current_block_read=0;
    return 0;
}

int Audio::process(int nframes)
{
    jack_default_audio_sample_t* input=(jack_default_audio_sample_t*)jack_port_get_buffer(this->in_port,nframes);
    this->current_block%=this->block_count;
    float* p=this->buffer+nframes*this->current_block;

    for(int i=0;i<nframes;i++)
        *(p++)=*(input++);

    this->current_block++;

    return 0;
}

int Audio::getBuffer(float** buffer)
{
    if(this->current_block_read==this->current_block) return 0;
    this->current_block_read%=this->block_count;
    *buffer=this->buffer+(this->current_block_read++)*this->block_size;
    return this->block_size;
}

bool Audio::dataAvailable()
{
    return this->current_block_read!=this->current_block;
}

int Audio::getSampleRate()
{
    return jack_get_sample_rate(this->client);
}
