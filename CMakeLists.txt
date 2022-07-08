cmake_minimum_required(VERSION 3.22)
project(CG)

set(CMAKE_CXX_STANDARD 17)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(CG main.cpp src/glad.c include/learnopengl/shader_s.h include/stb_image.h stb_image_wrap.cpp include/learnopengl/shader_m.h include/learnopengl/camera.h include/learnopengl/vertices.h include/learnopengl/utility.cpp include/learnopengl/utility.h include/learnopengl/mesh.cpp include/learnopengl/mesh.h include/learnopengl/model.cpp include/learnopengl/model.h)

target_link_libraries(CG ${PROJECT_SOURCE_DIR}/lib/glfw3.dll ${PROJECT_SOURCE_DIR}/lib/assimp-vc142-mtd.lib ${PROJECT_SOURCE_DIR}/lib/assimp-vc142-mtd.dll)
