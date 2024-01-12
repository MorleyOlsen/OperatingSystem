#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
using namespace std;

//pcb�Ľṹ������ 
typedef struct{
	string name;		//�������� 
	int flag;			//�Ƿ���ɣ�״̬��⣨1����ɣ�0��δ��ɣ� 
	int number;			//���������е�˳��
	int arrivetime;		//����ʱ�� 
	int runtime;		//����ʱ�� 
	int completetime;	//���ʱ�� 
	int remaintime;		//ʣ��ʱ�� 
}PCB;

//����ָ�� ��ȫ�֣� 
PCB p[100];				//������Ľ������� 
int timelen=1;			//ʱ��Ƭ���� 
int total;				//������������main�е��û����룩 

//��������ָ�� ��ȫ�֣� 
int current=0;			//��ǰӦ��ִ�еĽ��̱��
int head=0;				//���������еĶ���

//���̳�ʼ�� 
void initialize(){
	for(int i=0;i<total;i++){
		cout<<"�������"<<i+1<<"�����̵����֡�����ʱ�䡢����ʱ�䣺";
		cin>>p[i].name>>p[i].arrivetime>>p[i].runtime;
		
		//�������Եĳ�ʼ�� 
		p[i].remaintime=p[i].runtime;	//��ʼʣ��ʱ�� = ����ʱ�� 
		p[i].flag=0;					//���״̬ = δ��� 
		p[i].number=0;					//���������е�˳�� = 0
	}
	//��ӡ �����̿��ƿ�ĳ�ʼ״̬
	for(int i=0;i<total;i++){
		cout<<"�������֣�"<<p[i].name;
		cout<<"\t"<<"����״̬��"<<p[i].flag;
		cout<<"\t"<<"����˳��"<<p[i].number;
		cout<<"\t"<<"����ʱ�䣺"<<p[i].arrivetime;
		cout<<"\t"<<"����ʱ�䣺"<<p[i].runtime;
		cout<<"\t"<<"ʣ��ʱ�䣺"<<p[i].remaintime;
		cout<<endl; 
	}
}

//���յ���ʱ��������̣�ð������
void sortarrive(){
	for(int i=0;i<total-1;i++){
		for(int j=0;j<total-i-1;j++){
			//����ʱ���С�������� 
			if(p[j].arrivetime>p[j+1].arrivetime){
				//���ǰһ����ʱ����ں�һ�����򽻻��������̵�˳�� 
				PCB t=p[j+1];
				p[j+1]=p[j];
				p[j]=t;
			}
		}
	}
}

//ʱ��Ƭ��ת
void timeprocess(){
	int time=p[0].arrivetime;			//ϵͳ��ʼ��ʱ��������������̵ĵ���ʱ��
	int complete=0;						//�Ѿ���ɵĽ��̸���
	int n=0;							//��¼whileѭ�������ı��� 
	
	//���н���ִ�еĹ��� 
	while(complete<total){
		for(int i=0;i<total;i++){
			//��ǰ������̺�cur��ͬ����״̬Ϊδ��� 
			if(p[i].number==current && p[i].flag==0){
				//print current cycle
				cout<<endl<<"��ǰ��ʱ�������ǣ�T"<<current<<endl;
				
				//process ready list 
				string ready="";
				int list[100];
				int waitnumber[100];
				int cnt=0;
				for(int k=0;k<total;k++){
					//���ڵ�ǰcurrent�ģ������ھ�����������Ľ��� 
					if(p[k].number>current && p[k].flag==0){
						list[cnt]=k;
						waitnumber[cnt]=p[k].number;
						cnt++;
					}
				}
				cout<<"������������һ���У�"<<cnt<<"�����̣�";
				
				//����waitnumber���� ��ð������ 
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
				}//��֪��Ϊɶ��ѭ���ˡ��������� 
				
				
				//����waitnumber�Ĵ�С����push 
				for(int k=0;k<cnt;k++){
					int id=list[k];
					ready+=p[id].name;
					ready+=" ";
				}
				//print ready
				if(ready==""){
					cout<<"��������Ϊ��"<<endl;
				}
				else{
					cout<<"��ǰ�ľ�������Ϊ��"<<ready<<endl;
				}
				
				//ʱ��Ƭ���� 
				cout<<"�ж�ʱ��Ƭing����ǰӦ��ִ�н���"<<p[i].name<<endl;
				//current�������ڵ�ǰʱ��Ƭ������
				if(p[i].remaintime<=timelen){
					time+=p[i].remaintime;		//ϵͳʱ�䣬���Ӹý��̵�ʣ��ʱ�� 
					p[i].flag=1;				//״̬��Ϊ��� 
					p[i].completetime=time;		//�˽��̵����ʱ��Ϊ��ǰϵͳʱ�� 
					p[i].remaintime=0;			//ʣ��ʱ�䣬����Ϊ0
					 
					complete++;		//ϵͳ����ɵĽ�����+=1 
					current++;		//cur+=1 
					cout<<"�Ѿ�ִ����ϵĽ����ǣ�"<<p[i].name<<endl;
					
					//����Ƿ����µĵ������
					for(int j=i+1;j<total;j++){
						//���µĵ������ 
						if(p[j].arrivetime<=time && p[j].number==0){
							head++;				//�������и���+=1 
							p[j].number=head; 	//������̵ľ������Ϊhead
						}
					}
				}
				//current���̲����ڵ�ǰʱ��Ƭ������ 
				else{
					time+=timelen;
					p[i].remaintime-=timelen;
					current++;
					
					//����Ƿ����µĵ������ 
					for(int j=i+1;j<total;j++){
						if(p[j].arrivetime<=time && p[j].number==0){
							head++;
							p[j].number=head;
						}
					}
					
					//���¸�������̼���������� 
					head++;				//�ո�ִ�е�������̼�������������� 
					p[i].number=head;	//���ľ������
				}
				
			}
		}
		n++;	//���whileѭ���Ĵ��� 
	}
	cout<<"ִ����ϣ��ܹ�����ʱ�����ڣ�T"<<current<<endl;
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
//RR 
��������1��
5
p1 0 3
p2 2 6
p3 4 4
p4 6 5
p5 8 2	

*/
int main(){
	cout<<"���������������";
	cin>>total;			//�û�������̵����� 
	initialize();		//��ʼ��
	sortarrive();		//����ʱ�䴦�� 
	timeprocess();		//ʱ��Ƭ���� 
	cout<<endl;
	show();				//������յĽ��
	return 0;
}
