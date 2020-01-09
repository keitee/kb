include(ExternalProject)

set( INCBIN_FOUND TRUE )
set( INCBIN_VERSION master )


ExternalProject_Add(
        incbin-project

        PREFIX deps/incbin-${INCBIN_VERSION}

        # GIT_REPOSITORY "https://github.com/graphitemaster/incbin.git"
        # GIT_TAG "origin/master"
        # GIT_SHALLOW 1

        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/incbin/"
        DOWNLOAD_COMMAND ""

        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)

# Create jsoncpp imported library
ExternalProject_Get_Property( incbin-project SOURCE_DIR )

add_library( incbin INTERFACE IMPORTED )

set( INCBIN_INCLUDE_DIRS ${SOURCE_DIR} )
set_property( TARGET incbin PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${INCBIN_INCLUDE_DIRS} )

add_dependencies( incbin incbin-project )

unset( SOURCE_DIR )
