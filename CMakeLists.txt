# Require CMake 2.8
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME PNP)
project(${PROJECT_NAME})

# Set verbose output while testing CMake
#set(CMAKE_VERBOSE_MAKEFILE 1)

# Set CMake behavior
cmake_policy(SET CMP0004 OLD)

# Get DOLFIN configuration data (DOLFINConfig.cmake must be in DOLFIN_CMAKE_CONFIG_PATH)
find_package(DOLFIN)

# Need to get VTK config because VTK uses advanced VTK features which
# mean it's not enough to just link to the DOLFIN target. See
# http://www.vtk.org/pipermail/vtk-developers/2013-October/014402.html
find_package(VTK HINTS ${VTK_DIR} $ENV{VTK_DIR} NO_MODULE QUIET)

# Default build type (can be overridden by user)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
    "Choose the type of build, options are: Debug MinSizeRel Release RelWithDebInfo." FORCE)
endif()

# Compiler definitions
add_definitions(${DOLFIN_CXX_DEFINITIONS})

# Compiler flags
set(CMAKE_CXX_FLAGS "${DOLFIN_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

# BOOST lib
# find_package(Boost)
# include_directories(${Boost_INCLUDE_DIRS})
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Set FASP directories and libraries
set(FASP_DIR /usr/local/faspsolver)     # must be set for each new build
set(FASP4NS_DIR /usr/local/fasp4ns)     # must be set for each new build
set(FASP_LIB ${FASP_DIR}/lib/libfasp.a)
set(FASP4NS_LIB ${FASP4NS_DIR}/lib/libfasp4ns.a)

# Include directories
set(FASP_INCLUDE_DIR ${FASP_DIR}/base/include)
set(FASP4NS_INCLUDE_DIR ${FASP4NS_DIR}/include)
include_directories(${DOLFIN_INCLUDE_DIRS} ${FASP_INCLUDE_DIR} ${FASP4NS_INCLUDE_DIR} ./include)
include_directories(SYSTEM ${DOLFIN_3RD_PARTY_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH /usr/local/faspsolver/modules)
find_package(UMFPACK REQUIRED)
include_directories(${UMFPACK_INCLUDE_DIRS})



# Awesome OSX TARGET
# set(OSX_TARGET "/opt/local/lib/gcc6/gcc/x86_64-apple-darwin16/6.3.0/libgcc.a" "/opt/local/lib/gcc6/libquadmath.a" "/opt/local/lib/gcc6/libgfortran.a")
set(PNP_LIBRARY ${DOLFIN_LIBRARIES} ${DOLFIN_3RD_PARTY_LIBRARIES} ${FASP_LIB} ${OSX_TARGET} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${UMFPACK_LIBRARY})
set(PNP_STOKES_LIBRARY ${DOLFIN_LIBRARIES} ${DOLFIN_3RD_PARTY_LIBRARIES} ${FASP4NS_LIB} ${FASP_LIB} ${OSX_TARGET} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${UMFPACK_LIBRARY})

set(SRC_DIR ./src/domain.cpp ./src/dirichlet.cpp ./src/pde.cpp ./src/newton_status.cpp ./src/error.cpp ./src/mesh_refiner.cpp)

add_executable(test_poisson ./benchmarks/poisson/main.cpp ./benchmarks/poisson/poisson.cpp ${SRC_DIR})
target_link_libraries(test_poisson ${PNP_LIBRARY})

add_executable(test_vector_poisson ./benchmarks/vector_poisson/main.cpp ./benchmarks/vector_poisson/vector_poisson.cpp ${SRC_DIR})
target_link_libraries(test_vector_poisson ${PNP_LIBRARY})

add_executable(linear_experiment ./benchmarks/linearized_pnp_experiment/main.cpp ./benchmarks/linearized_pnp_experiment/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(linear_experiment ${PNP_LIBRARY})

add_executable(pnp_experiment ./benchmarks/pnp_experiment/main.cpp ./benchmarks/pnp_experiment/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_experiment ${PNP_LIBRARY})

add_executable(pnp_exact ./benchmarks/pnp_exact_solution/main.cpp ./benchmarks/pnp_exact_solution/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_exact ${PNP_LIBRARY})

add_executable(pnp_stokes_tensor ./benchmarks/pnp_stokes_tensor/main.cpp ./benchmarks/pnp_stokes_tensor/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(pnp_stokes_tensor ${PNP_STOKES_LIBRARY})

add_executable(pnp_stokes_refine_tensor ./benchmarks/pnp_stokes_tensor/main_refinement.cpp ./benchmarks/pnp_stokes_tensor/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(pnp_stokes_refine_tensor ${PNP_STOKES_LIBRARY})

add_executable(pnp_refine ./benchmarks/pnp_refine_exact/main.cpp ./benchmarks/pnp_refine_exact/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_refine ${PNP_LIBRARY})

add_executable(pnp_diode ./benchmarks/pnp_diode/main.cpp ./benchmarks/pnp_diode/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_diode ${PNP_LIBRARY})

add_executable(pnp_ns_pb ./benchmarks/pnp_pb/main_ns.cpp ./benchmarks/pnp_pb/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(pnp_ns_pb ${PNP_STOKES_LIBRARY})

add_executable(pnp_pb ./benchmarks/pnp_pb/main.cpp ./benchmarks/pnp_pb/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_pb ${PNP_LIBRARY})

add_executable(pnp_pb_refine ./benchmarks/pnp_pb/main_refinement.cpp ./benchmarks/pnp_pb/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(pnp_pb_refine ${PNP_LIBRARY})

add_executable(pnp_stokes ./benchmarks/pnp_stokes/main.cpp ./benchmarks/pnp_stokes/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(pnp_stokes ${PNP_STOKES_LIBRARY})

add_executable(pnp_stokes_refine ./benchmarks/pnp_stokes/main_refinement.cpp ./benchmarks/pnp_stokes/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(pnp_stokes_refine ${PNP_STOKES_LIBRARY})

add_executable(ns_spheres ./benchmarks/pnp_ns_spheres/main.cpp ./benchmarks/pnp_ns_spheres/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(ns_spheres ${PNP_STOKES_LIBRARY})

add_executable(ns_spheres_refinement ./benchmarks/pnp_ns_spheres/main_refinement.cpp ./benchmarks/pnp_ns_spheres/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(ns_spheres_refinement ${PNP_STOKES_LIBRARY})

add_executable(phys_pnp ./benchmarks/physic_bench/main.cpp ./benchmarks/physic_bench/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(phys_pnp ${PNP_LIBRARY})

add_executable(phys_pnp_ref ./benchmarks/physic_bench/main_refinement.cpp ./benchmarks/physic_bench/linear_pnp.cpp ${SRC_DIR})
target_link_libraries(phys_pnp_ref ${PNP_LIBRARY})

add_executable(phys_ns ./benchmarks/physic_bench/main_ns.cpp ./benchmarks/physic_bench/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(phys_ns ${PNP_STOKES_LIBRARY})

add_executable(phys_ns_ref ./benchmarks/physic_bench/main_ns_refinement.cpp ./benchmarks/physic_bench/linear_pnp_ns.cpp ${SRC_DIR})
target_link_libraries(phys_ns_ref ${PNP_STOKES_LIBRARY})
