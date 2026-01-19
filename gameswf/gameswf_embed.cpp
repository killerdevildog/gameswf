// gameswf_embed.cpp -- Embeddable GameSWF API Implementation
//
// This source code has been donated to the Public Domain.
// Do whatever you want with it.

#include "gameswf_embed.h"
#include "gameswf.h"
#include "gameswf_root.h"
#include "gameswf_player.h"
#include "base/tu_file.h"

#include <cstdarg>
#include <cstring>

// ============================================================================
// Internal state
// ============================================================================

namespace {
    gameswf::render_handler* g_render_handler = nullptr;
    bool g_owns_render_handler = false;
    gameswf_fscommand_callback g_fscommand_callback = nullptr;
    void* g_fscommand_user_data = nullptr;
    
    // Buffer for return values
    static char g_return_buffer[4096];
}

// ============================================================================
// Internal instance structure
// ============================================================================

struct gameswf_instance_s {
    gameswf::player* player;
    gameswf::gc_ptr<gameswf::root> root;
    int mouse_x, mouse_y, mouse_buttons;
    
    gameswf_instance_s() : player(nullptr), mouse_x(0), mouse_y(0), mouse_buttons(0) {}
    
    ~gameswf_instance_s() {
        root = nullptr;
        if (player) {
            delete player;
            player = nullptr;
        }
    }
};

// ============================================================================
// FSCommand callback bridge
// ============================================================================

static void internal_fscommand_callback(gameswf::character* movie, const char* command, const char* args) {
    if (g_fscommand_callback) {
        // We don't have the instance handle in the old callback interface
        // This is a limitation of the current design
        g_fscommand_callback(nullptr, command, args, g_fscommand_user_data);
    }
}

// ============================================================================
// C API Implementation
// ============================================================================

extern "C" {

void gameswf_init(int use_builtin_gl_renderer) {
    if (use_builtin_gl_renderer) {
        g_render_handler = gameswf::create_render_handler_ogl();
        g_owns_render_handler = true;
        gameswf::set_render_handler(g_render_handler);
        g_render_handler->open();
    }
}

void gameswf_shutdown(void) {
    gameswf::set_render_handler(nullptr);
    if (g_owns_render_handler && g_render_handler) {
        delete g_render_handler;
        g_render_handler = nullptr;
        g_owns_render_handler = false;
    }
}

gameswf_instance gameswf_load(const char* filename) {
    gameswf_instance inst = new gameswf_instance_s();
    
    inst->player = new gameswf::player();
    inst->root = inst->player->load_file(filename);
    
    if (!inst->root) {
        delete inst;
        return nullptr;
    }
    
    return inst;
}

void gameswf_unload(gameswf_instance inst) {
    if (inst) {
        delete inst;
    }
}

void gameswf_advance(gameswf_instance inst, float delta_time) {
    if (inst && inst->root) {
        inst->root->advance(delta_time);
    }
}

void gameswf_render(gameswf_instance inst, 
                    int viewport_x, int viewport_y,
                    int viewport_width, int viewport_height) {
    if (inst && inst->root) {
        inst->root->set_display_viewport(viewport_x, viewport_y, viewport_width, viewport_height);
        inst->root->display();
    }
}

void gameswf_goto_frame(gameswf_instance inst, int frame) {
    if (inst && inst->root) {
        inst->root->goto_frame(frame);
    }
}

void gameswf_play(gameswf_instance inst) {
    if (inst && inst->root) {
        inst->root->set_play_state(gameswf::character::PLAY);
    }
}

void gameswf_stop(gameswf_instance inst) {
    if (inst && inst->root) {
        inst->root->set_play_state(gameswf::character::STOP);
    }
}

int gameswf_is_playing(gameswf_instance inst) {
    if (inst && inst->root) {
        return inst->root->get_play_state() == gameswf::character::PLAY ? 1 : 0;
    }
    return 0;
}

int gameswf_get_width(gameswf_instance inst) {
    return (inst && inst->root) ? inst->root->get_movie_width() : 0;
}

int gameswf_get_height(gameswf_instance inst) {
    return (inst && inst->root) ? inst->root->get_movie_height() : 0;
}

float gameswf_get_fps(gameswf_instance inst) {
    return (inst && inst->root) ? inst->root->get_movie_fps() : 0.0f;
}

int gameswf_get_frame_count(gameswf_instance inst) {
    return (inst && inst->root) ? inst->root->get_frame_count() : 0;
}

int gameswf_get_current_frame(gameswf_instance inst) {
    return (inst && inst->root) ? inst->root->get_current_frame() : 0;
}

void gameswf_mouse_move(gameswf_instance inst, int x, int y) {
    if (inst && inst->root) {
        inst->mouse_x = x;
        inst->mouse_y = y;
        inst->root->notify_mouse_state(inst->mouse_x, inst->mouse_y, inst->mouse_buttons);
    }
}

void gameswf_mouse_button(gameswf_instance inst, int x, int y, int buttons) {
    if (inst && inst->root) {
        inst->mouse_x = x;
        inst->mouse_y = y;
        inst->mouse_buttons = buttons;
        inst->root->notify_mouse_state(inst->mouse_x, inst->mouse_y, inst->mouse_buttons);
    }
}

void gameswf_key_down(gameswf_instance inst, int key_code) {
    if (inst && inst->player) {
        inst->player->notify_key_event(static_cast<gameswf::key::code>(key_code), true);
    }
}

void gameswf_key_up(gameswf_instance inst, int key_code) {
    if (inst && inst->player) {
        inst->player->notify_key_event(static_cast<gameswf::key::code>(key_code), false);
    }
}

void gameswf_set_variable(gameswf_instance inst, const char* path, const char* value) {
    if (inst && inst->root) {
        inst->root->set_variable(path, value);
    }
}

const char* gameswf_get_variable(gameswf_instance inst, const char* path) {
    if (inst && inst->root) {
        const char* result = inst->root->get_variable(path);
        if (result) {
            strncpy(g_return_buffer, result, sizeof(g_return_buffer) - 1);
            g_return_buffer[sizeof(g_return_buffer) - 1] = '\0';
            return g_return_buffer;
        }
    }
    return nullptr;
}

const char* gameswf_call_method(gameswf_instance inst, const char* method_name, 
                                 const char* arg_fmt, ...) {
    if (inst && inst->root) {
        va_list args;
        va_start(args, arg_fmt);
        const char* result = inst->root->call_method_args(method_name, arg_fmt, args);
        va_end(args);
        
        if (result) {
            strncpy(g_return_buffer, result, sizeof(g_return_buffer) - 1);
            g_return_buffer[sizeof(g_return_buffer) - 1] = '\0';
            return g_return_buffer;
        }
    }
    return nullptr;
}

void gameswf_set_fscommand_callback(gameswf_fscommand_callback callback, void* user_data) {
    g_fscommand_callback = callback;
    g_fscommand_user_data = user_data;
    gameswf::register_fscommand_callback(internal_fscommand_callback);
}

void gameswf_set_render_handler(gameswf::render_handler* handler) {
    if (g_owns_render_handler && g_render_handler) {
        delete g_render_handler;
        g_owns_render_handler = false;
    }
    g_render_handler = handler;
    gameswf::set_render_handler(handler);
}

gameswf::render_handler* gameswf_get_ogl_render_handler(void) {
    return g_render_handler;
}

} // extern "C"
