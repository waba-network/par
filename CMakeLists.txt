cmake_minimum_required( VERSION 3.5 )

project( par )

enable_testing()

SET(CMAKE_MODULE_PATH "${EOS_HOME}/CMakeModules")

FIND_PACKAGE(Boost 1.67 REQUIRED COMPONENTS
        thread
        date_time
        filesystem
        system
        program_options
        signals
        serialization
        chrono
        unit_test_framework
        context
        locale
        iostreams)

include_directories(${Boost_INCLUDE_DIR})

include(wasm)

# add defaults for openssl
if ("${OPENSSL_ROOT_DIR}" STREQUAL "")
    if (NOT "$ENV{OPENSSL_ROOT_DIR}" STREQUAL "")
        set(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
        set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
    elseif (APPLE)
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
        set(OPENSSL_INCLUDE_DIR "/usr/local/opt/openssl/include")
    elseif(UNIX AND NOT APPLE)
        set(OPENSSL_ROOT_DIR "/usr/include/openssl")
        set(OPENSSL_INCLUDE_DIR "/usr/include/openssl/include")
    else()
        message(FATAL_ERROR "openssl not found and don't know where to look, please specify OPENSSL_ROOT_DIR")
    endif()
endif()

# hack for wasm module to find eosio-s2wasm command

add_executable(eosio-s2wasm IMPORTED)
set_property(TARGET eosio-s2wasm PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/externals/binaryen/bin/eosio-s2wasm")
add_executable(eosio-wast2wasm IMPORTED)
set_property(TARGET eosio-wast2wasm PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/libraries/wasm-jit/Source/Programs/eosio-wast2wasm")
add_library(eosiolib IMPORTED STATIC GLOBAL)
set_property(TARGET eosiolib PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/contracts/eosiolib/eosiolib.bc")
add_library(libc++ IMPORTED STATIC GLOBAL)
set_property(TARGET libc++ PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/contracts/libc++/libc++.bc")

add_subdirectory( contracts )
add_subdirectory( unittests )
