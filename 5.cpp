#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// 文件信息结构体（FCB，file control block） 
struct FileInfo {  
    string file_name;				// 文件名字 
    int file_size;					// 文件大小 
    vector<int> allocated_sectors;	// 文件分配的扇区 
};

// 存储管理器结构体（磁盘管理） 
struct StorageManager {  					
    int available_sectors;  				// 可用扇区总数
    vector<vector<int>> bitmap;  			// 磁盘位图
    unordered_map<string, FileInfo> files;  // 文件映射

    StorageManager();  						// 构造函数
    bool createFile(const string&, int);  	// 创建文件
    void displayStatus();  					// 显示存储状态
    void fileDetails(const FileInfo&);  	// 显示文件详情
    bool deleteFile(const string&);  		// 删除文件
};

// 构造函数 
// 假定现有一个磁盘组，共有8个柱面。每个柱面有4个磁道，每个磁道又划分成4个物理盘块。
StorageManager::StorageManager() : available_sectors(128) {
    bitmap.resize(8, vector<int>(16, 0));	// 构造8*16个空盘块 
    for (int j = 0; j < 16; ++j) {
        bitmap[0][j] = 1;  // 预留第一行作为系统文件，该柱面不可用 
        // 位示图的初始状态为第1个字为“1”，其他全部空闲。
    }
}

// 创建文件函数 
bool StorageManager::createFile(const string& name, int size) {
    if (files.find(name) != files.end()) {	// 如果文件名字不在末尾，则冲突 
        cout << "错误: 文件已存在。"<< endl;
        return false;
    }	
    if (size > available_sectors) {		// 如果文件大小大于空闲扇区大小 
        cout << "错误: 空间不足。"<< endl;
        return false;
    }
    
	// 创建新文件的FCB 
    FileInfo newFile{ name, size, {} };
    
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
        cout << pair.first << "\t大小: " << pair.second.file_size << endl;
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
        cout << "错误: 文件不存在。"<< endl;
        return false;
    }

    for (int sector : it->second.allocated_sectors) {
        int i = sector / 16;
        int j = sector % 16;
        bitmap[i][j] = 0;
        ++available_sectors;
    }

    files.erase(it);
    cout << "文件 '" << name << "' 已删除。"<< endl;
    return true;
}

// 菜单函数
void menu(StorageManager manager){
	manager.displayStatus();
    cout << "根据数字提示选择操作: " << endl;
    cout << "1:创建文件, 2:删除文件, 0:退出系统"<< endl;
    // 创建文件、打开文件、读文件、写文件、关闭文件、删除文件命令。
    
}

// 主函数
int main() {
    StorageManager manager;
    int action;
    string filename;
    int filesize;

    while (true) {
    	cout << endl;
        menu(manager);
        cin >> action;

        switch (action) {
            case 1:
                cout << "输入文件名: ";
                cin >> filename;
                cout << "输入文件大小: ";
                cin >> filesize;
                if (manager.createFile(filename, filesize)) {
                    cout << "文件创建成功。\n";
                }
                break;
            case 2:
                cout << "输入要删除的文件名: ";
                cin >> filename;
                if (manager.deleteFile(filename)) {
                    cout << "文件删除成功。\n";
                }
                break;
            case 0:
                cout << "程序退出。\n";
                return 0;
            default:
                cout << "无效的操作。\n";
        }
    }

    return 0;
}
