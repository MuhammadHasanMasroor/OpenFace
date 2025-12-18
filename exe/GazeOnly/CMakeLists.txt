# Local libraries
include_directories(${LandmarkDetector_SOURCE_DIR}/include)

add_executable(GazeOnly GazeOnly.cpp)
target_link_libraries(GazeOnly LandmarkDetector)
target_link_libraries(GazeOnly GazeAnalyser)
target_link_libraries(GazeOnly Utilities)

install (TARGETS GazeOnly DESTINATION bin)

