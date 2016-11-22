#include "gui.h"
#include "audio.h"
#include <iostream>
using namespace std;
#include "tone.h"
int main(int argc,char** argv)
{
    Audio audio=Audio();
    gui.init(argc,argv);
    gui.setAudio(&audio);
    gui.run();

    return 0;
}
