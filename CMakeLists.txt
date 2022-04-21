cmake_minimum_required(VERSION 3.10)
project("Waffle - Wayland Compositor" LANGUAGES CXX C)

set(TARGET "waffle")
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server)
pkg_check_modules(GLES2 REQUIRED glesv2)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

# depends on backend type.
if(${BACKEND_TYPE} MATCHES "DRM-(GBM|EGLSTREAM)")
  # DRM backend
  pkg_check_modules(DRM REQUIRED libdrm)
  pkg_check_modules(LIBINPUT REQUIRED libinput)
  pkg_check_modules(LIBUDEV REQUIRED libudev)
  pkg_check_modules(LIBSYSTEMD REQUIRED libsystemd)
  if(${BACKEND_TYPE} STREQUAL "DRM-GBM")
    pkg_check_modules(GBM REQUIRED gbm)
  endif()
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
elseif(${BACKEND_TYPE} STREQUAL "X11")
  pkg_check_modules(X11 REQUIRED x11)
endif()

include(cmake/generate_wayland_protocols.cmake)
set(_wayland_base_xml "$ENV{PKG_CONFIG_SYSROOT_DIR}/usr/share/wayland/wayland.xml")
set(_wayland_protocols_xml_dir "$ENV{PKG_CONFIG_SYSROOT_DIR}/usr/share/wayland-protocols")
set(_wayland_protocols_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/wayland/protocols")
file(MAKE_DIRECTORY "${_wayland_protocols_src_dir}")

# generates wayland-server-protocol.c/h
generate_wayland_server_protocol(
  PROTOCOL_FILE "${_wayland_base_xml}"
  CODE_FILE "${_wayland_protocols_src_dir}/wayland-server-protocol.c"
  HEADER_FILE "${_wayland_protocols_src_dir}/wayland-server-protocol.h")

# generates xdg-shell-server-protocol.c/h
generate_wayland_server_protocol(
  PROTOCOL_FILE "${_wayland_protocols_xml_dir}/unstable/xdg-shell/xdg-shell-unstable-v6.xml"
  CODE_FILE "${_wayland_protocols_src_dir}/xdg-shell-server-protocol.c"
  HEADER_FILE "${_wayland_protocols_src_dir}/xdg-shell-server-protocol.h")

# The platform-dependent definitions such as EGLNativeDisplayType and 
# EGLNativeWindowType depend on related include files or define such as gbm.h
# or "__GBM__". So, need to avoid a link error which is caused by the 
# include order of related header files. See: /usr/include/EGL/eglplatform.h
if(${BACKEND_TYPE} STREQUAL "DRM-GBM")
  add_definitions(-D__GBM__)
elseif(${BACKEND_TYPE} STREQUAL "DRM-EGLSTREAM")
  add_definitions(-DEGL_NO_X11)
elseif(${BACKEND_TYPE} STREQUAL "X11")
  add_definitions(-DUSE_X11)
else()
  add_definitions(-DWL_EGL_PLATFORM)
endif()

add_definitions(-DENABLE_WAFFLE_LOG)

# display backend type.
set(DISPLAY_BACKEND_SRC "")
if(${BACKEND_TYPE} STREQUAL "DRM-GBM")
  add_definitions(-DDISPLAY_BACKEND_TYPE_DRM_GBM)
  set(DISPLAY_BACKEND_SRC
  "src/waffle/backend/window/native_window_drm.cc"
  "src/waffle/backend/window/native_window_drm_gbm.cc")
  elseif(${BACKEND_TYPE} STREQUAL "DRM-EGLSTREAM")
  add_definitions(-DDISPLAY_BACKEND_TYPE_DRM_EGLSTREAM)
  # TODO
elseif(${BACKEND_TYPE} STREQUAL "X11")
  add_definitions(-DDISPLAY_BACKEND_TYPE_X11)
  set(DISPLAY_BACKEND_SRC
    "src/waffle/backend/window/waffle_window_x11.cc"
    "src/waffle/backend/window/native_window_x11.cc")
else()
  add_definitions(-DFLUTTER_TARGET_BACKEND_WAYLAND)
  # TODO
endif()

add_executable(${TARGET}
  "src/waffle/main.cc"
  "src/waffle/logger.cc"
  "src/waffle/wayland_server.cc"
  "src/waffle/backend/backend.cc"
  "src/waffle/backend/surface/context_egl.cc"
  "src/waffle/backend/surface/egl_utils.cc"
  "src/waffle/backend/surface/linux_egl_surface.cc"
  "src/waffle/backend/surface/surface_base.cc"
  "src/waffle/backend/surface/surface_gl.cc"
  "${DISPLAY_BACKEND_SRC}"
  "src/waffle/compositor/compositor.cc"
  "src/waffle/renderer/texture.cc"
  "src/waffle/renderer/texture_context.cc"
  "src/waffle/renderer/window_renderer.cc"
  "src/waffle/renderer/shader/shader.cc"
  "src/waffle/renderer/shader/shader_context.cc"
  "src/waffle/renderer/shader/shader_program.cc"
  "src/waffle/wayland/wayland_data_device_manager.cc"
  "src/waffle/wayland/wayland_resource.cc"
  "src/waffle/wayland/wayland_region.cc"
  "src/waffle/wayland/wayland_seat.cc"
  "src/waffle/wayland/wayland_surface.cc"
  "src/waffle/wayland/wayland_shell_surface.cc"
  "src/waffle/wayland/xdg_shell_surface.cc"
  "${_wayland_protocols_src_dir}/wayland-server-protocol.c"
  "${_wayland_protocols_src_dir}/xdg-shell-server-protocol.c"
)

target_link_libraries(${TARGET} PRIVATE "${EGL_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${GLES2_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${WAYLAND_SERVER_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "/usr/lib/libSOIL.so")
target_link_libraries(${TARGET} PRIVATE "${DRM_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${GBM_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${LIBINPUT_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${LIBUDEV_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${LIBSYSTEMD_LIBRARIES}")
target_link_libraries(${TARGET} PRIVATE "${X11_LIBRARIES}")
if(${BACKEND_TYPE} MATCHES "DRM-(GBM|EGLSTREAM)")
target_link_libraries(${TARGET} PRIVATE Threads::Threads)
endif()

set(THIRD_PARTY_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/third_party")
target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${TARGET} PRIVATE "${THIRD_PARTY_DIRS}")
target_include_directories(${TARGET} PRIVATE PkgConfig::EGL)
target_include_directories(${TARGET} PRIVATE PkgConfig::GLES2)

target_include_directories(${TARGET} PRIVATE ${DRM_INCLUDE_DIRS})
target_include_directories(${TARGET} PRIVATE ${GBM_INCLUDE_DIRS})
target_include_directories(${TARGET} PRIVATE ${LIBINPUT_INCLUDE_DIRS})
target_include_directories(${TARGET} PRIVATE ${LIBUDEV_INCLUDE_DIRS})
target_include_directories(${TARGET} PRIVATE ${LIBSYSTEMD_INCLUDE_DIRS})
target_include_directories(${TARGET} PRIVATE ${X11_INCLUDE_DIRS})
