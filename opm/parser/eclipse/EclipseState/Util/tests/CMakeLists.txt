add_executable(runOrderedMapTests OrderedMapTests.cpp)
target_link_libraries(runOrderedMapTests Parser ${Boost_LIBRARIES})
add_test(NAME runOrderedMapTests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${TEST_MEMCHECK_TOOL} ${EXECUTABLE_OUTPUT_PATH}/runOrderedMapTests )


add_executable(runValueTests ValueTests.cpp)
target_link_libraries(runValueTests Parser ${Boost_LIBRARIES})
add_test(NAME runValueTests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${TEST_MEMCHECK_TOOL} ${EXECUTABLE_OUTPUT_PATH}/runValueTests )


