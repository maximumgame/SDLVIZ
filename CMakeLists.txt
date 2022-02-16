cmake_minimum_required(VERSION 3.11)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
  set(ISEM TRUE)
else()
  message(" * C++ cimpiler: ${CMAKE_CXX_COMPILER_ID}")
  set(ISEM FALSE)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set (X86 TRUE)
    message(" * x86 architecture")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (X86 FALSE)
    message(" * x64 architecture")
endif()

#other compilers try to use vcpkg
if(NOT ISEM)
  #try to use vcpkg by default
  message(" * Trying to use vcpkg")
  if(DEFINED ENV{VCPKG_ROOT})
    set(vcpkg_dir $ENV{VCPKG_ROOT})
    message(" * vcpkg_dir: ${vcpkg_dir}")
    set(CMAKE_TOOLCHAIN_FILE ${vcpkg_dir}/scripts/buildsystems/vcpkg.cmake)
    #select vcpkg triplet
    if(X86)
      set(VCPKG_TARGET_TRIPLET x86-windows)
    else()
      set(VCPKG_TARGET_TRIPLET x64-windows)
    endif()
    message(" * VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
  endif()
endif()

#define project after vcpkg
project(SDLVIZ LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#list of directors to search for compilation files
set(CDIRS 
    src
    src/gfx
    src/gfx/shader
    src/gfx/shader/input
    src/ui
)
#iterate through the list of directories
foreach(dir ${CDIRS})
    file(GLOB FILES ${dir}/*.c)
    list(APPEND SRCS ${FILES})

    file(GLOB FILES ${dir}/*.cpp)
    list(APPEND SRCS ${FILES})

    file(GLOB FILES ${dir}/*.h)
    list(APPEND HEADERS ${FILES})

    file(GLOB FILES ${dir}/*.hpp)
    list(APPEND HEADERS ${FILES})
endforeach()


include_directories(src) #add root source directory
include_directories(lib/spdlog/include) #add spdlog


add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

#find package sdl2
find_package(SDL2 REQUIRED sdl2)
target_include_directories(${PROJECT_NAME} PUBLIC ${sdl2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)


#find_package(GLEW REQUIRED)
#target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::GLEW)


#find_package(glad REQUIRED)
#target_include_directories(${PROJECT_NAME} PUBLIC ${glad_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PUBLIC glad::glad)

find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)

find_package(imgui CONFIG)
if(NOT ${imgui_FOUND})
  message(" * imgui not found, adding subdirectory lib/imgui")
  add_subdirectory(lib/imgui)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)


add_subdirectory(lib/gladloader)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLADLOADER_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC gladloader)