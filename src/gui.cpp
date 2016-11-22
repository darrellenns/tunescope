#include "gui.h"
GUI gui;

#include "tone.h"
#include "math.h"

#include <iostream>
using namespace std;

//callback function wrappers
void gui_evtKeypress(unsigned char key, int x, int y){gui.keypress(key,x,y);}
void gui_evtTimer(int value){gui.timer(value);}
void gui_evtRender(void){gui.render();}
void gui_evtResize(int w, int h){gui.resize(w,h);}

GUI::GUI()
{
    this->ref=new TunerReference();//TODO: use actual sample rate
}

GUI::~GUI()
{
    delete this->ref;
}

void GUI::setAudio(Audio* audio)
{
    this->audio=audio;
    double sr=this->audio->getSampleRate();

    //recreate object with correct samplerate
    delete this->ref;
    this->ref=new TunerReference(sr);

    this->agc_coeff=sr/(sr+5.0);
}

void GUI::init(int argc,char** argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);//use double buffering

    glutCreateWindow("Tuning Scope");

    //enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //callback functions
    glutReshapeFunc(gui_evtResize);
    glutKeyboardFunc(gui_evtKeypress);
    glutIdleFunc(gui_evtRender);
    glutDisplayFunc(gui_evtRender);
    //glutTimerFunc(1000,gui_evtTimer,0);

    //read operations use the front buffer (needed for blur effect)
    glReadBuffer(GL_FRONT);

}

//fade out by drawing a transparent black quad
void GUI::renderFade()
{
    glBegin(GL_QUADS);
    glColor4f(0,0,0,0.2);//this "fudge factor" depends on frame rate, not audio samplerate
    glVertex2f(-2,-2);
    glVertex2f(-2,2);
    glVertex2f(2,2);
    glVertex2f(2,-2);
    glEnd();
}

void GUI::renderReticle()
{
    //draw reticle
    glBegin(GL_LINES);
    glColor4f(1,1,0,1.0);
    glVertex2f(0,1);glVertex2f(0,-1);
    glVertex2f(1,0);glVertex2f(-1,0);
    glEnd();

    //draw text
    glColor4f(1,1,0,1.0);
    glRasterPos2f(-0.95,-0.95);
    glutBitmapString(GLUT_BITMAP_9_BY_15,(const unsigned char *)this->ref->getNoteName());
}
void GUI::renderXYTrace(float* buf,int count)
{
    static float peakenv=1;
    static float last_sample=0;
    //draw xy scope trace
    glBegin(GL_LINE_STRIP);
    //glColor4f(0,1,0,0);//smooth out transitions between buffers
    glColor4f(0,1,0,1);
    glVertex2f(this->ref->getSinPrev(),last_sample);
    for(int i=0;i<count;i++)
    {
        if(fabsf(buf[i])>peakenv) peakenv=fabsf(buf[i]);
        if(peakenv>0.00001) peakenv*=this->agc_coeff;
        //glColor4f(0,1,0,(1-fabsf((float)i/count*2-1))*2);//smooth out transitions between buffers
        glVertex2f(this->ref->nextSin(),buf[i]/peakenv);
    }
    last_sample=buf[count-1]/peakenv;
    glEnd();
}

void GUI::renderNormTrace(float* buf,int count)
{
    static float last_sample=0;
    static float phasor_prev=0;
    static float peakenv=1;

    glBegin(GL_LINE_STRIP);
    glColor4f(1,0,0,1);

    glVertex2f(phasor_prev*2-1,last_sample/peakenv);
    for(int i=0;i<count;i++)
    {
        if(fabsf(buf[i])>peakenv) peakenv=fabsf(buf[i]);
        if(peakenv>0.00001) peakenv*=this->agc_coeff;

        phasor_prev=this->ref->getPhasor();
        if(this->ref->nextPhasor()<phasor_prev)
        {
            glEnd();
            glBegin(GL_LINE_STRIP);
        }
        glVertex2f(this->ref->getPhasor()*2-1,buf[i]/peakenv);
    }
    glEnd();
    last_sample=buf[count-1];
    phasor_prev=this->ref->getPhasor();
}

void GUI::render()
{
    float* buf;
    int count;
    static int frame_tick=0;
    const int frame_tick_mod=10;

    (++frame_tick)%=frame_tick_mod;
    if(frame_tick==0) this->ref->selectnote();

    glClear(GL_COLOR_BUFFER_BIT);

    //copy front buffer to back buffer
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    glBlitFramebuffer(vp[0],vp[1],vp[2],vp[3],vp[0],vp[1],vp[2],vp[3],GL_COLOR_BUFFER_BIT,GL_NEAREST);

    //fade old display
    this->renderFade();

    //draw new display
    while((count=this->audio->getBuffer(&buf))!=0)
    {
        this->ref->process(buf,count);

        this->renderXYTrace(buf,count);
        this->renderNormTrace(buf,count);
    }

    //draw reticle
    this->renderReticle();

    glutSwapBuffers();

}

void GUI::keypress(unsigned char key,int x,int y)
{
    switch ( key ) {
    case 27://escape
        exit(0);
    }
}

void GUI::timer(int value)
{
    //glutTimerFunc(1000,gui_evtTimer,0);
}

void GUI::resize(int w,int h)
{
    const double Xmin = -1.0, Xmax = 1.0;
    const double Ymin = -1.0, Ymax = 1.0;

    double scale, center;
    double windowXmin, windowXmax, windowYmin, windowYmax;

    // Define the portion of the window used for OpenGL rendering.
    glViewport( 0, 0, w, h );       // View port uses whole window

    // Set up the projection view matrix: orthographic projection
    // Determine the min and max values for x and y that should appear in the window.
    // The complication is that the aspect ratio of the window may not match the
    //              aspect ratio of the scene we want to view.
    w = (w==0) ? 1 : w;
    h = (h==0) ? 1 : h;
    if ( (Xmax-Xmin)/w < (Ymax-Ymin)/h ) {
        scale = ((Ymax-Ymin)/h)/((Xmax-Xmin)/w);
        center = (Xmax+Xmin)/2;
        windowXmin = center - (center-Xmin)*scale;
        windowXmax = center + (Xmax-center)*scale;
        windowYmin = Ymin;
        windowYmax = Ymax;
    }
    else {
        scale = ((Xmax-Xmin)/w)/((Ymax-Ymin)/h);
        center = (Ymax+Ymin)/2;
        windowYmin = center - (center-Ymin)*scale;
        windowYmax = center + (Ymax-center)*scale;
        windowXmin = Xmin;
        windowXmax = Xmax;
    }
    // Now that we know the max & min values for x & y that should be visible in the window,
    //              we set up the orthographic projection.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( windowXmin, windowXmax, windowYmin, windowYmax, -1, 1 );
}
