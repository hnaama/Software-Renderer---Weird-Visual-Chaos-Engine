cmake_minimum_required(VERSION 3.16)
project(software_renderer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 using sdl2-config
execute_process(
    COMMAND sdl2-config --cflags
    OUTPUT_VARIABLE SDL2_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND sdl2-config --libs
    OUTPUT_VARIABLE SDL2_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Parse the flags
string(REPLACE " " ";" SDL2_CFLAGS_LIST ${SDL2_CFLAGS})
string(REPLACE " " ";" SDL2_LIBS_LIST ${SDL2_LIBS})

# Add include directories from cflags
foreach(flag ${SDL2_CFLAGS_LIST})
    if(flag MATCHES "^-I")
        string(SUBSTRING ${flag} 2 -1 include_dir)
        include_directories(${include_dir})
    endif()
endforeach()

add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBS_LIST})

# On macOS we need to link frameworks explicitly
if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        "-framework Cocoa" 
        "-framework IOKit" 
        "-framework CoreVideo"
    )
endif()
