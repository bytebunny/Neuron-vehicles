cmake_minimum_required( VERSION 3.10 )

if ( NOT CMAKE_BUILD_TYPE ) # Default to Release to have this configuration's flags.
  set( CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE )
endif()

project( Neuron-vehicles VERSION 4.0 LANGUAGES CXX )

configure_file( src/config.hpp.in config.hpp ) # puts config.hpp under ${CMAKE_CURRENT_BINARY_DIR}.
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # specify the output directory as an include directory.

#------------------------------------ Dependencies ---------------------------------
find_package( OpenGL REQUIRED COMPONENTS OpenGL )
find_package( GLUT REQUIRED )
find_package( Catch2 REQUIRED ) # for unit tests.

#------------------------------ Targets and Properties -----------------------------
add_executable( neuron_vehicles.exe )

target_sources( neuron_vehicles.exe
  PRIVATE
  src/brait032.cpp
  src/constants.hpp
  src/vehicle_types.hpp
  src/bvglobalini.hpp
  src/Coordmatrix.hpp
  src/NBV.hpp
  src/Binoculus.hpp src/Binoculus.cpp
  src/NRWindow.hpp src/NRWindow.cpp
  src/glutMaster.hpp src/glutMaster.cpp
  src/glutWindow.hpp src/glutWindow.cpp )

target_include_directories( neuron_vehicles.exe
  PRIVATE
  ${GLUT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src )

target_link_libraries( neuron_vehicles.exe PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

target_compile_features( neuron_vehicles.exe PRIVATE cxx_std_17 ) # we use 'inline constexpr'

target_compile_options( neuron_vehicles.exe PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: # how to add other warning options when ..._ID:Intel?
  -Wall -Wpedantic -Weffc++ -Wextra -Wsign-conversion -Wshadow> )

if( CMAKE_BUILD_TYPE STREQUAL Release )
  message( "/// Release build." )
  target_compile_options( neuron_vehicles.exe PRIVATE -O3 )
elseif( CMAKE_BUILD_TYPE STREQUAL Debug )
  message( "/// Debug build." )
  target_compile_options( neuron_vehicles.exe PRIVATE -O0 -g )
endif()

# Target for unit tests
add_executable( tests.exe )

target_sources( tests.exe
  PRIVATE
  tests/tests.cpp
  src/glutWindow.cpp
  tests/tests-glutWindow.cpp )

target_include_directories( tests.exe
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/tests
  ${CMAKE_CURRENT_SOURCE_DIR}/src )

target_link_libraries( tests.exe PRIVATE Catch2::Catch2 )

#----------------------------------------- Testing --------------------------------
enable_testing() # CTest

include(Catch)
catch_discover_tests( tests.exe ) # auto-discover TEST_CASEs.
