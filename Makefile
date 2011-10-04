#gmake Makefile
EXECUTABLE = bcd_clock

OBJS   = 
SRC    = bcd_clock_gtk2.c

CFLAGS = 
LFLAGS = -O2 -funroll-loops -Wall `pkg-config --cflags gtk+-2.0` \
	 `pkg-config --libs gtk+-2.0`
CC = gcc
COMPILE= $(CC) -c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $< $(LFLAGS)
	@echo
	@echo "Hooray! Executable: $(EXECUTABLE)" 

clean:
	-rm ./*.o $(EXECUTABLE)

