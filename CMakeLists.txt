cmake_minimum_required(VERSION 3.10)

project(VideoPlayer VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets UiTools)

#指定头文件包含目录
include_directories("F:/ffmpeg-build/msvc/include")

#指定库目录
link_directories("F:/ffmpeg-build/msvc/lib/x64")

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SOURCE_H ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCE_UI ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

set(SOURCE_FILES
	${SOURCE_CPP}
	${SOURCE_H}#CMake 会自动处理包含在源文件中的头文件 , 因此这行可以不加。
	${SOURCE_UI}
)

#message(STATUS "######## ${CMAKE_CURRENT_SOURCE_DIR}")
#message(STATUS "######## ${SOURCE_CPP}")
#message(STATUS "######## ${SOURCE_H}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES} "Res.qrc")

target_link_libraries(${PROJECT_NAME} Qt6::Widgets 
libavformatd libavcodecd libavutild libavfilterd 
libavdeviced libswscaled libswresampled Qt6::UiTools)