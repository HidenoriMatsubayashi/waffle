# waffle

`waffle` is a Wayland compositor. Especially, it is developed assuming that it will be used in embedded devices.

## 1. Features

- Optimized for Embedded Systems
  - Lightweight
  - Minimal dependent libraries
  - arm64/x64 devices support
- Display backends  (Still developing!!)
  - [Wayland](https://wayland.freedesktop.org/)
  - Direct rendering module ([DRM](https://en.wikipedia.org/wiki/Direct_Rendering_Manager))
    - Generic Buffer Management ([GBM](https://en.wikipedia.org/wiki/Mesa_(computer_graphics)))
    - [EGLStream](https://docs.nvidia.com/drive/drive_os_5.1.6.1L/nvvib_docs/index.html#page/DRIVE_OS_Linux_SDK_Development_Guide/Graphics/graphics_eglstream_user_guide.html) for NVIDIA devices
  - X11
- Keyboard, mouse and touch inputs support

## 2. System requirements
You need to install the following dependent libraries to build this software. Here introduce how to install the libraries on Debian-based systems like Ubuntu.

### Operating Systems
**Ubuntu 20.04** is recommended to develop and build this software.

### Dependent libraries

#### Development environment
- clang
- cmake
- build-essential
- pkg-config

```Shell
$ sudo apt install clang cmake build-essential pkg-config
```

#### Common system level libraries
- EGL
- xkbcommon
- OpenGL ES (>=3.0)
- SOIL
- libwayland
- wayland-protocols (to generate the source files of Wayland protocols)

```Shell
$ sudo apt install libegl1-mesa-dev libxkbcommon-dev libgles2-mesa-dev \ 
                   libsoil-dev libwayland-dev wayland-protocols
```

#### System level libraries for X11 backends
- x11

```Shell
$ sudo apt install libx11-dev
```

#### System level libraries for DRM backends
- libdrm
- libgbm
- libinput
- libudev
- libsystemd

```Shell
$ sudo apt install libdrm-dev libgbm-dev libinput-dev libudev-dev \ 
                   libsystemd-dev
```

## 3. Building waffle

### X11 backend

```Shell
mkdir build && cd build
cmake -DBACKEND_TYPE=X11 -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### DRM-GBM backend (Still developing)

```Shell
mkdir build && cd build
cmake -DBACKEND_TYPE=DRM-GBM -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## 4. Running waffle

### X11 backend

```Shell
$ ./waffle
```

### DRM-GBM backend (Still developing)

You need to switch from GUI which is running X11 or Wayland to the Character User Interface (CUI). In addition, `WAFFLE_DRM_DEVICE` must be set properly. The default value is `/dev/dri/card0`.

```Shell
$ Ctrl + Alt + F3 # Switching to CUI
$ sudo WAFFLE_DRM_DEVICE=/dev/dri/card1 ./waffle
```

If you want to switch back from CUI to GUI, run Ctrl + Alt + F2 keys in a terminal.

#### Note

You need to run this program by a user who has the permission to access the input devices(/dev/input/xxx), if you use the DRM backend. Generally, it is a root user or a user who belongs to an input group.

## 5. Debugging waffle

### Logging levels

You can get more debugging logs using `WAFFLE_LOG_LEVELS` that is one of environment variables in `waffle`. If you want to do debugging, set `WAFFLE_LOG_LEVELS`. The default level is WARNING.

```Shell
$ WAFFLE_LOG_LEVELS=TRACE ./waffle
$ WAFFLE_LOG_LEVELS=INFO ./waffle
$ WAFFLE_LOG_LEVELS=WARNING ./waffle
$ WAFFLE_LOG_LEVELS=ERROR ./waffle
$ WAFFLE_LOG_LEVELS=FATAL ./waffle
```
