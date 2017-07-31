cmake_minimum_required(VERSION 3.0)
project(xmrix)
option(WITH_AEON     "CryptoNight-Lite support" ON)
include (CheckIncludeFile)
set(HEADERS)
set(HEADERS_CRYPTO)
set(SOURCES
    src/Mem.cpp
    src/xmrix.cpp
)
set(SOURCES_CRYPTO
    src/crypto/c_keccak.c
    src/crypto/c_groestl.c
    src/crypto/c_blake256.c
    src/crypto/c_jh.c
    src/crypto/c_skein.c
    src/crypto/CryptoNight.cpp
)
set(SOURCES_OS
	src/Mem_unix.cpp
)
set(EXTRA_LIBS pthread)
add_definitions(/DUNICODE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -maes -Wall -Wno-strict-aliasing")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -funroll-loops -fvariable-expansion-in-unroller -ftree-loop-if-convert-stores -fmerge-all-constants -fbranch-target-load-optimize2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -maes -Wall -std=c++14 -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -s -funroll-loops -fvariable-expansion-in-unroller -ftree-loop-if-convert-stores -fmerge-all-constants -fbranch-target-load-optimize2")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
add_definitions(/D_GNU_SOURCE)
include_directories(src)
include_directories(src/3rdparty)
include_directories(src/3rdparty/jansson)
add_subdirectory(src/3rdparty/jansson)
add_executable(xmrix ${HEADERS} ${SOURCES} ${SOURCES_OS} ${SOURCES_CPUID} ${HEADERS_CRYPTO} ${SOURCES_CRYPTO})
target_link_libraries(xmrix jansson ${EXTRA_LIBS})
