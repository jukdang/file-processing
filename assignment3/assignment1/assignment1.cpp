#include <iostream>
#include <fstream>
#include <memory.h>

using namespace std;

#define s_FLAG 105

// Student(unsigned ID, char name[20], float score, char dept[10])
// 1*20 + 4 + 4 + 1*10 = 38byte
// one block = 4096
// 105 record 38*105 = 3990
// 105 for flag + 3990 for data + 1 for IBG


bool* block_init(){
    bool* block = new bool[4096];
    memset(block, false, sizeof(bool)*4096);
    return block;
}

void file_exist_check(){
    ifstream f;
    f.open("student.dat");
    if(!f.is_open()) {
        ofstream output;
        output.open("student.dat", fstream::out | fstream::binary);

        bool* block = block_init();
        output.write((char*)block, sizeof(bool)*4096);
        output.close();
    }
        
    f.close();
}


bool insert(char* name, unsigned *ID, float *score, char* dept){
    // return: true – 입력이 성공, false – 입력이 실패
    // 입력되는 데이터는 각 parameter로 지정하며, ID가 중복되는 것이 있으면 false로 return
    // 만일 처음 입력되는 record이면 student.dat 파일을 생성

    file_exist_check();

    bool* flags = new bool[s_FLAG];

    fstream file("student.dat", ios::in | ios::out | ios::binary);

    long l = file.tellg();
    file.seekg(0, ios::end);
    long m = file.tellg();
    long total_block_size = (m-l)/4096;
    file.seekg(0, ios::beg);

    int b_idx = 0;
    int empty_idx = -1;
    while(b_idx < total_block_size){

        file.read((char*)flags, sizeof(bool)*s_FLAG);        
        int ID_read;
        for(int i=0; i<s_FLAG; i++){
            if(flags[i]){
                file.read((char*)(&ID_read), sizeof(ID_read));
                if(ID_read == *ID){
                    // duplicate
                    //printf("%d is duplicated\n", *ID);
                    file.close();
                    return false;
                }
                file.seekg(38-4, ios::cur);
            }
            else{
                if(empty_idx==-1){
                    empty_idx = b_idx*s_FLAG + i;
                }
                file.seekg(38, ios::cur);
            }
        }
        file.seekg(1, ios::cur);
        b_idx++;
    }

    bool flag = true;
    if(empty_idx!=-1){
        file.seekp(4096*(empty_idx/s_FLAG) + (empty_idx%s_FLAG), ios::beg);
        file.write((char*)(&flag), sizeof(bool));

        file.seekp(4096*(empty_idx/s_FLAG) + s_FLAG + (empty_idx%s_FLAG)*38, ios::beg);
        file.write((char*)(ID), sizeof(*ID));
        file.write(name, sizeof(char)*20);
        file.write((char*)(score), sizeof(*score));
        file.write(dept, sizeof(char)*10);
    }
    else{
        file.seekp(0, ios::end);
        bool* block = block_init();
        file.write((char*)block, sizeof(bool)*4096);

        file.seekp(4096*b_idx, ios::beg);
        file.write((char*)(&flag), sizeof(bool));

        file.seekp(4096*b_idx + s_FLAG, ios::beg);
        file.write((char*)(ID), sizeof(*ID));
        file.write(name, sizeof(char)*20);
        file.write((char*)(score), sizeof(*score));
        file.write(dept, sizeof(char)*10);
    }

    file.close();
    return true;
}

bool read(int n, char* name, unsigned *ID, float *score, char* dept){
    // return: true – 검색이 성공, false – 검색이 실패
    // 검색된 데이터는 나머지 parameter에 지정

    bool* flags = new bool[s_FLAG];

    ifstream file("student.dat", ios::in | ios::binary);
    if(!file){
        file.close();
        return false;
    }

    long l = file.tellg();
    file.seekg(0, ios::end);
    long m = file.tellg();
    long total_block_size = (m-l)/4096;
    file.seekg(0, ios::beg);

    int b_idx = 0;
    while(b_idx < total_block_size){
        file.seekg(b_idx*4096, ios::beg);
        file.read((char*)flags, sizeof(bool)*s_FLAG);
        
        int cnt=0;
        for(int i=0; i<s_FLAG; i++){
            if(flags[i]){
                if(cnt == n){
                    file.seekg(i*38, ios::cur);
                    // read
                    file.read((char*)(ID), sizeof(*ID));
                    file.read(name, sizeof(char)*20);
                    file.read((char*)(score), sizeof(*score));
                    file.read(dept, sizeof(char)*10);

                    file.close();
                    return true;
                }
                cnt++;
            }
        }
        n-=cnt;
        b_idx++;
    }

    file.close();
    return false;
}

bool search(char* name, unsigned ID, float *score, char* dept){
    // return: true – 검색이 성공, false – 검색이 실패
    // 검색한 데이터는 name, score, dept로 반환

    bool* flags = new bool[s_FLAG];

    ifstream file("student.dat", ios::in | ios::binary);
    if(!file){
        file.close();
        return false;
    }

    long l = file.tellg();
    file.seekg(0, ios::end);
    long m = file.tellg();
    long total_block_size = (m-l)/4096;
    file.seekg(0, ios::beg);

    int b_idx = 0;
    while(b_idx < total_block_size){

        file.seekg(b_idx*4096, ios::beg);
        file.read((char*)flags, sizeof(bool)*s_FLAG);        
        int ID_read;

        // for(int i=0; i<s_FLAG; i++){
        //     printf("%d",flags[i]);
        // }
        // printf("\n");

        for(int i=0; i<s_FLAG; i++){
            if(flags[i]){
                file.read((char*)(&ID_read), sizeof(ID_read));
                if(ID_read == ID){ 
                    // read
                    file.read(name, sizeof(char)*20);
                    file.read((char*)(score), sizeof(*score));
                    file.read(dept, sizeof(char)*10);
                    file.close();
                    return true;
                }
                file.seekg(38-4, ios::cur);
            }
            else{
                file.seekg(38, ios::cur);
            }
        }
        b_idx++;
    }

    file.close();
    return false;
}

bool drop(char* name, unsigned ID, float *score, char* dept){
    // return: true – 검색이 성공, false – 검색이 실패
    // 삭제된 자리에는 나중에 입력되는 record로 활용될 수 있어야한다.

    bool* flags = new bool[s_FLAG];

    fstream file("student.dat", ios::in | ios::out | ios::binary);
    if(!file){
        file.close();
        return false;
    }

    long l = file.tellg();
    file.seekg(0, ios::end);
    long m = file.tellg();
    long total_block_size = (m-l)/4096;
    file.seekg(0, ios::beg);

    int b_idx = 0;
    while(b_idx < total_block_size){
        file.read((char*)flags, sizeof(bool)*s_FLAG);        
        int ID_read;

        // for(int i=0; i<s_FLAG; i++){
        //     printf("%d",flags[i]);
        // }
        // printf("\n");

        for(int i=0; i<s_FLAG; i++){
            if(flags[i]){
                file.read((char*)(&ID_read), sizeof(ID_read));
                if(ID == ID_read){
                    // drop
                    file.seekp(4096*b_idx + i, ios::beg);
                    bool flag = false;
                    file.write((char*)(&flag), sizeof(bool));
                    file.close();
                    return true;
                }
                file.seekg(38-4, ios::cur);
            }
            else{
                file.seekg(38, ios::cur);
            }
            
        }
        file.seekg(1, ios::cur);
        b_idx++;
    }


    file.close();
    return false;
}

