#################################################
# makefile_global.mk                            #
#                                               #
# This file contains all the global parameters. #
#                                               #
# Created on: Sep 8, 2024                       #
# Author: Stéphane Amans                        #
#################################################


##################################
# Shortcuts to compilation tools #
##################################
CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZETOOL := arm-none-eabi-size


######################
# Folders definition #
######################
# Main sources folder:
SOURCES_FOLDER := $(subst project_side/,,$(ROOT_DIR))

# Main build folder:
BUILD_FOLDER := build/

#Basic software sources and build folders:
BSW := bsw/
BSW_SOURCES_FOLDER := $(subst project_side/,,$(BSW_ROOT_DIR))
BSW_BUILD_FOLDER := ${BUILD_FOLDER}${BSW}
BSW_BUILD_OBJ_FOLDER := ${BSW_BUILD_FOLDER}objects/

#Application software sources and build folders:
APPLICATIONS := applications/
APPLICATIONS_SOURCES_FOLDER := $(subst project_side/,,$(APPLICATIONS_ROOT_DIR))
APPLICATIONS_BUILD_FOLDER := ${BUILD_FOLDER}${APPLICATIONS}
APPLICATIONS_BUILD_OBJ_FOLDER := ${APPLICATIONS_BUILD_FOLDER}objects/


##############################
# Environement configuration #
##############################
# Debug mode inactive
DEBUG = 0


#############################
# Different prompts display #
#############################
PRINT_BANNER := "*************************************"
PRINT_SPACE := " "
PRINT_NEW_COMPILATION := "NEW BUILD"
PRINT_START_BUILD_APPLICATION := "BUILD APPLICATION"
PRINT_START_BUILD_BASIC_SOFTWARE := "BUILD BASIC SOFTWARE"
PRINT_NEW_COMPILATION := "NEW BUILD"
PRINT_BUILD := "Process build"
PRINT_DIRECTORY := "directory"
PRINT_BSW_OBJECTS := "basic software objects"
PRINT_APPLICATIONS_OBJECTS := "application software objects"
