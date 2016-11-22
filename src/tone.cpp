#include "tone.h"
#include <math.h>
#include <iostream>

const double TunerReference::notes[]={-31,-29,-24,-19,-14,-10,-5};
const char* TunerReference::notename[]={"D","E","A","D","G","B","E"};

const char* TunerReference::getNoteName()
{
    return this->notename[this->activenote];
}

TunerReference::TunerReference()
{
    this->activenote=0;
    this->setSampleRate(44100);
}
TunerReference::~TunerReference()
{

}

TunerReference::TunerReference(int samplerate)
{
    this->activenote=0;
    this->setSampleRate(samplerate);
}

int TunerReference::selectnote()
{
    double pwr[refcount];
    double avg_pwr=0;
    //power = s_prev2*s_prev2 + s_prev*s_prev - coeff*s_prev*s_prev2;
    for(int x=0;x<this->refcount;x++)
    {
        pwr[x]=g_val[x][2]*g_val[x][2]+g_val[x][1]*g_val[x][1]-coeff[x]*g_val[x][1]*g_val[x][2];
        avg_pwr+=pwr[x];
        g_val[x][0]=0;
        g_val[x][1]=0;
        g_val[x][2]=0;
    }
    avg_pwr/=refcount;
    double threshold=0.5*avg_pwr;

    for(int x=0;x<this->refcount;x++)
    {
        if(pwr[x]>threshold)
        {
            if(x!=this->activenote)
            {
                this->activenote=x;
                this->seedOsc();
            }
            break;
        }
    }
    return this->activenote;
}

void TunerReference::seedOsc()
{
        this->t_val[0]=this->osc_seed[this->activenote];
        this->t_val[1]=0;
        this->t_val[2]=0;
        this->phasor=0;
}

//process audio using the goertzel algorithm
void TunerReference::process(float* audio,int count)
{
    for(int i=0;i<count;i++)
    {
        for(int x=0;x<this->refcount;x++)
        {
            this->g_val[x][0]=audio[i]+this->coeff[x]*this->g_val[x][1]-this->g_val[x][2];
            this->g_val[x][2]=this->g_val[x][1];
            this->g_val[x][1]=this->g_val[x][0];

        }
    }
}

void TunerReference::setSampleRate(int samplerate)
{
    this->samplerate=samplerate;
    for(int i=0;i<this->refcount;i++)
    {
        double w=2*M_PI*this->notefreq(this->notes[i])/this->samplerate;
        this->coeff[i]=2.0*cos(w);
        this->osc_seed[i]=-sin(w);
        this->phasor_coeff[i]=this->notefreq(this->notes[i])/this->samplerate;
    }

    this->seedOsc();

    for(int i=0;i<3;i++)
    {
        for(int x=0;x<this->refcount;x++)
            g_val[x][i]=0;
    }
}

double TunerReference::notefreq(int note)
{
    return 440*pow(pow(2.0,1.0/12.0),note);
}

float TunerReference::nextSin()
{
    this->t_val[2]=this->t_val[1];
    this->t_val[1]=this->t_val[0];
    this->t_val[0]=this->coeff[this->activenote]*this->t_val[1]-this->t_val[2];
    return this->getSin();
}

float TunerReference::nextPhasor()
{
    this->phasor+=this->phasor_coeff[this->activenote];
    this->phasor-=(int)this->phasor;
    return this->phasor;
}
