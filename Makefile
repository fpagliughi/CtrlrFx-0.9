# Makefile for the CtrlrFx library

ifndef PLATFORM
  PLATFORM = linux
endif

library := libCtrlrFx.a
build_dir := lib/$(PLATFORM)
obj_dir := obj/$(PLATFORM)

_MK_OBJ_DIR := $(shell mkdir -p $(obj_dir))

include platform.mk

os_src_dir := src/os/$(CFX_OS)
os_include_dir := CtrlrFx/os/$(CFX_OS)

vpath %.cpp src $(os_src_dir) src/os/generic \
			src/io/net src/io/Device src/io/BinStream \
			src/data src/debug src/DistObj

vpath %.h  CtrlrFx $(os_include_dir) CtrlrFx/os/generic

# ----- Generate source list -----

sources := $(wildcard src/*.cpp \
		src/data/*.cpp \
		src/debug/*.cpp \
		src/io/net/*.cpp \
		src/io/Device/*.cpp \
		src/io/BinStream/*.cpp \
		src/DistObj/*.cpp \
		$(os_src_dir)/*.cpp)

ifeq "$(CFX_OS)" "posix"
  sources += $(wildcard src/os/generic/*.cpp)
endif

ifeq "$(PLATFORM)" "ecos"
  sources := $(wildcard src/*.cpp \
		src/io/Device/*.cpp \
		src/io/BinStream/*.cpp \
		src/DistObj/*.cpp \
		src/os/generic/*.cpp \
		$(os_src_dir)/*.cpp)
endif

dir_srcs := $(sources)
sources := $(notdir $(sources))

# ----- Determine object file names & dependencies -----

objects := $(subst .cpp,.o,$(sources))
objects := $(addprefix $(obj_dir)/,$(objects))

dependencies := $(subst .cpp,.d,$(dir_srcs))

# Compiler directives

#CPPFLAGS += $(addprefix -D, $(defines)) $(addprefix -I, $(include_dirs))

$(obj_dir)/%.o: %.cpp
	@echo $(CXX) $<
	$(QUIET) $(COMPILE.cpp) $(OUTPUT_OPTION) $<


# ----- Some sub-targets -----

examples := examples/DistObj/DigIOClient \
	examples/DistObj/DigIOSim \
	examples/net/devfwd \
	examples/net/echo/MtEchoServer \
	examples/net/echo/StEchoServer \
	examples/net/echo/tcpecho

# ----- Build Targets -----

all : $(build_dir)/$(library)

$(build_dir)/$(library): $(objects)
	@echo $(AR) $@
	$(QUIET) $(AR) $(ARFLAGS) $@ $^

.PHONY: examples $(examples)
examples: $(examples)

$(examples):
	$(MAKE) --directory=$@ PLATFORM=$(PLATFORM) $(TARGET)

.PHONY: dump
dump:
	@echo PLATFORM: $(PLATFORM)
	@echo OS: $(CFX_OS)
#	@echo $(sources)
#	@echo $(dependencies)
#	@echo $(obj_dir)
	@echo $(objects)

.PHONY: clean
clean:
	@echo RM lib and objs
	$(QUIET) $(RM) $(build_dir)/$(library) $(objects)

.PHONY: distclean
distclean:
	$(RM) $(build_dir)/$(library) $(objects) $(dependencies)

.PHONY: doc
doc:
	doxygen

# ----- Header dependencies -----

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(MAKECMDGOALS)" "dump"
      include $(dependencies)
    endif
  endif
endif

%.d: %.cpp
	@echo DEP $<
	$(QUIET) $(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($(*F)\)\.o[ :]*,$$(obj_dir)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

