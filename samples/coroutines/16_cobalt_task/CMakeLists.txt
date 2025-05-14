# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (16_cobalt_task "16_cobalt_task.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 16_cobalt_task PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(16_cobalt_task PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(16_cobalt_task PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(16_cobalt_task PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(16_cobalt_task PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(16_cobalt_task PRIVATE Boost::cobalt)

