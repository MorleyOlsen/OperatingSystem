#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <vector>
using namespace std;

//pcb�Ľṹ������ 
typedef struct{
	string name;		//�������� 
	int flag;			//�Ƿ���ɣ�״̬��⣨1����ɣ�0��δ��ɣ�
	int arrivetime;		//����ʱ�� 
	int runtime;		//����ʱ�� 
	int completetime;	//���ʱ��
	int remaintime;		//ʣ��ʱ�� 
	int prior;			//���ȼ�
	int lasttime;		//��һ�����е�ʱ�� 
}PCB;

//����ָ�� ��ȫ�֣� 
PCB p[100];				//������Ľ������� 
int timelen=1;			//ʱ��Ƭ���� 
int total;				//������������main�е��û����룩

//���̳�ʼ�� 
void initialize(){
	for(int i=0;i<total;i++){
		cout<<"�������"<<i+1<<"�����̵����֡�����ʱ�䡢����ʱ�䡢���ȼ���";
		cin>>p[i].name>>p[i].arrivetime>>p[i].runtime>>p[i].prior;
		
		//�������Եĳ�ʼ�� 
		p[i].remaintime=p[i].runtime;	//��ʼʣ��ʱ�� = ����ʱ�� 
		p[i].flag=0;					//���״̬ = δ���
		p[i].lasttime=p[i].arrivetime;	//��һ�����е�ʱ�� 
	}
	//��ӡ �����̿��ƿ�ĳ�ʼ״̬
	for(int i=0;i<total;i++){
		cout<<"�������֣�"<<p[i].name;
		cout<<"\t"<<"����״̬��"<<p[i].flag;
		cout<<"\t"<<"����ʱ�䣺"<<p[i].arrivetime;
		cout<<"\t"<<"����ʱ�䣺"<<p[i].runtime;
		cout<<"\t"<<"ʣ��ʱ�䣺"<<p[i].remaintime;
		cout<<"\t"<<"���ȼ���"<<p[i].prior;
		cout<<"\t"<<"��һ�α�Ϊ����̬��ʱ�䣺"<<p[i].lasttime;
		cout<<endl; 
	}
}

//��ʼ���������� 
void sortlist(vector<int> &wait,int cycle){
	//�����Ѿ�������δ��ɵĽ��� 
	for(int i=0;i<total;i++){
		if(p[i].arrivetime<=cycle && p[i].flag==0){
			wait.push_back(i);
		}
	}
	
	int n=wait.size();
	//����prior���֣�ѡ������ 
	for(int i=0;i<n-1;i++){
		int id=i;	//max to process
		for(int j=i+1;j<n;j++){
			int curprocess=wait[j];		//�������ĺ���Ľ������ 
			int maxprocess=wait[id];	//��ǰ���ȼ����Ľ������ 
			if(p[maxprocess].prior<p[curprocess].prior){
				//���������̵�prior������������ȼ����Ľ������ 
				id=j;
			}
		}
		if(id!=i){
			//����и��£��򽻻�����pcb��˳�� 
			int t=wait[id];
			wait[id]=wait[i];
			wait[i]=t;
		}
	}
	
	//��ͬprior����lasttime����
	int i=0;	//�Ӷ��׿�ʼ���� 
	while(i<n-1){
		int pos=i;	//��¼�뵱ǰ��Զ���Һ͵�ǰ�������ȼ���ͬ�Ľ��� 
		int nowid=wait[i];
		for(int j=1+i;j<n;j++){
			int processid=wait[j];
			if(p[processid].prior==p[nowid].prior){
				pos=j;	//����Ľ��̺����ڵĽ������ȼ���ͬ��������Զ��¼ 
			}
		}
		if(pos!=i){
			//sort between wait[i] and wait[pos] according to lasttime��ѡ������ 
			for(int a=i;a<pos;a++){
				int aid=a;
				for(int b=a+1;b<=pos;b++){
					int cur=wait[b];
					int min=wait[aid];
					if(p[min].lasttime>p[cur].lasttime){
						//��������lasttime��С
						aid=b;
					}
				}
				if(aid!=a){
					//����и��£��򽻻�����pcb��˳��
					int tt=wait[aid];
					wait[aid]=wait[a];
					wait[a]=tt;
				}
			}
		}
		i=pos+1;	//�ƶ�����һ����û���жϹ����ȼ��Ƿ���ȵ�pcb�� 
	} 
}

void psa(){
	//ϵͳ��ʼʱ�� ����ʼ���� 
	int starttime=p[0].arrivetime;
	for(int i=1;i<total;i++){
		if(starttime>p[i].arrivetime){
			starttime=p[i].arrivetime;	//���½������絽���ʱ�� 
		}
	}
	
	int cycle=0;		//ִ�е������� 
	int complete=0;		//��ɵĽ����� 
	
	//ִ�����н��� 
	while(complete<total){
		//wait list
		vector<int> wait;
		sortlist(wait,cycle);
		cout<<endl<<"��ִ�и�ʱ��Ƭǰ��T"<<cycle<<"�ľ�������Ϊ��"; 
		for(int i=0;i<wait.size();i++){
			int proid=wait[i];
			cout<<p[proid].name<<" ";
		}
		cout<<endl;
		
		//ִ��wait[0]��λ�ھ������ж��׵�ʱ��Ƭ��
		int curpro=wait[0];
		cout<<"�ж�ʱ��Ƭing����ǰӦ��ִ�н��̣�"<<p[curpro].name<<endl;
		
		//�жϵ�ǰʱ��Ƭ�ڣ�curpro�Ƿ���ִ�����
		if(p[curpro].remaintime<=timelen){	
			//can
			p[curpro].flag=1;							//�޸Ľ���״̬����flag==1֮�󣬾Ͳ���Ҫ��prior�� 
			p[curpro].completetime=starttime+cycle+1;	//�޸����ʱ�� 
			p[curpro].remaintime=0;						//�޸�ʣ��ʱ�� 
			complete++;									//��ɵĽ����� += 1 
			cout<<"�����ʱ��Ƭ�ڣ��Ѿ�ִ������̣�"<<p[curpro].name<<endl;
		}
		else{	
			//cannot
			p[curpro].remaintime-=timelen;	//�޸�ʣ��ʱ�� 
			p[curpro].prior--;				//�޸����ȼ�
			p[curpro].lasttime=cycle+1;		//�޸��ϴ�ʹ��ʱ�� 
		}
		
		cycle++;	//ʱ������ += 1 
		//complete++;	//temporary break while to debug
	}
	cout<<endl<<"ִ����ϣ��ܹ�����ʱ�����ڣ�T"<<cycle<<endl;
}

void show(){
	double x=0;		//����תʱ�� 		��תʱ�� = ���ʱ�� - ����ʱ��
	double y=0;		//�ܴ�Ȩ��תʱ��	��Ȩ��תʱ�� = ��תʱ�� / ����ʱ��
	
	//���ÿ�����̵Ļ�����Ϣ 
	for(int i=0;i<total;i++){
		double px=(double)p[i].completetime-p[i].arrivetime;	//��ǰ���̵���תʱ�� 
		double py=px/(double)p[i].runtime;						//��ǰ���̵Ĵ�Ȩ��תʱ��
		cout<<"�������֣�"<<p[i].name;
		cout<<"\t"<<"��תʱ�䣺"<<px;
		cout<<"\t"<<"��Ȩ��תʱ�䣺"<<py;
		cout<<endl; 
		cout<<"\t"<<"����ʱ�䣺"<<p[i].arrivetime;
		cout<<"\t"<<"����ʱ�䣺"<<p[i].runtime;
		cout<<"\t"<<"���ʱ�䣺"<<p[i].completetime;
		cout<<endl; 
		x+=px;
		y+=py;
	} 
	double ret1=x/total;	//ƽ����תʱ��
	double ret2=y/total;	//ƽ����Ȩ��תʱ��
	cout<<"ƽ����תʱ�䣺"<<ret1<<endl;
	cout<<"ƽ����Ȩ��תʱ�䣺"<<ret2<<endl;
}

/*
//���ȼ� 
��������2��
4
p1 0 2 2
p2 0 2 3
p3 0 4 1
p4 0 3 4
����ִ�е�˳��Ӧ���ǣ�p4 p2 p4 p1 p2 p4 p3 p1 p3 p3 p3
                      0  1  2  3  4  5  6  7  8  9  10
7.5
2.8125
*/
int main(){
	cout<<"���������������";
	cin>>total;			//�û�������̵����� 
	initialize();		//��ʼ��
	psa();				//psa process
	cout<<endl;
	show();				//������յĽ��
	return 0;
}
