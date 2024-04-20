cmake_minimum_required(VERSION 3.5)

project(shaders)

# Find all shaders.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.tese"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.comp"
)

# Add the shaders to a custom target (OPTIONAL)
add_custom_target(${PROJECT_NAME} SOURCES ${SHADERS})

# Optionally, if you want to copy shaders to the build directory
foreach(shader ${SHADERS})
    file(RELATIVE_PATH shader_rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${shader}")
    configure_file(${shader} "${CMAKE_BINARY_DIR}/${shader_rel_path}" COPYONLY)
endforeach()
