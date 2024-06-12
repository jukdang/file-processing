#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define MAX_BLOCK_SIZE 4096
#define RECORD_SIZE 38
#define EMPTY_RECORD 0x0
#define BLOCKING_FACTOR 105

#define INTER_BLOCK_GAP_SIZE 30
#define INTER_BLOCK_GAP "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"

bool insert(char *name, unsigned *ID, float *score, char *dept);
bool searchByID(char* name, unsigned ID, float *score, char* dept);
bool searchByScore(unsigned *number, unsigned *ID, float scoreMin, float scoreMax);

bool insert(char *name, unsigned *ID, float *score, char *dept)
{
    // check if there exist student.dat;
    fstream isopen("Student.dat", ios::in | ios::out | ios::binary);
    if (!isopen.is_open())
    {
        ofstream makefile("Student.dat", ios::out | ios::binary);
        // makefile.write(writeBuf, RECORD_SIZE);
        makefile.close();
        makefile.close();
        isopen.close();
        return true;
    }

    // check if there exist same ID record;
    // using dynamic hash


    // insert record
    int readBlockSize;
    int blockIdx = 0;
    fstream file("Student.dat", ios::in | ios::out | ios::binary);

    // find empty space before last block & if empty space exist, insert there 
    char buf[MAX_BLOCK_SIZE];
    file.read(buf, MAX_BLOCK_SIZE);
    readBlockSize = file.gcount();
    while (readBlockSize >= MAX_BLOCK_SIZE)
    {
        for (int i = 0; i < readBlockSize; i += RECORD_SIZE)
        {

            if (*(buf + i) == EMPTY_RECORD)
            {
                file.seekp(blockIdx * MAX_BLOCK_SIZE + i, ios_base::beg);
                // fbinout.write(writeBuf, RECORD_SIZE);
                file.close();
                return true;
            }
        }
        file.read(buf, MAX_BLOCK_SIZE);
        readBlockSize = file.gcount();
        blockIdx += 1;
    }
    // find empty space in last block & if empty space exist, insert there 
    for (int i = 0; i < readBlockSize; i += RECORD_SIZE)
    {
        if (*(buf + i) == EMPTY_RECORD)
        {
            file.seekp(blockIdx * MAX_BLOCK_SIZE + i, ios_base::beg);
            // fbinout.write(writeBuf, RECORD_SIZE);
            file.close();
            return true;
        }
    }

    // insert record to end of file (no empty space exist)

    // last block is not enough to insert
    if (readBlockSize >= MAX_BLOCK_SIZE - INTER_BLOCK_GAP_SIZE)
    {
        // add inter-block-gap
        file.seekp(0, ios_base::end);
        file.write(INTER_BLOCK_GAP, INTER_BLOCK_GAP_SIZE); 
    }

    file.seekp(0, ios_base::end);
    // fbinout.write(writeBuf, RECORD_SIZE);
    file.close();
    return true;
};


bool searchByID(char* name, unsigned ID, float *score, char* dept){

}

bool searchByScore(unsigned *number, unsigned *ID, float scoreMin, float scoreMax){

}