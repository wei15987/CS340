#ifndef PCB_H
#define PCB_H

#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include "sysgen.h"
int *dtrack;
int *curtrackhead;
double alpha;//history parameter
double tao;//initial burst estimate
double tcputotal; //total cpu time of completed proc
int comprocnum; //number of completed processes
unsigned long Memsize;  //totoal memory size
unsigned long Pagesize;  //the size of a page
unsigned long Maxsize;  //Maximum size of a process
int framenum;  //number of frames in system

struct PCB{
	int PID;
	PCB *prev=NULL;
	PCB *next=NULL;
	string filename;
	unsigned long startloc;
	char rorw;
	long filesize;
	int cylnum; //number of cylinder
	double taonext;//next burst time
	double tprev;//previous burst time
	int burstnum; //total number of cpu burst
	double tave;//average burst time
	double ttotal;//total burst time
	double lotime;//left over time
	unsigned long psize; //process size
	int pageNum;  //number of pages needed
	int* pagetable;  //page table array entrance
};
struct queue{
	PCB *head=NULL;
	PCB *tail=NULL;
};
queue *printer;
queue *disk;
queue *flash;

int pidtrack=0;
vector<PCB*>prolist;
queue jobpool;

#endif