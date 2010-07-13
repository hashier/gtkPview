cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULES_PATH} ${CMAKE_SOURCE_DIR}/Modules)

SET(PROJECT_NAME "gtkPview")

include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")


PROJECT(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS "-g")
SET(CMAKE_CXX_FLAGS_DEBUG)

SET( ${PROJECT_NAME}_SRCS
     main.c )


find_package(GTK2 COMPONENTS gtk)
find_package(CURL)

INCLUDE_DIRECTORIES(
  ${GTK2_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIR}
)

# possible options
set(OPTLIST PORN APINA APINAPORN TITS)
# remove all
remove_definitions(${OPTLIST})
# Always use the first one that occurs on the cmd line
if(tits)
  add_definitions(-DTITS)
elseif(porn)
  add_definitions(-DPORN)
elseif(apinaporn)
  add_definitions(-DAPINAPORN)
elseif(apina)
  add_definitions(-DAPINA)
endif()


ADD_EXECUTABLE( ${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${GTK2_LIBRARIES} ${CURL_LIBRARIES} )

