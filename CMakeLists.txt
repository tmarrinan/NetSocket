cmake_minimum_required (VERSION 3.0)

#################################
# Project
#################################
if( NOT CMAKE_VERSION VERSION_LESS 3.9 )
    project( netsocket VERSION 1.0.0 DESCRIPTION "C++ Network Socket Interface" )
else()
    project( netsocket VERSION 1.0.0 )
endif()

# set paths for build outputs
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )


#################################
# NetSocket Library
#################################

# configure
 set( CMAKE_CXX_FLAGS "-DASIO_STANDALONE -std=c++11" CACHE TYPE INTERNAL FORCE )

# directories to search for .h files
include_directories (
    "${PROJECT_SOURCE_DIR}/third_party/asio/include"
    "${PROJECT_SOURCE_DIR}/include"
    )

# add the library
add_library(netsocket SHARED 
    src/server.cpp
    src/clientconnection.cpp
    src/client.cpp
    )

# add libraries to link against
# n/a


#################################
# NetSocket Demo App
#################################

# add the server executable
add_executable(demoserver 
    example/demoserver.cpp
    )

# add libraries to link against
link_directories(${PROJECT_BINARY_DIR})
target_link_libraries (demoserver 
    netsocket
    )

# add the client executable
add_executable(democlient 
    example/democlient.cpp
    )

# add libraries to link against
link_directories(${PROJECT_BINARY_DIR})
target_link_libraries (democlient 
    netsocket
    )
