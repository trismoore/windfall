# CMake entry point
cmake_minimum_required (VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)

set(PROJECTNAME Windfall)

project (${PROJECTNAME})

# we use our own find modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# copy some files over
file(COPY "${PROJECT_SOURCE_DIR}/data" DESTINATION "${PROJECT_BINARY_DIR}/")

# awesomium files
#file(COPY "${PROJECT_SOURCE_DIR}/locales" DESTINATION "${PROJECT_BINARY_DIR}/")
#file(COPY "${PROJECT_SOURCE_DIR}/chrome.pak" DESTINATION "${PROJECT_BINARY_DIR}/")
#file(COPY "${PROJECT_SOURCE_DIR}/AwesomiumProcess" DESTINATION "${PROJECT_BINARY_DIR}/")
#file(COPY "${PROJECT_SOURCE_DIR}/libawesomium-1.6.3.so" DESTINATION "${PROJECT_BINARY_DIR}/")

#file(COPY "${PROJECT_SOURCE_DIR}/lib" DESTINATION "${PROJECT_BINARY_DIR}/")

#######################################

# add git version info into CMake files, and from there the application
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
set(GIT_REV 0)
execute_process(COMMAND git rev-list HEAD
                COMMAND wc -l
                OUTPUT_STRIP_TRAILING_WHITESPACE
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                OUTPUT_VARIABLE GIT_REV )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)
#list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" GitSHA1.h)
# get number of commits, to use as 'revision' number
message("Git version info: v${GIV_REV} = ${GIT_SHA1}")
#######################################
# find packages
find_package(OpenGL REQUIRED)
set(ALL_LIBS ${ALL_LIBS} ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
set(ALL_LIBS ${ALL_LIBS} ${GLEW_GLEW_LIBRARY})
message("GLEW: inc: ${GLEW_INCLUDE_DIR}, lib: ${GLEW_GLEW_LIBRARY}")

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_PATH})
set(ALL_LIBS ${ALL_LIBS} ${GLFW_LIBRARY})
message("GLFW: inc: ${GLFW_INCLUDE_PATH}, lib: ${GLFW_LIBRARY}")

add_definitions(
  -DGLEW_STATIC
  -D_CRT_SECURE_NO_WARNINGS
)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

# horrible hack
IF(NOT WIN32)
 IF(NOT APPLE)
  set(ALL_LIBS ${ALL_LIBS} -lXxf86vm)
 ENDIF(NOT APPLE)
ENDIF(NOT WIN32)

#######################################
# do each of these in turn

set(MODULES
 base
 test
# landscape
# windfall
)

# include them first, so the others can use them
foreach(m ${MODULES})
  set(ALL_LIBS ${ALL_LIBS} ${m})
  include_directories(${m})
endforeach(m)
message("ALL LIBS is ${ALL_LIBS}")

# then compile them into a library
foreach(m ${MODULES})
  add_subdirectory(${m})
endforeach(m)

########################################
# and then compile the main program

add_executable(${PROJECTNAME}
  main.cpp
  ${PROJECT_BINARY_DIR}/GitSHA1.cpp
)

target_link_libraries(${PROJECTNAME} ${ALL_LIBS})
