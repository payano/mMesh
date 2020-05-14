cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -pedantic -O2 -Werror -Wextra -fexceptions")

if(UNIX)
	add_definitions(-DUNIX) 
endif (UNIX)

if(APPLE)
	add_definitions(-DUNIX) 
	add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__")
endif (APPLE)

project(hello_library)
find_package (Threads REQUIRED)

add_subdirectory(test)


############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(Mesh STATIC 
    src/mesh/Mesh.cpp
    src/mesh/MeshNetworkHandler.cpp
    src/mesh/NetworkData.cpp
)

target_link_libraries (Mesh
${CMAKE_THREAD_LIBS_INIT}
)

add_library(Node STATIC 
    src/node/Node.cpp
)

target_link_libraries (Node
${CMAKE_THREAD_LIBS_INIT}
)

add_library(Network STATIC 
    src/network/DummyNWInterface.cpp
    src/network/Nrf24.cpp
    src/network/RF24/RF24.cpp
)

add_library(Islands STATIC 
    src/islands/Islands.cpp
)

add_library(NetAlgorithm STATIC 
    src/netAlgorithm/LazyAlgorithm.cpp
)

add_library(Spi STATIC
	src/spi/SPIStm32f103.cpp
)

add_library(Syscalls STATIC
	src/syscalls/LinuxSyscalls.cpp
	src/syscalls/STM32Syscalls.cpp
)

add_library(GPIO STATIC
	src/GPIO/GPIOStm32f103.cpp
)

add_library(Debug STATIC
	src/debug/LinuxDebug.cpp
	src/debug/STM32Debug.cpp
	src/debug/DebugSingleton.cpp
)

#target_include_directories(hello_library
#    PUBLIC 
#        ${PROJECT_SOURCE_DIR}/include
#	${PROJECT_SOURCE_DIR}/libraries/googletest-release-1.10.0/googletest/include
#)

include_directories(
"src"
"src/network"
"src/network/RF24"
"src/netAlgorithm"
"src/mesh"
"src/node"
"src/islands"
"src/spi"
"src/GPIO"
"src/syscalls"
"src/debug"
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(hello_binary 
#    src/mesh/MeshTest.cpp
#    src/main.cpp
    main.cpp
#    src/node/Node.cpp
#    src/mesh/Mesh.cpp
#    src/mesh/MeshNetworkHandler.cpp
#    src/mesh/NetworkData.cpp
#    src/netAlgorithm/LazyAlgorithm.cpp
#    src/network/DummyNWInterface.cpp
#    src/network/Nrf24.cpp
)


target_link_libraries (hello_binary 
${CMAKE_THREAD_LIBS_INIT}
#Mesh
)
# link the new hello_library target with the hello_binary target
#target_link_libraries( hello_binary
#    PRIVATE 
#        hello_library
#)
include_directories(cmake-build-debug/test/gtest/src/gtest/googletest/include)
