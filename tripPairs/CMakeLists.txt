CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

## make Visual Studio behave nicely
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tripPairs")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tripPairs")
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/bin")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/tripPairs")
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/tripPairs")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

include_directories(${tripRoot_SOURCE_DIR})

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -m64 -O3")
endif(NOT WIN32)

if(WIN32)
  set(NO_N4_128 1)
endif(WIN32)

if(WIN32)
  set(extension ".dll")
else(WIN32)
  set(extension ".so")
endif(WIN32)

if(NOT NO_N4_128)
  add_definitions(-DN4INT128)
endif(NOT NO_N4_128)

if(NOT NOEXTRACT)
  add_definitions(-DdoExtractAndContract)
endif(NOT NOEXTRACT)

if(CONTRACT_NUM)
  add_definitions("-DCONTRACT_MAX_EXTRA_SIZE=$(CONTRACT_NUM)")
endif(CONTRACT_NUM)

message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

## LIBRARIES
## EXECUTABLES

add_executable(tripPair triplet_pairing.cpp
  CountingArray.cpp
  HDT.cpp
  HDTCountingCCToC.cpp
  HDTCountingCTransform.cpp
  HDTCountingG.cpp
  HDTCountingIGToC.cpp
  HDTCountingLeaf.cpp
  HDTFactory.cpp
  HDTListUtils.cpp
  NewickParser.cpp
  RootedTree.cpp
  RootedTreeFactory.cpp
  TripletPairing.cpp
  TripletPairing.cpp
  int_stuff.cpp)


## Installation
#install(TARGETS tripPairs RUNTIME DESTINATION bin)
#if(WIN32)
#  install(FILES VCredist/msvcp120.dll VCredist/msvcr120.dll DESTINATION bin)
#endif(WIN32)
