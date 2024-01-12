#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <vector>
using namespace std;

//pcb的结构体设置 
typedef struct{
	string name;		//进程名字 
	int flag;			//是否完成，状态检测（1是完成，0是未完成）
	int arrivetime;		//到达时间 
	int runtime;		//服务时间 
	int completetime;	//完成时间
	int remaintime;		//剩余时间 
	int prior;			//优先级
	int lasttime;		//上一次运行的时间 
}PCB;

//进程指标 （全局） 
PCB p[100];				//可输入的进程数量 
int timelen=1;			//时间片长度 
int total;				//进程总数（由main中的用户输入）

//进程初始化 
void initialize(){
	for(int i=0;i<total;i++){
		cout<<"请输入第"<<i+1<<"个进程的名字、到达时间、服务时间、优先级：";
		cin>>p[i].name>>p[i].arrivetime>>p[i].runtime>>p[i].prior;
		
		//其他属性的初始化 
		p[i].remaintime=p[i].runtime;	//初始剩余时间 = 服务时间 
		p[i].flag=0;					//完成状态 = 未完成
		p[i].lasttime=p[i].arrivetime;	//上一次运行的时间 
	}
	//打印 各进程控制块的初始状态
	for(int i=0;i<total;i++){
		cout<<"进程名字："<<p[i].name;
		cout<<"\t"<<"进程状态："<<p[i].flag;
		cout<<"\t"<<"到达时间："<<p[i].arrivetime;
		cout<<"\t"<<"服务时间："<<p[i].runtime;
		cout<<"\t"<<"剩余时间："<<p[i].remaintime;
		cout<<"\t"<<"优先级："<<p[i].prior;
		cout<<"\t"<<"上一次变为就绪态的时间："<<p[i].lasttime;
		cout<<endl; 
	}
}

//初始化就绪队列 
void sortlist(vector<int> &wait,int cycle){
	//遍历已经到来且未完成的进程 
	for(int i=0;i<total;i++){
		if(p[i].arrivetime<=cycle && p[i].flag==0){
			wait.push_back(i);
		}
	}
	
	int n=wait.size();
	//按照prior划分（选择排序） 
	for(int i=0;i<n-1;i++){
		int id=i;	//max to process
		for(int j=i+1;j<n;j++){
			int curprocess=wait[j];		//遍历到的后面的进程序号 
			int maxprocess=wait[id];	//当前优先级最大的进程序号 
			if(p[maxprocess].prior<p[curprocess].prior){
				//如果后面进程的prior更大，则更新优先级最大的进程序号 
				id=j;
			}
		}
		if(id!=i){
			//如果有更新，则交换两个pcb的顺序 
			int t=wait[id];
			wait[id]=wait[i];
			wait[i]=t;
		}
	}
	
	//相同prior按照lasttime划分
	int i=0;	//从队首开始分析 
	while(i<n-1){
		int pos=i;	//记录离当前最远，且和当前进程优先级相同的进程 
		int nowid=wait[i];
		for(int j=1+i;j<n;j++){
			int processid=wait[j];
			if(p[processid].prior==p[nowid].prior){
				pos=j;	//后面的进程和现在的进程优先级相同，更新最远记录 
			}
		}
		if(pos!=i){
			//sort between wait[i] and wait[pos] according to lasttime（选择排序） 
			for(int a=i;a<pos;a++){
				int aid=a;
				for(int b=a+1;b<=pos;b++){
					int cur=wait[b];
					int min=wait[aid];
					if(p[min].lasttime>p[cur].lasttime){
						//如果后面的lasttime更小
						aid=b;
					}
				}
				if(aid!=a){
					//如果有更新，则交换两个pcb的顺序
					int tt=wait[aid];
					wait[aid]=wait[a];
					wait[a]=tt;
				}
			}
		}
		i=pos+1;	//移动到下一个还没有判断过优先级是否相等的pcb块 
	} 
}

void psa(){
	//系统开始时间 （初始化） 
	int starttime=p[0].arrivetime;
	for(int i=1;i<total;i++){
		if(starttime>p[i].arrivetime){
			starttime=p[i].arrivetime;	//更新进程最早到达的时间 
		}
	}
	
	int cycle=0;		//执行的周期数 
	int complete=0;		//完成的进程数 
	
	//执行所有进程 
	while(complete<total){
		//wait list
		vector<int> wait;
		sortlist(wait,cycle);
		cout<<endl<<"在执行该时间片前，T"<<cycle<<"的就绪队列为："; 
		for(int i=0;i<wait.size();i++){
			int proid=wait[i];
			cout<<p[proid].name<<" ";
		}
		cout<<endl;
		
		//执行wait[0]（位于就绪队列队首的时间片）
		int curpro=wait[0];
		cout<<"判断时间片ing，当前应该执行进程："<<p[curpro].name<<endl;
		
		//判断当前时间片内，curpro是否能执行完毕
		if(p[curpro].remaintime<=timelen){	
			//can
			p[curpro].flag=1;							//修改进程状态，且flag==1之后，就不需要管prior了 
			p[curpro].completetime=starttime+cycle+1;	//修改完成时间 
			p[curpro].remaintime=0;						//修改剩余时间 
			complete++;									//完成的进程数 += 1 
			cout<<"在这个时间片内，已经执行完进程："<<p[curpro].name<<endl;
		}
		else{	
			//cannot
			p[curpro].remaintime-=timelen;	//修改剩余时间 
			p[curpro].prior--;				//修改优先级
			p[curpro].lasttime=cycle+1;		//修改上次使用时间 
		}
		
		cycle++;	//时间周期 += 1 
		//complete++;	//temporary break while to debug
	}
	cout<<endl<<"执行完毕，总共花费时钟周期：T"<<cycle<<endl;
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
		cout<<endl; 
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
//优先级 
测试用例2：
4
p1 0 2 2
p2 0 2 3
p3 0 4 1
p4 0 3 4
进程执行的顺序应该是：p4 p2 p4 p1 p2 p4 p3 p1 p3 p3 p3
                      0  1  2  3  4  5  6  7  8  9  10
7.5
2.8125
*/
int main(){
	cout<<"请输入进程总数：";
	cin>>total;			//用户输入进程的总数 
	initialize();		//初始化
	psa();				//psa process
	cout<<endl;
	show();				//输出最终的结果
	return 0;
}
