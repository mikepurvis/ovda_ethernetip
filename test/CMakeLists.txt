find_package(GTest REQUIRED)

# odva_ethernetip Test
add_executable(${PROJECT_NAME}_odva_ethernetip_unit 
  serialization/buffer_writer_test.cpp)
target_link_libraries(${PROJECT_NAME}_odva_ethernetip_unit ${GTEST_BOTH_LIBRARIES} ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}_odva_ethernetip_unit PRIVATE -Wsuggest-override -Wconversion -Wsign-conversion)
if(CXX_FEATURE_FOUND EQUAL "-1")
    target_compile_options(${PROJECT_NAME}_odva_ethernetip_unit PUBLIC -std=c++11)
else()
    target_compile_features(${PROJECT_NAME}_odva_ethernetip_unit PRIVATE cxx_std_11)
endif()
target_include_directories(${PROJECT_NAME}_odva_ethernetip_unit PRIVATE ${GTEST_INCLUDE_DIRS})
if(${CMAKE_VERSION} VERSION_LESS "3.10.0")
    gtest_add_tests(${PROJECT_NAME}_odva_ethernetip_unit "" AUTO)
else()
    gtest_discover_tests(${PROJECT_NAME}_odva_ethernetip_unit)
endif()

add_dependencies(run_tests ${PROJECT_NAME}_odva_ethernetip_unit)

#    test/
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
