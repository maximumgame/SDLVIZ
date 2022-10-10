# SDLVIZ
Old project for fun learning sdl, opengl, imgui, and cross platform compilation

Implements uniforms iTime and iResolution in the same format as https://shadertoy.com

## Building 
1. Install SDL2 ex ``apt install libsdl2-dev`` or ``vcpkg install sdl2`` vcpkg users remember to set the triplet you want e.g. ``vcpkg install sdl2:x64-windows``
2. Clone this repo ``git clone --recursive https://github.com/maximumgame/SDLVIZ.git``
3. cd inside ``cd SDLVIZ``
4. Create a build folder and move inside ``mkdir build && cd ./build``
5. Generate project files ``cmake ..``
6. Build the project as you normally would ex.
> Linux: ``make -j``

> Windows: ``cmake --build . --config Debug -j``

### Vcpkg notes:
Support for ``VCPKG_ROOT`` enviroment variable is built in. Specifing the vcpkg cmake file is not required if this variable is set.

