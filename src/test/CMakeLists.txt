add_library(test STATIC
  testrenderer.cpp
)

target_link_libraries(test ${ALL_LIBS})