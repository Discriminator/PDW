# CMakeLists.txt
#
# Base build script for building with cmake (http://www.cmake.org)

cmake_minimum_required( VERSION 2.8 )
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
)

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
)
