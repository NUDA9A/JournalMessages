add_executable(journal_collector src/main.cpp src/tcp_server.cpp src/statistics.cpp)

set_target_properties(journal_collector PROPERTIES OUTPUT_NAME "JournalCollector")

target_include_directories(journal_collector PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_link_libraries(journal_collector PRIVATE JournalMessages::Static)

target_compile_features(journal_collector PRIVATE cxx_std_17)

target_compile_options(journal_collector PRIVATE -Wall -Wextra -Wpedantic)