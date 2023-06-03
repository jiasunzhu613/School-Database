cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(School-Database VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

set(PRJ_HEADERS Person.hpp Student.hpp Teacher.hpp Course.hpp JSONBackend.hpp SchoolDB.hpp)
set(PRJ_SOURCES Person.cpp Student.cpp Teacher.cpp Course.cpp JSONBackend.cpp SchoolDB.cpp)
set(PRJ_MAIN database.cpp)
#set(PRJ_MAIN main.cpp)
add_executable(${PROJECT_NAME} ${PRJ_HEADERS} ${PRJ_SOURCES} ${PRJ_MAIN})

#add_executable(${PROJECT_NAME} ${PRJ_MAIN})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED) # v3.10
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

#find_package(unofficial-sqlite3 CONFIG REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::sqlite3::sqlite3)