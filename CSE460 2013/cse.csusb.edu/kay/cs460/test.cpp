#include <iostream>
#include <fstream>
#include <list>
#include <cassert>

using namespace std;

class OS;

class VM {
	int pc;
public:
	VM(): pc(0) { }
	void run(fstream & in, fstream & out) { 
		int i; 
		in >> i; 
		out << i; }
friend
	class OS;
};

class PCB {
	int pc;
	fstream in;
	fstream out;
public:
	PCB(const int p): pc(p) { }
friend
	class OS;
};

class OS {
	VM vm;
	list<PCB *> jobs;
	PCB * pcb;
public:
	OS();
	//void loadState() { vm.pc = pcb.pc; }
	void copy();
};

void OS::copy()
{
	vm.run(jobs.front()->in, jobs.front()->out);
	vm.run(jobs.back()->in, jobs.back()->out);
	jobs.front()->out.close();
	jobs.back()->out.close();
}

OS::OS()
{
	pcb = new PCB(1);
	pcb->in.open("add10.in", ios::in);
	pcb->out.open("add10.out", ios::out);
	assert(pcb->in.is_open() and pcb->out.is_open());
	jobs.push_back(pcb);

	pcb = new PCB(2);
	pcb->in.open("sub.in", ios::in);
	pcb->out.open("sub.out", ios::out);
	assert(pcb->in.is_open() and pcb->out.is_open());
	jobs.push_back(pcb);
}

main()
{
	OS os;
	os.copy();
}
