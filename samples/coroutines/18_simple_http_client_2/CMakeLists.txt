# Нужно установить OpenSSL
find_package(OpenSSL REQUIRED)

# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (18_simple_http_client_2 "18_simple_http_client_2.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 18_simple_http_client_2 PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(18_simple_http_client_2 PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(18_simple_http_client_2 PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(18_simple_http_client_2 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(18_simple_http_client_2 PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(18_simple_http_client_2 PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(18_simple_http_client_2 PRIVATE Boost::cobalt OpenSSL::SSL)

