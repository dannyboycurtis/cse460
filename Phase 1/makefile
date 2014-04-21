CC=g++
CFLAGS=-c
EXECUTABLES=os
SOURCES=Assembler.cpp VirtualMachine.cpp os.cpp
OFILES=$(SOURCES:.cpp=.o)
HFILES=Assembler.h VirtualMachine.h

all: $(SOURCES) $(OFILES) $(EXECUTABLES)

$(EXECUTABLES): $(OFILES) $(HFILES)
	$(CC) $(OFILES) -o $(EXECUTABLES)

.cpp.o: $(HFILES)
	$(CC) $(CFLAGS) $< -o $@
