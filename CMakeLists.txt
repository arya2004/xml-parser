cmake_minimum_required(VERSION 3.26)
project(XMLParser)

set(CMAKE_C_STANDARD 17)

include_directories(include)

add_executable(XMLParser src/main.c src/xml_parser.c)
