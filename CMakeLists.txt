cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULES_PATH} ${CMAKE_SOURCE_DIR}/Modules)

SET(PROJECT_NAME "gtkPview")

include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")


PROJECT(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS "-g")
SET(CMAKE_CXX_FLAGS_DEBUG)
SET( gtkPview_SRCS
     main.c )


find_package(GTK2 COMPONENTS gtk)
find_package(CURL)

INCLUDE_DIRECTORIES(
  /usr/include
  ${GTK2_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIR}
)

# possible options
set(OPTLIST porn apina apinaporn tits)
# default choice
set(SELECTED_OPT tits)
# set the one we have in list
foreach(OPT ${OPTLIST})
  if(${${OPT}})
    set(SELECTED_OPT ${OPT})
  endif()
endforeach(OPT)
# remove all
remove_definitions(${OPTLIST})
# add the selected
if(${SELECTED_OPT})
  add_definitions(-D${SELECTED_OPT})
endif()


ADD_EXECUTABLE( ${PROJECT_NAME} ${gtkPview_SRCS} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${GTK2_LIBRARIES} ${CURL_LIBRARIES} )

