# tunescope
Tunescope is an oscilloscope style guitar tuner. It uses jack for audio input and opengl for rendering. The signal is displayed in both normal and XY mode, using an automatically selected not as the reference. 

Features:
  - Simultaneous XY and normal mode
  - Automatic note selection
  - OpenGL rendering with phosphor-like persistence
  - Jack audio input

Build requirements:
  - cmake
  - glut
  - jack

Building:
```sh
$ cd build
$ cmake ..
$ make
```
