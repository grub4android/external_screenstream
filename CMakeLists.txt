cmake_minimum_required(VERSION 2.8) 
project(screenstream)

# modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# sdl
Set (SDL1_FALLBACK FALSE)

Find_Package(SDL2)
if(NOT SDL2_FOUND)
	message(ERROR "Couldn't find SDL2 development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-dev' package.")
else()
	include_directories(${SDL2_INCLUDE_DIR})
endif()

Find_Package(SDL2_image)
if(NOT SDL2IMAGE_FOUND)
	message(FATAL_ERROR "Couldn't find SDL2-image development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-image-dev' package.")
else()
	include_directories(${SDL2IMAGE_INCLUDE_DIR})
endif()

# main executable
file(GLOB MAIN_SRCS "src/*.c")
add_executable(screenstream ${MAIN_SRCS})
target_link_libraries(screenstream ${CMAKE_LD_FLAGS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2MIXER_LIBRARY} ${SDL2TTF_LIBRARY})

# warn settings
set(CMAKE_CXX_FLAGS "i${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunused -Wshadow -Wunreachable-code -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

