# buildtgts.mk
#
# Build target for simple applications built with the Controller Framework
# It will compile all the source files in the current directory, link them
# with the CtrlrFx library, and place the executable in a subdirectory named
# for the target.

ifeq "$(PLATFORM)" "cygwin"
  EXE := $(EXE).exe
endif  
      
TGT = $(PLATFORM)/$(EXE)
sources += $(wildcard *.cpp)

all: $(TGT)

$(TGT): $(sources) $(CFX_DIR)/lib/$(PLATFORM)/libCtrlrFx.a
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: clean
clean:
	$(QUIET) rm -f *~ $(TGT)

