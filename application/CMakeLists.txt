add_executable(journal_application src/main.cpp src/message_queue.cpp)

set_target_properties(journal_application PROPERTIES OUTPUT_NAME "JournalMessages")

target_include_directories(journal_application PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_link_libraries(journal_application PRIVATE JournalMessages::Static)

target_compile_features(journal_application PRIVATE cxx_std_17)

target_compile_options(journal_application PRIVATE -Wall -Wextra -Wpedantic)