cmake_minimum_required( VERSION 3.10.3 )
project(user VERSION 1.0 )

# JSONUtils::JSONUtils
find_package(JSONUtils 1.0 REQUIRED)

if(NOT JSONUtils_FOUND)
  message(FATAL_ERROR "JSONUtils's not found")
endif()

# Now simply link against gtest as needed. Eg
add_executable(user main.cpp)
target_compile_features(user PRIVATE cxx_auto_type)
target_link_libraries(user JSONUtils::JSONUtils)
