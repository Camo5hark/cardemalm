cmake_minimum_required(VERSION 3.29)
project(cardemalm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

add_link_options(-Wall -Wno-attributes)

# copy libraries

execute_process(COMMAND rm -rf ${PROJECT_BINARY_DIR}/lib)
execute_process(COMMAND cp -r ${PROJECT_SOURCE_DIR}/frontend/lib ${PROJECT_BINARY_DIR})



# frontend executable

execute_process(COMMAND rm -rf ${PROJECT_BINARY_DIR}/res)
execute_process(COMMAND cp -r ${PROJECT_SOURCE_DIR}/frontend/res ${PROJECT_BINARY_DIR})
add_executable(cardemalm
        include/cardemalm.h
        include/cardemalm_card.h
        frontend/src/cardemalm_fe.h
        frontend/src/glad.c
        frontend/src/main.c
        frontend/src/window.c
        frontend/src/gl_util.c
        frontend/src/io_util.c
        frontend/src/active_card.c
        frontend/src/shader.c
        frontend/src/texture.c
        frontend/src/mesh.c
        frontend/src/card_essential.c
)
target_link_options(cardemalm PRIVATE -Wl,-rpath,./lib -rdynamic)
target_include_directories(cardemalm PRIVATE include)
target_link_directories(cardemalm PRIVATE frontend/lib)
target_link_libraries(cardemalm PRIVATE glfw)



# --- CARD SHARED OBJECTS ---

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/cards)
execute_process(COMMAND rm -rf ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
execute_process(COMMAND mkdir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})



# card: default

set(CARD_NAME default)
set(CARD_BINARY_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CARD_NAME})
execute_process(COMMAND mkdir ${CARD_BINARY_DIR})
execute_process(COMMAND cp -r ${PROJECT_SOURCE_DIR}/cards/${CARD_NAME}/res ${CARD_BINARY_DIR})
add_library(card_${CARD_NAME} SHARED
        cards/${CARD_NAME}/src/card.c
)
target_include_directories(card_${CARD_NAME} PRIVATE include)



# test
