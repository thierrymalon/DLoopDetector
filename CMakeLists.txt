cmake_minimum_required(VERSION 2.8)
project(DLoopDetector)

#list( APPEND CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS} ")

option(WITH_OPENMP      "Enable OpenMP multithreading"               ON)
option(BUILD_DemoSIFT   "Build demo application with SIFT features"  ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/.cmake")

set(HDRS
  include/DLoopDetector/DLoopDetector.h         include/DLoopDetector/TemplatedLoopDetector.h)

find_package(OpenCV REQUIRED)
find_package(DLib REQUIRED)
find_package(DBoW2 REQUIRED)

include_directories(include/DLoopDetector/ ${OpenCV_INCLUDE_DIRS} ${DLIB_INCLUDE_DIRS} ${DBOW2_INCLUDE_DIRS})

if (WITH_OPENMP)
    find_package(OpenMP QUIET)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPEN_MP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else()
        message("")
        UPDATE_CACHE_VARIABLE(WITH_OPENMP OFF)
    endif(OPENMP_FOUND)
endif(WITH_OPENMP)

if(BUILD_DemoSIFT)
  add_executable(demo_sift demo/demo_sift.cpp)
  target_link_libraries(demo_sift ${OpenCV_LIBS} ${DLIB_LIBRARIES} ${DBOW2_LIBRARIES})
endif(BUILD_DemoSIFT)

if(BUILD_DemoSIFT)
  set(RESOURCE_FILE ${CMAKE_BINARY_DIR}/resources.tar.gz)
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/resources/)
    if(NOT EXISTS ${RESOURCE_FILE})
      file(DOWNLOAD http://doriangalvez.com/resources/DLoopDetector/resources.tar.gz
        ${RESOURCE_FILE} STATUS status SHOW_PROGRESS EXPECTED_MD5 c001da68ddaceed1de9c16aaac22ed80)
      if(${status})
        message(FATAL_ERROR "Error downloading resources for demo applications")
      endif(${status})
    endif(NOT EXISTS ${RESOURCE_FILE})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${RESOURCE_FILE})
    file(REMOVE ${RESOURCE_FILE})
  endif(NOT EXISTS ${CMAKE_BINARY_DIR}/resources/)
endif(BUILD_DemoSIFT)

configure_file(src/DLoopDetector.cmake.in
  "${PROJECT_BINARY_DIR}/DLoopDetectorConfig.cmake" @ONLY)

install(DIRECTORY include/DLoopDetector DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/DLoopDetectorConfig.cmake"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
install(FILES "${PROJECT_BINARY_DIR}/DLoopDetectorConfig.cmake"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/DLoopDetector/)
