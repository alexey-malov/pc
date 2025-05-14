# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (13_delay "13_delay.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 13_delay PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(13_delay PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(13_delay PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(13_delay PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(13_delay PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(13_delay PRIVATE Boost::cobalt)

