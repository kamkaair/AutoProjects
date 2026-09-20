  
cmake_minimum_required(VERSION 3.5) 
set(CMAKE_GENERATOR_TOOLSET "v142") 
set(CMAKE_CXX_STANDARD 17) 
project(Automation) 
file(GLOB_RECURSE HDR_FILES "src/*.h") 
source_group("Header Files", FILES ${HDR_FILES}) 
file(GLOB_RECURSE SRC_FILES "src/*.cpp") 
source_group("Source Files", FILES ${SRC_FILES}) 
 
add_executable(Automation ${HDR_FILES} ${SRC_FILES}) 
 
add_executable(CalculateFolderSizes src/CalculateFolderSizes.cpp ${HDR_FILES}) 
add_executable(ConvertIntoTalk src/ConvertIntoTalk.cpp ${HDR_FILES}) 
add_executable(pixelReading src/pixelReading.cpp ${HDR_FILES}) 
