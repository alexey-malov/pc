# Нужно установить OpenSSL
find_package(OpenSSL REQUIRED)

# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (17_simple_http_client "17_simple_http_client.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 17_simple_http_client PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(17_simple_http_client PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(17_simple_http_client PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(17_simple_http_client PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(17_simple_http_client PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(17_simple_http_client PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(17_simple_http_client PRIVATE Boost::cobalt OpenSSL::SSL)

