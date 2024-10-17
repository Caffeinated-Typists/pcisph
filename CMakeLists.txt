cmake_minimum_required(VERSION 3.11)

project(pcisph)
set(TARGET ${CMAKE_PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

file(GLOB IMGUI_SOURCES "libs/imgui/*.cpp" )
list(REMOVE_ITEM IMGUI_SOURCES "${PROJECT_SOURCE_DIR}/libs/imgui/imgui_impl_glut.cpp")

file (GLOB SRC "src/*.cpp")
file(GLOB GLAD_SOURCES "libs/glad/*.c")
file(GLOB INCLUDES "${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    ${SRC}
    ${IMGUI_SOURCES}
	${GLAD_SOURCES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wcast-align -Wlogical-op -g")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

add_executable(${TARGET} ${SOURCES})

set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_NAME "${TARGET}.out")

target_include_directories(${TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libs/imgui
	${GLFW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS/../include}
	${INCLUDES}
	)
target_link_libraries(${TARGET} ${OPENGL_LIBRARIES} dl glfw GLEW::GLEW)

