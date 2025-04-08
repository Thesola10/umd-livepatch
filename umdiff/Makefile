TARGET = umdiff
C_OBJS = main.c.o rdiff.c.o compare.c.o file.c.o patch.c.o
OBJS   = $(C_OBJS)

CMAKE := cmake
RAGEL := ragel
DOCOPT2RAGEL := docopt2ragel

LIBRSYNC_SOURCE := librsync

ifneq (,${LIBRSYNC_LIBS})
	CFLAGS += ${LIBRSYNC_CFLAGS}
	LIBS += ${LIBRSYNC_LIBS}
else ifeq (,$(wildcard /usr/lib*/librsync.*))
	CFLAGS += -I$(LIBRSYNC_SOURCE)/src
	OBJS += librsync.a
else
	LIBS += -lrsync
endif

$(TARGET): $(OBJS) usage.rl.h
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

usage.rl: USAGE
	$(DOCOPT2RAGEL) USAGE > $@

%.rl.h: %.rl
	$(RAGEL) $< -o $@

%.c.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

librsync:
	git clone https://github.com/librsync/librsync

librsync.a: $(LIBRSYNC_SOURCE)
	mkdir -p librsync.build
	cd librsync.build && $(CMAKE) -DBUILD_SHARED_LIBS=0 $(shell realpath $(LIBRSYNC_SOURCE)) && make
	mv librsync.build/librsync.a .
	rm -rf librsync.build


clean:
	rm -rf librsync $(OBJS) *.rl $(TARGET)

.PHONY: clean
