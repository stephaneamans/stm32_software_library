#################################################
# makefile_flags.mk                             #
#                                               #
# This file contains the compilation            #
# and linking files.                            #
#                                               #
# Created on: Sep 8, 2024                       #
# Author: Stéphane Amans                        #
#################################################


#####################
# Compilation flags #
#####################
ASFLAGS := \
          -mcpu=cortex-m3 \
          -mthumb
          
CFLAGS_GLOBAL := \
          $(ASFLAGS) \
          -Wall \
          -Wextra \
          -fmessage-length=0 \
          -ffunction-sections \
          -fdata-sections \
          -c

CFLAGS := \
          $(CFLAGS_GLOBAL) \
          -O0

CFLAGS_DEBUG := \
                $(CFLAGS_GLOBAL) \
                -g3


#################
# Linking flags #
#################
DFLAGS := \
          $(ASFLAGS) \
          -T"$(ROOT_DIR)target/stm32f103c8tx/LinkerScript.ld" \
          -Wl,-Map=output.map \
          -Wl,--gc-sections \
          -specs=nano.specs \
          -specs=nosys.specs
          
