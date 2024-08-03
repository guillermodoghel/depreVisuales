# DepreVisuales
Las visuales mas depre de github.

Implements projectM to render the audio coming from the default audio input source

projectM is an open-source project that reimplements the
esteemed [Winamp Milkdrop](https://en.wikipedia.org/wiki/MilkDrop) by Geiss in a more modern, cross-platform reusable
library.


### Instructions
Get all dependencies (projectM, imgui, and presets)
```
git submodule update --init --recursive
```

build and install `dependencies/projectm` following the instructions in the repo. you may have to install other dependencies, like `libglew-dev` and `libglfw3-dev`.
More instructions here https://github.com/projectM-visualizer/projectm/wiki/Building-libprojectM

```
cd dependencies/projectm
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make
make install
```




This repo included 9000+ presets. Build, run and enjoy.

To filter glitchy presets, the program automatically changes the preset if the framerate drops below 30fps, or if the screen is all black.




