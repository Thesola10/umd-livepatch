TARGET = umd_livepatch
C_OBJS = patch.o io_funcs.o main.o
OBJS = $(C_OBJS) imports.o
all: $(TARGET).prx
INCDIR = $(ARKROOT)/common/include $(ARKROOT)/core/systemctrl/include $(LIBRSYNC)/src
CFLAGS = -std=c99 -Os -G0 -Wall

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

LDFLAGS =  -nostartfiles -L .
LIBS = -lpspsystemctrl_kernel -lpspumd

PSP_FW_VERSION = 660

PRX_EXPORTS = exports.exp

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

include $(ARKROOT)/common/make/global.mak
PSPSDK = $(shell psp-config --pspsdk-path)

libpspsystemctrl_kernel.a:
	$(MAKE) -C $(ARKROOT)/libs/SystemCtrlForKernel
	cp $(ARKROOT)/libs/SystemCtrlForKernel/libpspsystemctrl_kernel.a .

$(TARGET).prx:: libpspsystemctrl_kernel.a

include $(PSPSDK)/lib/build.mak
include $(ARKROOT)/common/make/beauty.mak
