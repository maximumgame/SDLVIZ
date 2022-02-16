project(gladloader)

add_library(${PROJECT_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#find_package(GLAD REQUIRED)
#target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})
