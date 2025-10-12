# CMakeList.txt : CMake project for dcmtk-data, include source and define
# project specific logic here.
#

find_package(DCMTK REQUIRED)

# Add source to this project's executable.
add_executable (dcmtk-data "dcmtk-data.cpp" "dcmtk-data.h") 
add_executable (dcmtk-scu "dcmtk-scu.cpp")
add_executable (dcmtk-scp "dcmtk-scp.cpp")


target_link_libraries(dcmtk-data PRIVATE DCMTK::DCMTK)
target_link_libraries(dcmtk-scu PRIVATE DCMTK::DCMTK)
target_link_libraries(dcmtk-scp PRIVATE DCMTK::DCMTK)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dcmtk-data PROPERTY CXX_STANDARD 20)
  set_property(TARGET dcmtk-scu PROPERTY CXX_STANDARD 20)
  set_property(TARGET dcmtk-scp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

