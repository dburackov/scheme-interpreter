cmake_minimum_required(VERSION 3.24)
project(scheme)

set(CMAKE_CXX_STANDARD 17)

add_library(scheme-lib
        scheme/Tokenizer.cpp
        scheme/Object.cpp
        scheme/Parser.cpp
        scheme/Scheme.cpp
        scheme/Scheme.h)

add_executable(scheme
        main.cpp)

target_link_libraries(scheme
        scheme-lib)



add_executable(test_tokenizer
        test/TokenizerTest.cpp)

target_link_libraries(test_tokenizer
        scheme-lib)



add_executable(test_parser
        test/ParserTest.cpp)

target_link_libraries(test_parser
        scheme-lib)