cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

# Include the subdirectory containing the scene
add_subdirectory(scene)

# Build and link executable.
add_executable ( ${PROJECT_NAME}
    main.cpp
)

# Link the executable with the scene library
target_link_libraries(${PROJECT_NAME} scene)

# Define the path to the shaders directory
set(SHADERS_DIR "${CMAKE_BINARY_DIR}/shaders")

# Pass the shaders directory path as a compile definition
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADERS_DIR="${SHADERS_DIR}")

# Link the executable with labhelper library
target_link_libraries(${PROJECT_NAME} labhelper)

config_build_output()
