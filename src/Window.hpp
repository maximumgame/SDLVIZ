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
    int getWidth() { return m_width; };
    int getHeight() { return m_height; };

    int m_width;
    int m_height;
protected:

    SDL_Window* window;
    SDL_GLContext glcontext;


};