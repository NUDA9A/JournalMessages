set(JOURNAL_TEST_SOURCES
        test_main.cpp
        log_level_tests.cpp
        file_journal_tests.cpp
        message_queue_tests.cpp
        ${PROJECT_SOURCE_DIR}/application/src/message_queue.cpp
)

if (JOURNAL_ENABLE_SOCKET)
    list(APPEND JOURNAL_TEST_SOURCES
            socket_journal_integration_tests.cpp
            ${PROJECT_SOURCE_DIR}/collector/src/tcp_server.cpp
    )
endif()

function(add_journal_test_target target_name test_name library_target)
    add_executable(${target_name} ${JOURNAL_TEST_SOURCES})

    target_include_directories(${target_name}
            PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/application/include/>
    )

    if (JOURNAL_ENABLE_SOCKET)
        target_include_directories(${target_name}
                PRIVATE
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/collector/include/>
        )
    endif()

    target_link_libraries(${target_name} PRIVATE ${library_target})

    target_compile_features(${target_name} PRIVATE cxx_std_17)
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)

    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${target_name}>)

    set_tests_properties(${test_name}
            PROPERTIES
            TIMEOUT 10
    )

    if (JOURNAL_ENABLE_SOCKET)
        set_tests_properties(${test_name}
                PROPERTIES
                RUN_SERIAL TRUE
        )
    endif()
endfunction()

add_journal_test_target(
        journal_tests
        journalTestStatic
        JournalMessages::Static
)

add_journal_test_target(
        journal_tests_shared
        journalTestShared
        JournalMessages::Shared
)