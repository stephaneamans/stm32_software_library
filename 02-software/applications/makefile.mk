#############################################
# applications                              #
# makefile.mk                               #
#                                           #
# This file is intended to create objects   #
# lists from c files lists and used to      #
# compile the application software          #
#                                           #
# Created on: Jun 25, 2025                  #
# Author: Stéphane Amans                    #
#############################################


############################################################################
# Include application software configuration makefile (generated) if exits #
############################################################################
include applications/configuration/generated_makefile_config.mk


###################################################
# Create application software include folder list #
###################################################
FULL_APPLICATIONS_INCLUDE_FOLDER := \
${BSW_INCLUDE_FOLDER} \
${APPLICATIONS_INCLUDE_FOLDER}


#####################################################################################
# Assemble automaticaly different applications lists needed for compilation process #
#####################################################################################
# Create application software objects list from source files path.
APPLICATIONS_OBJ_TARGET1 := $(foreach filename, $(addsuffix .o, $(basename $(notdir $(APPLICATIONS_SRC_C_FILES_TARGET1)))),${APPLICATIONS_BUILD_OBJ_FOLDER}${filename})
APPLICATIONS_OBJ_TARGET2 := $(foreach filename, $(addsuffix .o, $(basename $(notdir $(APPLICATIONS_SRC_C_FILES_TARGET2)))),${APPLICATIONS_BUILD_OBJ_FOLDER}${filename})

# Create application software list os source folders from source files paths.
APPLICATIONS_SRC_TARGET1 := $(dir $(APPLICATIONS_SRC_C_FILES_TARGET1))
APPLICATIONS_SRC_TARGET2 := $(dir $(APPLICATIONS_SRC_C_FILES_TARGET2))


##############################
# Application software rules #
##############################
# Rule for C objects modules
${APPLICATIONS_BUILD_OBJ_FOLDER}%.o: %.c
	@echo "Process ""$@" "module."
	@$(CC) $(CFLAGS) $(FULL_APPLICATIONS_INCLUDE_FOLDER) -o "$@" "$<"
