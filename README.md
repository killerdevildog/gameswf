# GameSWF - Modern Fork

A modernized fork of the original GameSWF library from the tu-testbed project.

GameSWF is an open-source SWF (Shockwave Flash) player library that uses OpenGL for rendering. This fork has been updated to compile and run on modern Linux systems with current toolchains.

## About This Fork

This is a modernized version of the original GameSWF library by Thatcher Ulrich. The original source was retrieved from the [tu-testbed project on SourceForge](http://sourceforge.net/projects/tu-testbed) and has been updated to work with:

- **C++17** standard
- **SDL2** (migrated from SDL 1.2)
- **Modern GCC** (tested with GCC 13.3.0)
- **CMake** build system (replacing the old Autotools setup)
- **64-bit systems** (fixed hash table issues on 64-bit platforms)

## Building

### Prerequisites (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake libsdl2-dev libgl1-mesa-dev \
    zlib1g-dev libpng-dev libjpeg-dev libfreetype-dev
```

Optional for sound support:
```bash
sudo apt install libsdl2-mixer-dev
```

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

The player executable will be at `build/bin/gameswf_test_ogl`.

## Usage

```bash
./build/bin/gameswf_test_ogl [options] movie.swf
```

### Options

| Option | Description |
|--------|-------------|
| `-h` | Print help |
| `-v` | Verbose output (print log messages) |
| `-va` | Verbose about movie Actions |
| `-vp` | Verbose about parsing |
| `-a <level>` | Antialiasing level (0,1,2,4,8,16,...) |
| `-r <0\|1\|2>` | 0=no render/sound, 1=render+sound, 2=render only |
| `-1` | Play once and exit |
| `-p` | Run full speed, log frame rate |
| `-f` | Force realtime framerate |
| `-w <W>x<H>` | Window size (e.g., 1024x768) |

### Keyboard Controls

| Key | Action |
|-----|--------|
| ESC / Ctrl+Q / Ctrl+W | Quit |
| Ctrl+P | Toggle Pause |
| Ctrl+[ / KP- | Previous frame |
| Ctrl+] / KP+ | Next frame |
| Ctrl+A | Toggle antialiasing |
| Ctrl+T | Debug: show wireframe |
| Ctrl+B | Toggle background color |

## Sample Files

Sample SWF files are included in `gameswf/samples/`. Try:

```bash
./build/bin/gameswf_test_ogl gameswf/samples/gameswf_logo.swf
./build/bin/gameswf_test_ogl gameswf/samples/test_gradients_alpha.swf
./build/bin/gameswf_test_ogl gameswf/samples/test_shape_tweening.swf
```

## Changes from Original

- Migrated build system to CMake
- Updated for SDL2 compatibility (from SDL 1.2)
- Fixed 64-bit hash table corruption issues
- Fixed deprecated C++ code for modern compilers
- Fixed libpng API compatibility
- Fixed OpenGL extension type conflicts
- Removed SDL_KillThread (deprecated in SDL2)
- Added SDL2 compatibility layer for legacy code

## License

This source code has been donated to the **Public Domain**. Do whatever you want with it.

See [http://tulrich.com/geekstuff/gameswf.html](http://tulrich.com/geekstuff/gameswf.html) for more info about the original project.

## Embedding in Other Applications

GameSWF can be embedded in game engines like Godot, custom engines, or any application with an OpenGL context. See `gameswf/gameswf_embed.h` for the simplified C/C++ API.

### Quick Start (C++)

```cpp
#include "gameswf/gameswf_embed.h"

// Initialize (your OpenGL context must be current)
gameswf_embed::init();

// Load and use
gameswf_embed::SWFMovie movie;
movie.load("animation.swf");

// In your render loop:
movie.advance(delta_time);           // Update animation
movie.render(0, 0, width, height);   // Render to GL context

// Handle input
movie.mouse_move(x, y);
movie.mouse_button(x, y, 1);         // Left click
movie.key_down(GAMESWF_KEY_SPACE);

// Cleanup
movie.unload();
gameswf_embed::shutdown();
```

### Quick Start (C)

```c
#include "gameswf/gameswf_embed.h"

gameswf_init(1);  // 1 = use built-in OpenGL renderer

gameswf_instance swf = gameswf_load("animation.swf");

// In render loop:
gameswf_advance(swf, delta_time);
gameswf_render(swf, 0, 0, width, height);

// Input
gameswf_mouse_move(swf, x, y);
gameswf_mouse_button(swf, x, y, GAMESWF_MOUSE_LEFT);

// Cleanup
gameswf_unload(swf);
gameswf_shutdown();
```

### Custom Render Handler (Vulkan, etc.)

For Vulkan or other rendering APIs, implement the `gameswf::render_handler` interface and call `gameswf_set_render_handler()` before loading movies. See `gameswf/example_embed.cpp` for a skeleton implementation.

## Original Author

- Thatcher Ulrich <tu@tulrich.com>

## Fork Maintainer

- Quaylyn Rimer ([@killerdevildog](https://github.com/killerdevildog))
