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
if( WIN32 )
    set( CMAKE_CXX_FLAGS "/EHsc /D_WIN32_WINNT=0x0501 /DASIO_STANDALONE /D_VARIADIC_MAX=10" CACHE TYPE INTERNAL FORCE )
else()
    set( CMAKE_CXX_FLAGS "-std=c++11 -DASIO_STANDALONE -D_VARIADIC_MAX=10" CACHE TYPE INTERNAL FORCE )
endif()

# find OpenSSL
find_package(OpenSSL REQUIRED)
 
# directories to search for .h files
include_directories (
    "${PROJECT_SOURCE_DIR}/third_party/asio-1.12.1/include"
    "${PROJECT_SOURCE_DIR}/include"
    "${OPENSSL_INCLUDE_DIR}"
    )

# add the library
add_library(netsocket SHARED
    src/netsocket.cpp 
    src/server.cpp
    src/clientconnection.cpp
    src/client.cpp
    src/socket.cpp
    src/basicsocket.cpp
    src/securesocket.cpp
    )

# add libraries to link against
if( UNIX AND NOT APPLE )
    target_link_libraries (netsocket 
        ${OPENSSL_LIBRARIES}
        pthread
        )
else()
    target_link_libraries (netsocket 
        ${OPENSSL_LIBRARIES}
        )
endif()


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
