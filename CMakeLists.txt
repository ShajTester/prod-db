cmake_minimum_required(VERSION 3.2)

project(prod-db VERSION 0.0.2)

#find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT kolobrod2002@yandex.ru)

include (CPack)

configure_file("${PROJECT_SOURCE_DIR}/src/version.h.in" "${PROJECT_BINARY_DIR}/version.h")
configure_file("${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}.conf" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.conf" COPYONLY)

enable_testing()

add_subdirectory(test)

install (FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.conf DESTINATION /etc/${PROJECT_NAME} COMPONENT config)

# add_test(test_version_valid test_${PROJECT_NAME})
