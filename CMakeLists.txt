cmake_minimum_required(VERSION 3.4.1)

project(knng)
message(status "Project Directory: ${PROJECT_SOURCE_DIR}")
add_compile_options(-std=gnu++17)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O2 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(
    ${catkin_INCLUDE_DIRS}
    # /home/mashiroyuki/intel/oneapi/mkl/latest/include/
)

link_directories(
    ${catkin_LIB_DIRS}
    # /home/mashiroyuki/intel/oneapi/mkl/latest/lib/
)


aux_source_directory(. DIR_SRCS)

add_executable(knng ${DIR_SRCS})

target_link_libraries(knng ${OpenMP_LIBS} libmkl_rt.so) 