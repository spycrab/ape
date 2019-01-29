set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Tests")

include(GoogleTest)

add_executable(StringTest Common/StringTest.cpp)
set_target_properties(StringTest PROPERTIES FOLDER ${CMAKE_BINARY_DIR}/Tests)
target_link_libraries(StringTest PRIVATE gtest_main)
target_include_directories(StringTest PUBLIC ${GTEST_INCLUDE_DIR})

gtest_add_tests(TARGET StringTest)

add_custom_target(tests ${CMAKE_CTEST_COMMAND} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DEPENDS StringTest)
