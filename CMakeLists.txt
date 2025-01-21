cmake_minimum_required(VERSION 3.28)
project(SFMLRPG LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

ADD_LIBRARY(SharedDir STATIC 
    src/shared/PacketTypes.h
    src/shared/PacketTypes.cpp
    src/shared/NetworkDefinitions.h
)
ADD_LIBRARY(ClientLib STATIC 
    src/client/Client.h
    src/client/Client.cpp
)
ADD_LIBRARY(ServerLib STATIC 
    src/server/Server.h
    src/server/Server.cpp
)
ADD_EXECUTABLE(client src/clientExec/main.cpp)
ADD_EXECUTABLE(server src/serverExec/main.cpp)
target_link_libraries(SharedDir sfml-graphics)
target_link_libraries(SharedDir sfml-audio)
target_link_libraries(SharedDir sfml-network)
target_link_libraries(SharedDir shlwapi)
target_link_libraries(ClientLib shlwapi)
target_link_libraries(ServerLib shlwapi)
target_link_libraries(ServerLib SharedDir)
target_link_libraries(ClientLib SharedDir)
target_link_libraries(client SharedDir)
target_link_libraries(server SharedDir)
target_link_libraries(client sfml-graphics)
target_link_libraries(client sfml-audio)
target_link_libraries(client sfml-network)
target_link_libraries(ClientLib sfml-graphics)
target_link_libraries(ClientLib sfml-audio)
target_link_libraries(ClientLib sfml-network)
target_link_libraries(ServerLib sfml-graphics)
target_link_libraries(ServerLib sfml-audio)
target_link_libraries(ServerLib sfml-network)
target_link_libraries(server sfml-system)
target_link_libraries(server sfml-audio)
target_link_libraries(server sfml-network)
target_link_libraries(server ServerLib)
target_link_libraries(client ClientLib)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)
target_include_directories(ServerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)
target_include_directories(ClientLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/server)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/client)
target_compile_features(client PRIVATE cxx_std_17)
target_compile_features(server PRIVATE cxx_std_17)
if(WIN32)
    add_custom_command(
        TARGET client
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:client>
        VERBATIM)
    add_custom_command(
        TARGET server
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:server>
        VERBATIM)
endif()