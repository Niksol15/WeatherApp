cmake_minimum_required(VERSION 3.16)
project(WeatherApp)

set(CMAKE_CXX_STANDARD 14)

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system thread program_options REQUIRED)

if(Boost_FOUND)
    set(S src/sources/)
    set(H src/headers/)

    set(SOURCES ${S}application.cpp ${S}client.cpp ${S}network.cpp)
    set(HEADERS ${H}application.hpp ${H}client.hpp ${H}network.hpp)

    add_executable(WeatherApp ${S}main.cpp ${HEADERS} ${SOURCES})

    target_include_directories(WeatherApp PUBLIC ${H})
    target_include_directories(WeatherApp PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(WeatherApp ${Boost_LIBRARIES})
endif()







