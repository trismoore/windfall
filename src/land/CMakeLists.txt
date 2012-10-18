add_library(land STATIC
  landscape.cpp
  qtree.cpp
  landpatch.cpp
)

target_link_libraries(land ${ALL_LIBS})
