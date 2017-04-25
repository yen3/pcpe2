## Set compiler flags to turn off optimization and enable coverage
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")

## Build Test Executables
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include)
SET(TEST_BIN max_comsubseq_unittests)

ADD_EXECUTABLE(${TEST_BIN} ${TEST_SRCS} ${PROJECT_SRCS})
SET_PROPERTY(TARGET ${TEST_BIN} PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET ${TEST_BIN} PROPERTY CXX_STANDARD_REQUIRED ON)
TARGET_LINK_LIBRARIES(${TEST_BIN} gtest_main ${CMAKE_THREAD_LIBS_INIT})

## Setup unit test
ADD_CUSTOM_TARGET(check ${EXECUTABLE_OUTPUT_PATH}/${TEST_BIN}
    DEPENDS ${TEST_BIN} COMMENT "Executing unit tests..." SOURCES ${TEST_SRCS})

## Copy test input files
FILE(COPY ${MAINFOLDER}/test/testdata DESTINATION ${CMAKE_BINARY_DIR}/test)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/testoutput)

## Setup Code Coverage if feature is available via compiler
#IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
   #"${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	#SETUP_TARGET_FOR_COVERAGE(${PROJECT_NAME}-coverage ${TEST_BIN} coverage)
#ENDIF()

