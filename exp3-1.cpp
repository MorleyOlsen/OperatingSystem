#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
using namespace std;

//pcb的结构体设置 
typedef struct{
	string name;		//进程名字 
	int flag;			//是否完成，状态检测（1是完成，0是未完成） 
	int number;			//就绪队列中的顺序
	int arrivetime;		//到达时间 
	int runtime;		//服务时间 
	int completetime;	//完成时间 
	int remaintime;		//剩余时间 
}PCB;

//进程指标 （全局） 
PCB p[100];				//可输入的进程数量 
int timelen=1;			//时间片长度 
int total;				//进程总数（由main中的用户输入） 

//就绪队列指标 （全局） 
int current=0;			//当前应该执行的进程编号
int head=0;				//就绪队列中的队首

//进程初始化 
void initialize(){
	for(int i=0;i<total;i++){
		cout<<"请输入第"<<i+1<<"个进程的名字、到达时间、服务时间：";
		cin>>p[i].name>>p[i].arrivetime>>p[i].runtime;
		
		//其他属性的初始化 
		p[i].remaintime=p[i].runtime;	//初始剩余时间 = 服务时间 
		p[i].flag=0;					//完成状态 = 未完成 
		p[i].number=0;					//就绪队列中的顺序 = 0
	}
	//打印 各进程控制块的初始状态
	for(int i=0;i<total;i++){
		cout<<"进程名字："<<p[i].name;
		cout<<"\t"<<"进程状态："<<p[i].flag;
		cout<<"\t"<<"就绪顺序："<<p[i].number;
		cout<<"\t"<<"到达时间："<<p[i].arrivetime;
		cout<<"\t"<<"服务时间："<<p[i].runtime;
		cout<<"\t"<<"剩余时间："<<p[i].remaintime;
		cout<<endl; 
	}
}

//按照到达时间排序进程（冒泡排序）
void sortarrive(){
	for(int i=0;i<total-1;i++){
		for(int j=0;j<total-i-1;j++){
			//到达时间从小到大排序 
			if(p[j].arrivetime>p[j+1].arrivetime){
				//如果前一个的时间大于后一个，则交换两个进程的顺序 
				PCB t=p[j+1];
				p[j+1]=p[j];
				p[j]=t;
			}
		}
	}
}

//时间片轮转
void timeprocess(){
	int time=p[0].arrivetime;			//系统开始的时间是最早就绪进程的到达时间
	int complete=0;						//已经完成的进程个数
	int n=0;							//记录while循环次数的变量 
	
	//所有进程执行的过程 
	while(complete<total){
		for(int i=0;i<total;i++){
			//当前这个进程和cur相同，且状态为未完成 
			if(p[i].number==current && p[i].flag==0){
				//print current cycle
				cout<<endl<<"当前的时钟周期是：T"<<current<<endl;
				
				//process ready list 
				string ready="";
				int list[100];
				int waitnumber[100];
				int cnt=0;
				for(int k=0;k<total;k++){
					//大于当前current的，都是在就绪队列里面的进程 
					if(p[k].number>current && p[k].flag==0){
						list[cnt]=k;
						waitnumber[cnt]=p[k].number;
						cnt++;
					}
				}
				cout<<"就绪队列里面一共有："<<cnt<<"个进程，";
				
				//按照waitnumber排序 （冒泡排序） 
				for (int k = 0; k < cnt - 1; k++) {
				    for (int kk = 0; kk < cnt - 1 - k; kk++) {
				        if (waitnumber[kk] > waitnumber[kk + 1]) {
				            int tempfig = waitnumber[kk + 1];
				            waitnumber[kk + 1] = waitnumber[kk];
				            waitnumber[kk] = tempfig;
				            int templist = list[kk + 1];
				            list[kk + 1] = list[kk];
				            list[kk] = templist;
				        }
				    }
				}//不知道为啥死循环了。。调好了 
				
				
				//按照waitnumber的大小进行push 
				for(int k=0;k<cnt;k++){
					int id=list[k];
					ready+=p[id].name;
					ready+=" ";
				}
				//print ready
				if(ready==""){
					cout<<"就绪队列为空"<<endl;
				}
				else{
					cout<<"当前的就绪队列为："<<ready<<endl;
				}
				
				//时间片工作 
				cout<<"判断时间片ing，当前应该执行进程"<<p[i].name<<endl;
				//current进程能在当前时间片运行完
				if(p[i].remaintime<=timelen){
					time+=p[i].remaintime;		//系统时间，增加该进程的剩余时间 
					p[i].flag=1;				//状态变为完成 
					p[i].completetime=time;		//此进程的完成时间为当前系统时间 
					p[i].remaintime=0;			//剩余时间，设置为0
					 
					complete++;		//系统中完成的进程数+=1 
					current++;		//cur+=1 
					cout<<"已经执行完毕的进程是："<<p[i].name<<endl;
					
					//检查是否有新的到达进程
					for(int j=i+1;j<total;j++){
						//有新的到达进程 
						if(p[j].arrivetime<=time && p[j].number==0){
							head++;				//就绪队列个数+=1 
							p[j].number=head; 	//这个进程的就绪编号为head
						}
					}
				}
				//current进程不能在当前时间片运行完 
				else{
					time+=timelen;
					p[i].remaintime-=timelen;
					current++;
					
					//检查是否有新的到达进程 
					for(int j=i+1;j<total;j++){
						if(p[j].arrivetime<=time && p[j].number==0){
							head++;
							p[j].number=head;
						}
					}
					
					//重新给这个进程加入就绪队列 
					head++;				//刚刚执行的这个进程继续进入就绪队列 
					p[i].number=head;	//更改就绪编号
				}
				
			}
		}
		n++;	//检测while循环的次数 
	}
	cout<<"执行完毕，总共花费时钟周期：T"<<current<<endl;
} 

void show(){
	double x=0;		//总周转时间 		周转时间 = 完成时间 - 到达时间
	double y=0;		//总带权周转时间	带权周转时间 = 周转时间 / 服务时间
	
	//输出每个进程的基本信息 
	for(int i=0;i<total;i++){
		double px=(double)p[i].completetime-p[i].arrivetime;	//当前进程的周转时间 
		double py=px/(double)p[i].runtime;						//当前进程的带权周转时间
		cout<<"进程名字："<<p[i].name;
		cout<<"\t"<<"周转时间："<<px;
		cout<<"\t"<<"带权周转时间："<<py;
		cout<<"\t"<<"到达时间："<<p[i].arrivetime;
		cout<<"\t"<<"服务时间："<<p[i].runtime;
		cout<<"\t"<<"完成时间："<<p[i].completetime;
		cout<<endl; 
		x+=px;
		y+=py;
	} 
	double ret1=x/total;	//平均周转时间
	double ret2=y/total;	//平均带权周转时间
	cout<<"平均周转时间："<<ret1<<endl;
	cout<<"平均带权周转时间："<<ret2<<endl;
}

/*
//RR 
测试用例1：
5
p1 0 3
p2 2 6
p3 4 4
p4 6 5
p5 8 2	

*/
int main(){
	cout<<"请输入进程总数：";
	cin>>total;			//用户输入进程的总数 
	initialize();		//初始化
	sortarrive();		//到达时间处理 
	timeprocess();		//时间片处理 
	cout<<endl;
	show();				//输出最终的结果
	return 0;
}
