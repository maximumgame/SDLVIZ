project(imgui)

set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
message(STATUS "IMGUI_PATH: ${IMGUI_PATH}")

set(imgui_FOUND TRUE)

set(IMGUI_SOURCES
  ${IMGUI_PATH}/imgui.cpp
  ${IMGUI_PATH}/imgui_demo.cpp
  ${IMGUI_PATH}/imgui_draw.cpp
  ${IMGUI_PATH}/imgui_tables.cpp
  ${IMGUI_PATH}/imgui_widgets.cpp
  ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE -DGL_GLES_PROTOTYPES)
target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_PATH}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_PATH}/backends")

find_package(SDL2 REQUIRED sdl2)
target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

