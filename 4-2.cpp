#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int mem;	// �洢���ռ��С 

struct MemoryBlock {
    int startAddress;
    int length;
    int pid; // ���ڿ�������pid Ϊ 0
    
    // ����Ƚϲ�����
    bool operator==(const MemoryBlock& other) const {
        return startAddress == other.startAddress && length == other.length && pid == other.pid;
    }
};

vector<MemoryBlock> allocated;  // �ѷ������б�
vector<MemoryBlock> available;  // �������б�

// ������������ʼ��ַ����
bool compareByStartAddress(const MemoryBlock& a, const MemoryBlock& b) {
    return a.startAddress < b.startAddress;
}

// ������������������
bool compareByLength(const MemoryBlock& a, const MemoryBlock& b) {
    return a.length < b.length;
}

// �ֶ�����ѷ�������
void AddAllocatedBlock(int startAddress, int length, int pid) {
    if (length <= 0 || startAddress < 0 || startAddress + length > mem) {
        cout << "�ֶ����ʧ��: ��ַ�򳤶Ȳ�����" << endl;
        return;
    }

    // ����Ƿ������е��ѷ��������ͻ
    for (const auto& block : allocated) {
        if (!(startAddress >= block.startAddress + block.length || startAddress + length <= block.startAddress)) {
        	// ���ѷ���������н��� 
            cout << "�ֶ����ʧ��: �������ѷ��������ͻ��" << endl;
            return;
        }
    }

    // ���Ҳ����º��ʵĿ�������
    bool isAllocated = false;
    for (auto& block : available) {
        if (startAddress >= block.startAddress && startAddress + length <= block.startAddress + block.length) {
            // �ҵ����ʵĿ�������
            isAllocated = true;
            int newEnd = block.startAddress + block.length;
            int originalStart = block.startAddress;

            // ���¿�������
            if (startAddress == originalStart) {
                block.startAddress += length;
                block.length -= length;
            } 
			else if (startAddress + length == newEnd) {
                block.length -= length;
            } 
			else {
                // �ָ��������
                block.length = startAddress - originalStart;
                available.push_back({startAddress + length, newEnd - (startAddress + length), 0});
            }

            if (block.length == 0) {
                available.erase(remove(available.begin(), available.end(), block), available.end());
            }

            break;
        }
    }

    if (isAllocated) {
        allocated.push_back({startAddress, length, pid});
        cout << "�ֶ���ӳɹ�" << endl;
    }
	else {
        cout << "�ֶ����ʧ��: û���ҵ����ʵĿ�������" << endl;
    }
}

// ������̷���ռ�
bool Alloc(int pid, int size) {
    sort(allocated.begin(), allocated.end(), compareByStartAddress);
    sort(available.begin(), available.end(), compareByLength);

    int index = -1;
    for (int i = 0; i < available.size(); i++) {
        if (available[i].length >= size) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return false;
    }

    if (available[index].length == size) {
        allocated.push_back({available[index].startAddress, size, pid});
        available.erase(available.begin() + index);
    } 
	else {
        allocated.push_back({available[index].startAddress, size, pid});
        available[index].startAddress += size;
        available[index].length -= size;
    }

    sort(allocated.begin(), allocated.end(), compareByStartAddress);
    sort(available.begin(), available.end(), compareByLength);
    return true;
}

// ���ս���
bool Free(int pid) {
    sort(available.begin(), available.end(), compareByStartAddress);
    sort(available.begin(), available.end(), compareByLength);

    int index = -1;
    for (int i = 0; i < allocated.size(); i++) {
        if (allocated[i].pid == pid) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return false;
    }

    MemoryBlock freedBlock = allocated[index];
    allocated.erase(allocated.begin() + index);

    // �ϲ��ڽӿ�����
    for (int i = 0; i < available.size(); i++) {
        if (available[i].startAddress == freedBlock.startAddress + freedBlock.length) {
            freedBlock.length += available[i].length;
            available.erase(available.begin() + i);
            break;
        }
    }

    for (int i = 0; i < available.size(); i++) {
        if (available[i].startAddress + available[i].length == freedBlock.startAddress) {
            available[i].length += freedBlock.length;
            return true;
        }
    }

    available.push_back({freedBlock.startAddress, freedBlock.length, 0});
    return true;
}

// ��ʾ��Ϣ
void Information() {
    sort(allocated.begin(), allocated.end(), compareByStartAddress);
    sort(available.begin(), available.end(), compareByLength);
	cout<<endl; 
    cout << "---------------���������---------------" << endl;
    cout << "��ʼ��ַ\t����\t��ֹ��ַ\t�Ƿ����" << endl;
    for (auto& item : available) {
        cout << item.startAddress << "\t\t" << item.length << "\t\t" 
             << item.startAddress + item.length - 1 << "\t\t" << item.pid << endl;
    }

    cout << "---------------�ѷ������---------------" << endl;
    cout << "��ʼ��ַ\t����\t��ֹ��ַ\t���̺�" << endl;
    for (auto& item : allocated) {
        cout << item.startAddress << "\t\t" << item.length << "\t\t" 
             << item.startAddress + item.length - 1 << "\t\t" << item.pid << endl;
    }
}

// ���к���
void Run() {
    string key;
    int pid, size, startAddress;
    while (true) {
        Information();
        cout << "�����������[�ֶ���ӽ���-0/�������-1/���ս���-2/�˳�����-3]��";
        cin >> key;

        if (key == "0") {
            cout << "��������̺ţ�";
            cin >> pid;
            cout << "��������ʼ��ַ��";
            cin >> startAddress;
            cout << "��������̳��ȣ�";
            cin >> size;
            AddAllocatedBlock(startAddress, size, pid);
        }
		else if (key == "1") {
            cout << "��������̺ţ�";
            cin >> pid;
            cout << "��������Ҫ������ڴ�ռ��С��";
            cin >> size;
            if (Alloc(pid, size)) {
                cout << "����ɹ���" << endl;
            }
			else {
                cout << "����ʧ�ܣ�" << endl;
            }
        }
		else if (key == "2") {
            cout << "��������Ҫ���յĽ��̺ţ�";
            cin >> pid;
            if (Free(pid)) {
                cout << "���ճɹ���" << endl;
            } else {
                cout << "����ʧ�ܣ�" << endl;
            }
        }
		else if (key == "3") {
            break;
        }
        else {
        	cout<<"�����Ŵ������������룡"<<endl;
		}
    }
    cout << endl;
}

int main() {
    cout << "�����ڴ��С��";
    cin >> mem;
    available.push_back({0, mem, 0});
    Run();
    return 0;
    /*
    �״���Ӧ�㷨����ַ����
	���͵�ַС��Ƭ�࣬�ߵ�ַ�д��������
	�����Ӧ�㷨����������
	������Ŀ��з�����С������Ĵ�С��Ϊ�ӽ���С��Ƭ�࣬�д�������� 
	ѭ���״���Ӧ�㷨����ַ������Ϊѭ������
	�����״θ��죬����������ױ��棩 
    */
}

/*
�ֶ���ӵĲ�������
0
1
0
15

0
2
38
10

0
3
68
12

0
4
110
10
*/

/*
�����½��̵Ĳ�������
1
5
5

1
6
13 
*/

/*
// �ֶ�����ѷ�������
void AddAllocatedBlock(int startAddress, int length, int pid) {
	bool flag=1;
	sort(allocated.begin(), allocated.end(), compareByStartAddress);	// ����ʼ��ַ�����ѷ��� 
	sort(available.begin(), available.end(), compareByLength);			// ���ռ��С���������
	
	// �����洢����ַ��Χ 
	if(startAddress < 0 || startAddress >= mem){
		flag=0;
	}
	
	// pid �Ѵ���
	for(auto& t : allocated){
		if(t.pid == pid){
			flag=0;
			break;
		}
	} 
	
	// �½��̵ķ�Χ����available����
	bool in=0;
	int index=-1;
	for (int i = 0; i < available.size(); i++) {
	    MemoryBlock& tt = available[i];
	    if(tt.startAddress <= startAddress && tt.startAddress + tt. length >= startAddress + length){
			in=1;
			index=i;
			break;
		}
	}
	if(in == 0){
		flag=0;
	}
	
	// ������� + ����allocated��available 
    if(flag == 1){
    	cout<<"�ֶ���ӳɹ�"<<endl;
    	// �����û��� startAddress �� length ���з���
		allocated.push_back({startAddress, length, pid});
		 
		if(available[index].startAddress == startAddress && available[index].length == length){
			// �����պ�
			available.erase(available.begin() + index);		
		}
		else if(available[index].startAddress == startAddress && available[index].length != length){
			// �׵�ַ�غ� 
			available[index].startAddress += length;
		}
		else if(available[index].startAddress != startAddress && available[index].length == length){
			// β��ַ�غ�
			available[index].length -= length;
		}
		else{
			// �����غϣ�available������1��
			available.push_back({startAddress + length, available[index].length - (startAddress + length), 0});
			available[index].length = startAddress - available[index].startAddress;
		}
	}
	else{
		cout<<"�ֶ����ʧ��"<<endl;
	}
}
*/
