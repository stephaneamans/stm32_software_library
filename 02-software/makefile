#################################################
# makefile.mk                                   #
#                                               #
# Main make file entry point, this file is used #
# as a central point for this build system.     #
#                                               #
# Created on: Sep 8, 2024                       #
# Author: Stéphane Amans                        #
#################################################


# Create root directory path:
ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include make/makefile_flags.mk
include make/makefile_global.mk
include bsw/makefile.mk
include applications/makefile.mk

# Create VPATH from source files paths:
VPATH = \
        ${BSW_SRC} \
        ${APPLICATIONS_SRC_TARGET1} \
        ${APPLICATIONS_SRC_TARGET2}


###################################
# Check for make custom arguments #
###################################
# Store in the vriable the others arguments, from the second to the last one $(words $(MAKECMDGOALS)).
RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  
# Turn them into do-nothing targets
$(eval $(RUN_ARGS):;@:)

# If the debug mode is found as argument, then
ifeq (debug, $(findstring ${RUN_ARGS}, debug))

  # Debug mode is switched to active.
  DEBUG = 1

  # CFLAGS for debug are used
  CFLAGS = ${CFLAGS_DEBUG}
endif


###########
# Targets #
###########
target1.elf : print_debug create_bsw_folder ${BSW_OBJ} create_applications_folder ${APPLICATIONS_OBJ_TARGET1}

target2.elf : print_debug create_bsw_folder ${BSW_OBJ} create_applications_folder ${APPLICATIONS_OBJ_TARGET2}

.PHONY: clean create_bsw_folder create_applixcations_folder print_debug

create_bsw_folder:
	@echo ${PRINT_BANNER} ${PRINT_NEW_COMPILATION} ${PRINT_BANNER}
	@echo ${PRINT_BUILD} ${PRINT_DIRECTORY}
	@mkdir -p ${BUILD_FOLDER}
	@echo ${PRINT_BANNER} ${PRINT_START_BUILD_BASIC_SOFTWARE} ${PRINT_BANNER}
	@echo ${PRINT_BUILD} ${PRINT_BSW_OBJECTS} ${PRINT_DIRECTORY} : ${BSW_BUILD_OBJ_FOLDER}
	@mkdir -p ${BSW_BUILD_OBJ_FOLDER}

create_applications_folder:
	@echo ${PRINT_BANNER} ${PRINT_START_BUILD_APPLICATION} ${PRINT_BANNER}
	@echo ${PRINT_BUILD} ${PRINT_APPLICATIONS_OBJECTS} ${PRINT_DIRECTORY} : ${APPLICATIONS_BUILD_OBJ_FOLDER}
	@mkdir -p ${APPLICATIONS_BUILD_OBJ_FOLDER}

print_debug:
	@echo debug = ${DEBUG}
	@echo cflags = ${CFLAGS}

clean:
	@rm -rf ${BUILD_FOLDER}
