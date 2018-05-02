cmake_minimum_required( VERSION 3.5 )

project( par )

SET(CMAKE_MODULE_PATH "${EOS_HOME}/CMakeModules")

FIND_PACKAGE(Boost 1.66 REQUIRED COMPONENTS
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

include_directories(${EOS_HOME}/contracts ${EOS_HOME}/contracts/libc++/upstream/include ${EOS_HOME}/contracts/musl/upstream/include ${EOS_HOME}/externals/magic_get/include ${Boost_INCLUDE_DIR})

include(wasm)

# hack for wasm module to find eosio-s2wasm command

add_executable(eosio-s2wasm IMPORTED)
set_property(TARGET eosio-s2wasm PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/externals/binaryen/bin/eosio-s2wasm")
add_library(eosiolib IMPORTED STATIC GLOBAL)
set_property(TARGET eosiolib PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/contracts/eosiolib/eosiolib.bc")
add_library(libc++ IMPORTED STATIC GLOBAL)
set_property(TARGET libc++ PROPERTY IMPORTED_LOCATION "${EOS_HOME}/build/contracts/libc++/libc++.bc")

add_subdirectory( contracts/waba.token.backend )
