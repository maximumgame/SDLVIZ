cmake_minimum_required(VERSION 3.11)



if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set (X86 TRUE)
    message(" * x86 architecture")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (X86 FALSE)
    message(" * x64 architecture")
endif()

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
target_include_directories(${PROJECT_NAME} PRIVATE ${sdl2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

find_package(imgui CONFIG)
if(NOT ${imgui_FOUND})
  message(" * imgui not found, adding subdirectory lib/imgui")
  add_subdirectory(lib/imgui)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

if(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
endif()

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
  set(ISEM TRUE)
else()
  message(" * C++ cimpiler: ${CMAKE_CXX_COMPILER_ID}")
  set(ISEM FALSE)
endif()

#glad loader is only for desktop
if(NOT ISEM)
  add_subdirectory(lib/gladloader)
  target_include_directories(${PROJECT_NAME} PUBLIC ${GLADLOADER_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} PUBLIC gladloader)
endif()

if(ISEM)
  set(EM_FLAGS "-s FULL_ES2=1 -s FULL_ES3=1 -s USE_SDL=2 -s WASM=1 -s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_STACK=32mb -s INITIAL_MEMORY=128mb")
  target_link_options(${PROJECT_NAME} PUBLIC ${EM_FLAGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EM_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EM_FLAGS}")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  message(STATUS " * Linking with Emscripten flags: ${EM_FLAGS}")
  message(STATUS " * Emscripten building html")

  #add dockerfile and other files
  file(COPY resources/Dockerfile DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY resources/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()