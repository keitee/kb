include(ExternalProject)

set( JSONCPP_FOUND TRUE )
set( JSONCPP_VERSION 1.9.1 )

# Disable implicit fallthrough warning in jsoncpp for gcc >= 7 until the upstream handles it properly
if( ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
    set( JSONCCP_EXTRA_FLAGS -Wno-implicit-fallthrough )
else()
    set( JSONCCP_EXTRA_FLAGS "" )
endif()

if( CMAKE_CROSSCOMPILING )
    set( JSONCCP_EXTRA_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${PROJECT_SOURCE_DIR}/${CMAKE_TOOLCHAIN_FILE}" )
endif()

ExternalProject_Add(
        jsoncpp-project

        PREFIX deps/jsoncpp-${JSONCPP_VERSION}

        # DOWNLOAD_COMMAND ""
        # SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/jsoncpp-${JSONCPP_VERSION}"

        DOWNLOAD_NAME   jsoncpp-${JSONCPP_VERSION}.tar.gz
        URL             https://github.com/open-source-parsers/jsoncpp/archive/1.9.1.tar.gz
        URL_HASH        SHA256=c7b40f5605dd972108f503f031b20186f5e5bca2b65cd4b8bd6c3e4ba8126697

        CMAKE_COMMAND ${JSONCPP_CMAKE_COMMAND}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        # Build static lib but suitable to be included in a shared lib.
        -DCMAKE_POSITION_INDEPENDENT_CODE=On
        -DJSONCPP_WITH_TESTS=Off
        -DJSONCPP_WITH_PKGCONFIG_SUPPORT=Off
        -DCMAKE_CXX_FLAGS=${JSONCCP_EXTRA_FLAGS}
        ${JSONCCP_EXTRA_CMAKE_ARGS}

        # Overwrite build and install commands to force Release build on MSVC.
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target install
        COMMAND ${CMAKE_COMMAND} -E create_symlink <INSTALL_DIR>/include/json <INSTALL_DIR>/include/jsoncpp
)

# Create jsoncpp imported library
ExternalProject_Get_Property( jsoncpp-project INSTALL_DIR )

add_library( JsonCpp::JsonCpp STATIC IMPORTED )

set( JSONCPP_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX} )
set( JSONCPP_INCLUDE_DIR ${INSTALL_DIR}/include )
set( JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR} )

file( MAKE_DIRECTORY ${JSONCPP_INCLUDE_DIR})  # Must exist.
set_property( TARGET JsonCpp::JsonCpp PROPERTY IMPORTED_LOCATION ${JSONCPP_LIBRARY} )
set_property( TARGET JsonCpp::JsonCpp PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${JSONCPP_INCLUDE_DIR} )

add_dependencies( JsonCpp::JsonCpp jsoncpp-project )

unset( INSTALL_DIR )
