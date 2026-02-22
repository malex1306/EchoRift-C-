cmake_minimum_required(VERSION 4.1)
project(EchoRift CXX)

set(CMAKE_CXX_STANDARD 20)

# 1. Header-Suche aktivieren
include_directories(include)

# 2. Alle .cpp Dateien in 'src' finden
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 3. Pfade basierend auf deinen Screenshots setzen
set(VCPKG_PATH "C:/Users/alexa/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic")

# 4. Raylib suchen
find_path(RAYLIB_INCLUDE_DIR raylib.h PATHS "${VCPKG_PATH}/include")
find_library(RAYLIB_LIB NAMES raylib PATHS "${VCPKG_PATH}/lib")

if(RAYLIB_INCLUDE_DIR)
    include_directories(${RAYLIB_INCLUDE_DIR})
endif()

# 5. Das Programm bauen
add_executable(${PROJECT_NAME} main.cpp ${SOURCES}
        include/components/UiComponents.h)

# 6. Verlinken
if(RAYLIB_LIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB} winmm gdi32 shell32)
else()
    message(FATAL_ERROR "Raylib Bibliothek wurde nicht gefunden!")
endif()

# 7. DLL KOPIEREN (Angepasst an deinen Screenshot: libraylib.dll)
file(COPY "${VCPKG_PATH}/bin/libraylib.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${VCPKG_PATH}/bin/glfw3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})