# platform.mk

ifndef VERBOSE
  QUIET := @
endif
  
ifndef CFX_DIR
  ifdef CTRLR_FX_DIR
    CFX_DIR = $(CTRLR_FX_DIR)
  else
    CFX_DIR = $(HOME)/CtrlrFx
  endif
endif  

ifndef PLATFORM
  PLATFORM = linux
endif

#ifneq "$(DEBUG_LEVEL)" ""
ifdef DEBUG_LEVEL
  CPPFLAGS += -DCFX_DEBUG_LEVEL=$(DEBUG_LEVEL)
endif

ifeq "$(PLATFORM)" "ts7200"
  CXX = arm-9tdmi-linux-gnu-g++
  AR  = arm-9tdmi-linux-gnu-ar
  CFX_OS = posix
endif

ifeq "$(PLATFORM)" "ecos"
  CFX_OS = ecos
endif

# ----- Operating Systems -----

ifeq "$(CFX_OS)" ""
  CFX_OS = posix
endif

ifeq "$(CFX_OS)" "posix"
  PLATFORM_DEFS := -D_REENTRANT -DCFX_POSIX

  ifeq "$(PLATFORM)" "cygwin"
    LDLIBS += -lpthread
  else
    LDLIBS += -lpthread -lrt
  endif
endif

ifeq "$(CFX_OS)" "ecos"
  include $(ECOS_INSTALL_DIR)/include/pkgconf/ecos.mak

  CC  = $(ECOS_COMMAND_PREFIX)gcc
  CXX = $(ECOS_COMMAND_PREFIX)g++
  LD  = $(CC)

  PLATFORM_DEFS := -D_ECOS -I$(ECOS_INSTALL_DIR)/include
  LDFLAGS += -nostartfiles -L$(ECOS_INSTALL_DIR)/lib -Ttarget.ld -nostdlib
endif

# ----- Common Build Requirements -----

CXXFLAGS += -Wall
CPPFLAGS += -DPLATFORM=$(PLATFORM) -DCFX_OS=$(CFX_OS) $(PLATFORM_DEFS) -I$(CFX_DIR) -I$(CFX_DIR)/CtrlrFx/os/$(CFX_OS) -I$(CFX_DIR)/CtrlrFx/os/generic
LDLIBS   += -L$(CFX_DIR)/lib/$(PLATFORM) -lCtrlrFx

ifndef DEBUG
  CPPFLAGS += -O2 -DNDEBUG
else
  CPPFLAGS += -g -O0 -D_DEBUG
endif

build_dir ?= $(PLATFORM)
_MK_BUILD_DIR := $(shell mkdir -p $(build_dir))

RM = rm -f


