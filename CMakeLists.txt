cmake_minimum_required(VERSION 3.5.0)
project(odva_ethernetip VERSION 0.1.0 LANGUAGES CXX)

find_package(console_bridge REQUIRED)
find_package(Boost 1.47 REQUIRED COMPONENTS system)

## Declare a cpp library for just Ethernet/IP generic stuff
add_library(${PROJECT_NAME} SHARED
  src/connection.cpp
  src/copy_serializable.cpp
  src/cpf_item.cpp
  src/cpf_packet.cpp
  src/encap_header.cpp
  src/encap_packet.cpp
  src/identity_item_data.cpp
  src/io_scanner.cpp
  src/message_router_response.cpp
  src/path.cpp
  src/rr_data.cpp
  src/session.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${Boost_LIBRARIES}
  ${console_bridge_LIBRARIES}
)

if(CXX_FEATURE_FOUND EQUAL "-1")
    target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11)
else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${console_bridge_INCLUDE_DIRS})
  
  
list (APPEND PACKAGE_LIBRARIES ${PROJECT_NAME})

# Mark executables and/or libraries for installation
install(TARGETS ${PACKAGE_LIBRARIES}
    EXPORT ${PROJECT_NAME}-targets DESTINATION lib)
install(EXPORT ${PROJECT_NAME}-targets
    NAMESPACE odva_ethernetip:: DESTINATION lib/cmake/${PROJECT_NAME})
 
# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}
  DESTINATION include
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
  PATTERN ".svn" EXCLUDE
 )
 
install(FILES package.xml DESTINATION share/${PROJECT_NAME})
 
# Create cmake config files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
 
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
 
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME})
 
export(EXPORT ${PROJECT_NAME}-targets FILE
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake)
  
if (ENABLE_TESTS)
  enable_testing()
  add_custom_target(run_tests ALL
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -V)

  add_subdirectory(test)
endif()

#if (CATKIN_ENABLE_TESTING)
#  catkin_add_gtest(${PROJECT_NAME}-test
#    test/serialization/buffer_writer_test.cpp
#    test/serialization/buffer_reader_test.cpp
#    test/serialization/stream_writer_test.cpp
#    test/serialization/stream_reader_test.cpp
#    test/serialization/serializable_buffer_test.cpp
#    test/serialization/serializable_primitive_test.cpp
#    test/test_socket_test.cpp
#    test/encap_header_test.cpp
#    test/encap_packet_test.cpp
#    test/cpf_item_test.cpp
#    test/cpf_packet_test.cpp
#    test/sequenced_address_item_test.cpp
#    test/identity_item_data_test.cpp
#    test/path_test.cpp
#    test/rr_data_test.cpp
#    test/message_router_request_test.cpp
#    test/message_router_response_test.cpp
#    test/rr_data_request_test.cpp
#    test/rr_data_response_test.cpp
#    test/forward_open_request_test.cpp
#    test/forward_open_success_test.cpp
#    test/forward_close_request_test.cpp
#    test/forward_close_success_test.cpp
#    test/session_test.cpp
#    test/test_main.cpp
#  )
#  target_link_libraries(${PROJECT_NAME}-test ${Boost_LIBRARIES} ${PROJECT_NAME})
#endif()

