//
//  SDLTest.cpp
//  Created by Vraiment on September 7th, 2015
//
//  File crated to verify an issue I'm having with SDL under windows.
//  Through this file I use std::shared_ptr to automatically manage and destroy
//  the different pointes created by SDL and make the code easier to read.
//

#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <string>

using namespace std;

// Dimentions of the window
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

// Simple class to initialize and close SDL
class SDL
{
public:
    SDL();
    ~SDL();

    bool isLoaded() const;

private:
    bool loaded;
};

// Function to create a SDL window
shared_ptr<SDL_Window> createWindow();

// Function to create a SDL renderer from the given window
shared_ptr<SDL_Renderer> createRenderer(shared_ptr<SDL_Window> window);

// Function to create a texture with the given color and rederer
shared_ptr<SDL_Texture> createRGBTexture(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    shared_ptr<SDL_Renderer> renderer);

int main(int argc, char *argv[])
{
    // This line initializes SDL and makes sure that whenever
    // we quit from the main function, SDL_Quit() gets called.
    SDL sdl;

    // Check if SDL was loaded
    if (!sdl.isLoaded())
    {
        cerr << "Could not initialize SDL" << endl;
        return 1;
    }

    // Create both window and renderer
    auto window = createWindow();
    auto renderer = createRenderer(window);

    // Create a red texture
    auto t1 = createRGBTexture(0xFF, 0x00, 0x00, renderer);
    if (!t1)
    {
        cerr << "Could not create first texture" << endl;
        return 1;
    }

    // Create a cyan texcture
    auto t2 = createRGBTexture(0x00, 0xFF, 0xFF, renderer);
    if (!t2)
    {
        cerr << "Could not create first texture" << endl;
        return 1;
    }

    int ticks = SDL_GetTicks(), seconds = 0;
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (SDL_QUIT == event.type)
            {
                quit = true;
            }
        }

        // With this code we get how many milliseconds have passed
        // between each frame, for every second we print it to the
        // standard output.
        int currentTicks = SDL_GetTicks();
        if ((currentTicks - ticks) >= 1000)
        {
            cout << seconds++ << endl;
            ticks = currentTicks;
        }

        SDL_RenderClear(renderer.get());

        // This is just a 'clever' way to switch textures for every
        // second that has passed.
        shared_ptr<SDL_Texture> t = (seconds & 1) ? t1 : t2;
        SDL_RenderCopy(renderer.get(), t.get(), nullptr, nullptr);

        SDL_RenderPresent(renderer.get());
    }

    // We don't have to do any clean up, everything is done automatically.

    return 0;
}

SDL::SDL() : loaded(false)
{
    loaded = !SDL_Init(SDL_INIT_EVERYTHING);
}

SDL::~SDL()
{
    if (loaded)
    {
        SDL_Quit();
    }
}

bool SDL::isLoaded() const
{
    return loaded;
}

shared_ptr<SDL_Window> createWindow()
{
    shared_ptr<SDL_Window> result;
    SDL_Window *window;

    window = SDL_CreateWindow(
        "Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (nullptr != window)
    {
        result.reset(window, SDL_DestroyWindow);
    }

    return result;
}

shared_ptr<SDL_Renderer> createRenderer(shared_ptr<SDL_Window> window)
{
    shared_ptr<SDL_Renderer> result;
    SDL_Renderer *renderer;

    renderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);
    if (nullptr != renderer)
    {
        result.reset(renderer, SDL_DestroyRenderer);
    }

    return result;
}

shared_ptr<SDL_Texture> createRGBTexture(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    shared_ptr<SDL_Renderer> renderer)
{
    shared_ptr<SDL_Texture> result;
    SDL_Surface *surface = SDL_CreateRGBSurface(
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        32, 0, 0, 0, 0);
    
    if (nullptr != surface)
    {
        uint32_t rgbMap = SDL_MapRGB(surface->format, r, g, b);
        if (!SDL_FillRect(surface, nullptr, rgbMap))
        {
            SDL_Texture *t = SDL_CreateTextureFromSurface(renderer.get(), surface);
            if (nullptr != t)
            {
                result.reset(t, SDL_DestroyTexture);
            }
        }
    }

    return result;
}
