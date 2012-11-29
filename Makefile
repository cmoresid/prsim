CC		 = gcc
SOURCES  = prsim.c
OBJECTS  = $(SOURCES:.c=.o)
TARGET   = prsim 
CFLAGS   = -g -c -Wall
LDFLAGS	 = -lpthread

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm *.o
	-rm ${TARGET}

