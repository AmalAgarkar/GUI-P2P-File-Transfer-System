cmake_minimum_required(VERSION 3.16)
project(P2PFileTransfer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(OpenSSL REQUIRED)

set(SHARED_SRC
    shared/crypto.cpp
    shared/protocol.cpp
)

# ── Tracker ───────────────────────────────────────────────────────────────────
add_executable(p2p_tracker
    ${SHARED_SRC}
    tracker/main.cpp
    tracker/TrackerBackend.cpp
    tracker/TrackerWindow.cpp
)
target_include_directories(p2p_tracker PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/tracker
    ${CMAKE_BINARY_DIR}/p2p_tracker_autogen/include
)
target_link_libraries(p2p_tracker PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Network
    OpenSSL::SSL OpenSSL::Crypto)

# ── Client ────────────────────────────────────────────────────────────────────
add_executable(p2p_client
    ${SHARED_SRC}
    client/main.cpp
    client/LoginDialog.cpp
    client/TrackerSocket.cpp
    client/PeerServer.cpp
    client/PeerServer.h
    client/TransferManager.cpp
    client/MainWindow.cpp
)
target_include_directories(p2p_client PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_BINARY_DIR}/p2p_client_autogen/include
)
target_link_libraries(p2p_client PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Network
    OpenSSL::SSL OpenSSL::Crypto)
