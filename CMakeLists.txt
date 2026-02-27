cmake_minimum_required(VERSION 3.16...3.27)
project(G4Worker)

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

# ---------------------------------------------------------
# nlohmann/json
# ---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(nlohmann_json)

# ---------------------------------------------------------

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    src/Core/include
    src/Core/include/Messengers
    src/Core/include/Detectors
    src/Core/include/SourceGenerators
    src/Core/include/Actions
    src/Infrastructure
    src/Infrastructure/Services
    src/Infrastructure/Services/Interfaces
    src/Utils
    Experiments
)

target_link_libraries(${PROJECT_NAME}
    ${Geant4_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Найти все .mac файлы
file(GLOB MACRO_FILES "${CMAKE_SOURCE_DIR}/macroses/*.mac")

add_custom_target(copy_macros ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MACRO_FILES}
            ${CMAKE_BINARY_DIR}
    COMMENT "Copying .mac files to build directory"
)

add_dependencies(${PROJECT_NAME} copy_macros)