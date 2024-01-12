#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

const int MAXOPEN = 4;
const int MAXBYTE = 50;

// FCB or one-UFD
struct FileInfo {  
    string file_name;				// �ļ����� 
    int file_size;					// �ļ���С 
    vector<int> allocated_sectors;	// �ļ���������� 
    int protection;					// �ļ�������ʽ 
};

// UOF
struct UOF {
	string file_name;	//�ļ����� 
	int protection;		//�ļ�������ʽ 
	int file_size;		//�ļ���С ����ǰ�� 
	int read;			//�ļ���ָ�� 
	int write;			//�ļ�дָ�� 
	int state;  		//�ļ�״̬
	//int file_path;		//�ļ���ַ ������ֱ�ӿ����̡� 
};

// Disk 
struct StorageManager {  					
    int available_sectors;  				// ������������
    vector<vector<int>> bitmap;  			// ����λͼ
    unordered_map<string, FileInfo> files;  // �ļ�ӳ��
    unordered_map<string, UOF> user;		// ���ļ�ӳ�� 

    StorageManager();  						// ���캯��yes1
    bool createFile(const string&, int, int);  	// �����ļ�yes1
    void displayStatus();  					// ��ʾ�洢״̬yes1
    void fileDetails(const FileInfo&);  	// ��ʾ�ļ�����yes1
    bool deleteFile(const string&);  		// ɾ���ļ�yes1
    
    bool openFile(const string&, int);		// ���ļ����ļ���+������ʽyes 
	bool closeFile(const string&);			// �ر��ļ����ļ���yes 
	bool writeFile(const string&, int);		// д���ļ����ļ���+д���ֽ���yes 
	bool readFile(const string&, int);		// ��ȡ�ļ����ļ���+��ȡ�ֽ��� yes 
	void displayUOF();						// ��ʾUOF 
};

// show UOF
void StorageManager::displayUOF(){
	cout<<"�ļ��򿪱�UOF��"<<endl;
	for(const auto& pair:user){
		cout<<pair.first<<"�ļ���";
		cout<<"[�ļ�������ʽ]"<<pair.second.protection<<"\t";
		cout<<"[�ļ���ǰ��С]"<<pair.second.file_size<<"\t";
		cout<<"[�ļ���ָ��]"<<pair.second.read<<"\t";
		cout<<"[�ļ�дָ��]"<<pair.second.write<<"\t";
		cout<<"[�ļ�״̬]"<<pair.second.state;
		cout<<endl;
	}
} 

// ��ȡ�ļ�
bool StorageManager::readFile(const string& name, int byte){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"��ȡ�ļ�����UOF��û������ļ���"<<endl;
		return false;
	}
	
	UOF temp = it->second;
	if(temp.protection != 3 and temp.protection != 1){
		cout<<"��ȡ�ļ����󣺸��ļ����ܲ�������"<<endl;
		return false;
	}
	
	if(temp.read + byte > temp.file_size){
		cout<<"��ȡ�ļ����󣺶�ȡ�ļ����ȳ������ޡ�"<<endl;
		return false;
	}
	
	// allow reading
	it->second.read += byte;
	
	return true;
}

// д���ļ�
bool StorageManager::writeFile(const string& name, int byte){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"д���ļ�����UOF��û������ļ���"<<endl;
		return false;
	}
	
	UOF temp = it->second;
	if(temp.protection != 3 and temp.protection != 2){
		cout<<"д���ļ����󣺸��ļ����ܲ���д��"<<endl;
		return false;
	}
	
	if(temp.write + byte > MAXBYTE){
		cout<<"д���ļ�����д���ļ����ȳ������ޡ�"<<endl;
		return false;
	}
	
	// check disk
	if(available_sectors < byte){
		cout<<"д���ļ����󣺴��̿ռ䲻�㡣"<<endl;
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
	it->second.write += byte;		// �޸�дָ�� 
	it->second.file_size += byte;	// �޸ĵ�ǰ�ļ�����
	
	// update FCB
	find->second.file_size += byte;
	fileDetails(find->second);
	
	return true;
}

// �ر��ļ����� 
bool StorageManager::closeFile(const string& name){
	auto it = user.find(name);
	if(it == user.end()){
		cout<<"�ر��ļ�����UOF��û������ļ���"<<endl;
		return false;
	}
	
	user.erase(it);
	
	return true;
}

// ���ļ����� 
bool StorageManager::openFile(const string& name, int opt){
	auto it = files.find(name);
	if(it == files.end()){
		cout<<"���ļ�����û������ļ���"<<endl;
		return false;
	}
	
	FileInfo temp = it->second;
	
	if(temp.protection == 1){
		if(opt != 1){
			cout<<"���ļ������ļ�������ʽ�����ơ�"<<endl;
			return false;
		}
	}
	else if(temp.protection == 2){
		if(opt != 2){
			cout<<"���ļ������ļ�������ʽ�����ơ�"<<endl;
			return false;
		}
	}
	
	auto newit = user.find(name);
	if(newit != user.end()){
		cout<<"���ļ���������ļ��Ѿ��򿪡�"<<endl;
		return false;
	}
	
	if(user.size() >= MAXOPEN){
		cout<<"���ļ����󣺴��ļ���Ŀ�������ޡ�"<<endl;
		return false;
	}
	
	// �ļ����֡���������ǰ��С����ptr��дptr��״̬ 
	UOF t{name, opt, temp.file_size, 1, temp.file_size, 0};
	user[name] = t;		//hash setting
	
	return true;
}

// ���캯��  8�����档ÿ��������4���ŵ���ÿ���ŵ��ֻ��ֳ�4�������̿顣
StorageManager::StorageManager() : available_sectors(128) {
    bitmap.resize(8, vector<int>(16, 0));	// ����8*16�����̿� 
    for (int j = 0; j < 16; ++j) {
        bitmap[0][j] = 1;  // Ԥ����һ����Ϊϵͳ�ļ��������治����
    }
}

// �����ļ�����
bool StorageManager::createFile(const string& name, int size, int protection) {
    if (files.find(name) != files.end()) {	// ����ļ����ֲ���ĩβ�����ͻ 
        cout << "�����ļ�����: �ļ��Ѵ��ڡ�"<< endl;
        return false;
    }	
    if (size > available_sectors) {		// ����ļ���С���ڿ���������С 
        cout << "�����ļ�����: ���̿ռ䲻�㡣"<< endl;
        return false;
    }
    
	// �������ļ���FCB 
    FileInfo newFile{ name, size, {} , protection};
    
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
        cout <<"�ļ�'"<< pair.first << "'�Ĵ�С: " << pair.second.file_size << endl;
        fileDetails(pair.second);
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
        cout << "ɾ���ļ�����: �ļ������ڡ�"<< endl;
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
    
    cout << "�ļ� '" << name << "' ��ɾ����"<< endl;
    return true;
}

// �˵�����
void menu(StorageManager manager){
    cout << "����������ʾѡ�����: " << endl;
    cout << "0:�˳�ϵͳ��1:�����ļ�, 2:ɾ���ļ�" << endl;
    cout << "4:���ļ���5:��ȡ�ļ�, 6:д���ļ�, 7:�ر��ļ�" << endl;
    cout << "3:����λʾͼ��8���û��ļ��򿪱�"<<endl; 
}

// ������
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
        	// 1�������ļ� 
            case 1:
                cout << "�����ļ���: ";
                cin >> filename;
                while (1){
                	cout << "�����ļ���С: ";
                	cin >> filesize;
                	if(filesize<=0){
                		cout<<"�ļ���С����Ϊ�������������룡"<<endl; 
					}
					else{
						break;
					}
				}
                while (1) {
                	cout << "�����ļ�������ʽ��";
                	cin >> protection;
                	if(protection == 1 or protection == 2 or protection == 3){
                		break;
					}
					else{
						cout<<"�ļ�������ʽ����������������룡"<<endl;
					}
				}
                if (manager.createFile(filename, filesize, protection)) {
                    cout << "�ļ������ɹ���"<<endl;
                }
                break;
            //2��ɾ���ļ� 
            case 2:
                cout << "����Ҫɾ�����ļ���: ";
                cin >> filename;
                if (manager.deleteFile(filename)) {
                    cout << "�ļ�ɾ���ɹ���"<<endl;
                }
                break;
            //3������λʾͼ 
            case 3:
            	manager.displayStatus();
            	break;
            //4:���ļ�
            case 4:
            	cout << "����Ҫ�򿪵��ļ���: ";
                cin >> filename;
                while(1){
                	cout<<"�������ļ�������ʽ��";
					cin >> protection;
					if(protection != 1 and protection != 2 and protection != 3){
						cout<<"�ļ�������ʽ����������1��2��3�е�����һ����"<<endl;
					}
					else {
						break;
					}
				} 
                if (manager.openFile(filename,protection)) {
                    cout << "�ļ��򿪳ɹ���"<<endl;
                }
            	break;
            //5:��ȡ�ļ�
            case 5:
            	cout<<"����Ҫ��ȡ���ļ�����";
            	cin>>filename;
            	while(1){
            		cout<<"����Ҫ��ȡ���ֽ�����";
            		cin>>byte;
            		if(byte <= 0){
                		cout<<"��ȡ�ֽڴ�С����Ϊ�������������룡"<<endl; 
					}
					else{
						break;
					}
				}
				if(manager.readFile(filename, byte)){
            		cout<<"�ļ���ȡ�ɹ���"<<endl;
				}
            	break;
            //6:д���ļ�
            case 6:
            	cout<<"����Ҫд����ļ�����";
            	cin>>filename;
            	while(1){
            		cout<<"����Ҫд����ֽ�����";
            		cin>>byte;
            		if(byte <= 0){
                		cout<<"д���ֽڴ�С����Ϊ�������������룡"<<endl; 
					}
					else{
						break;
					}
				}
            	if(manager.writeFile(filename, byte)){
            		cout<<"�ļ�д��ɹ���"<<endl;
				}
            	break;
            //7:�ر��ļ�
            case 7:
            	cout << "����Ҫ�رյ��ļ���: ";
                cin >> filename;
                if(manager.closeFile(filename)){
                	cout << "�ļ��رճɹ���"<<endl;
				}
            	break;
            //8:UOF
			case 8:
				manager.displayUOF(); 
				break; 
            case 0:
                cout << "�����˳�����лʹ�á�"<<endl;
                flag = 0;
                break;
            default:
                cout << "��Ч�����ֲ��������������롣"<<endl;
                break;
        }
        
        if(!flag){
        	break;
		}
    }
	
    return 0;
}

