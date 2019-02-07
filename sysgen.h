/*******************************************************************************
  Title          : sysgen
  Author         : Wei Lian
  Created on     : September 27, 2018
  Description    : Implementation to the system gen class with interface
  Purpose        : 
  Usage          : 
  Build with     : 
  Modifications  : 

*******************************************************************************/
#ifndef SYSGEN_H
#define SYSGEN_H

#include <iostream>
#include <string>
using namespace std;

class sysGen{
	public:
		void create_system(int num_printer,int num_disk_drive,int num_flash_drive){
			printer=num_printer;
			disk_drive=num_disk_drive;
			flash_drive=num_flash_drive;
			


		}
		void display_system(){
			cout<<"System info: "<<endl;
			cout<<"The number of printers: "<<printer<<endl;
			cout<<"The number of disk drive: "<<disk_drive<<endl;
			for(int i=0;i<disk_drive;i++){
			cout<<"the number of cylinders for Disk"<<i+1<<":";
			cout<<dtrack[i]<<endl;
			}
			cout<<"The number of flash drive: "<<flash_drive<<endl;
			cout<<"The number of history parameter alpha: "<<alpha<<endl;
			cout<<"The number of initial burst estiamate: "<<tao<<endl;




		}
		int get_flash_drive(){
			return flash_drive;

		}
		bool isPowerOfTwo(unsigned long x){
			return ((x!=0)&&((x&(~x+1))==x));
		}
		void input_system(){
			cout<<"Enter the number of printers"<<endl;
			cin>>printer;
			cout<<"Enter the number of disk_drive"<<endl;
			cin>>disk_drive;
			int *dtracktemp=new int[disk_drive];
			curtrackhead=new int[disk_drive];
			for(int i=0;i<disk_drive;i++){
				cout<<"Enter the number of cylinders for Disk"<<i+1<<":";
				cin>>dtracktemp[i];
				curtrackhead[i]=0;
			}
			dtrack=dtracktemp;
			cout<<"Enter the number of flash_drive"<<endl;
			cin>>flash_drive;
			cout<<"Enter the history parameter alpha 0<=alpha<=1: "<<endl;
			cin>>alpha;
			while(alpha<0||alpha>1)
			{
				cout<<"Please enter valid parameter for 0<=alpha<=1: "<<endl;
				cin>>alpha;
			}
			cout<<"Enter the initial burst estimate: "<<endl;
			cin>>tao;
			while(true){
				cout<<"Enter the total memory size: "<<endl;
				cin>>Memsize;
				cout<<"Enter the size of a page: "<<endl;
				cin>>Pagesize;
				while(!isPowerOfTwo(Pagesize))
				{
					cout<<"Please enter page sizes as power of two: "<<endl;
					cin>>Pagesize;
				}
				if(Memsize%Pagesize==0)
					break;
				else 
					cout<<"Error, page size should divide up memory size."<<endl;
			}
			cout<<"Enter the maximum size of a process: "<<endl;
			cin>>Maxsize;
			while(Maxsize>Memsize)
			{
				cout<<"Error, max process size must be less than Memory size!"<<endl;
				cin>>Maxsize;
			}
			framenum=Memsize/Pagesize;//initialization of framenum
		}
		int get_printer(){
			return printer;
		}
		int get_disk_drive(){
			return disk_drive;
		}
		int *get_dtrack(){
			return dtrack;
		}
		int *get_curtrackhead(){
			return curtrackhead;
		}
		double get_alpha(){
			return alpha;
		}
		double get_tao(){
			return tao;
		}
		unsigned long get_Memsize(){
			return Memsize;
		}
		unsigned long get_Pagesize(){
			return Pagesize;
		}
		unsigned long get_Maxsize(){
			return Maxsize;
		}
		int get_framenum(){
			return framenum;
		}

		private:
		int printer;
		int disk_drive;
		int flash_drive;
		double alpha;//history parameter
		double tao;//initial burst estimate
		int* dtrack;//aaray keep track numbers of each disk
		int* curtrackhead;//keep track of current track head
		unsigned long Memsize;  //totoal memory size
		unsigned long Pagesize;  //the size of a page
		unsigned long Maxsize;  //Maximum size of a process
		int framenum;  //number of frames in system

};





#endif /*SYSGEN_H*/
