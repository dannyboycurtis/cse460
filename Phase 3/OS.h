/********************************************************************************
 * James Small & William Elder
 * OS.h
 * 6/7/13
 * This program is the main OS class that uses an assembler and virtual machine
 * objects to run our programs.  This class takes in the program files, converts
 * them to object code, loads them all into memory, and then proceeds to run them
 * all using context switches to allow all of the programs to run concurrently,
 * allowing each program to have access to the cpu at some point.
 ********************************************************************************/

#ifndef OS_H
#define OS_H

#include "Assembler.h"
#include "VirtualMachine.h"
#include "PCB.h"
#include <list>
#include <queue>
#include <string>
#include <cstring>

using namespace std;

class OS {
private:
    //variables
    Assembler as;
    VirtualMachine vm;
    list<PCB *> jobs;
    queue<PCB *> readyQ, waitQ;
    PCB * running;
    int contextSwitchClock, idleClock;
    int MULTIPROGCAP;
    list<int> unusedFrames;
    fstream readProgs;
    bool lessThanMultiProgCap;
    int pageReplaced;
    
    // internal functions
    void getPCBFromVM();
    void givePCBToVM();
    void outputErrorCode(string s);
    bool done();
    void readIn(string line);
    void replaceAlgorithm();

    
public:
    void start(int lruValue);
    OS();
    ~OS();
};

#endif
