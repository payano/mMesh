file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(meshTests ${SRCS})

TARGET_LINK_LIBRARIES(meshTests
    Mesh
    Node
    Network
    Islands
    NetAlgorithm
    libgtest
    libgmock
)


include_directories(
"../../src"
"../../src/network"
"../../src/netAlgorithm"
"../../src/mesh"
"../../src/node"
"../../src/islands"
)

add_test(NAME meshTests
         COMMAND meshTests)
