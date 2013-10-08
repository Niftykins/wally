CC = clang++

DEBUG = -O4

OPT = 
#-O2 
# -D_NO_ERR_CHECK \
#  -D_MT

CPPFLAGS = -I /usr/lib/c++/v1

LDFLAGS = -Wall


# NOTE: ld requires dependency ordering to be in reverse!
# -lpng for linux, -lpng12 for osx
LIBS = -lpng12 -lz -lm -std=c++11 -lpthread -stdlib=libc++

OBJS = thread.o PNGCodecRGB24.o
OBJS_CHECK = check.o PNGCodecRGB24.o

EXEC = wally
CHECK = check

all:	$(EXEC) $(CHECK)

$(EXEC):	$(OBJS) 
		$(CC) $(DEBUG) $(OPT) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

$(CHECK):	$(OBJS_CHECK) 
		$(CC) $(DEBUG) $(OPT) $(LDFLAGS) -o $(CHECK) $(OBJS_CHECK) $(LIBS)

%.o:	%.cpp
		$(CC) $(DEBUG) $(OPT) $(CPPFLAGS) -c $*.cpp

clean:
		rm -f *.o *~ $(EXEC) $(CHECK) *x*.png results/*
