sdk_generate_library()

sdk_add_compile_definitions(-DLV_KCONFIG_IGNORE)

sdk_add_include_directories(.)
sdk_add_include_directories(lvgl)
file(GLOB_RECURSE lvgl_sources "${CMAKE_CURRENT_SOURCE_DIR}/lvgl/*.c")
sdk_library_add_sources(${lvgl_sources})

sdk_add_include_directories(port)
file(GLOB_RECURSE lvgl_port_sources "${CMAKE_CURRENT_SOURCE_DIR}/port/*.c")
sdk_library_add_sources(${lvgl_port_sources})