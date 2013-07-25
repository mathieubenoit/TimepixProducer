EXTERNAL_DEFS += EUDAQ_FUNC=__PRETTY_FUNCTION__ EUDAQ_PLATFORM=PF_$(PLATFORM)
EXTERNAL_LIBS += eudaq dl
EXTERNAL_LIBDIRS += ../bin
EXTERNAL_INCS += ../main/include ./include
EXTERNAL_INCS += $(PIXELMAN_INSTALL)/_other_/headers

include ../Makefile.common

PIXELMAN_INSTALL:=$(TPPROD)/Pixelman_SCL_2011_12_07
PIXELMAN_HWLIBS:=$(PIXELMAN_INSTALL)/hwlibs


BIN = $(TPPROD)

CFLAGS += -c -m32 -isystem $(PIXELMAN_INSTALL)/_other_/headers -I$(PIXELMAN_INSTALL)/_other_/headers
LDFLAGS +=  -m32 -L$(PIXELMAN_INSTALL) -lMpxManager -lMpxCtrl -L$(PIXELMAN_HWLIBS) -lusb -lstdc++ -ldl -lc 

ifeq ($(PLATFORM),WIN32)
EXTERNAL_LIBS += Ws2_32
endif

default: exe

exe: $(EXE_FILES)

$(EXE_FILES): $(OBJ_FILES)

lib: $(TARGET)

all: exe

.PHONY: exe lib all default
