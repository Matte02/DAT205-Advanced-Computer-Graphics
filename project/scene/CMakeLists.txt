# Build the scene library.
add_library(scene
    terrain.cpp
    terrain.h
    triangle_list.cpp
    triangle_list.h
    height_map_generator.h
    height_map_generator.cpp
    geomip_grid.h
    geomip_grid.cpp
    lod_manager.cpp
    lod_manager.h
    camera.h)

# Include the necessary directories for this library.
target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link the subfolder library with labhelper library
target_link_libraries(scene PRIVATE labhelper)