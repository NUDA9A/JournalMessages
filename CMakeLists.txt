cmake_minimum_required(VERSION 3.20)

project(JournalMessages VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

option(JOURNAL_ENABLE_SOCKET "Build socket logging support" OFF)

add_subdirectory(library)
add_subdirectory(application)

if (JOURNAL_ENABLE_SOCKET)
    add_subdirectory(collector)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

