# to build a app(client)

# this makes that the output, app, executable is made in `apps` folder in the
# build output directory

add_executable(app app.cxx)

target_compile_features(app PRIVATE cxx_std_11)

target_link_libraries(app PRIVATE student)
