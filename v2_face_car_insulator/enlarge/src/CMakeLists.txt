file (GLOB srcs *.cpp)

add_library (Raspberry-Qt SHARED ${srcs})
target_link_libraries (Raspberry-Qt Qt5::Widgets)

add_subdirectory (tools/)
