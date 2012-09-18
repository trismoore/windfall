add_library(base STATIC
  console.cpp
  ogl.cpp
  config.cpp
  renderer.cpp
  shader.cpp
  texture.cpp
  vbo.cpp
  camera.cpp
)

target_link_libraries(base ${ALL_LIBS})