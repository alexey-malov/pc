# Переменная окружения BOOST_ROOT должна указывать на путь к каталогу с установленным Boost.
find_package(Boost REQUIRED COMPONENTS cobalt)
add_executable (12_boost_cobalt "12_boost_cobalt.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET 12_boost_cobalt PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(12_boost_cobalt PRIVATE /W4 /WX)
	# Явно включаем компиляцию под Windows 10
	target_compile_definitions(12_boost_cobalt PRIVATE _WIN32_WINNT=0x0A00)
else()
	target_compile_options(12_boost_cobalt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(12_boost_cobalt PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(12_boost_cobalt PRIVATE Boost::cobalt)

