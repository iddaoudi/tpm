cmake_minimum_required(VERSION 3.2)
project(tpm_trace_no_ompt)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# set up path
set(ROOT )
set(OMP_INSTALL ${ROOT}/llvm-project/build-openmp)
set(OMP_LIB_PATH ${OMP_INSTALL}/lib)
set(OMP_INCLUDE ${OMP_INSTALL}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(SOURCE_FILES
    tpm_trace_no_ompt.c
    tpm_trace_no_ompt.h
    include/hashmap.c
    include/hashmap.h
)

include_directories(${OMP_INCLUDE})
add_library(tpm_trace_no_ompt SHARED ${SOURCE_FILES})
target_link_libraries(tpm_trace_no_ompt -lzmq)
install(TARGETS tpm_trace_no_ompt DESTINATION lib)
