// example_glfw.cpp -- Example of embedding GameSWF with GLFW (no SDL)
//
// This demonstrates that GameSWF can work with any OpenGL context,
// not just SDL. The rendering uses GameSWF's built-in OpenGL renderer.
//
// Build:
//   g++ -o example_glfw example_glfw.cpp -I.. -L../build/lib \
//       -lgameswf -lbase -lGL -lGLU -lglfw -lz -lpng -ljpeg -ldl -lpthread
//
// Run:
//   LD_LIBRARY_PATH=../build/lib ./example_glfw ../gameswf/samples/test_gradients_alpha.swf

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

// GameSWF headers
#include "gameswf/gameswf.h"
#include "gameswf/gameswf_root.h"
#include "gameswf/gameswf_player.h"
#include "gameswf/gameswf_types.h"
#include "base/tu_file.h"

// File opener callback for GameSWF
static tu_file* file_opener(const char* url) {
    return new tu_file(url, "rb");
}

// Global state
static gameswf::player* g_player = nullptr;
static gameswf::gc_ptr<gameswf::root> g_root;
static gameswf::render_handler* g_render_handler = nullptr;
static int g_mouse_x = 0, g_mouse_y = 0, g_mouse_buttons = 0;
static int g_window_width = 800, g_window_height = 600;
static float g_scale_x = 1.0f, g_scale_y = 1.0f;

// Convert GLFW key to GameSWF key code
static gameswf::key::code glfw_key_to_gameswf(int key) {
    switch (key) {
        case GLFW_KEY_BACKSPACE: return gameswf::key::BACKSPACE;
        case GLFW_KEY_TAB: return gameswf::key::TAB;
        case GLFW_KEY_ENTER: return gameswf::key::ENTER;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT: return gameswf::key::SHIFT;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL: return gameswf::key::CONTROL;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT: return gameswf::key::ALT;
        case GLFW_KEY_CAPS_LOCK: return gameswf::key::CAPSLOCK;
        case GLFW_KEY_ESCAPE: return gameswf::key::ESCAPE;
        case GLFW_KEY_SPACE: return gameswf::key::SPACE;
        case GLFW_KEY_PAGE_UP: return gameswf::key::PGUP;
        case GLFW_KEY_PAGE_DOWN: return gameswf::key::PGDN;
        case GLFW_KEY_END: return gameswf::key::END;
        case GLFW_KEY_HOME: return gameswf::key::HOME;
        case GLFW_KEY_LEFT: return gameswf::key::LEFT;
        case GLFW_KEY_UP: return gameswf::key::UP;
        case GLFW_KEY_RIGHT: return gameswf::key::RIGHT;
        case GLFW_KEY_DOWN: return gameswf::key::DOWN;
        case GLFW_KEY_INSERT: return gameswf::key::INSERT;
        case GLFW_KEY_DELETE: return gameswf::key::DELETEKEY;
        case GLFW_KEY_0: return gameswf::key::_0;
        case GLFW_KEY_1: return gameswf::key::_1;
        case GLFW_KEY_2: return gameswf::key::_2;
        case GLFW_KEY_3: return gameswf::key::_3;
        case GLFW_KEY_4: return gameswf::key::_4;
        case GLFW_KEY_5: return gameswf::key::_5;
        case GLFW_KEY_6: return gameswf::key::_6;
        case GLFW_KEY_7: return gameswf::key::_7;
        case GLFW_KEY_8: return gameswf::key::_8;
        case GLFW_KEY_9: return gameswf::key::_9;
        case GLFW_KEY_A: return gameswf::key::A;
        case GLFW_KEY_B: return gameswf::key::B;
        case GLFW_KEY_C: return gameswf::key::C;
        case GLFW_KEY_D: return gameswf::key::D;
        case GLFW_KEY_E: return gameswf::key::E;
        case GLFW_KEY_F: return gameswf::key::F;
        case GLFW_KEY_G: return gameswf::key::G;
        case GLFW_KEY_H: return gameswf::key::H;
        case GLFW_KEY_I: return gameswf::key::I;
        case GLFW_KEY_J: return gameswf::key::J;
        case GLFW_KEY_K: return gameswf::key::K;
        case GLFW_KEY_L: return gameswf::key::L;
        case GLFW_KEY_M: return gameswf::key::M;
        case GLFW_KEY_N: return gameswf::key::N;
        case GLFW_KEY_O: return gameswf::key::O;
        case GLFW_KEY_P: return gameswf::key::P;
        case GLFW_KEY_Q: return gameswf::key::Q;
        case GLFW_KEY_R: return gameswf::key::R;
        case GLFW_KEY_S: return gameswf::key::S;
        case GLFW_KEY_T: return gameswf::key::T;
        case GLFW_KEY_U: return gameswf::key::U;
        case GLFW_KEY_V: return gameswf::key::V;
        case GLFW_KEY_W: return gameswf::key::W;
        case GLFW_KEY_X: return gameswf::key::X;
        case GLFW_KEY_Y: return gameswf::key::Y;
        case GLFW_KEY_Z: return gameswf::key::Z;
        case GLFW_KEY_F1: return gameswf::key::F1;
        case GLFW_KEY_F2: return gameswf::key::F2;
        case GLFW_KEY_F3: return gameswf::key::F3;
        case GLFW_KEY_F4: return gameswf::key::F4;
        case GLFW_KEY_F5: return gameswf::key::F5;
        case GLFW_KEY_F6: return gameswf::key::F6;
        case GLFW_KEY_F7: return gameswf::key::F7;
        case GLFW_KEY_F8: return gameswf::key::F8;
        case GLFW_KEY_F9: return gameswf::key::F9;
        case GLFW_KEY_F10: return gameswf::key::F10;
        case GLFW_KEY_F11: return gameswf::key::F11;
        case GLFW_KEY_F12: return gameswf::key::F12;
        default: return gameswf::key::INVALID;
    }
}

// GLFW callbacks
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // Scale window coordinates to SWF movie coordinates
    g_mouse_x = (int)(xpos / g_scale_x);
    g_mouse_y = (int)(ypos / g_scale_y);
    if (g_root) {
        g_root->notify_mouse_state(g_mouse_x, g_mouse_y, g_mouse_buttons);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    int mask = 0;
    if (button == GLFW_MOUSE_BUTTON_LEFT) mask = 1;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) mask = 2;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) mask = 4;
    
    if (action == GLFW_PRESS) {
        g_mouse_buttons |= mask;
    } else {
        g_mouse_buttons &= ~mask;
    }
    
    if (g_root) {
        g_root->notify_mouse_state(g_mouse_x, g_mouse_y, g_mouse_buttons);
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Close on Escape
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    
    if (g_player) {
        gameswf::key::code k = glfw_key_to_gameswf(key);
        if (k != gameswf::key::INVALID) {
            g_player->notify_key_event(k, action == GLFW_PRESS || action == GLFW_REPEAT);
        }
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    g_window_width = width;
    g_window_height = height;
    glViewport(0, 0, width, height);
    
    // Update scale factors for mouse coordinate conversion
    if (g_root) {
        g_scale_x = (float)width / g_root->get_movie_width();
        g_scale_y = (float)height / g_root->get_movie_height();
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("GameSWF GLFW Example\n");
        printf("Usage: %s <file.swf>\n", argv[0]);
        printf("\nThis demonstrates embedding GameSWF in a GLFW OpenGL context.\n");
        return 1;
    }
    
    const char* swf_file = argv[1];
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }
    
    // Request OpenGL 2.1 (GameSWF uses fixed-function pipeline)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(g_window_width, g_window_height, 
                                           "GameSWF - GLFW Example", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // VSync
    
    // Set callbacks
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Get actual framebuffer size
    glfwGetFramebufferSize(window, &g_window_width, &g_window_height);
    
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Window size: %dx%d\n", g_window_width, g_window_height);
    
    // Initialize GameSWF render handler
    // Note: The OpenGL context must be current before this!
    g_render_handler = gameswf::create_render_handler_ogl();
    gameswf::set_render_handler(g_render_handler);
    g_render_handler->open();
    
    // Register file opener callback
    gameswf::register_file_opener_callback(file_opener);
    
    // Create player and load SWF
    g_player = new gameswf::player();
    g_root = g_player->load_file(swf_file);
    
    if (!g_root) {
        fprintf(stderr, "Failed to load: %s\n", swf_file);
        delete g_player;
        gameswf::set_render_handler(nullptr);
        delete g_render_handler;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    
    printf("Loaded: %s\n", swf_file);
    printf("  Movie size: %dx%d\n", g_root->get_movie_width(), g_root->get_movie_height());
    printf("  FPS: %.1f\n", g_root->get_movie_fps());
    printf("  Frames: %d\n", g_root->get_frame_count());
    printf("\nControls:\n");
    printf("  ESC - Quit\n");
    printf("  Mouse - Interact with SWF\n");
    
    // Calculate scale factors for mouse coordinate conversion
    g_scale_x = (float)g_window_width / g_root->get_movie_width();
    g_scale_y = (float)g_window_height / g_root->get_movie_height();
    printf("  Scale: %.2fx%.2f\n", g_scale_x, g_scale_y);
    
    // Set up OpenGL state
    glViewport(0, 0, g_window_width, g_window_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    // Main loop
    double last_time = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        double current_time = glfwGetTime();
        float delta_time = (float)(current_time - last_time);
        last_time = current_time;
        
        // Update SWF animation
        g_root->advance(delta_time);
        
        // Clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Set viewport and render SWF
        g_root->set_display_viewport(0, 0, g_window_width, g_window_height);
        g_root->display();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    printf("\nShutting down...\n");
    g_root = nullptr;
    delete g_player;
    g_player = nullptr;
    
    gameswf::set_render_handler(nullptr);
    delete g_render_handler;
    g_render_handler = nullptr;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    printf("Done.\n");
    return 0;
}
