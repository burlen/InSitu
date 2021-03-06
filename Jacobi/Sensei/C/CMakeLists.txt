CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
PROJECT (PJACOBI)

OPTION(INSITU_COUPLING "create an executable using SENSEI" ON)
if(INSITU_COUPLING)
  # make use of sense 2.0 API
  # see branch request_specific_meshes
  OPTION(SENSEI_2 "use SENSEI 2.0 API" OFF)
  find_package(SENSEI REQUIRED)
endif()

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")

FIND_PACKAGE(MPI REQUIRED)
INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})

set(sources solvers.c pjacobi.c)
set(libs m)

if(ENABLE_SENSEI)
  list(APPEND sources JacobiDataAdaptor.cxx Bridge.cxx)
  list(APPEND libs sensei)
  add_definitions("-DENABLE_SENSEI")
  if (SENSEI_2)
    add_definitions("-DSENSEI_2")
  endif()
endif()

ADD_EXECUTABLE(pjacobi ${sources})
TARGET_LINK_LIBRARIES(pjacobi ${MPI_C_LIBRARIES} ${libs})
SET_PROPERTY(TARGET pjacobi APPEND PROPERTY COMPILE_FLAGS "-DPARALLEL")
