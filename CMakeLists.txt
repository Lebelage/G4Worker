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
