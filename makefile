NAME=raytracing
OBJECTS=`ls *.c ./*/*.c`
OUTPUT=$(NAME).bin

CFLAGS=-I/usr/local/Mesa-3.4/include -g -Wall
#LDLIBS=-lX11 -lglut -lMesaGLU -lMesaGL -lm -lXext -lXmu
LDLIBS=-lX11 -lglut -lGLU -lGL -lm -lXext -lXmu
LDFLAGS=-L/usr/local/Mesa-3.4/lib -L/usr/X11R6/lib

$(OUTPUT): $(OBJECTS) $(ASM_OBJECT)
	cc $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(OUTPUT) $(OBJECTS) $(ASM_OBJECT)

$(OBJECTS): $(NAME).h

clean:
	rm -f *.o
	rm -f $(NAME)
