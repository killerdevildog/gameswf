// example_embed.cpp -- Example of embedding GameSWF in an existing OpenGL application
//
// This example shows how to use GameSWF in your own application without SDL.
// You need to provide your own window management and OpenGL context.

#include "gameswf/gameswf_embed.h"
#include <cstdio>

// Example: Minimal integration with your existing game engine
// 
// In your game engine (Godot, Unity via native plugin, custom engine, etc.):
//
// 1. Initialize once at startup:
//    gameswf_init(1);  // Use built-in OpenGL renderer
//
// 2. Load SWF files as needed:
//    gameswf_instance swf = gameswf_load("my_animation.swf");
//
// 3. In your render loop:
//    gameswf_advance(swf, delta_time);  // Update animation
//    gameswf_render(swf, x, y, width, height);  // Render to current GL context
//
// 4. Handle input events:
//    gameswf_mouse_move(swf, mouse_x, mouse_y);
//    gameswf_mouse_button(swf, mouse_x, mouse_y, GAMESWF_MOUSE_LEFT);
//    gameswf_key_down(swf, GAMESWF_KEY_SPACE);
//
// 5. Cleanup:
//    gameswf_unload(swf);
//    gameswf_shutdown();

#ifdef EXAMPLE_MAIN

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file.swf>\n", argv[0]);
        return 1;
    }
    
    // Initialize GameSWF with built-in OpenGL renderer
    // NOTE: You must have a valid OpenGL context BEFORE calling this!
    gameswf_init(1);
    
    // Load the SWF
    gameswf_instance swf = gameswf_load(argv[1]);
    if (!swf) {
        printf("Failed to load: %s\n", argv[1]);
        gameswf_shutdown();
        return 1;
    }
    
    printf("Loaded: %s\n", argv[1]);
    printf("  Size: %dx%d\n", gameswf_get_width(swf), gameswf_get_height(swf));
    printf("  FPS: %.1f\n", gameswf_get_fps(swf));
    printf("  Frames: %d\n", gameswf_get_frame_count(swf));
    
    // In your render loop you would do:
    // float delta_time = 1.0f / 60.0f;  // Or actual frame time
    // gameswf_advance(swf, delta_time);
    // gameswf_render(swf, 0, 0, window_width, window_height);
    
    gameswf_unload(swf);
    gameswf_shutdown();
    
    return 0;
}

#endif // EXAMPLE_MAIN

// ============================================================================
// Example: Custom render handler for Vulkan (skeleton)
// ============================================================================
//
// For Vulkan or other rendering APIs, implement gameswf::render_handler:
//
// class VulkanRenderHandler : public gameswf::render_handler {
// public:
//     // Texture management
//     bitmap_info* create_bitmap_info_empty() override { ... }
//     bitmap_info* create_bitmap_info_alpha(int w, int h, unsigned char* data) override { ... }
//     bitmap_info* create_bitmap_info_rgb(image::rgb* im) override { ... }
//     bitmap_info* create_bitmap_info_rgba(image::rgba* im) override { ... }
//     video_handler* create_video_handler() override { return nullptr; }
//
//     // Frame rendering
//     void begin_display(rgba bg, int vx, int vy, int vw, int vh, 
//                        float x0, float x1, float y0, float y1) override { ... }
//     void end_display() override { ... }
//
//     // Transform state
//     void set_matrix(const matrix& m) override { ... }
//     void set_cxform(const cxform& cx) override { ... }
//
//     // Mesh rendering
//     void draw_mesh_strip(const void* coords, int vertex_count) override { ... }
//     void draw_triangle_list(const void* coords, int vertex_count) override { ... }
//     void draw_line_strip(const void* coords, int vertex_count) override { ... }
//
//     // Styles
//     void fill_style_disable(int fill_side) override { ... }
//     void fill_style_color(int fill_side, const rgba& color) override { ... }
//     void fill_style_bitmap(int fill_side, bitmap_info* bi, const matrix& m,
//                            bitmap_wrap_mode wm, bitmap_blend_mode bm) override { ... }
//     void line_style_disable() override { ... }
//     void line_style_color(rgba color) override { ... }
//     void line_style_width(float width) override { ... }
//
//     // Bitmap rendering  
//     void draw_bitmap(const matrix& m, bitmap_info* bi, const rect& coords,
//                      const rect& uv_coords, rgba color) override { ... }
//
//     // Misc
//     void set_antialiased(bool enable) override { ... }
//     bool test_stencil_buffer(const rect& bound, Uint8 pattern) override { return false; }
//     void begin_submit_mask() override { ... }
//     void end_submit_mask() override { ... }
//     void disable_mask() override { ... }
//     bool is_visible(const rect& bound) override { return true; }
//     void open() override { ... }
// };
//
// Usage:
//   VulkanRenderHandler* vulkan_handler = new VulkanRenderHandler();
//   gameswf_set_render_handler(vulkan_handler);
//   gameswf_instance swf = gameswf_load("movie.swf");
//   ...

// ============================================================================
// Example: Godot GDExtension integration (pseudocode)
// ============================================================================
//
// In Godot GDExtension:
//
// class SWFPlayer : public Control {
//     GDCLASS(SWFPlayer, Control);
//     
//     gameswf_instance m_swf = nullptr;
//     
// protected:
//     static void _bind_methods() {
//         ClassDB::bind_method(D_METHOD("load_swf", "path"), &SWFPlayer::load_swf);
//         ClassDB::bind_method(D_METHOD("play"), &SWFPlayer::play);
//         ClassDB::bind_method(D_METHOD("stop"), &SWFPlayer::stop);
//     }
//     
// public:
//     void _ready() override {
//         gameswf_init(1);  // Initialize with GL renderer
//     }
//     
//     void load_swf(const String& path) {
//         if (m_swf) gameswf_unload(m_swf);
//         m_swf = gameswf_load(path.utf8().get_data());
//     }
//     
//     void _process(double delta) override {
//         if (m_swf) {
//             gameswf_advance(m_swf, (float)delta);
//             queue_redraw();
//         }
//     }
//     
//     void _draw() override {
//         if (m_swf) {
//             Rect2 rect = get_rect();
//             gameswf_render(m_swf, rect.position.x, rect.position.y, 
//                           rect.size.x, rect.size.y);
//         }
//     }
//     
//     void _gui_input(const Ref<InputEvent>& event) override {
//         if (!m_swf) return;
//         
//         Ref<InputEventMouseMotion> mm = event;
//         if (mm.is_valid()) {
//             Vector2 pos = mm->get_position();
//             gameswf_mouse_move(m_swf, pos.x, pos.y);
//         }
//         
//         Ref<InputEventMouseButton> mb = event;
//         if (mb.is_valid()) {
//             Vector2 pos = mb->get_position();
//             int buttons = mb->is_pressed() ? GAMESWF_MOUSE_LEFT : 0;
//             gameswf_mouse_button(m_swf, pos.x, pos.y, buttons);
//         }
//     }
//     
//     void play() { if (m_swf) gameswf_play(m_swf); }
//     void stop() { if (m_swf) gameswf_stop(m_swf); }
//     
//     ~SWFPlayer() {
//         if (m_swf) gameswf_unload(m_swf);
//     }
// };
