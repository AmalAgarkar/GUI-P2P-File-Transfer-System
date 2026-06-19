# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/p2p_client_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/p2p_client_autogen.dir/ParseCache.txt"
  "CMakeFiles/p2p_tracker_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/p2p_tracker_autogen.dir/ParseCache.txt"
  "p2p_client_autogen"
  "p2p_tracker_autogen"
  )
endif()
