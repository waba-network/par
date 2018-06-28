find_package(LLVM 4.0 REQUIRED CONFIG)

set( CMAKE_CXX_STANDARD 14 )

file(GLOB UNIT_TESTS "*.cpp")

include_directories(${EOS_HOME}/libraries/chainbase/include
        ${EOS_HOME}/libraries/chain/include
        ${EOS_HOME}/build/libraries/chain/include
        ${EOS_HOME}/libraries/testing/include
        ${EOS_HOME}/libraries/fc/include
        ${EOS_HOME}/libraries/wasm-jit/include
        ${EOS_HOME}/libraries/softfloat/source/include)

link_directories(${LLVM_LIBRARY_DIR}
        ${EOS_HOME}/build/libraries/abi_generator
        ${EOS_HOME}/build/libraries/chainbase
        ${EOS_HOME}/build/libraries/chain
        ${EOS_HOME}/build/libraries/builtins
        ${EOS_HOME}/build/libraries/testing
        ${EOS_HOME}/build/libraries/fc
        ${EOS_HOME}/build/libraries/utilities
        ${EOS_HOME}/build/libraries/builtins
        ${EOS_HOME}/build/libraries/wasm-jit/source/Runtime
        ${EOS_HOME}/build/libraries/wasm-jit/source/Platform
        # ${EOS_HOME}/build/libraries/wasm-jit/source/WASM
        ${EOS_HOME}/build/libraries/wasm-jit/source/WAST
        ${EOS_HOME}/build/libraries/wasm-jit/source/IR
        ${EOS_HOME}/build/libraries/wasm-jit/source/Logging
        ${EOS_HOME}/build/libraries/wasm-jit/source/Platform
        ${EOS_HOME}/build/libraries/softfloat
        ${EOS_HOME}/build/externals/binaryen/lib
        ${OPENSSL_ROOT_DIR}/lib
        ${WASM_ROOT}/lib)

add_executable( unit_test ${UNIT_TESTS} )

target_link_libraries( unit_test abi_generator eosio_chain eosio_testing builtins crypto boost_system boost_iostreams boost_filesystem boost_date_time boost_chrono softfloat secp256k1 ${PLATFORM_SPECIFIC_LIBS})

# from abi_generator
target_link_libraries(unit_test eosio_chain
        clangRewrite
        clangTooling
        clangToolingCore
        clangFrontend
        clangDriver
        clangSerialization
        clangParse
        clangSema
        clangAnalysis
        clangAST
        clangBasic
        clangEdit
        clangLex
        )

target_link_libraries(unit_test
        LLVMX86AsmParser # MC, MCParser, Support, X86CodeGen, X86Desc, X86Info
        LLVMX86CodeGen # Analysis, AsmPrinter, CodeGen, Core, MC, Support, Target,
        # X86AsmPrinter, X86Desc, X86Info, X86Utils
        LLVMX86Desc # MC, MCDisassembler, Object, Support, X86AsmPrinter, X86Info
        LLVMX86AsmPrinter # MC, Support, X86Utils
        LLVMX86Info # Support
        LLVMX86Utils # Core, Support
        LLVMCodeGen # Analysis, Core, MC, Scalar, Support, Target, TransformUtils
        LLVMipo
        LLVMScalarOpts
        LLVMInstCombine
        LLVMTransformUtils
        LLVMTarget # Analysis, MC, Core, Support
        LLVMAnalysis # Core, Support
        LLVMOption # Support
        LLVMMCDisassembler # MC, Support
        LLVMMCParser # MC, Support
        LLVMMC # Object, Support
        LLVMProfileData # Core, Support, Object
        LLVMObject # BitReader, Core, Support
        LLVMBitReader # Core, Support
        LLVMCore # BinaryFormat, Support
        #LLVMBinaryFormat # Support
        LLVMSupport # Demangle
        LLVMDemangle
        )

#from chain
# had to set full path to wasm-jit WASM because it clashes naming with binaryen wasm (and OSX is case insensitive on the file system)
target_link_libraries( unit_test eos_utilities fc chainbase Logging IR WAST ${EOS_HOME}/build/libraries/wasm-jit/source/WASM/libWASM.a Runtime
        wasm asmjs passes cfg ast emscripten-optimizer support softfloat builtins)

#from Runtime
llvm_map_components_to_libnames(LLVM_LIBS support core passes mcjit native DebugInfoDWARF)
target_link_libraries(unit_test Platform ${LLVM_LIBS})

target_include_directories( unit_test PUBLIC
                            ${CMAKE_SOURCE_DIR}/libraries/testing/include
                            ${CMAKE_SOURCE_DIR}/contracts
                            ${CMAKE_BINARY_DIR}/contracts
                            ${CMAKE_CURRENT_BINARY_DIR}/include )
add_dependencies(unit_test waba.token.backend)

#Manually run unit_test for all supported runtimes
#To run unit_test with all log from blockchain displayed, put --verbose after --, i.e. unit_test -- --verbose
add_test(NAME unit_test_binaryen COMMAND unit_test
 -t \!wasm_tests/weighted_cpu_limit_tests
 --report_level=detailed --color_output -- --binaryen)
add_test(NAME unit_test_wavm COMMAND unit_test
 -t \!wasm_tests/weighted_cpu_limit_tests
 --report_level=detailed --color_output --catch_system_errors=no -- --wavm)