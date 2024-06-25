#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

const int MAXOPEN = 4;
const int MAXBYTE = 50;

// FCB or one-UFD
struct FileInfo {  
    string file_name;				// 文件名字 
    int file_size;					// 文件大小 
    vector<int> allocated_sectors;	// 文件分配的扇区 
    int protection;					// 文件保护方式 
};

// UOF
struct UOF {
	string file_name;	//文件名字 
	int protection;		//文件操作方式 
	int file_size;		//文件大小 （当前） 
	int read;			//文件读指针 
	int write;			//文件写指针 
	int state;  		//文件状态
	//int file_path;		//文件地址 ，不如直接看磁盘。 
};

// Disk 
struct StorageManager {  					
    int available_sectors;  				// 可用扇区总数
    vector<vector<int>> bitmap;  			// 磁盘位图
    unordered_map<string, FileInfo> files;  // 文件映射
    unordered_map<string, UOF> user;		// 打开文件映射 

    StorageManager();  						// 构造函数yes1
    bool createFile(const string&, int, int);  	// 创建文件yes1
    void displayStatus();  					// 显示存储状态yes1
    void fileDetails(const FileInfo&);  	// 显示文件详情yes1
    bool deleteFile(const string&);  		// 删除文件yes1
    
    bool openFile(const string&, int);		// 打开文件，文件名+操作方式yes 
	bool closeFile(const string&);			// 关闭文件，文件名yes 
	bool writeFile(const string&, int);		// 写入文件，文件名+写入字节数yes 
	bool readFile(const string&, int);		// 读取文件，文件名+读取字节数 yes 
	void displayUOF();						// 显示UOF 
};

// show UOF
void StorageManager::displayUOF(){
	cout<<"文件打开表UOF："<<endl;
	for(const auto& pair:user){
		cout<<pair.first<<"文件：";
		cout<<"[文件操作方式]"<<pair.second.protection<<"\t";
		cout<<"[文件当前大小]"<<pair.second.file_size<<"\t";
		cout<<"[文件读指针]"<<pair.second.read<<"\t";
		cout<<"[文件写指针]"<<pair.second.write<<"\t";
		cout<<"[文件状态]"<<pair.second.state;
		cout<<endl;
	}
} 

// 读取文件
bool StorageManager::readFile(const string& name, int byte){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"读取文件错误：UOF里没有这个文件。"<<endl;
		return false;
	}
	
	UOF temp = it->second;
	if(temp.protection != 3 and temp.protection != 1){
		cout<<"读取文件错误：该文件不能操作读。"<<endl;
		return false;
	}
	
	if(temp.read + byte > temp.file_size){
		cout<<"读取文件错误：读取文件长度超过上限。"<<endl;
		return false;
	}
	
	// allow reading
	it->second.read += byte;
	
	return true;
}

// 写入文件
bool StorageManager::writeFile(const string& name, int byte){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"写入文件错误：UOF里没有这个文件。"<<endl;
		return false;
	}
	
	UOF temp = it->second;
	if(temp.protection != 3 and temp.protection != 2){
		cout<<"写入文件错误：该文件不能操作写。"<<endl;
		return false;
	}
	
	if(temp.write + byte > MAXBYTE){
		cout<<"写入文件错误：写入文件长度超过上限。"<<endl;
		return false;
	}
	
	// check disk
	if(available_sectors < byte){
		cout<<"写入文件错误：磁盘空间不足。"<<endl;
		return false;
	}
	
	auto find = files.find(name);
	FileInfo add = find->second;
	
	int addbyte = byte;
	// modify bitmap
	for(int i=0;i<bitmap.size()&&addbyte>0;++i){
		for(int j=0;j<bitmap[i].size()&&addbyte>0;++j){
			if(bitmap[i][j]==0){
				bitmap[i][j]=1;
				find->second.allocated_sectors.push_back(i*16+j);	//allocated_sectors
				--addbyte;
				--available_sectors;
			}
		}
	}
	
	// allow writing
	it->second.write += byte;		// 修改写指针 
	it->second.file_size += byte;	// 修改当前文件长度
	
	// update FCB
	find->second.file_size += byte;
	fileDetails(find->second);
	
	return true;
}

// 关闭文件函数 
bool StorageManager::closeFile(const string& name){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"关闭文件错误：UOF里没有这个文件。"<<endl;
		return false;
	}
	
	user.erase(it);
	
	return true;
}

// 打开文件函数 
bool StorageManager::openFile(const string& name, int opt){
	auto it = files.find(name);
	if(it == files.end()){
		cout<<"打开文件错误：没有这个文件。"<<endl;
		return false;
	}
	
	FileInfo temp = it->second;
	
	if(temp.protection == 1){
		if(opt != 1){
			cout<<"打开文件错误：文件保护方式有限制。"<<endl;
			return false;
		}
	}
	else if(temp.protection == 2){
		if(opt != 2){
			cout<<"打开文件错误：文件保护方式有限制。"<<endl;
			return false;
		}
	}
	
	auto newit = user.find(name);
	if(newit != user.end()){
		cout<<"打开文件错误：这个文件已经打开。"<<endl;
		return false;
	}
	
	if(user.size() >= MAXOPEN){
		cout<<"打开文件错误：打开文件数目超过上限。"<<endl;
		return false;
	}
	
	// 文件名字、操作、当前大小、读ptr、写ptr、状态 
	UOF t{name, opt, temp.file_size, 1, temp.file_size, 0};
	user[name] = t;		//hash setting
	
	return true;
}

// 构造函数  8个柱面。每个柱面有4个磁道，每个磁道又划分成4个物理盘块。
StorageManager::StorageManager() : available_sectors(128) {
    bitmap.resize(8, vector<int>(16, 0));	// 构造8*16个空盘块 
    for (int j = 0; j < 16; ++j) {
        bitmap[0][j] = 1;  // 预留第一行作为系统文件，该柱面不可用
    }
}

// 创建文件函数
bool StorageManager::createFile(const string& name, int size, int protection) {
    if (files.find(name) != files.end()) {	// 如果文件名字不在末尾，则冲突 
        cout << "创建文件错误: 文件已存在。"<< endl;
        return false;
    }	
    if (size > available_sectors) {		// 如果文件大小大于空闲扇区大小 
        cout << "创建文件错误: 磁盘空间不足。"<< endl;
        return false;
    }
    
	// 创建新文件的FCB 
    FileInfo newFile{ name, size, {} , protection};
    
    // 更新位示图中信息
    for (int i = 0; i < bitmap.size() && size > 0; ++i) {
        for (int j = 0; j < bitmap[i].size() && size > 0; ++j) {
            if (bitmap[i][j] == 0) {	// 如果这个盘块为空 
                bitmap[i][j] = 1;	// 设置为已经分配 
                newFile.allocated_sectors.push_back(i * 16 + j);	// 分配区增加这个盘块 
                --size;		// 空闲区大小-=1 
                --available_sectors;	// 同上 
            }
        }
    }

    files[name] = newFile;	// hash表设置 
    fileDetails(newFile);	// 显示新建文件详情 
    return true;
}

// 显示磁盘分配情况函数
void StorageManager::displayStatus() {
    cout << "磁盘位图：" << endl;
    for (int i = 0; i < bitmap.size(); ++i) {
        for (int j = 0; j < bitmap[i].size(); ++j) {
            cout << bitmap[i][j] << ' ';
        }
        cout << endl;	// 换行显示 
    }
    
    cout << endl << "文件列表:" << endl;
    for (const auto& pair : files) {
        cout <<"文件'"<< pair.first << "'的大小: " << pair.second.file_size << endl;
        fileDetails(pair.second);
    }
    cout << endl;
}

// 显示文件内存详情函数
void StorageManager::fileDetails(const FileInfo& file) {
    cout << "文件 '" << file.file_name << "' 的详细信息："<< endl;
    for (int sector : file.allocated_sectors) {
        cout << "扇区: " << sector << "；";	//遍历扇区 
    }
    cout << endl;
}

// 删除文件函数
bool StorageManager::deleteFile(const string& name) {
    auto it = files.find(name);
    if (it == files.end()) {
        cout << "删除文件错误: 文件不存在。"<< endl;
        return false;
    }

    for (int sector : it->second.allocated_sectors) {
        int i = sector / 16;
        int j = sector % 16;
        bitmap[i][j] = 0;
        ++available_sectors;
    }

    files.erase(it);
    auto another = user.find(name);
    if (another != user.end()){
    	user.erase(another);
	}
    
    cout << "文件 '" << name << "' 已删除。"<< endl;
    return true;
}

// 菜单函数
void menu(StorageManager manager){
    cout << "根据数字提示选择操作: " << endl;
    cout << "0:退出系统，1:创建文件, 2:删除文件" << endl;
    cout << "4:打开文件，5:读取文件, 6:写入文件, 7:关闭文件" << endl;
    cout << "3:磁盘位示图，8：用户文件打开表"<<endl; 
}

// 主函数
int main() {
    StorageManager manager;
    int action;
    string filename;
    int filesize;
    int protection;
    int byte;

    while (true) {
    	bool flag = 1;
    	cout << endl;
        menu(manager);
        cin >> action;
        cout << endl;

        switch (action) {
        	// 1，创建文件 
            case 1:
                cout << "输入文件名: ";
                cin >> filename;
                while (1){
                	cout << "输入文件大小: ";
                	cin >> filesize;
                	if(filesize<=0){
                		cout<<"文件大小必须为正，请重新输入！"<<endl; 
					}
					else{
						break;
					}
				}
                while (1) {
                	cout << "输入文件保护方式：";
                	cin >> protection;
                	if(protection == 1 or protection == 2 or protection == 3){
                		break;
					}
					else{
						cout<<"文件保护方式输入错误，请重新输入！"<<endl;
					}
				}
                if (manager.createFile(filename, filesize, protection)) {
                    cout << "文件创建成功。"<<endl;
                }
                break;
            //2，删除文件 
            case 2:
                cout << "输入要删除的文件名: ";
                cin >> filename;
                if (manager.deleteFile(filename)) {
                    cout << "文件删除成功。"<<endl;
                }
                break;
            //3，磁盘位示图 
            case 3:
            	manager.displayStatus();
            	break;
            //4:打开文件
            case 4:
            	cout << "输入要打开的文件名: ";
                cin >> filename;
                while(1){
                	cout<<"请输入文件操作方式：";
					cin >> protection;
					if(protection != 1 and protection != 2 and protection != 3){
						cout<<"文件操作方式错误，请输入1、2、3中的其中一个。"<<endl;
					}
					else {
						break;
					}
				} 
                if (manager.openFile(filename,protection)) {
                    cout << "文件打开成功。"<<endl;
                }
            	break;
            //5:读取文件
            case 5:
            	cout<<"输入要读取的文件名：";
            	cin>>filename;
            	while(1){
            		cout<<"输入要读取的字节数：";
            		cin>>byte;
            		if(byte <= 0){
                		cout<<"读取字节大小必须为正，请重新输入！"<<endl; 
					}
					else{
						break;
					}
				}
				if(manager.readFile(filename, byte)){
            		cout<<"文件读取成功。"<<endl;
				}
            	break;
            //6:写入文件
            case 6:
            	cout<<"输入要写入的文件名：";
            	cin>>filename;
            	while(1){
            		cout<<"输入要写入的字节数：";
            		cin>>byte;
            		if(byte <= 0){
                		cout<<"写入字节大小必须为正，请重新输入！"<<endl; 
					}
					else{
						break;
					}
				}
            	if(manager.writeFile(filename, byte)){
            		cout<<"文件写入成功。"<<endl;
				}
            	break;
            //7:关闭文件
            case 7:
            	cout << "输入要关闭的文件名: ";
                cin >> filename;
                if(manager.closeFile(filename)){
                	cout << "文件关闭成功。"<<endl;
				}
            	break;
            //8:UOF
			case 8:
				manager.displayUOF(); 
				break; 
            case 0:
                cout << "程序退出，感谢使用。"<<endl;
                flag = 0;
                break;
            default:
                cout << "无效的数字操作，请重新输入。"<<endl;
                break;
        }
        
        if(!flag){
        	break;
		}
    }
	
    return 0;
}

