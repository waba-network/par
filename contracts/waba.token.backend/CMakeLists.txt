file(GLOB ABI_FILES "*.abi")
configure_file("${ABI_FILES}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

get_property(INCLUDE_DIRECTORIES DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

add_wast_executable(
        SOURCE_FILES waba.token.backend.cpp mutual_credit_system_contract.cpp
        TARGET waba.token.backend
        INCLUDE_FOLDERS "${INCLUDE_DIRECTORIES}"
        LIBRARIES libc++ eosiolib
        DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
        )
