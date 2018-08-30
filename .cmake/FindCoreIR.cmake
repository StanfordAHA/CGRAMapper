# - Try to Find CoreIR
# Once done, this will define
#
#  COREIR_FOUND - system has CoreIR dir.
#  COREIR_INCLUDE_DIR - directories which contain the CoreIR headers.
#  COREIR_LIBRARIES - libraries required to link against CoreIR.
#  Copyleft @ Keyi Zhang
#  If current CoreIR use any auto build system, things will be much easier

if(DEFINED COREIR_DIR AND IS_DIRECTORY "${COREIR_DIR}")
    set(COREIR_FOUND TRUE)
elseif(DEFINED ENV{COREIR_DIR} AND IS_DIRECTORY "$ENV{COREIR_DIR}")
    set(COREIR_FOUND TRUE)
    set(COREIR_DIR $ENV{COREIR_DIR})
    message(STATUS "Using COREIR_DIR ${COREIR_DIR} from environment variable")
else()
    message(WARNING "COREIR_DIR not specified. Use env variable $COREIR_DIR "
                    "or specify it on the command line with -D COREIR_DIR=...")
endif()

# convert to absolute
if (NOT IS_ABSOLUTE ${COREIR_DIR})
    MESSAGE(FATAL_ERROR "${COREIR_DIR} has to be absolute")
endif()

find_path(COREIR_INCLUDE_DIR NAMES coreir.h
    PATHS "${COREIR_DIR}/include"
)

# that's lots of libraries. more than most common FOSS!
find_library(COREIR_LIB NAMES coreir
    PATHS "${COREIR_DIR}/lib"
)

find_library(COREIR_AETHERLING coreir-aetherlinglib
    PATHS "${COREIR_DIR}/lib"
)

find_library(COREIR_COMMON coreir-commonlib
    PATHS "${COREIR_DIR}/lib"
)

find_library(COREIR_LIB_C coreir-c
    PATHS "${COREIR_DIR}/lib"
)

find_library(COREIR_ICE coreir-ice40
    PATHS "${COREIR_DIR}/lib"
)

find_library(COREIR_RTL coreir-rtlil
    PATHS "${COREIR_DIR}/lib"
)

set(COREIR_LIBRARIES ${COREIR_LIB} ${COREIR_AETHERLING} ${COREIR_COMMON}
    ${COREIR_LIB_C} ${COREIR_ICE} ${COREIR_RTL})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreIR DEFAULT_MSG COREIR_FOUND
                                  COREIR_INCLUDE_DIR
                                  COREIR_LIB
                                  COREIR_AETHERLING
                                  COREIR_COMMON
                                  COREIR_LIB_C
                                  COREIR_ICE
                                  COREIR_RTL)

mark_as_advanced(COREIR_FOUND COREIR_INCLUDE_DIR COREIR_LIBRARIES)
