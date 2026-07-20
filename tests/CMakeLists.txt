add_executable(journal_tests test_main.cpp log_level_tests.cpp file_journal_tests.cpp)

target_link_libraries(journal_tests PRIVATE JournalMessages::Static)

target_compile_features(journal_tests PRIVATE cxx_std_17)
target_compile_options(journal_tests PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME journalTest COMMAND $<TARGET_FILE:journal_tests>)