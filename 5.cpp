#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// �ļ���Ϣ�ṹ�壨FCB��file control block�� 
struct FileInfo {  
    string file_name;				// �ļ����� 
    int file_size;					// �ļ���С 
    vector<int> allocated_sectors;	// �ļ���������� 
};

// �洢�������ṹ�壨���̹��� 
struct StorageManager {  					
    int available_sectors;  				// ������������
    vector<vector<int>> bitmap;  			// ����λͼ
    unordered_map<string, FileInfo> files;  // �ļ�ӳ��

    StorageManager();  						// ���캯��
    bool createFile(const string&, int);  	// �����ļ�
    void displayStatus();  					// ��ʾ�洢״̬
    void fileDetails(const FileInfo&);  	// ��ʾ�ļ�����
    bool deleteFile(const string&);  		// ɾ���ļ�
};

// ���캯�� 
// �ٶ�����һ�������飬����8�����档ÿ��������4���ŵ���ÿ���ŵ��ֻ��ֳ�4�������̿顣
StorageManager::StorageManager() : available_sectors(128) {
    bitmap.resize(8, vector<int>(16, 0));	// ����8*16�����̿� 
    for (int j = 0; j < 16; ++j) {
        bitmap[0][j] = 1;  // Ԥ����һ����Ϊϵͳ�ļ��������治���� 
        // λʾͼ�ĳ�ʼ״̬Ϊ��1����Ϊ��1��������ȫ�����С�
    }
}

// �����ļ����� 
bool StorageManager::createFile(const string& name, int size) {
    if (files.find(name) != files.end()) {	// ����ļ����ֲ���ĩβ�����ͻ 
        cout << "����: �ļ��Ѵ��ڡ�"<< endl;
        return false;
    }	
    if (size > available_sectors) {		// ����ļ���С���ڿ���������С 
        cout << "����: �ռ䲻�㡣"<< endl;
        return false;
    }
    
	// �������ļ���FCB 
    FileInfo newFile{ name, size, {} };
    
    // ����λʾͼ����Ϣ
    for (int i = 0; i < bitmap.size() && size > 0; ++i) {
        for (int j = 0; j < bitmap[i].size() && size > 0; ++j) {
            if (bitmap[i][j] == 0) {	// �������̿�Ϊ�� 
                bitmap[i][j] = 1;	// ����Ϊ�Ѿ����� 
                newFile.allocated_sectors.push_back(i * 16 + j);	// ��������������̿� 
                --size;		// ��������С-=1 
                --available_sectors;	// ͬ�� 
            }
        }
    }

    files[name] = newFile;	// hash������ 
    fileDetails(newFile);	// ��ʾ�½��ļ����� 
    return true;
}

// ��ʾ���̷����������
void StorageManager::displayStatus() {
    cout << "����λͼ��" << endl;
    for (int i = 0; i < bitmap.size(); ++i) {
        for (int j = 0; j < bitmap[i].size(); ++j) {
            cout << bitmap[i][j] << ' ';
        }
        cout << endl;	// ������ʾ 
    }
    
    cout << endl << "�ļ��б�:" << endl;
    for (const auto& pair : files) {
        cout << pair.first << "\t��С: " << pair.second.file_size << endl;
    }
    cout << endl;
}

// ��ʾ�ļ��ڴ����麯��
void StorageManager::fileDetails(const FileInfo& file) {
    cout << "�ļ� '" << file.file_name << "' ����ϸ��Ϣ��"<< endl;
    for (int sector : file.allocated_sectors) {
        cout << "����: " << sector << "��";	//�������� 
    }
    cout << endl;
}

// ɾ���ļ�����
bool StorageManager::deleteFile(const string& name) {
    auto it = files.find(name);
    if (it == files.end()) {
        cout << "����: �ļ������ڡ�"<< endl;
        return false;
    }

    for (int sector : it->second.allocated_sectors) {
        int i = sector / 16;
        int j = sector % 16;
        bitmap[i][j] = 0;
        ++available_sectors;
    }

    files.erase(it);
    cout << "�ļ� '" << name << "' ��ɾ����"<< endl;
    return true;
}

// �˵�����
void menu(StorageManager manager){
	manager.displayStatus();
    cout << "����������ʾѡ�����: " << endl;
    cout << "1:�����ļ�, 2:ɾ���ļ�, 0:�˳�ϵͳ"<< endl;
    // �����ļ������ļ������ļ���д�ļ����ر��ļ���ɾ���ļ����
    
}

// ������
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
                cout << "�����ļ���: ";
                cin >> filename;
                cout << "�����ļ���С: ";
                cin >> filesize;
                if (manager.createFile(filename, filesize)) {
                    cout << "�ļ������ɹ���\n";
                }
                break;
            case 2:
                cout << "����Ҫɾ�����ļ���: ";
                cin >> filename;
                if (manager.deleteFile(filename)) {
                    cout << "�ļ�ɾ���ɹ���\n";
                }
                break;
            case 0:
                cout << "�����˳���\n";
                return 0;
            default:
                cout << "��Ч�Ĳ�����\n";
        }
    }

    return 0;
}
