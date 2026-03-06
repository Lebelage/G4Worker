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
    src/Utils/Constants
    Experiments
)

target_link_libraries(${PROJECT_NAME}
    ${Geant4_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# 1. Находим все .mac файлы
file(GLOB MACRO_FILES "${CMAKE_SOURCE_DIR}/macroses/*.mac")

# 2. Определяем целевую директорию для удобства
set(DEST_DIR "${CMAKE_BINARY_DIR}/AppConfigs")

# 3. Создаем цель для копирования
add_custom_target(copy_macros ALL
    # Сначала создаем папку AppConfigs (если её нет)
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_DIR}
    # Затем копируем файлы
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MACRO_FILES}
            ${DEST_DIR}
    COMMENT "Copying .mac files to ${DEST_DIR}"
)

# 4. Устанавливаем зависимость
add_dependencies(${PROJECT_NAME} copy_macros)