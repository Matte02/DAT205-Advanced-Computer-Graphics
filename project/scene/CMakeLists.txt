# Build the scene library.
add_library(scene
    terrain.cpp terrain.h
    triangle_list.cpp triangle_list.h
    geomip_grid.h geomip_grid.cpp
    lod_manager.cpp lod_manager.h
    camera.h
    erosion.h  erosion.cpp
    quad_list.cpp quad_list.h
    terrain_technique.h terrain_technique.cpp
    texture_config.h
    generation_technique.h generation_technique.cpp
    NoiseSettings.h
    normal_map_technique.h normal_map_technique.cpp)

# Include the necessary directories for this library.
target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link the subfolder library with labhelper library
target_link_libraries(scene PRIVATE labhelper)