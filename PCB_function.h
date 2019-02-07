#ifndef PCB_FUNCTION_H
#define PCB_FUNCTION_H

#include <vector>
#include <string>
#include <iomanip>

#include "PCB.h"
#include "framelist.h"

PCB *CPU=NULL;
queue readq;
void set_frame(int framenum_temp)
{
	framenum=framenum_temp;
}
void set(double alpha_temp,double tao_temp,double tcputotal_temp,double comprocnum_temp, int *dtrack_temp, int *curtrackhead_temp, 
	unsigned long Memsize_temp, unsigned long Pagesize_temp, unsigned long Maxsize_temp)
{
	alpha=alpha_temp;
	tao=tao_temp;
	tcputotal=tcputotal_temp;
	comprocnum=comprocnum_temp;
	dtrack=dtrack_temp;
	curtrackhead=curtrackhead_temp;
	Memsize=Memsize_temp;
	Pagesize=Pagesize_temp;
	Maxsize=Maxsize_temp;
}
void UpdateTime(PCB& temp)
{
	// only called when one burst is done
	temp.tprev=temp.ttotal-temp.tave*temp.burstnum;  
	temp.burstnum++;  //burst num increase only when leaves cpu
	
	//update estimated next burst time
	temp.taonext=alpha*temp.taonext+(1-alpha)*temp.tprev;
	temp.lotime=temp.taonext;
	if(temp.burstnum!=0)  //calculate ave time if num is non-zero
		temp.tave=temp.ttotal/temp.burstnum;
}
void QueryTime(PCB& temp)
{
	double tempd;
	cout<<"CPU time used:";
	cin>>tempd;
	temp.ttotal=temp.ttotal+tempd;
	temp.lotime=temp.lotime-tempd;
	if(temp.lotime<0)
		temp.lotime=0;
}
int ptPrint(queue& qtem){  //print page table info in each queue
	PCB* iter=qtem.head;
	int protrack=0;
	while(true){
		if (iter == NULL) break;
		printf("%-4d%-4d", iter->PID, iter->pageNum);
		protrack++;
		for(int i=0; i<iter->pageNum; i++)
			cout<<iter->pagetable[i]<<" ";
		cout<<endl;
		if(iter->next==NULL||iter==qtem.tail)
			break;
		iter=iter->next;
	}
	return protrack;
}
//sort disk queue using C-LOOK
int SortDiskQ(queue& qtem, PCB& newtemp, int TNCylinder, int curhead) 
{
	bool  status=false;  //to see if the comparison reaches the end
	PCB* comtemp=qtem.head;  // used for loop comparison
	if(qtem.head==NULL)
	{
		qtem.head=& newtemp;
		qtem.tail=& newtemp;
		newtemp.prev=NULL;
		newtemp.next=NULL;
		return 0;
	}
	//decide whether it goes to be the first pcb after head
	if(curhead<=comtemp->cylnum)
	{
		if(curhead<=newtemp.cylnum&&newtemp.cylnum<comtemp->cylnum)
		{
			//insert to the qtem head first one
			newtemp.next=qtem.head;
			qtem.head->prev=&newtemp;
			qtem.head=&newtemp;
			newtemp.prev=NULL;
			return 1; //insertion finished and stop func
		}
	}
	if(curhead>comtemp->cylnum)
	{
		bool bb1=curhead<=newtemp.cylnum&&newtemp.cylnum<=TNCylinder;
		bool bb2=0<=newtemp.cylnum&&newtemp.cylnum<comtemp->cylnum;
		if(bb1||bb2)
		{
			//insert to the qtem head first one
			newtemp.next=qtem.head;
			qtem.head->prev=&newtemp;
			qtem.head=&newtemp;
			newtemp.prev=NULL;
			return 1; //insertion finished and stop func
		}
	}
		
		
	while(comtemp!=NULL)
	{
		
		
		
		if(comtemp->next!=NULL)
		{
			if(comtemp->cylnum<=newtemp.cylnum&&newtemp.cylnum<comtemp->next->cylnum)
			{ 	 //if the new track is btw prev and next insert
				newtemp.prev=comtemp;  //PCB insert start
				newtemp.next=comtemp->next;
				comtemp->next=&newtemp;
				newtemp.next->prev=&newtemp;  //PCB insert finish
				status=true;
				break;
			}
			if(comtemp->cylnum>comtemp->next->cylnum)  //last one this direction
			{
				bool b1=comtemp->cylnum<=newtemp.cylnum&&newtemp.cylnum<=TNCylinder;
				bool b2=0<=newtemp.cylnum&&newtemp.cylnum<comtemp->next->cylnum;
				if(b1||b2)
				{
					newtemp.prev=comtemp;  //PCB insert start
					newtemp.next=comtemp->next;
					comtemp->next=&newtemp;
					newtemp.next->prev=&newtemp;  //PCB insert finish
					status=true;
					break;
				}
			}
		}
		if(comtemp->next==NULL)
		{
			if(status==false)
			{
				newtemp.prev=comtemp;  //PCB insert start
				newtemp.next=NULL;
				comtemp->next=&newtemp;  //PCB end insert finish
				qtem.tail=&newtemp;
				break;
			}
		}  //compiled
		comtemp=comtemp->next;  //iteration
	}
	return 2;
}
void Print(queue *temp, int size, char a)  //compliled
{
	string str;  //for display
	if(a=='p') str="printer";
	if(a=='r') str="ready queue";
	if(a=='d') str="disk";
	if(a=='f') str="flash";
	int temn;
	PCB* iter;  //iterator
	PCB* itercpu;  //proc in cpu
	//output average total cpu time of completed processes
	if(comprocnum!=0)
		printf("\naverage CPU time of completed processes: %.1f\n",
			tcputotal/comprocnum);
	if(comprocnum==0)
		printf("\nNo process has been completed.\n");

	//formating with printf function
	if(a!='r') {

	printf("%-5s%-20s%-10s%-5s%-10s%-8s%-8s%-10s\n", " ", " ",
		" ", " ", " ", "total", "avg.", "disk");
	printf("%-5s%-20s%-10s%-5s%-10s%-8s%-8s%-10s\n", "PID", "Filename",
	"MEMstart", "R/W", "Length", "time", "burst", "cylinder");}
	if(a=='r') {
	printf("%-5s%-20s%-10s%-5s%-10s%-8s%-8s%-10s\n", " ", " ",
		" ", " ", " ", "total", "avg.", "next");
	printf("%-5s%-20s%-10s%-5s%-10s%-8s%-8s%-10s\n", "PID", "Filename",
	"MEMstart", "R/W", "Length", "time", "burst", "burst");}
	if(a!='r'&&a!='d') {
	for(temn=1; temn<=size; temn++)
	{
		printf("---%s%d\n", str.c_str(), temn);
		if(temp[temn-1].head==NULL) printf("%-s\n", "Empty"); //empty queue
		else
		{
			iter=temp[temn-1].head;
			while(true){
				printf("%-5d%-20s%-10lx%-5c%-10ld%-8.1f%-8.1f\n", iter->PID,
					iter->filename.c_str(), iter->startloc, iter->rorw,
					iter->filesize, iter->ttotal, iter->tave);
				if(iter==temp[temn-1].tail||iter==NULL)
					break; //when reaching the end
				else iter=iter->next;
			}
		}
	}
	cout<<"page table:"<<endl;
	printf("%-4s%-4s%-s", "PID", "Pg#", "frame list (to ascending page#)\n");
	int sem = 0;
	for(int sem1=0; sem1<size; sem1++)	
	{
		int sem2=ptPrint(temp[sem1]);
		sem = sem + sem2;
	}
	if (sem == 0) cout << "Empty" << endl;
	}
	if(a=='r'){
		if (temp->head == NULL) 
		{
			printf("---CPU:\n");  //print PCB in cpu
			printf("%-s\n", "Empty"); //empty queue
			printf("---%s\n", str.c_str());
			printf("%-s\n", "Empty"); //empty queue
		}
		else if((temp->head)->next==NULL) 
		{
			itercpu=temp->head;
			printf("---CPU:\n");  //print PCB in cpu
			printf("%-5d%-20s%-10lx%-5c%-10ld%-8.1f%-8.1f\n", itercpu->PID,
					itercpu->filename.c_str(), itercpu->startloc, itercpu->rorw,
					itercpu->filesize, itercpu->ttotal, itercpu->tave);
			//taonext is meaningless bc this burst is not over
			printf("---%s\n", str.c_str());
			printf("%-s\n", "Empty"); //still empty
		}
		else {
			iter=(temp->head)->next;
			itercpu=temp->head;
			printf("---CPU:\n");  //print PCB in cpu
			printf("%-5d%-20s%-10lx%-5c%-10ld%-8.1f%-8.1f\n", itercpu->PID,
					itercpu->filename.c_str(), itercpu->startloc, itercpu->rorw,
					itercpu->filesize, itercpu->ttotal, itercpu->tave);
			printf("---%s\n", str.c_str());
			while(true){
				printf("%-5d%-20s%-10lx%-5c%-10ld%-8.1f%-8.1f%-10.1f\n", iter->PID,
					iter->filename.c_str(), iter->startloc, iter->rorw,
					iter->filesize, iter->ttotal, iter->tave, iter->lotime);
				if(iter==temp->tail||iter==NULL)
					break; //when reaching the end
				else iter=iter->next;
			}
		}
		cout<<"page table:"<<endl;
		printf("%-4s%-4s%-s", "PID", "Pg#", "frame list (to ascending page#)\n");
		int sem=ptPrint(readq);
		if(sem==0) cout<<"Empty"<<endl;
	}
	if(a=='d') {
	for(temn=1; temn<=size; temn++)
	{
		printf("---%s%d\n", str.c_str(), temn);
		if(temp[temn-1].head==NULL) printf("%-s\n", "Empty"); //empty queue
		else
		{
			iter=temp[temn-1].head;
			while(true){
				printf("%-5d%-20s%-10lx%-5c%-10ld%-8.1f%-8.1f%-10d\n", iter->PID,
					iter->filename.c_str(), iter->startloc, iter->rorw,
					iter->filesize, iter->ttotal, iter->tave, iter->cylnum);
				if(iter==temp[temn-1].tail||iter==NULL)
					break; //when reaching the end
				else iter=iter->next;
			}
		}
	}
	cout<<"page table:"<<endl;
	printf("%-4s%-4s%-s", "PID", "Pg#", "frame list (to ascending page#)\n");
	int sem = 0;
	for (int sem1 = 0; sem1 < size; sem1++)
	{
		int sem2 = ptPrint(temp[sem1]);
		sem = sem + sem2;
	}
	if (sem == 0) cout << "Empty" << endl;
	}
}
int checkinput(string& s)   //if the input is wrong format return -1
{
	int temp0, temp1, temp2=0;
	if (s[0] == '\0')  //first check whether it is empty string
	{
		cout << "Wrong input format!" << endl;
		return -1;
	}
	else if (s[1] == '\0')  //check second char
		return 0;
	else if(s[2]!='\0')  //check whether its two digits
	{
		temp0=s[2]-'0';
		if(temp0<0||temp0>9)
		{
			cout<<"Wrong input format!"<<endl;
			return -1;
		}
	}
	else
	{
		temp0 = -1;  //if it only has one digit
		temp1 = s[1] - '0';
	}
	if(temp1<0||temp1>9)
		{
			cout<<"Wrong input format!"<<endl;
			return -1;
		}
	if(temp0==-1) temp2=temp1;
	if(temp0!=-1) temp2=temp1*10+temp0;
	return temp2;
}
int checkcpu(){
	if(CPU==NULL){
		cout<<"Currently no process is in CPU."<<endl;
		return -1;
	}
	readq.head=readq.head->next;
	if(readq.head==NULL){
		readq.tail=readq.head;
		CPU->next=NULL;//move to cpu
		CPU->prev=NULL;
	}
	if(readq.head!=NULL){
		readq.head->prev=NULL;
	}
	QueryTime(*CPU);
	return 1;
}
unsigned long getHexinput(unsigned long & testn,unsigned long  uplimit, PCB& tempPro){
	while(true)
	{
		if(cin>>hex>>testn&&testn<=uplimit)
			{
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cin>>dec;
				break;
			}
		cout<<"Error! Memory address illegal."<<endl;
		cout<<"Starting location:";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	//int offsetlen=(int) (log((double)Pagesize)/log(2.0));
	unsigned long offset=testn%Pagesize;
	int temppagenum=testn/Pagesize;  //get page num
	int tempframenum=tempPro.pagetable[temppagenum];  //find frame num
	unsigned long physadd=tempframenum*Pagesize+offset;  //physical address
	cout<<"Physical address:";  //print out
	cout<<hex<<physadd<<endl;  //hex out
	cout<<dec;
	tempPro.startloc=physadd;  //add to startloc
	return physadd;
}
void pdcSC(queue& qtemp, int pornot){
	UpdateTime(*CPU);
	cout<<"Please input file name:";  //ask for input
	cin>>CPU->filename;
	cout<<"Starting location in memory:"; //ask for parameters
	unsigned long tempstartloc;
	getHexinput(tempstartloc,CPU->psize,*CPU);//get ox input, call func
	
	if (pornot!=1){      //check whether it is a printer or not
		cout<<"Read(r) or Write(w):";
		while(true)  //Check whether input is right or not
		{
		if(cin>>(CPU->rorw)&&(CPU->rorw=='r'||CPU->rorw=='w')) break;
		cout<<"Error! Enter 'r'(read) or 'w'(write):";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	if(pornot==1) CPU->rorw='w';
	if(CPU->rorw=='w'||CPU->rorw=='W'){   //if it is a write, ask for size
		cout<<"File size is:";
		while(!(cin>>CPU->filesize))  //check successful input or not
	{
		cout<<"Error! Enter filesize in integer number:";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	}
	if(pornot<2){
	//put the current process in CPU into the end of requested queue
	if(qtemp.tail==NULL){
		qtemp.head=CPU;
		qtemp.tail=CPU;
	}
	else {
		qtemp.tail->next=CPU;
		CPU->prev=qtemp.tail;
		qtemp.tail=qtemp.tail->next;
	}
	}
	if(pornot>=2)
	{
		int ntem=pornot-2;
		cout<<"Entry cylinder is:";
		cin>>CPU->cylnum;
		if(qtemp.head==NULL)
			curtrackhead[ntem]=CPU->cylnum;
		SortDiskQ(qtemp,*CPU,dtrack[ntem],curtrackhead[ntem]);
	}

		
	// make sure to move current process out of cpu
	CPU=NULL;
}
void Arrival(PCB &tempA){
	//initialization
	tempA.PID=pidtrack;
	tempA.filename = "";
	tempA.startloc = 0;
	tempA.rorw = ' ';
	tempA.filesize = 0;
	tempA.cylnum=0;
	tempA.taonext=tao;
	tempA.tprev=0;
	tempA.burstnum=0;
	tempA.tave=0;
	tempA.ttotal=0;
	tempA.lotime=tao;
	pidtrack++;
	prolist.push_back(&tempA);
}
void sortReadQ(PCB& temp)
{
	//if ready queue is empty pcb is head & tail
	if(readq.head==NULL)
	{
		readq.head=&temp;
		readq.tail=&temp;  //change tail pointer
	}
	if(readq.head!=NULL){
	PCB* comtemp=readq.head;  //used for loop comparison
	bool inserted=false;
	while(comtemp!=NULL)
	{
		
		//in loop when burst < current burst, insert
		if(temp.lotime<comtemp->lotime)
		{
			inserted=true;
			temp.prev=comtemp->prev;
			temp.next=comtemp;
			comtemp->prev=&temp;  //insertion completed
			
			if(temp.prev==NULL)  //if temp became the queue head
				readq.head=&temp;
			else  //otherwise set the next pointer of prev PCB
				(temp.prev)->next=&temp;
			break;
		}
		
		comtemp=comtemp->next;  //iteration
	}
	if(inserted==false)  //if reached the end
		{
			temp.prev=readq.tail;
			readq.tail->next=&temp;
			readq.tail=&temp;
			temp.next=NULL;
		}
	CPU=readq.head;  //after sort make sure cpu is head
	}
}

void sortJobPool(PCB & temp)  //add to jobpool's right pos
{
	//if job pool is empty then pcb is head & tail
	if(jobpool.head==NULL)
	{
		jobpool.head=&temp;
		jobpool.tail=&temp;  //tail pointer too
	}
	else
	{
		PCB* comtemp=jobpool.head;  //used for loop comparison
		bool inserted=false;
		while(comtemp!=NULL)
		{
			//in loop when bigger than size, insert
			if(temp.psize>comtemp->psize)
			{
				inserted=true;
				temp.prev=comtemp->prev;
				temp.next=comtemp;
				comtemp->prev=&temp;  //insertion completed
				if(temp.prev==NULL)  //if new head for jobpool
					jobpool.head=&temp;
				else  //otherwise set the prev pcb's next pointer
					(temp.prev)->next=&temp;
				break;
			}
			comtemp=comtemp->next;  //iteration
		}
		if(inserted==false)  //if reached the end
		{
			temp.prev=jobpool.tail;
			jobpool.tail->next=&temp;
			jobpool.tail=&temp;
			temp.next=NULL;
		}
	}
}
//function for arrival of new pcb and deciding JobPool or not
void ArrivalnJobPool(framelist& fltem){
	unsigned long tempsize;
	cout<<"Size of the new process:";
	cin>>tempsize;
	if(tempsize>Maxsize){
		cout<<"Error! Size of the new process is beyond the maximum size."<<endl;
	}
	else {
		PCB *temp=new PCB();
		Arrival(*temp);
		temp->psize=tempsize;  //add to new pcb's psize
		double temp1=((double) tempsize)/((double) Pagesize);
		temp->pageNum=(int)ceil(temp1);
		temp->pagetable=new int[temp->pageNum];  //set up page table
		bool getff=fltem.getFrame(temp->pageNum, *temp);
		if(getff==true){    //add new pcb to system/readq
			int cpusta0=checkcpu();
			sortReadQ(*CPU);
			sortReadQ(*temp);  //insert the newcomer
		}
		else {
			sortJobPool(*temp);
		}
	}
}

void PDFinterrupt(queue &qtemp){
	if(qtemp.head==NULL){
		cout<<"No process in this device queue."<<endl;
	}
	if(qtemp.head!=NULL){
		int cpusta1=checkcpu();
		sortReadQ(*CPU);
		PCB *qheadtemp=qtemp.head->next;
		sortReadQ(*qtemp.head);
		qtemp.head=qheadtemp;
		if(qtemp.head!=NULL){
			qtemp.head->prev=NULL;
		}
		if(qtemp.head==NULL){
			qtemp.tail=qtemp.head;
		}
	}
}
void PrintJP(){  //job pool printing func
	PCB* iter=jobpool.head;
	cout<<"job pool list:"<<endl;
	while(iter!=NULL){
		cout<<iter->PID<<" ";
		iter=iter->next;
	}
	cout<<endl;
}
PCB* removePCB(PCB& temp, queue& qtem){
	if(temp.prev==NULL){  //head of the queue
		qtem.head=temp.next;
		if(temp.next==NULL)
			qtem.tail=NULL;
		else
			temp.next->prev=NULL;
		temp.next=NULL;
	}
	else if(temp.next==NULL){  //endl of queue
		qtem.tail=temp.prev;
		if(temp.prev==NULL)
			qtem.head=NULL;
		else
			temp.prev->next=NULL;
		temp.prev=NULL;
	}
	else {
		temp.prev->next=temp.next;
		temp.next->prev=temp.prev;
		temp.prev=NULL;
		temp.next=NULL;
	}
	return &temp;
}
PCB* CheckJobPool(framelist& fltem){
	PCB* comtemp=jobpool.head;  //used for loop comparison
	PCB* temp1=NULL;  //used to store the one to return
	unsigned long asize=fltem.ffnum*Pagesize;  //used for comparing
	bool getted=false;
	if(jobpool.head==NULL)
		return temp1;
	while(comtemp!=NULL)
	{
		if(comtemp->psize<=asize)
		{
			temp1=removePCB(*comtemp, jobpool);
			getted=true;
			break;   //single extraction from the job pool
		}
		comtemp=comtemp->next;
	}
	return temp1;  //if cant find, return NULL
	
}
PCB* findProc(int pidtem, int pnum, int dnum, int cnum){
	PCB* iter;  //for iteration
	PCB* target=NULL;  //for storing target PCB
	bool found=false;  //found status
	iter=readq.head;
	while(iter!=NULL){
		if(pidtem==iter->PID){
			target=removePCB(*iter, readq);
			found=true;
			break;
		}
		iter=iter->next;
	}
	if(found==true) return target;
	for(int i=0; i<pnum; i++){
		iter=printer[i].head;
		while(iter!=NULL){
			if(pidtem==iter->PID){
				target=removePCB(*iter, printer[i]);
				found=true;
				break;
			}
			iter=iter->next;
		}
		if(found==true) break;
	}
	if(found==true) return target;
	for(int i=0; i<dnum; i++){
		iter=disk[i].head;
		while(iter!=NULL){
			if(pidtem==iter->PID){
				target=removePCB(*iter, disk[i]);
				found=true;
				break;
			}
			iter=iter->next;
		}
		if(found==true) break;
	}
	if(found==true) return target;
	for(int i=0; i<cnum; i++){
		iter=flash[i].head;
		while(iter!=NULL){
			if(pidtem==iter->PID){
				target=removePCB(*iter, flash[i]);
				found=true;
				break;
			}
			iter=iter->next;
		}
		if(found==true) break;
	}
	return target;
}



#endif