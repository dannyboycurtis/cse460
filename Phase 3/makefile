#makefile

os : OS.o Assembler.o VirtualMachine.o
	g++ -o os OS.o Assembler.o VirtualMachine.o

OS.o : OS.cpp
	g++ -c OS.cpp

Assembler.o : Assembler.cpp
	g++ -c Assembler.cpp

VirtualMachine.o : VirtualMachine.cpp
	g++ -c VirtualMachine.cpp

clean:
	rm *.o os

# END OF MAKE FILE
#
