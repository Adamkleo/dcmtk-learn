# CMakeList.txt : CMake project for dcmtk-win-learn, include source and define
# project specific logic here.
#

find_package(DCMTK REQUIRED)

# Add source to this project's executable.
add_executable (dcmtk-win-learn "dcmtk-win-learn.cpp" "dcmtk-win-learn.h")

target_link_libraries(dcmtk-win-learn PRIVATE DCMTK::DCMTK)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dcmtk-win-learn PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

