CC	 = gcc
SOURCES  = dstruct.c policies.c prsim.c
OBJECTS  = $(SOURCES:.c=.o)
TARGET   = prsim
CFLAGS   = -c -Wall -O3
LDFLAGS	 = -lm

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) main.c -o main.o
	$(CC) -O3 $(OBJECTS) main.o $(LDFLAGS) -o $@

tests: $(OBJECTS)
	$(CC) $(CFLAGS) prsim_tests.c -o prsim_tests.o
	$(CC) $(OBJECTS) prsim_tests.o $(LDFLAGS) -o prsim_tests
	-./prsim_tests

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm *.o
	-rm ${TARGET}

