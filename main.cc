/*******************************************************************************
  Title          : main.cc
  Author         : Wei Lian
  Created on     : Oct 7, 2018
  Description    : main interface
  Purpose        : 
  Usage          : 
  Build with     : 
  Modifications  : 

*******************************************************************************/
#include "PCB_function.h"

using namespace std;

int main(){
	sysGen system;
	system.input_system();
	string input;
	int pnum,dnum,fnum;
	int *dtrack;
	int *curtrackhead;
	double alpha;//history parameter
	double tao;//initial burst estimate
	double tcputotal=0; //total cpu time of completed proc
	int comprocnum=0; //number of completed processes
	unsigned long Memsize;  //totoal memory size
	unsigned long Pagesize;  //the size of a page
	unsigned long Maxsize;  //Maximum size of a process
	int framenum;  //number of frames in system
	pnum=system.get_printer();
	dnum=system.get_disk_drive();
	fnum=system.get_flash_drive();
	alpha=system.get_alpha();
	tao=system.get_tao();
	dtrack=system.get_dtrack();
	curtrackhead=system.get_curtrackhead();
	Memsize=system.get_Memsize();
	Pagesize=system.get_Pagesize();
	Maxsize=system.get_Maxsize();
	framenum=system.get_framenum();
	printer=new queue[pnum];
	disk=new queue[dnum];
	flash=new queue[fnum];
	set_frame(framenum);
	framelist fl;
	while(true){
		set(alpha,tao,tcputotal,comprocnum,dtrack,curtrackhead,Memsize,Pagesize,Maxsize);
		cout<<"Command: ";
		if(readq.head==NULL){
			readq.tail=readq.head;
		}
		if(!(cin>>input))  //check whether the input is successful
		{
			cout<<"Error: Wrong input!"<<endl;
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		CPU=readq.head;  //Pair with checkcpu()
		if(input[0]=='A'){
			if(checkinput(input)!=0) continue;

			ArrivalnJobPool(fl);
		}
		else if(input[0]=='P'){
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>pnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			PDFinterrupt(printer[incheck-1]);
		}
		else if(input[0]=='D')  //Disk interrupt handling routine
		{
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>dnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			if(disk[incheck-1].head!=NULL)
			{
				if(disk[incheck-1].head->next!=NULL)
					curtrackhead[incheck-1]=disk[incheck-1].head->next->cylnum;
			}
			PDFinterrupt(disk[incheck-1]);
		
		}
		else if(input[0]=='F')  //CDRW interrupt handling routine
		{
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>fnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			PDFinterrupt(flash[incheck-1]);
		
		}
		else if(input[0]=='p')
		{
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>pnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			int cpusta=checkcpu();  //checkcpu() should be called only once
			if(cpusta==-1) continue;  //if no process in CPU
            if(cpusta==1) pdcSC(printer[incheck-1], 1);
		}
		else if(input[0]=='d')
		{
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>dnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			int cpusta=checkcpu();  //checkcpu() should be called only once
			if(cpusta==-1) continue;  //if no process in CPU
			if(cpusta==1) pdcSC(disk[incheck-1], incheck+1);
		}
		else if(input[0]=='f')
		{
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if(incheck>fnum||incheck<=0) 
			{ cout<<"No such device"<<endl; continue;}
			int cpusta=checkcpu();  //checkcpu() should be called only once
			if(cpusta==-1) continue;  //if no process in CPU
			if(cpusta==1) pdcSC(flash[incheck-1], 0);
		}
		else if(input[0]=='t')
		{
			if(checkinput(input)!=0) continue;
			int cpusta=checkcpu();  //checkcpu() should be called only once
			if(cpusta==-1) continue;  //no process in cpu
			if(cpusta==1){
				UpdateTime(*CPU);
				cout<<"Terminated:"<<endl;
				//output for pid
				printf("%-19s%-d\n", "PID:", CPU->PID);
				//output for total
				printf("%-19s%-.1f\n", "total CPU time:", CPU->ttotal);
				//output avg
				printf("%-19s%-.1f\n", "avg burst time:",CPU->tave);
				//update total cpu completed time
				tcputotal=tcputotal+CPU->ttotal;
				comprocnum++;
				fl.setFrame(*CPU);
				delete CPU;
				CPU=NULL;
				PCB* tempJP;
				while(true){
					tempJP=CheckJobPool(fl);
					if(tempJP==NULL) break;
					else{
						bool bt=fl.getFrame(tempJP->pageNum, *tempJP);
						if(bt==false) cout<<"bug"<<endl; //shouldbe true
						sortReadQ(*tempJP);  //put the new proc in readq
					}
				}
			}

		}
		else if(input[0]=='K'){
			int incheck=checkinput(input);
			//if wrong format wait for next command
			if(incheck==-1) continue;
			else if((incheck==0&&input[1]=='\0')||incheck>pidtrack)
				{ cout<<"No such process"<<endl; continue;}
			if(CPU!=NULL){
				if(incheck==CPU->PID){
					checkcpu();
					UpdateTime(*CPU);
				                    
				cout<<"Killed:"<<endl;
				//output pid
				printf("%-19s%-d\n", "PID:", CPU->PID);
				//output total
				printf("%-19s%-.1f\n", "total CPU time:", CPU->ttotal);
				//output avg
				printf("%-19s%-.1f\n", "avg burst time:",CPU->tave);
				//update total cpu completed time
				tcputotal=tcputotal+CPU->ttotal;
				comprocnum++;
				//new from here
				fl.setFrame(*CPU);
				delete CPU;  //release memory
				CPU=NULL;  //set cpu free
				PCB* tempJP;
				while(true){
					tempJP=CheckJobPool(fl);  //comp jobpool with fl
					if(tempJP==NULL) break;
					else {
						bool bt=fl.getFrame(tempJP->pageNum, *tempJP);
						if(bt==false) cout<<"bug"<<endl; //shouldbe true
						sortReadQ(*tempJP);  //put the new proc in readq
					}
				}
				continue;
				}
			}
			PCB* target=findProc(incheck, pnum, dnum, fnum);
			if(target==NULL)
				cout<<"Process not in memory, cannot be killed."<<endl;
			else {
				cout<<"Killed:"<<endl;
				//output pid
				printf("%-19s%-d\n", "PID:", target->PID);
				//output total
				printf("%-19s%-.1f\n", "total CPU time:", target->ttotal);
				//output avg
				printf("%-19s%-.1f\n", "avg burst time:", target->tave);
				//update total cpu completed time
				tcputotal=tcputotal+target->ttotal;
				comprocnum++;
				fl.setFrame(*target);  //release used frames
				delete target;  //release memory
				PCB* tempJP;
				while(true){
					tempJP=CheckJobPool(fl);  //comp jobpool with fl
					if(tempJP==NULL) break;
					else {
						bool bt=fl.getFrame(tempJP->pageNum, *tempJP);
						if(bt==false) cout<<"bug"<<endl; //shouldbe true
						sortReadQ(*tempJP);  //put the new proc in readq
					}
				}
				
			}
		}
		else if(input[0]=='S')
		{
			if(input[1]=='\0') {
				cout<<"Select r,p,d,f,m,j: ";
				cin>>input[1];
			}
			if(input[1]=='p'||input[1]=='P')  //printer queue
			{
				Print(printer, pnum, 'p');
			}
			else if(input[1]=='d'||input[1]=='D')  //disk queue
			{
				Print(disk, dnum, 'd');
			}
			else if(input[1]=='f'||input[1]=='F')  //flash queue
			{
				Print(flash, fnum, 'f');
			}
			else if(input[1]=='r'||input[1]=='R')  //ready queue
			{
				Print(&readq, 1, 'r');
			}
			else if(input[1]=='m'||input[1]=='M')  //memory info
			{
				cout<<"Memory information:"<<endl;
				fl.Print();
			}
			else if(input[1]=='j'||input[1]=='J')
			{
				PrintJP();
			}
			else
			{
				cout<<"Wrong command."<<endl;
			}
				
		}

		else
			cout<<"Wrong command."<<endl;

		if(CPU==NULL) CPU=readq.head;  //double check whether necessary
	}

	delete []printer;
	delete []disk;
	delete []flash;
	return 0;
}