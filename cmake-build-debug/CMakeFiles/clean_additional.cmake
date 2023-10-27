# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/QuietFriend_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/QuietFriend_autogen.dir/ParseCache.txt"
  "Framework/CMakeFiles/Framework_autogen.dir/AutogenUsed.txt"
  "Framework/CMakeFiles/Framework_autogen.dir/ParseCache.txt"
  "Framework/Framework_autogen"
  "QuietFriend_autogen"
  "third/glew/build/cmake/CMakeFiles/glew_autogen.dir/AutogenUsed.txt"
  "third/glew/build/cmake/CMakeFiles/glew_autogen.dir/ParseCache.txt"
  "third/glew/build/cmake/CMakeFiles/glew_s_autogen.dir/AutogenUsed.txt"
  "third/glew/build/cmake/CMakeFiles/glew_s_autogen.dir/ParseCache.txt"
  "third/glew/build/cmake/glew_autogen"
  "third/glew/build/cmake/glew_s_autogen"
  "third/glfw/src/CMakeFiles/glfw_autogen.dir/AutogenUsed.txt"
  "third/glfw/src/CMakeFiles/glfw_autogen.dir/ParseCache.txt"
  "third/glfw/src/glfw_autogen"
  )
endif()
