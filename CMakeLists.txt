cmake_minimum_required( VERSION 3.10 )

if ( NOT CMAKE_BUILD_TYPE ) # Default to Release to have this configuration's flags.
  set( CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE )
endif()

project( Neuron-vehicles VERSION 4.0 LANGUAGES CXX )

configure_file( config.hpp.in config.hpp ) # puts config.h under ${CMAKE_CURRENT_BINARY_DIR}
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#------------------------------------ Dependencies ---------------------------------
find_package( OpenGL REQUIRED COMPONENTS OpenGL )
find_package( GLUT REQUIRED )

#------------------------------ Targets and Properties -----------------------------
add_executable( neuron_vehicles )

target_sources( neuron_vehicles
  PRIVATE brait032.cpp nrWindow.cpp glutMaster.cpp glutWindow.cpp )

target_include_directories( neuron_vehicles PRIVATE ${GLUT_INCLUDE_DIRS} )

target_link_libraries( neuron_vehicles PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

target_compile_features( neuron_vehicles PRIVATE cxx_std_17 ) # we use 'inline constexpr'

target_compile_options( neuron_vehicles PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: # how to add other warning options when ..._ID:Intel?
  -Wall -Wpedantic -Weffc++ -Wextra -Wsign-conversion -Wshadow> )

if( CMAKE_BUILD_TYPE STREQUAL Release )
  message( "/// Release build." )
  target_compile_options( neuron_vehicles PRIVATE -O3 )
elseif( CMAKE_BUILD_TYPE STREQUAL Debug )
  message( "/// Debug build." )
  target_compile_options( neuron_vehicles PRIVATE -O0 -g )
endif()
