# CMakeLists.txt
#
# Base build script for building with cmake (http://www.cmake.org)

cmake_minimum_required( VERSION 3.5 )
project( PDW )

# Project version number
set( PDW_VERSION_MAJOR 3 )
set( PDW_VERSION_MINOR 1 )
set( PDW_VERSION_MICRO 0 )

# Set base variables
set( PDW_GFXS )
set( PDW_INCLUDES )
set( PWD_SRCS )

# Subdirectories to go to
add_subdirectory( GFX )
add_subdirectory( Headers )
add_subdirectory( utils )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.0.2/include" )
link_directories( "${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.0.2/lib/VC/static" )

# Microsoft Visual Studio Specific definitions
if( MSVC )
  add_definitions( "-D_CRT_SECURE_NO_WARNINGS" )
endif( MSVC )

# Include Files
set( PDW_INCLUDES
  ${PDW_INCLUDES}
  resrc1.h
)

# Source Files
set( PDW_SRCS
  ${PDW_SRCS}
  Flex.cpp
  Gfx.cpp
  Initapp.cpp
  Misc.cpp
  Rsrc.rc
  PDW.cpp
  Pocsag.cpp
  acars.cpp
  decode.cpp
  ermes.cpp
  helper_funcs.cpp
  language.cpp
  menu.cpp
  mobitex.cpp
  toolbar.cpp
  printer.cpp
  sigind.cpp
  sound_in.cpp
  iob_shim.cpp
)

if( MSVC )
  foreach( flag_var
          CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
          CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
          CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
          CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
    string( REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}" )
  endforeach()
endif()

# Create executable
add_executable( PDW WIN32
  ${PDW_INCLUDES}
  ${PDW_SRCS}
)

# Required libraries
target_link_libraries( PDW
  Comctl32.lib
  Winmm.lib
  Ws2_32.lib
  libeay32MT.lib
  ssleay32MT.lib
  legacy_stdio_definitions.lib
)
