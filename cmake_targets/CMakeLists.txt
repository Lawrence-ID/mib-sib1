#/*
# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this file
# * except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *      http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *      contact@openairinterface.org
# */

# Author: laurent THOMAS, Lionel GAUTHIER

cmake_minimum_required (VERSION 3.0)


# System packages that are required
# We use either the cmake buildin, in ubuntu are in: /usr/share/cmake*/Modules/
# or cmake provide a generic interface to pkg-config that widely used
###################################


#########################################################
# Base directories, compatible with legacy OAI building #
#########################################################
set (OPENAIR_DIR     $ENV{OPENAIR_DIR})
if("${OPENAIR_DIR}" STREQUAL "")
  string(REGEX REPLACE "/cmake_targets.*$" "" OPENAIR_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

set (NFAPI_DIR       ${OPENAIR_DIR}/nfapi/open-nFAPI)
set (NFAPI_USER_DIR  ${OPENAIR_DIR}/nfapi/oai_integration)
set (OPENAIR1_DIR    ${OPENAIR_DIR}/openair1)
set (OPENAIR2_DIR    ${OPENAIR_DIR}/openair2)
#set (OPENAIR_TARGETS ${OPENAIR_DIR}/targets)
#set (OPENAIR3_DIR    ${OPENAIR_DIR}/openair3)
set (OPENAIR_CMAKE   ${OPENAIR_DIR}/cmake_targets)
set (OPENAIR_ANS1   ${OPENAIR_DIR}/asn1_decoder)
set (OPENAIR_ANS1_LIB   ${OPENAIR_DIR}/asn1_decoder/asn1_lib/)
set (OPENAIR_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})

project (OpenAirInterface)

##############################################
# Base CUDA setting
##############################################
find_package(CUDA)
if (CUDA_FOUND)
    message ("cuda include ${CUDA_INCLUDE_DIRS}")
    message ("cuda library ${CUDA_LIBRARY_DIRS}")

    add_definitions("-L/usr/local/cuda/lib64")

    SET(CUDA_NVCC_FLAGS
      "${CUDA_NVCC_FLAGS};-arch=sm_60;")

    # Disable warnings for CUDA
    SET(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS};-lpthread;-w;-O3;--default-stream;per-thread;-I/usr/local/cuda/inc;-L/usr/local/cuda/lib -lcutil;-rdc=true;-lcudadevrt")
    SET(CUDA_VERBOSE_BUILD ON)
    SET(CUDA_HOST_COMPILER "/usr/bin/g++")

    SET(CUDA_SEPARABLE_COMPILATION ON)

else (CUDA_FOUND)
   message ("No CUDA tool installed")
endif ()


###########################################
# macros to define options as there is numerous options in oai
################################################
macro(add_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_option)

macro(add_boolean_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY TYPE BOOL)
  if (${value})
    add_definitions("-D${name}")
  endif (${value})
endmacro(add_boolean_option)

macro(add_integer_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_integer_option)

macro(add_list1_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${name}=${value}")
  endif()
endmacro(add_list1_option)

macro(add_list2_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${value}=1")
  endif()
endmacro(add_list2_option)

macro(add_list_string_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${name}=\"${value}\"")
  endif()
endmacro(add_list_string_option)

# this function should produce the same value as the macro MAKE_VERSION defined in the C code (file types.h)
function(make_version VERSION_VALUE)
  math(EXPR RESULT "0")
  foreach (ARG ${ARGN})
    math(EXPR RESULT "${RESULT} * 16 + ${ARG}")
  endforeach()
  set(${VERSION_VALUE} "${RESULT}" PARENT_SCOPE)
endfunction()
####################################################
# compilation flags
#############################################

#set(CMAKE_BUILD_TYPE "Debug")
if (CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
add_list_string_option(CMAKE_BUILD_TYPE "RelWithDebInfo" "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." Debug Release RelWithDebInfo MinSizeRel)

Message("Architecture is ${CMAKE_SYSTEM_PROCESSOR}")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  set(C_FLAGS_PROCESSOR "-gdwarf-2 -lgcc -lrt -fsigned-char ")
else (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  if(EXISTS  "/proc/cpuinfo")
    file(STRINGS "/proc/cpuinfo" CPUINFO REGEX flags LIMIT_COUNT 1)
    if (CPUINFO MATCHES "avx2")
      set(C_FLAGS_PROCESSOR "${C_FLAGS_PROCESSOR} -mavx2")
      set(COMPILATION_AVX2 "True")
    else()
      set(COMPILATION_AVX2 "False")
    endif()
    if (CPUINFO MATCHES "sse4_1")
      set(C_FLAGS_PROCESSOR "${C_FLAGS_PROCESSOR} -msse4.1")
    endif()
    if (CPUINFO MATCHES "ssse3")
      set(C_FLAGS_PROCESSOR "${C_FLAGS_PROCESSOR} -mssse3")
    endif()
  else()
    Message("/proc/cpuinfo does not exit. We will use manual CPU flags")
  endif()
endif()

set(C_FLAGS_PROCESSOR " ${C_FLAGS_PROCESSOR} ${CFLAGS_PROCESSOR_USER}")

Message("C_FLAGS_PROCESSOR is ${C_FLAGS_PROCESSOR}")

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
  if ( (NOT( C_FLAGS_PROCESSOR MATCHES "ssse3")) OR (NOT( C_FLAGS_PROCESSOR MATCHES "msse4.1")) )
    Message(FATAL_ERROR "For x86 Architecture, you must have following flags: -mssse3 -msse4.1. The current detected flags are: ${C_FLAGS_PROCESSOR}. You can pass the flags manually in build script, for example: ./build_oai --cflags_processor \"-mssse3 -msse4.1 -mavx2\" ")
  endif()
endif()

#
# add autotools definitions that were maybe used!
add_definitions("-DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_STRERROR=1 -DHAVE_SOCKET=1 -DHAVE_MEMSET=1 -DHAVE_GETTIMEOFDAY=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC=1 -DHAVE_LIBSCTP")

set(commonOpts "-pipe -Wno-packed-bitfield-compat -fPIC -Wall -fno-strict-aliasing -rdynamic")

set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} ${C_FLAGS_PROCESSOR} ${commonOpts} -std=gnu99 -funroll-loops")
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} ${C_FLAGS_PROCESSOR}  ${commonOpts} -std=c++11")

# cuda compiler bug (limitation) on complex macro definition
if (CUDA_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCUDA_FLAG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCUDA_FLAG")
endif()

if (SANITIZE_ADDRESS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-common")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-common")
endif ()

add_definitions("-DASN_DISABLE_OER_SUPPORT")

#########################
#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -ggdb3 -Wl,-Map=output.map -Wl,-rpath ${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=output.map -Wl,-rpath ${CMAKE_CURRENT_BINARY_DIR}")
#########################
# set a flag for changes in the source code
# these changes are related to hardcoded path to include .h files
#set(debugOpt "-ggdb3 -DMALLOC_CHECK_=3 -fno-delete-null-pointer-checks")
set(CMAKE_C_FLAGS_DEBUG "${debugOpt} -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${debugOpt} -O2")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(GIT_BRANCH        "UNKNOWN")
set(GIT_COMMIT_HASH   "UNKNOWN")
set(GIT_COMMIT_DATE   "UNKNOWN")

find_package(Git)
if(GIT_FOUND)
  message("git found: ${GIT_EXECUTABLE}")
  # Get the current working branch
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get the latest commit date of the working branch
  execute_process(
    COMMAND git log -1 --format=%cd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

# Below is a hard-coded info
set (FIRMWARE_VERSION "No svn information")
add_definitions("-DFIRMWARE_VERSION=\"${FIRMWARE_VERSION}\"")
add_definitions("-DPACKAGE_VERSION=\"Branch: ${GIT_BRANCH} Abrev. Hash: ${GIT_COMMIT_HASH} Date: ${GIT_COMMIT_DATE}\"")
add_definitions("-DPACKAGE_BUGREPORT=\"openair4g-devel@lists.eurecom.fr\"")
#add_definitions("-DEMIT_ASN_DEBUG=1")


# Debug related options
#########################################
add_boolean_option(MSG_PRINT           False "print debug messages")
add_boolean_option(DISABLE_XER_PRINT   False "print XER Format")
add_boolean_option(XER_PRINT           False "print XER Format")
add_boolean_option(PRINT_STATS         False "This adds the possibility to see the status")
add_boolean_option(T_TRACER            false  "Activate the T tracer, a debugging/monitoring framework" )
add_boolean_option(UE_AUTOTEST_TRACE   False "Activate UE autotest specific logs")
add_boolean_option(UE_DEBUG_TRACE      False "Activate UE debug trace")
add_boolean_option(UE_TIMING_TRACE     False "Activate UE timing trace")
add_boolean_option(DEBUG_CONSOLE       False "disables stdout/stderr buffering")



# Hardware dependant options
###################################
add_list1_option(NB_ANTENNAS_RX "4" "Number of antennas in reception" "1" "2" "4")
add_list1_option(NB_ANTENNAS_TX "4" "Number of antennas in transmission" "1" "2" "4")

# add_list2_option(RF_BOARD "EXMIMO" "RF head type" "None" "OAI_USRP" "OAI_BLADERF" "OAI_LMSSDR" "OAI_SIMU" "OAI_YUNSDR")

# add_list2_option(TRANSP_PRO "None" "Transport protocol type" "None" "ETHERNET")
#NOKIA config enhancement
#  set (CONFIG_ROOTDIR
#    ${OPENAIR_DIR}/common/config
#    )
#  set (CONFIG_SOURCES
#    ${CONFIG_ROOTDIR}/config_load_configmodule.c
#    ${CONFIG_ROOTDIR}/config_userapi.c
#    ${CONFIG_ROOTDIR}/config_cmdline.c
#    )
# #  set (CONFIG_LIBCONFIG_SOURCES
# #    ${CONFIG_ROOTDIR}/libconfig/config_libconfig.c
# #    )
# #  add_library(CONFIG_LIB ${CONFIG_SOURCES})
# #  add_library(params_libconfig MODULE ${CONFIG_LIBCONFIG_SOURCES} )
# #  target_link_libraries(params_libconfig config)
# # shared library loader
# set (SHLIB_LOADER_SOURCES
#   ${OPENAIR_DIR}/common/utils/load_module_shlib.c
# )

##########################################################

#include_directories ("${OPENAIR_TARGETS}/ARCH/COMMON")

Message("DEADLINE_SCHEDULER flag  is ${DEADLINE_SCHEDULER}")
Message("CPU_Affinity flag is ${CPU_AFFINITY}")

##############################################################
#    ???!!! TO BE DOCUMENTED OPTIONS !!!???
##############################################################

add_boolean_option(NO_RRM                  True  "DO WE HAVE A RADIO RESSOURCE MANAGER: NO")

add_boolean_option(OAI_NW_DRIVER_TYPE_ETHERNET False "????")
add_boolean_option(DEADLINE_SCHEDULER False "Use the Linux scheduler SCHED_DEADLINE: kernel >= 3.14")
add_boolean_option(CPU_AFFINITY False "Enable CPU Affinity of threads (only valid without deadline scheduler). It is enabled only with >2 CPUs")

########################
# Include order
##########################
add_boolean_option(ENB_MODE True "Swap the include directories between openair2 and openair3" )

##########################
# SCHEDULING/REAL-TIME/PERF options
##########################
add_boolean_option(ENABLE_USE_CPU_EXECUTION_TIME False "Add data in vcd traces: disable it if perf issues")
add_boolean_option(ENABLE_VCD              False  "always true now, time measurements of proc calls and var displays")
add_boolean_option(ENABLE_VCD_FIFO         False  "time measurements of proc calls and var displays sent to FIFO (one more thread)")
add_boolean_option(LINUX                   False "used in weird memcpy() in pdcp.c ???")
add_boolean_option(LINUX_LIST              False "used only in lists.c: either use OAI implementation of lists or Linux one (should be True, but it is False")

##########################
# PHY options
##########################
add_boolean_option(DRIVER2013              True "only relevant for EXMIMO")
add_boolean_option(EXMIMO_IOT              True "????")
add_boolean_option(LOCALIZATION            False "???")
add_integer_option(MAX_NUM_CCs             1     "????")
add_boolean_option(PHYSIM                  False  "for L1 simulators (dlsim, ulsim, ...)")
add_boolean_option(SMBV                    False "Rohde&Schwarz SMBV100A vector signal generator")
add_boolean_option(DEBUG_PHY               False "Enable PHY layer debugging options")
add_boolean_option(DEBUG_PHY_PROC          False "Enable debugging of PHY layer procedures")
add_boolean_option(DEBUG_DLSCH             False "Enable debugging of DLSCH physical layer channel")
add_boolean_option(MEX                     False "Enabling compilation with mex")


##########################
# S1AP LAYER OPTIONS
##########################
# none

# add the binary tree to the search path for include files
#######################################################
# We will find ConfigOAI.h after generation in target directory
include_directories("${OPENAIR_BIN_DIR}")
# add directories to find all include files
# the internal rule is to use generic names such as defs.h
# but to make it uniq name as adding the relative path in the include directtive
# example: #include "RRC/LTE/rrc_defs.h"
#find_path (include_dirs_all *.h ${OPENAIR_DIR})
#find_path (include_dirs_all *.h PATHS /usr/include NO_CMAKE_PATH)
#include_directories("${include_dirs_all}")

# Legacy exact order

include_directories("${OPENAIR_DIR}/executables")
include_directories("${OPENAIR1_DIR}")
include_directories("${OPENAIR_DIR}")
include_directories("${OPENAIR_ANS1}")
include_directories("${OPENAIR_ANS1_LIB}")



set(SCHED_SRC_NR_UE
  ${OPENAIR1_DIR}/SCHED_NR_UE/phy_procedures_nr_ue.c
  #${OPENAIR1_DIR}/SCHED_NR/phy_procedures_nr_common.c
  #${OPENAIR1_DIR}/SCHED_NR_UE/fapi_nr_ue_l1.c
 #${OPENAIR1_DIR}/SCHED_NR_UE/phy_frame_config_nr_ue.c
 # ${OPENAIR1_DIR}/SCHED_NR_UE/harq_nr.c
  #${OPENAIR1_DIR}/SCHED_NR_UE/pucch_uci_ue_nr.c
)
add_library(SCHED_NR_UE_LIB ${SCHED_SRC_NR_UE})


set(PHY_POLARSRC
  ${OPENAIR1_DIR}/PHY/CODING/nr_polar_init.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_bitwise_operations.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_crc_byte.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_crc.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_decoder.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_decoding_tools.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_encoder.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_interleaving_pattern.c
  #${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_kernal_operation.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_kronecker_power_matrices.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_matrix_and_array.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_procedures.c
  ${OPENAIR1_DIR}/PHY/CODING/nrPolar_tools/nr_polar_sequence_pattern.c
)

  set(PHY_NR_UE_SRC
  ${OPENAIR1_DIR}/PHY/CODING/nr_segmentation.c
  ${OPENAIR1_DIR}/PHY/CODING/nr_rate_matching.c
  ${OPENAIR1_DIR}/PHY/CODING/crc_byte.c
  ${OPENAIR1_DIR}/PHY/INIT/nr_parms.c
  ${OPENAIR1_DIR}/PHY/INIT/nr_common.c
  ${OPENAIR1_DIR}/PHY/MODULATION/nr_modulation.c
  ${OPENAIR1_DIR}/PHY/MODULATION/slot_fep_nr.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/pss_nr.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/sss_nr.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/cic_filter_nr.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_initial_sync.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_ue_rf_helpers.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_pbch.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_sib.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_dlsch_demodulation.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_ulsch_coding.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_dlsch_decoding.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_dlsch_llr_computation.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_tbs_tools.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_compute_tbs_common.c
  #${OPENAIR1_DIR}/PHY/NR_TRANSPORT/nr_prach_common.c
  #${OPENAIR1_DIR}/PHY/NR_TRANSPORT/nr_sch_dmrs.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_prach.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/srs_modulation_nr.c
  ${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/dci_nr.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/dci_tools_nr.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/pucch_nr.c
  #${OPENAIR1_DIR}/PHY/NR_TRANSPORT/nr_uci_tools_common.c
  #${OPENAIR1_DIR}/PHY/NR_UE_TRANSPORT/nr_ulsch_ue.c
  #${OPENAIR1_DIR}/PHY/NR_REFSIG/ul_ref_seq_nr.c
  ${OPENAIR1_DIR}/PHY/NR_REFSIG/nr_dmrs_rx.c
  ${OPENAIR1_DIR}/PHY/NR_REFSIG/dmrs_nr.c
  #${OPENAIR1_DIR}/PHY/NR_REFSIG/ptrs_nr.c
  ${OPENAIR1_DIR}/PHY/NR_REFSIG/nr_gold_ue.c
  ${OPENAIR1_DIR}/PHY/NR_REFSIG/nr_gen_mod_table.c
  ${OPENAIR1_DIR}/PHY/NR_REFSIG/lte_gold_generic.c
  ${OPENAIR1_DIR}/PHY/NR_UE_ESTIMATION/filt16a_32.c
  ${OPENAIR1_DIR}/PHY/NR_UE_ESTIMATION/nr_dl_channel_estimation.c
  #${OPENAIR1_DIR}/PHY/NR_UE_ESTIMATION/nr_adjust_synch_ue.c
  #${OPENAIR1_DIR}/PHY/NR_UE_ESTIMATION/nr_ue_measurements.c
  #${OPENAIR1_DIR}/PHY/NR_UE_ESTIMATION/nr_adjust_gain.c
  ${OPENAIR1_DIR}/PHY/TOOLS/file_output.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cadd_vv.c
 # ${OPENAIR1_DIR}/PHY/TOOLS/lte_dfts.c
  ${OPENAIR1_DIR}/PHY/TOOLS/log2_approx.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cmult_sv.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cmult_vv.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cdot_prod.c
  ${OPENAIR1_DIR}/PHY/TOOLS/signal_energy.c
  ${OPENAIR1_DIR}/PHY/TOOLS/dB_routines.c
  ${OPENAIR1_DIR}/PHY/TOOLS/sqrt.c
  ${OPENAIR1_DIR}/PHY/TOOLS/time_meas.c
  ${OPENAIR1_DIR}/PHY/TOOLS/lut.c
  ${OPENAIR1_DIR}/PHY/INIT/nr_init_ue.c
  ${OPENAIR1_DIR}/PHY/TOOLS/oai_dfts.c
  ${OPENAIR1_DIR}/PHY/CODING/nrLDPC_decoder/nrLDPC_decoder.c
  #  ${OPENAIR1_DIR}/SIMULATION/NR_UE_PHY/unit_tests/src/pucch_uci_test.c
  ${PHY_POLARSRC}
  #${PHY_SMALLBLOCKSRC}
  #${PHY_NR_CODINGIF}
  )

# add_library(PHY_COMMON ${PHY_SRC_COMMON})
# #add_dependencies(PHY_COMMON rrc_flag)
# add_dependencies(PHY_COMMON dfts)
#add_library(PHY ${PHY_SRC})

#add_dependencies(PHY rrc_flag)
#add_library(PHY_UE ${PHY_SRC_UE})
#add_dependencies(PHY_UE rrc_flag)
#add_library(PHY_NR_COMMON ${PHY_NR_SRC_COMMON})
#add_library(PHY_NR ${PHY_NR_SRC})
add_library(PHY_NR_UE ${PHY_NR_UE_SRC})


file(GLOB ASN1_LIBS "${OPENAIR_ANS1}/asn1_lib/*.c")

set(ASN1_DECODER_SRC
  ${OPENAIR_ANS1}/sib1_asn1_decoder.c
  ${ASN1_LIBS}
)
add_library(ASN1_DECODER ${ASN1_DECODER_SRC})


add_executable(nr-uesoftmodem
#  ${rrc_h}
#  ${s1ap_h}
#  ${OPENAIR_BIN_DIR}/messages_xml.h
#  ${OPENAIR_DIR}/common/utils/threadPool/thread-pool.c
  #${OPENAIR_DIR}/executables/nr-uesoftmodem.c
  ${OPENAIR_DIR}/executables/nr-uesoftmodem_new.c
  #${OPENAIR_DIR}/executables/nr-ue.c
  #${OPENAIR_DIR}/executables/softmodem-common.c
  #${OPENAIR1_DIR}/SIMULATION/TOOLS/taus.c
  #${OPENAIR_TARGETS}/ARCH/COMMON/common_lib.c
  #${OPENAIR_TARGETS}/ARCH/COMMON/record_player.c
  #${OPENAIR2_DIR}/RRC/NAS/nas_config.c
  #${OPENAIR2_DIR}/LAYER2/NR_MAC_COMMON/nr_mac_common.c
  #${OPENAIR2_DIR}/RRC/NAS/rb_config.c
  #${OPENAIR1_DIR}/SIMULATION/ETH_TRANSPORT/netlink_init.c
  #${OPENAIR3_DIR}/NAS/UE/nas_ue_task.c
  #${OPENAIR_DIR}/common/utils/utils.c
  #${OPENAIR_DIR}/common/utils/system.c
  #${OPENAIR_DIR}/common/utils/nr/nr_common.c
  #${XFORMSINTERFACE_SOURCE}
  #${T_SOURCE}
  #${UTIL_SRC}
  #${CONFIG_SOURCES}
  #${SHLIB_LOADER_SOURCES}
  )

 target_link_libraries (nr-uesoftmodem
   -Wl,--start-group
   SCHED_NR_UE_LIB
   PHY_NR_UE 
   ASN1_DECODER
#   NFAPI_USER_LIB MISC_NFAPI_NR_LIB S1AP_LIB S1AP_ENB
#   ${MSC_LIB} ${RAL_LIB} ${NAS_UE_LIB} ITTI ${FLPT_MSG_LIB} ${ASYNC_IF_LIB} LFDS7 ${ATLAS_LIBRARIES}
#   NFAPI_USER_LIB S1AP_LIB S1AP_ENB
#   ${MSC_LIB} ${RAL_LIB}  ITTI ${FLPT_MSG_LIB} ${ASYNC_IF_LIB} LFDS7 ${ATLAS_LIBRARIES} 
   -Wl,--end-group z dl)


target_link_libraries (nr-uesoftmodem m )
#target_link_libraries (nr-uesoftmodem ${LIB_LMS_LIBRARIES})
#target_link_libraries (nr-uesoftmodem )

#add_dependencies( nr-uesoftmodem ldpc )


