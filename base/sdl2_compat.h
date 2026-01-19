// sdl2_compat.h -- SDL2 compatibility layer for GameSWF
// This header provides SDL2 compatibility for code originally written for SDL 1.2

#ifndef SDL2_COMPAT_H
#define SDL2_COMPAT_H

#include <SDL.h>
#include <SDL_thread.h>

// SDL2 uses SDL_Window and SDL_GLContext instead of SDL_Surface
extern SDL_Window* g_sdl_window;
extern SDL_GLContext g_gl_context;

// SDL 1.2 used SDLKey, SDL2 uses SDL_Keycode
typedef SDL_Keycode SDLKey;

// SDL 1.2 key constants that changed names in SDL2
#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9

// SDL_INIT_EVENTTHREAD was removed in SDL2 (events are always threadsafe)
#ifndef SDL_INIT_EVENTTHREAD
#define SDL_INIT_EVENTTHREAD 0
#endif

// SDL_VIDEORESIZE event was replaced with SDL_WINDOWEVENT in SDL2
#ifndef SDL_VIDEORESIZE
#define SDL_VIDEORESIZE SDL_WINDOWEVENT
#endif

// SDL_EnableKeyRepeat was removed in SDL2 (key repeat is always on)
inline void SDL_EnableKeyRepeat(int delay, int interval) {
    (void)delay;
    (void)interval;
    // Key repeat is always enabled in SDL2
}

// SDL 1.2 video mode flags - these are now window flags or obsolete in SDL2
#ifndef SDL_OPENGL
#define SDL_OPENGL 0x00000002  // This is handled by SDL_WINDOW_OPENGL in our wrapper
#endif
#ifndef SDL_RESIZABLE
#define SDL_RESIZABLE SDL_WINDOW_RESIZABLE
#endif
#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN SDL_WINDOW_FULLSCREEN
#endif

// SDL_SetVideoMode replacement for SDL2
// Returns 1 on success, 0 on failure (inverted from SDL 1.2 which returned SDL_Surface*)
inline int SDL2_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
    (void)bpp;  // SDL2 uses desktop color depth
    
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (flags & SDL_WINDOW_RESIZABLE) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    
    g_sdl_window = SDL_CreateWindow(
        "GameSWF",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        window_flags
    );
    
    if (!g_sdl_window) {
        return 0;
    }
    
    g_gl_context = SDL_GL_CreateContext(g_sdl_window);
    if (!g_gl_context) {
        SDL_DestroyWindow(g_sdl_window);
        g_sdl_window = nullptr;
        return 0;
    }
    
    // Enable vsync
    SDL_GL_SetSwapInterval(1);
    
    return 1;
}

// Replacement for SDL_SetVideoMode - compatibility macro
#define SDL_SetVideoMode(w, h, bpp, flags) SDL2_SetVideoMode(w, h, bpp, flags)

// SDL_GL_SwapBuffers replacement for SDL2
inline void SDL2_GL_SwapBuffers() {
    if (g_sdl_window) {
        SDL_GL_SwapWindow(g_sdl_window);
    }
}

#define SDL_GL_SwapBuffers() SDL2_GL_SwapBuffers()

// SDL_WM_SetCaption replacement for SDL2
inline void SDL2_SetWindowTitle(const char* title, const char* icon) {
    (void)icon;  // SDL2 handles icons differently
    if (g_sdl_window) {
        SDL_SetWindowTitle(g_sdl_window, title);
    }
}

#define SDL_WM_SetCaption(title, icon) SDL2_SetWindowTitle(title, icon)

// Get window dimensions
inline void SDL2_GetWindowSize(int* w, int* h) {
    if (g_sdl_window) {
        SDL_GetWindowSize(g_sdl_window, w, h);
    } else {
        *w = 0;
        *h = 0;
    }
}

// Cleanup function
inline void SDL2_Cleanup() {
    if (g_gl_context) {
        SDL_GL_DeleteContext(g_gl_context);
        g_gl_context = nullptr;
    }
    if (g_sdl_window) {
        SDL_DestroyWindow(g_sdl_window);
        g_sdl_window = nullptr;
    }
}

#endif // SDL2_COMPAT_H
