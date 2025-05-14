# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (15_promise "15_promise.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 15_promise PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(15_promise PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(15_promise PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(15_promise PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(15_promise PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(15_promise PRIVATE Boost::cobalt)

