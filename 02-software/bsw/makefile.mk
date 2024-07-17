#############################################
# basic software                            #
# makefile.mk                               #
#                                           #
# This file is intended to create objects   #
# lists from c files lists and used to      #
# compile the basic software                #
#                                           #
# Created on: Sep 8, 2024                   #
# Author: Stéphane Amans                    #
#############################################


#############################################################
# Include basic software configuration makefile (generated) #
#############################################################
include bsw/configuration/generated_makefile_config.mk


############################################################################
# Assemble automaticaly different BSW lists needed for compilation process #
############################################################################
# Create basic software objects list from source files path.
BSW_OBJ := $(foreach filename, $(addsuffix .o, $(basename $(notdir $(BSW_SRC_C_FILES)))),${BSW_BUILD_OBJ_FOLDER}${filename})

# Create basic software list os source folders from source files paths.
BSW_SRC := $(dir $(BSW_SRC_C_FILES))


########################
# Basic software rules #
########################
# Rule for C objects modules
${BSW_BUILD_OBJ_FOLDER}%.o: %.c
	@echo "Process ""$@" "module."
	@$(CC) $(CFLAGS) $(BSW_INCLUDE_FOLDER) -o "$@" "$<"
