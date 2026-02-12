cmake_minimum_required(VERSION 3.16...3.27)
project(G4Worker)

find_package(Geant4 REQUIRED)

include(${Geant4_USE_FILE})

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    src/Core/include
    src/Core/include/messengers
    Experiments
)

target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES})


# Найти все .mac файлы в папке macroses
file(GLOB MACRO_FILES "${CMAKE_SOURCE_DIR}/macroses/*.mac")

# Цель для копирования .mac файлов прямо в папку сборки (build/)
add_custom_target(copy_macros ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MACRO_FILES}
            ${CMAKE_BINARY_DIR}
    COMMENT "Copying .mac files to build directory"
)

# Обеспечить выполнение копирования при сборке
add_dependencies(${PROJECT_NAME} copy_macros)
