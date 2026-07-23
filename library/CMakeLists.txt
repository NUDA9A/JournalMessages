add_library(journal_objects OBJECT src/log_level.cpp src/journal_status.cpp src/file_journal.cpp)

if (JOURNAL_ENABLE_SOCKET)
    target_sources(journal_objects PRIVATE src/socket_journal.cpp)
endif()

target_compile_features(journal_objects PRIVATE cxx_std_17)
set_target_properties(journal_objects PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(journal_objects PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_compile_options(journal_objects PRIVATE -Wall -Wextra -Wpedantic)

add_library(journal_static STATIC $<TARGET_OBJECTS:journal_objects>)
add_library(journal_shared SHARED $<TARGET_OBJECTS:journal_objects>)

if (JOURNAL_ENABLE_SOCKET)
    target_compile_definitions(journal_static PUBLIC JOURNAL_ENABLE_SOCKET=1)
    target_compile_definitions(journal_shared PUBLIC JOURNAL_ENABLE_SOCKET=1)
else()
    target_compile_definitions(journal_static PUBLIC JOURNAL_ENABLE_SOCKET=0)
    target_compile_definitions(journal_shared PUBLIC JOURNAL_ENABLE_SOCKET=0)
endif()

target_include_directories(journal_static PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(journal_shared PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

set_target_properties(journal_static PROPERTIES OUTPUT_NAME "journal")
set_target_properties(journal_shared PROPERTIES OUTPUT_NAME "journal")

target_compile_features(journal_static PUBLIC cxx_std_17)
target_compile_features(journal_shared PUBLIC cxx_std_17)

add_library(JournalMessages::Static ALIAS journal_static)
add_library(JournalMessages::Shared ALIAS journal_shared)