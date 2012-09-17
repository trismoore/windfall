add_library(base STATIC
  console.cpp
  ogl.cpp
  config.cpp
)

target_link_libraries(base ${ALL_LIBS})