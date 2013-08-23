CC = g++

DEBUG = -g -pg

OPT = 
#-O2 
# -D_NO_ERR_CHECK \
#  -D_MT

CPPFLAGS = -I ./

LDFLAGS = -Wall


# NOTE: ld requires dependency ordering to be in reverse!
# -lpng for linux, -lpng12 for osx
LIBS = -lpng12 -lz -lm -std=c++11 -lpthread -stdlib=libc++

OBJS = thread.o PNGCodecRGB24.o

EXEC = wally

all:	$(EXEC)

$(EXEC):	$(OBJS) 
		$(CC) $(DEBUG) $(OPT) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

%.o:	%.cpp
		$(CC) $(DEBUG) $(OPT) $(CPPFLAGS) -c $*.cpp

clean:
		rm -f *.o *~ $(EXEC) *x*.png results/*
