cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

# Build and link executable.
add_executable ( ${PROJECT_NAME}
    main.cpp
	terrain.cpp
	terrain.h
	triangle_list.cpp
	triangle_list.h
    perline_noise_terrain.h
    perline_noise_terrain.cpp
    geomip_grid.h
    geomip_grid.cpp
    lod_manager.cpp
    lod_manager.h)
   
    # Define the path to the shaders directory
set(SHADERS_DIR "${CMAKE_BINARY_DIR}/shaders")

# Pass the shaders directory path as a compile definition
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADERS_DIR="${SHADERS_DIR}")
target_link_libraries ( ${PROJECT_NAME} labhelper )
config_build_output()
