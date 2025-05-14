# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу, в котором находится Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (14_co_main "14_co_main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 14_co_main PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(14_co_main PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(14_co_main PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(14_co_main PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(14_co_main PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(14_co_main PRIVATE Boost::cobalt)

