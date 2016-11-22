#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>


#include "audio.h"
#include "tone.h"

class GUI
{
    public:
        GUI();
        ~GUI();
        void init(int argc,char** argv);
        void render();
        void keypress(unsigned char key,int x,int y);
        void timer(int value);
        void resize(int w,int h);
        void run() {glutMainLoop();};
        void setAudio(Audio* audio);
    private:
        Audio* audio;
        TunerReference* ref;
        void renderFade();
        void renderReticle();
        void renderXYTrace(float* buf,int count);
        void renderNormTrace(float* buf,int count);
        int block_tick_mod;
        double agc_coeff;

};

extern GUI gui;

void gui_evtKeypress(unsigned char key, int x, int y);
void gui_evtTimer(int value);
void gui_evtRender(void);
void gui_evtResize(int w, int h);

#endif // GUI_H_INCLUDED

