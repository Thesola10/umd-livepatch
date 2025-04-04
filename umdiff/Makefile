TARGET = umdiff
C_OBJS = main.o rdiff.o
OBJS   = $(C_OBJS)

CMAKE := cmake

LIBRSYNC_SOURCE := librsync

ifeq (,$(wildcard /usr/lib*/librsync.*))
	CFLAGS += -I$(LIBRSYNC_SOURCE)/src
	OBJS += librsync.a
else
	LIBS += -lrsync
endif

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

librsync:
	git clone https://github.com/librsync/librsync

librsync.a: $(LIBRSYNC_SOURCE)
	mkdir -p librsync.build
	cd librsync.build && $(CMAKE) -DBUILD_SHARED_LIBS=0 $(shell realpath $(LIBRSYNC_SOURCE)) && make
	mv librsync.build/librsync.a .
	rm -rf librsync.build

