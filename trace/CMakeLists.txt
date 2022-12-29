cmake_minimum_required(VERSION 3.2)
project(tpm_trace)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE ON)

# set up path
set(ROOT /home/xps/Argonne/Software)
set(OMP_INSTALL ${ROOT}/llvm-project/build-openmp)
set(OMP_LIB_PATH ${OMP_INSTALL}/lib)
set(OMP_INCLUDE ${OMP_INSTALL}/include)

# For tracing and output
set(FEATURES "${FEATURES} -DOMPT_MEASUREMENT_SUPPORT=1")
set(FEATURES "${FEATURES} -DOMPT_TRACING_SUPPORT=1")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FEATURES} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FEATURES} -g")

set(SOURCE_FILES
    tpm_trace.h
    tpm_trace.c
)

include_directories(${OMP_INCLUDE})
add_library(tpm_trace SHARED ${SOURCE_FILES})
target_link_libraries(tpm_trace -lzmq)
install(TARGETS tpm_trace DESTINATION lib)
