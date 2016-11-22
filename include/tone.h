#ifndef TONE_H_INCLUDED
#define TONE_H_INCLUDED

class TunerReference
{
    public:
        void process(float* audio,int count);
        int selectnote();
        TunerReference();
        TunerReference(int samplerate);
        ~TunerReference();
        void setSampleRate(int samplerate);
        double notefreq(int note);
        float nextSin();
        float nextPhasor();
        inline float getSin() {return this->t_val[1];};
        inline float getSinPrev() {return this->t_val[2];};

        inline float getPhasor() {return this->phasor;};
        inline float getPhasorPrev() {float x;x=this->phasor-this->phasor_coeff[this->activenote];return x-(int)x;};

        const char* getNoteName();
    private:
        void seedOsc();
        int activenote;
        int samplerate;
        static const int refcount=7;
        static const double notes[];
        static const char* notename[];
        double coeff[refcount];
        double osc_seed[refcount];
        double phasor_coeff[refcount];
        double t_val[3];//tone IIR samples
        double g_val[refcount][3];//goertzel IIR samples
        double phasor;
};
#endif // TONE_H_INCLUDED
