#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int mem;	// 存储器空间大小 

struct MemoryBlock {
    int startAddress;
    int length;
    int pid; // 对于空闲区，pid 为 0
    
    // 定义比较操作符
    bool operator==(const MemoryBlock& other) const {
        return startAddress == other.startAddress && length == other.length && pid == other.pid;
    }
};

vector<MemoryBlock> allocated;  // 已分配区列表
vector<MemoryBlock> available;  // 空闲区列表

// 排序函数，按起始地址排序
bool compareByStartAddress(const MemoryBlock& a, const MemoryBlock& b) {
    return a.startAddress < b.startAddress;
}

// 排序函数，按长度排序
bool compareByLength(const MemoryBlock& a, const MemoryBlock& b) {
    return a.length < b.length;
}

// 手动添加已分配区域
void AddAllocatedBlock(int startAddress, int length, int pid) {
    if (length <= 0 || startAddress < 0 || startAddress + length > mem) {
        cout << "手动添加失败: 地址或长度不合理。" << endl;
        return;
    }

    // 检查是否与现有的已分配区域冲突
    for (const auto& block : allocated) {
        if (!(startAddress >= block.startAddress + block.length || startAddress + length <= block.startAddress)) {
        	// 与已分配的区域，有交集 
            cout << "手动添加失败: 与现有已分配区域冲突。" << endl;
            return;
        }
    }

    // 查找并更新合适的空闲区域
    bool isAllocated = false;
    for (auto& block : available) {
        if (startAddress >= block.startAddress && startAddress + length <= block.startAddress + block.length) {
            // 找到合适的空闲区域
            isAllocated = true;
            int newEnd = block.startAddress + block.length;
            int originalStart = block.startAddress;

            // 更新空闲区域
            if (startAddress == originalStart) {
                block.startAddress += length;
                block.length -= length;
            } 
			else if (startAddress + length == newEnd) {
                block.length -= length;
            } 
			else {
                // 分割空闲区域
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
        cout << "手动添加成功" << endl;
    }
	else {
        cout << "手动添加失败: 没有找到合适的空闲区域。" << endl;
    }
}

// 申请进程分配空间
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

// 回收进程
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

    // 合并邻接空闲区
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

// 显示信息
void Information() {
    sort(allocated.begin(), allocated.end(), compareByStartAddress);
    sort(available.begin(), available.end(), compareByLength);
	cout<<endl; 
    cout << "---------------空闲区情况---------------" << endl;
    cout << "起始地址\t长度\t终止地址\t是否分配" << endl;
    for (auto& item : available) {
        cout << item.startAddress << "\t\t" << item.length << "\t\t" 
             << item.startAddress + item.length - 1 << "\t\t" << item.pid << endl;
    }

    cout << "---------------已分配情况---------------" << endl;
    cout << "起始地址\t长度\t终止地址\t进程号" << endl;
    for (auto& item : allocated) {
        cout << item.startAddress << "\t\t" << item.length << "\t\t" 
             << item.startAddress + item.length - 1 << "\t\t" << item.pid << endl;
    }
}

// 运行函数
void Run() {
    string key;
    int pid, size, startAddress;
    while (true) {
        Information();
        cout << "请输入操作码[手动添加进程-0/申请进程-1/回收进程-2/退出程序-3]：";
        cin >> key;

        if (key == "0") {
            cout << "请输入进程号：";
            cin >> pid;
            cout << "请输入起始地址：";
            cin >> startAddress;
            cout << "请输入进程长度：";
            cin >> size;
            AddAllocatedBlock(startAddress, size, pid);
        }
		else if (key == "1") {
            cout << "请输入进程号：";
            cin >> pid;
            cout << "请输入需要申请的内存空间大小：";
            cin >> size;
            if (Alloc(pid, size)) {
                cout << "申请成功！" << endl;
            }
			else {
                cout << "申请失败！" << endl;
            }
        }
		else if (key == "2") {
            cout << "请输入需要回收的进程号：";
            cin >> pid;
            if (Free(pid)) {
                cout << "回收成功！" << endl;
            } else {
                cout << "回收失败！" << endl;
            }
        }
		else if (key == "3") {
            break;
        }
        else {
        	cout<<"输入编号错误，请重新输入！"<<endl;
		}
    }
    cout << endl;
}

int main() {
    cout << "输入内存大小：";
    cin >> mem;
    available.push_back({0, mem, 0});
    Run();
    return 0;
    /*
    首次适应算法：地址递增
	（低地址小碎片多，高地址有大空闲区域）
	最佳适应算法：容量递增
	（分配的空闲分区大小与申请的大小最为接近，小碎片多，有大空闲区域） 
	循环首次适应算法：地址递增成为循环链表
	（比首次更快，大空闲区域不易保存） 
    */
}

/*
手动添加的测试用例
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
申请新进程的测试用例
1
5
5

1
6
13 
*/

/*
// 手动添加已分配区域
void AddAllocatedBlock(int startAddress, int length, int pid) {
	bool flag=1;
	sort(allocated.begin(), allocated.end(), compareByStartAddress);	// 按起始地址排序已分配 
	sort(available.begin(), available.end(), compareByLength);			// 按空间大小排序空闲区
	
	// 超出存储器地址范围 
	if(startAddress < 0 || startAddress >= mem){
		flag=0;
	}
	
	// pid 已存在
	for(auto& t : allocated){
		if(t.pid == pid){
			flag=0;
			break;
		}
	} 
	
	// 新进程的范围不在available里面
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
	
	// 输出内容 + 更新allocated和available 
    if(flag == 1){
    	cout<<"手动添加成功"<<endl;
    	// 按照用户的 startAddress 和 length 进行分配
		allocated.push_back({startAddress, length, pid});
		 
		if(available[index].startAddress == startAddress && available[index].length == length){
			// 这个块刚好
			available.erase(available.begin() + index);		
		}
		else if(available[index].startAddress == startAddress && available[index].length != length){
			// 首地址重合 
			available[index].startAddress += length;
		}
		else if(available[index].startAddress != startAddress && available[index].length == length){
			// 尾地址重合
			available[index].length -= length;
		}
		else{
			// 都不重合，available会增加1块
			available.push_back({startAddress + length, available[index].length - (startAddress + length), 0});
			available[index].length = startAddress - available[index].startAddress;
		}
	}
	else{
		cout<<"手动添加失败"<<endl;
	}
}
*/
