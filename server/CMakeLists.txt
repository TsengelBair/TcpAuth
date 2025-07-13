cmake_minimum_required(VERSION 3.14)

project(server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql Network)
find_package(Protobuf)

# директория с прото файлами на директорию ниже текущего проекта
set (GENERATED_PB_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

file(GLOB_RECURSE PROTO_FILES ${GENERATED_PB_FILES_DIR}/*.pb.cc ${GENERATED_PB_FILES_DIR}/*.pb.h)

add_executable(server
  main.cpp
  server.h
  server.cpp
  ${PROTO_FILES}
  IDbHandler.h
  structs.h
  postgresdb.h postgresdb.cpp
)
target_link_libraries(server Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
                                       Qt${QT_VERSION_MAJOR}::Sql ${PROTOBUF_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
