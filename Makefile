TARGET		= hidimg
OBJS_TARGET	= hidimg.o

CFLAGS += -g
LIBS += -lm -lc

include Makefile.in
