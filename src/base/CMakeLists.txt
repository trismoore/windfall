add_library(base STATIC
  console.cpp
  ogl.cpp
  config.cpp
  renderer.cpp
)

target_link_libraries(base ${ALL_LIBS})