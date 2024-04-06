cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

# Find *all* shaders.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)

# Separate filter for shaders.
source_group("Shaders" FILES ${SHADERS})
source_group("Main" FILES main.cpp)
# Build and link executable.
add_executable ( ${PROJECT_NAME}
    main.cpp
	terrain.cpp
	terrain.h
	triangle_list.cpp
	triangle_list.h
    ${SHADERS}
    perline_noise_terrain.h
    perline_noise_terrain.cpp
    geomip_grid.h
    geomip_grid.cpp
    lod_manager.cpp
    lod_manager.h)
   

target_link_libraries ( ${PROJECT_NAME} labhelper )
config_build_output()
