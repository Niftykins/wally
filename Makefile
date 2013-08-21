CC = g++

DEBUG = -g

OPT = 
#-O2 
# -D_NO_ERR_CHECK \
#  -D_MT

CPPFLAGS = ${DEBUG} ${OPT} \
	-I./ 

LDFLAGS = -Wall -Wl


# NOTE: ld requires dependency ordering to be in reverse!
# -lpng for linux, -lpng12 for osx
LIBS = -lpng12 -lz -lm 

OBJS = wally.o PNGCodecRGB24.o

EXEC = wally

all:	$(EXEC)

$(EXEC):	$(OBJS) 
		$(CC) $(DEBUG) $(OPT) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

%.o:	%.cpp
		$(CC) $(DEBUG) $(OPT) $(CPPFLAGS) -c $*.cpp

clean:
		rm -f *.o *~ $(EXEC) *x*.png
