CC      ?= cc
CFLAGS  ?= -O2 -Wall -Wextra -pedantic
CPPFLAGS?= -Iincludes
LDFLAGS ?=
LDLIBS  ?=

OBJS = src/ahr_tid.o src/AHRtID.o

all: AHRtID

AHRtID: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o AHRtID

.PHONY: all clean
