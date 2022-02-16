#pragma once
#include <SDL2/SDL.h>

class Window
{
public:
    Window();
    ~Window();
    void createWindow();
    SDL_Window* getWindow() { return window; };
    SDL_GLContext getContext() { return glcontext; };
    const int* getWidth() { return &m_width; };
    const int* getHeight() { return &m_height; };
    
protected:

    int m_width;
    int m_height;
    SDL_Window* window;
    SDL_GLContext glcontext;


};