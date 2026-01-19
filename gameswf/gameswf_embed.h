// gameswf_embed.h -- Embeddable GameSWF API
// 
// This header provides a simplified C API for embedding GameSWF in other
// applications like game engines (Godot, Unity, etc.) or any program with
// an existing OpenGL/Vulkan context.
//
// Usage:
//   1. Initialize GameSWF with your own render handler or use the built-in OpenGL one
//   2. Load a SWF file
//   3. In your render loop:
//      - Call gameswf_advance() to update animation
//      - Call gameswf_render() to draw to your context
//   4. Pass input events as they occur
//   5. Clean up when done

#ifndef GAMESWF_EMBED_H
#define GAMESWF_EMBED_H

#include "gameswf.h"
#include "gameswf_root.h"
#include "gameswf_player.h"
#include "gameswf_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Opaque handle types
// ============================================================================

typedef struct gameswf_instance_s* gameswf_instance;

// ============================================================================
// Initialization / Shutdown
// ============================================================================

// Initialize GameSWF. Call once at startup.
// If use_builtin_gl_renderer is true, uses the built-in OpenGL renderer.
// Otherwise, you must provide your own render_handler via gameswf_set_render_handler()
void gameswf_init(int use_builtin_gl_renderer);

// Shutdown GameSWF. Call once at program exit.
void gameswf_shutdown(void);

// ============================================================================
// Movie Loading
// ============================================================================

// Load a SWF file and return a handle to it
// Returns NULL on failure
gameswf_instance gameswf_load(const char* filename);

// Unload a SWF instance
void gameswf_unload(gameswf_instance inst);

// ============================================================================
// Playback Control
// ============================================================================

// Advance the movie by delta_time seconds
// Call this in your game loop, typically with your frame delta time
void gameswf_advance(gameswf_instance inst, float delta_time);

// Render the movie
// Your OpenGL context must be current before calling this
// viewport_x, viewport_y: position of the viewport in pixels
// viewport_width, viewport_height: size of the viewport in pixels
void gameswf_render(gameswf_instance inst, 
                    int viewport_x, int viewport_y,
                    int viewport_width, int viewport_height);

// Go to a specific frame (0-indexed)
void gameswf_goto_frame(gameswf_instance inst, int frame);

// Play/Stop control
void gameswf_play(gameswf_instance inst);
void gameswf_stop(gameswf_instance inst);

// Check if movie is playing
int gameswf_is_playing(gameswf_instance inst);

// Get movie info
int gameswf_get_width(gameswf_instance inst);
int gameswf_get_height(gameswf_instance inst);
float gameswf_get_fps(gameswf_instance inst);
int gameswf_get_frame_count(gameswf_instance inst);
int gameswf_get_current_frame(gameswf_instance inst);

// ============================================================================
// Input Events
// ============================================================================

// Mouse state - call whenever mouse moves or buttons change
// x, y: mouse position in pixels relative to the movie viewport
// buttons: bitmask of mouse buttons (1=left, 2=right, 4=middle)
void gameswf_mouse_move(gameswf_instance inst, int x, int y);
void gameswf_mouse_button(gameswf_instance inst, int x, int y, int buttons);

// Key events
// Use gameswf key codes (see gameswf_key enum below)
void gameswf_key_down(gameswf_instance inst, int key_code);
void gameswf_key_up(gameswf_instance inst, int key_code);

// Key codes matching Flash key codes
typedef enum {
    GAMESWF_KEY_UNKNOWN = 0,
    GAMESWF_KEY_BACKSPACE = 8,
    GAMESWF_KEY_TAB = 9,
    GAMESWF_KEY_CLEAR = 12,
    GAMESWF_KEY_ENTER = 13,
    GAMESWF_KEY_SHIFT = 16,
    GAMESWF_KEY_CONTROL = 17,
    GAMESWF_KEY_ALT = 18,
    GAMESWF_KEY_PAUSE = 19,
    GAMESWF_KEY_CAPSLOCK = 20,
    GAMESWF_KEY_ESCAPE = 27,
    GAMESWF_KEY_SPACE = 32,
    GAMESWF_KEY_PAGEUP = 33,
    GAMESWF_KEY_PAGEDOWN = 34,
    GAMESWF_KEY_END = 35,
    GAMESWF_KEY_HOME = 36,
    GAMESWF_KEY_LEFT = 37,
    GAMESWF_KEY_UP = 38,
    GAMESWF_KEY_RIGHT = 39,
    GAMESWF_KEY_DOWN = 40,
    GAMESWF_KEY_INSERT = 45,
    GAMESWF_KEY_DELETE = 46,
    GAMESWF_KEY_0 = 48,
    GAMESWF_KEY_1 = 49,
    GAMESWF_KEY_2 = 50,
    GAMESWF_KEY_3 = 51,
    GAMESWF_KEY_4 = 52,
    GAMESWF_KEY_5 = 53,
    GAMESWF_KEY_6 = 54,
    GAMESWF_KEY_7 = 55,
    GAMESWF_KEY_8 = 56,
    GAMESWF_KEY_9 = 57,
    GAMESWF_KEY_A = 65,
    GAMESWF_KEY_B = 66,
    GAMESWF_KEY_C = 67,
    GAMESWF_KEY_D = 68,
    GAMESWF_KEY_E = 69,
    GAMESWF_KEY_F = 70,
    GAMESWF_KEY_G = 71,
    GAMESWF_KEY_H = 72,
    GAMESWF_KEY_I = 73,
    GAMESWF_KEY_J = 74,
    GAMESWF_KEY_K = 75,
    GAMESWF_KEY_L = 76,
    GAMESWF_KEY_M = 77,
    GAMESWF_KEY_N = 78,
    GAMESWF_KEY_O = 79,
    GAMESWF_KEY_P = 80,
    GAMESWF_KEY_Q = 81,
    GAMESWF_KEY_R = 82,
    GAMESWF_KEY_S = 83,
    GAMESWF_KEY_T = 84,
    GAMESWF_KEY_U = 85,
    GAMESWF_KEY_V = 86,
    GAMESWF_KEY_W = 87,
    GAMESWF_KEY_X = 88,
    GAMESWF_KEY_Y = 89,
    GAMESWF_KEY_Z = 90,
    GAMESWF_KEY_F1 = 112,
    GAMESWF_KEY_F2 = 113,
    GAMESWF_KEY_F3 = 114,
    GAMESWF_KEY_F4 = 115,
    GAMESWF_KEY_F5 = 116,
    GAMESWF_KEY_F6 = 117,
    GAMESWF_KEY_F7 = 118,
    GAMESWF_KEY_F8 = 119,
    GAMESWF_KEY_F9 = 120,
    GAMESWF_KEY_F10 = 121,
    GAMESWF_KEY_F11 = 122,
    GAMESWF_KEY_F12 = 123,
    GAMESWF_KEY_NUMLOCK = 144,
    GAMESWF_KEY_SCROLLLOCK = 145
} gameswf_key;

// Mouse button masks
#define GAMESWF_MOUSE_LEFT   1
#define GAMESWF_MOUSE_RIGHT  2
#define GAMESWF_MOUSE_MIDDLE 4

// ============================================================================
// ActionScript Interaction
// ============================================================================

// Set a variable in the SWF
void gameswf_set_variable(gameswf_instance inst, const char* path, const char* value);

// Get a variable from the SWF (returns NULL if not found)
// The returned string is valid until the next call to gameswf_get_variable
const char* gameswf_get_variable(gameswf_instance inst, const char* path);

// Call an ActionScript method
// Returns the result as a string (valid until next call)
const char* gameswf_call_method(gameswf_instance inst, const char* method_name, 
                                 const char* arg_fmt, ...);

// Register a callback for fscommand() calls from ActionScript
typedef void (*gameswf_fscommand_callback)(gameswf_instance inst, 
                                            const char* command, 
                                            const char* args,
                                            void* user_data);
void gameswf_set_fscommand_callback(gameswf_fscommand_callback callback, void* user_data);

// ============================================================================
// Advanced: Custom Render Handler
// ============================================================================

// For Vulkan or custom OpenGL contexts, you can provide your own render handler.
// See gameswf::render_handler in gameswf.h for the interface.
// Call this BEFORE loading any movies.
void gameswf_set_render_handler(gameswf::render_handler* handler);

// Get the built-in OpenGL render handler (if you want to customize it)
gameswf::render_handler* gameswf_get_ogl_render_handler(void);

#ifdef __cplusplus
}
#endif

// ============================================================================
// C++ API (more convenient for C++ applications)
// ============================================================================

#ifdef __cplusplus

namespace gameswf_embed {

// RAII wrapper for SWF instances
class SWFMovie {
public:
    SWFMovie() : m_player(nullptr), m_root(nullptr), m_mouse_x(0), m_mouse_y(0), m_mouse_buttons(0) {}
    
    ~SWFMovie() {
        unload();
    }
    
    // Load a SWF file
    bool load(const char* filename) {
        unload();
        
        m_player = new gameswf::player();
        m_root = m_player->load_file(filename);
        
        if (!m_root) {
            delete m_player;
            m_player = nullptr;
            return false;
        }
        
        return true;
    }
    
    // Unload the movie
    void unload() {
        m_root = nullptr;
        if (m_player) {
            delete m_player;
            m_player = nullptr;
        }
    }
    
    // Check if a movie is loaded
    bool is_loaded() const { return m_root != nullptr; }
    
    // Advance animation by delta_time seconds
    void advance(float delta_time) {
        if (m_root) {
            m_root->advance(delta_time);
        }
    }
    
    // Render to the current OpenGL context
    void render(int viewport_x, int viewport_y, int viewport_width, int viewport_height) {
        if (m_root) {
            m_root->set_display_viewport(viewport_x, viewport_y, viewport_width, viewport_height);
            m_root->display();
        }
    }
    
    // Mouse input
    void mouse_move(int x, int y) {
        m_mouse_x = x;
        m_mouse_y = y;
        if (m_root) {
            m_root->notify_mouse_state(m_mouse_x, m_mouse_y, m_mouse_buttons);
        }
    }
    
    void mouse_button(int x, int y, int buttons) {
        m_mouse_x = x;
        m_mouse_y = y;
        m_mouse_buttons = buttons;
        if (m_root) {
            m_root->notify_mouse_state(m_mouse_x, m_mouse_y, m_mouse_buttons);
        }
    }
    
    // Key input (use gameswf::key::code values)
    void key_down(int key_code) {
        if (m_player) {
            m_player->notify_key_event(static_cast<gameswf::key::code>(key_code), true);
        }
    }
    
    void key_up(int key_code) {
        if (m_player) {
            m_player->notify_key_event(static_cast<gameswf::key::code>(key_code), false);
        }
    }
    
    // Playback control
    void play() {
        if (m_root) {
            m_root->set_play_state(gameswf::character::PLAY);
        }
    }
    
    void stop() {
        if (m_root) {
            m_root->set_play_state(gameswf::character::STOP);
        }
    }
    
    void goto_frame(int frame) {
        if (m_root) {
            m_root->goto_frame(frame);
        }
    }
    
    // Get movie properties
    int get_width() const { return m_root ? m_root->get_movie_width() : 0; }
    int get_height() const { return m_root ? m_root->get_movie_height() : 0; }
    float get_fps() const { return m_root ? m_root->get_movie_fps() : 0; }
    int get_frame_count() const { return m_root ? m_root->get_frame_count() : 0; }
    int get_current_frame() const { return m_root ? m_root->get_current_frame() : 0; }
    
    // ActionScript interaction
    void set_variable(const char* path, const char* value) {
        if (m_root) {
            m_root->set_variable(path, value);
        }
    }
    
    const char* get_variable(const char* path) const {
        return m_root ? m_root->get_variable(path) : nullptr;
    }
    
    // Access internal objects for advanced use
    gameswf::player* get_player() { return m_player; }
    gameswf::root* get_root() { return m_root.get_ptr(); }
    
private:
    gameswf::player* m_player;
    gameswf::gc_ptr<gameswf::root> m_root;
    int m_mouse_x, m_mouse_y, m_mouse_buttons;
};

// Initialize GameSWF (call once at startup)
inline void init(bool use_builtin_gl_renderer = true) {
    if (use_builtin_gl_renderer) {
        gameswf::render_handler* render = gameswf::create_render_handler_ogl();
        gameswf::set_render_handler(render);
        render->open();
    }
}

// Shutdown GameSWF (call once at exit)
inline void shutdown() {
    gameswf::set_render_handler(nullptr);
}

} // namespace gameswf_embed

#endif // __cplusplus

#endif // GAMESWF_EMBED_H
