
# Add source to this project's executable.
add_executable (expr-evaluation-conflict "expr-evaluation-conflict.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET expr-evaluation-conflict PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
