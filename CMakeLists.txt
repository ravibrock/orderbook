cmake_minimum_required(VERSION 3.10)
project(orderbook)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# explicitly include project's include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# find required packages
find_package(cpr REQUIRED)
find_package(Crow REQUIRED)

# collect all source files from src directory
file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# create executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# link cpr and crow
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr Crow::Crow)
