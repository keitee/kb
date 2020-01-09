include(ExternalProject)

set( LIBASSERVICE_EXTRA_CMAKE_ARGS )
if( RDK_SDK )
    set( LIBASSERVICE_EXTRA_CMAKE_ARGS ${LIBASSERVICE_EXTRA_CMAKE_ARGS} "-DRDK_SDK=${RDK_SDK}" )
endif()
if( CMAKE_CROSSCOMPILING )
    set( LIBASSERVICE_EXTRA_CMAKE_ARGS ${LIBASSERVICE_EXTRA_CMAKE_ARGS} "-DCMAKE_TOOLCHAIN_FILE=${PROJECT_SOURCE_DIR}/${CMAKE_TOOLCHAIN_FILE}" )
endif()

ExternalProject_Add(
        libasservice-project

        PREFIX deps/libasservice-develop

        GIT_REPOSITORY "gitlab@git.bskyb.com:llama/ASServiceLib.git"
        GIT_TAG "origin/develop"
        GIT_SHALLOW 1

        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        ${LIBASSERVICE_EXTRA_CMAKE_ARGS}

        # BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release
        # INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target install

)


# Create ASServiceLib imported library
ExternalProject_Get_Property(libasservice-project INSTALL_DIR)

add_library( libasservice::libasservice STATIC IMPORTED )
set( LIBASSERVICE_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}asservice${CMAKE_STATIC_LIBRARY_SUFFIX} )
set( LIBASSERVICE_LIBRARIES ${LIBASSERVICE_LIBRARY} )
set( LIBASSERVICE_INCLUDE_DIR ${INSTALL_DIR}/include )
set( LIBASSERVICE_INCLUDE_DIRS ${LIBASSERVICE_INCLUDE_DIR} )

file( MAKE_DIRECTORY ${LIBASSERVICE_INCLUDE_DIR} )  # Must exist.
set_property( TARGET libasservice::libasservice PROPERTY IMPORTED_LOCATION ${LIBASSERVICE_LIBRARY})
set_property( TARGET libasservice::libasservice PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERVICE_INCLUDE_DIRS})
add_dependencies( libasservice::libasservice libasservice-project )
unset( INSTALL_DIR )
