cmake_minimum_required(VERSION 3.28)
project(SFMLRPG LANGUAGES CXX C)

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

FetchContent_Declare(libpq
    GIT_REPOSITORY https://gitlab.com/sabelka/libpq-standalone
    GIT_TAG REL_12_16
    SOURCE_SUBDIR libpq)
FetchContent_MakeAvailable(libpq)


ADD_LIBRARY(SharedDir STATIC 
    src/sharedDir/PacketTypes.h
    src/sharedDir/PacketTypes.cpp
    src/sharedDir/NetworkDefinitions.h
    src/sharedDir/C_Base.h
    src/sharedDir/C_Client.h
    src/sharedDir/C_Collidable.h
    src/sharedDir/C_Controller.h
    src/sharedDir/C_Health.h
    src/sharedDir/C_Movable.h
    src/sharedDir/C_Name.h
    src/sharedDir/C_Position.h
    src/sharedDir/C_State.h
    src/sharedDir/C_TimedComponentBase.h
    src/sharedDir/Directions.h
    src/sharedDir/ECS_Types.h
    src/sharedDir/ResourceManager.h
    src/sharedDir/Utilities.h
    src/sharedDir/Message.h
    src/sharedDir/Observer.h
    src/sharedDir/S_Base.h
    src/sharedDir/Bitmask.h
    src/sharedDir/Entity_Manager.h
    src/sharedDir/Entity_Manager.cpp
    src/sharedDir/System_Manager.h
    src/sharedDir/System_Manager.cpp
    src/sharedDir/Event_Queue.h
    src/sharedDir/Communicator.h
    src/sharedDir/EntityMessages.h
    src/sharedDir/EntityEvents.h
    src/sharedDir/MessageHandler.h
    src/sharedDir/S_Base.h
    src/sharedDir/S_Base.cpp
    src/sharedDir/S_Control.h
    src/sharedDir/S_Control.cpp
    src/sharedDir/S_State.h
    src/sharedDir/S_State.cpp
    src/sharedDir/EntitySnapshot.h
    src/sharedDir/EntitySnapshot.cpp
)
ADD_LIBRARY(ClientLib STATIC 
    src/clientLib/Client.h
    src/clientLib/Client.cpp
    src/clientLib/C_Drawable.h
    src/clientLib/TextureManager.h
    src/clientLib/Anim_Base.h
    src/clientLib/Anim_Base.cpp
    src/clientLib/Anim_Directional.h
    src/clientLib/Anim_Directional.cpp
    src/clientLib/C_SpriteSheet.h
    src/clientLib/SpriteSheet.h
    src/clientLib/SpriteSheet.cpp
    src/clientLib/SoundProps.h
    src/clientLib/AudioManager.h
    src/clientLib/SoundManager.h
    src/clientLib/SoundManager.cpp
    src/clientLib/C_SoundEmitter.h
    src/clientLib/C_SoundListener.h
    src/clientLib/C_UI_Element.h
    src/clientLib/StateManager.h
    src/clientLib/EventManager.h
    src/clientLib/EventManager.cpp
    src/clientLib/GUI_Event.h
    src/clientLib/Map.h
    src/clientLib/Map.cpp
    src/clientLib/Client_Entity_Manager.h
    src/clientLib/Client_Entity_Manager.cpp
    src/clientLib/SharedContext.h
    src/clientLib/Window.h
    src/clientLib/Window.cpp
    src/clientLib/BaseState.h
    src/clientLib/StateManager.cpp
    src/clientLib/State_Intro.h
    src/clientLib/State_Intro.cpp
    src/clientLib/NetSettings.h
    src/clientLib/S_Network.h
    src/clientLib/S_Network.cpp
    src/clientLib/S_Renderer.h
    src/clientLib/S_Renderer.cpp
    src/clientLib/State_Game.h
    src/clientLib/State_Game.cpp
    src/clientLib/S_Movement.h
    src/clientLib/S_Movement.cpp
    src/clientLib/State_MainMenu.h
    src/clientLib/State_MainMenu.cpp
    src/clientLib/State_Paused.h
    src/clientLib/State_Paused.cpp
    src/clientLib/State_GameOver.h
    src/clientLib/State_GameOver.cpp
    src/clientLib/Client_System_Manager.h
    src/clientLib/Client_System_Manager.cpp
    src/clientLib/S_CharacterUI.h
    src/clientLib/S_CharacterUI.cpp
    src/clientLib/S_Collision.h
    src/clientLib/S_Collision.cpp
    src/clientLib/S_SheetAnimation.h
    src/clientLib/S_SheetAnimation.cpp
    src/clientLib/S_Sound.h
    src/clientLib/S_Sound.cpp
    src/clientLib/GUI_Element.h
    src/clientLib/GUI_Element.cpp
    src/clientLib/GUI_Interface.h
    src/clientLib/GUI_Interface.cpp
    src/clientLib/GUI_Manager.h
    src/clientLib/GUI_Manager.cpp
    src/clientLib/GUI_Style.h
    src/clientLib/GUI_Label.h
    src/clientLib/GUI_Label.cpp
    src/clientLib/GUI_Scrollbar.h
    src/clientLib/GUI_Scrollbar.cpp
    src/clientLib/GUI_Textfield.h
    src/clientLib/GUI_Textfield.cpp
    src/clientLib/Game.h
    src/clientLib/Game.cpp
)
ADD_LIBRARY(ServerLib STATIC 
    src/serverLib/Server.h
    src/serverLib/Server.cpp
    src/serverLib/C_Attacker.h
    src/serverLib/Map.h
    src/serverLib/Map.cpp
    src/serverLib/S_Movement.h
    src/serverLib/S_Movement.cpp
    src/serverLib/Map.h
    src/serverLib/Map.cpp
    src/serverLib/Server_Entity_Manager.h
    src/serverLib/Server_Entity_Manager.cpp
    src/serverLib/NetSettings.h
    src/serverLib/S_Collision.h
    src/serverLib/S_Collision.cpp
    src/serverLib/S_Combat.h
    src/serverLib/S_Combat.cpp
    src/serverLib/S_Network.h
    src/serverLib/S_Network.cpp
    src/serverLib/S_Timers.h
    src/serverLib/S_TImers.cpp
    src/serverLib/Server_System_Manager.h
    src/serverLib/Server_System_Manager.cpp
    src/serverLib/World.h
    src/serverLib/World.cpp
)

ADD_EXECUTABLE(client src/client/Main.cpp)
target_link_libraries(client PostgreSQL::PostgreSQL)
ADD_EXECUTABLE(server src/server/Server_Main.cpp)
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
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(ServerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(ClientLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/serverLib)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/clientLib)
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
