cmake_minimum_required(VERSION 3.2)

project(matrix VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

add_executable(matrix main.cpp)

set_target_properties(
	matrix PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_compile_definitions(
	test_version
	PRIVATE "BOOST_TEST_DYN_LINK=1"
)

install(TARGETS matrix RUNTIME DESTINATION bin)

set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rejdok")
set(CPAK_PACKAGE_CONTACT "lvbnu@yandex.ru")
include(CPack)
