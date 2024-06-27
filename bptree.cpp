#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>
using namespace std;

#define MAX_BLOCK_SIZE 4096
#define RECORD_SIZE 38
#define EMPTY_RECORD 0x0

#define NODE_MAX_SIZE 510

#define INTER_BLOCK_GAP_SIZE 30
#define INTER_BLOCK_GAP "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"

unordered_map<int, int> hash_table;
int root_node_blockNum = 0;

struct Node
{
    bool is_leaf; // 4B
    int size; // 4B
    float values[NODE_MAX_SIZE]; // 4*510=2040B
    int blockNum[NODE_MAX_SIZE+1]; // 4*511=2044B
    int nextNode; // 4B

    Node(bool leaf = true) : is_leaf(leaf), size(0), nextNode(-1)
    {
        fill_n(values, NODE_MAX_SIZE, 0.0);
        fill_n(blockNum, NODE_MAX_SIZE+1, -1.0);
    };
}; // 4096B


bool insert(char *name, unsigned *ID, float *score, char *dept);
bool searchByID(char* name, unsigned ID, float *score, char* dept);
bool searchByScore(unsigned *number, unsigned *ID, float scoreMin, float scoreMax);

bool bptreeInsert(float *score, int blockIdx);

bool insert(char *name, unsigned *ID, float *score, char *dept)
{
    char writeBuf[38];
    memcpy(writeBuf, name, 20);
    memcpy(writeBuf + 20, (char *)ID, 4);
    memcpy(writeBuf + 24, (char *)score, 4);
    memcpy(writeBuf + 28, dept, 10);


    // check if there exist Student.dat;
    ifstream isopen("./data/Student.dat");
    if (!isopen.is_open())
    {
        ofstream makefile("./data/Student.dat", ios::out | ios::binary);
        
        // insert record
        makefile.write(writeBuf, 38);

        // add dynamic hash
        hash_table[*ID] = 0 + 1;

        // insert b+-tree
        ofstream makeindexfile("./data/Score.idx", ios::out | ios::binary);
        Node* node = new Node(true);
        node->values[0] = *score;
        node->blockNum[0] = 0;
        node->size = 1;
        makeindexfile.write(reinterpret_cast<char*>(node), sizeof(*node));
        makeindexfile.close();

        makefile.close();
        return true;
    }
    isopen.close();

    // check if there exist same ID record using dynamic hash
    if(hash_table[*ID] != 0)
    {
        return false;
    }

    // insert record
    int readBlockSize;
    int blockIdx = 0;
    fstream file("./data/Student.dat", ios::in | ios::out | ios::binary);

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
                file.clear();
                file.seekp(blockIdx * MAX_BLOCK_SIZE + i, ios::beg);
                // insert record
                file.write(writeBuf, 38);
                // add dynamic hash
                hash_table[*ID] = blockIdx + 1;
                // add bptree
                bptreeInsert(score, blockIdx);

                file.close();
                return true;
            }
        }
        file.read(buf, MAX_BLOCK_SIZE);
        readBlockSize = file.gcount();
        blockIdx += 1;
    }

    // file status can be fail when trying to read over remain file size
    file.clear();

    // find empty space in last block & if empty space exist, insert there 
    for (int i = 0; i < readBlockSize; i += RECORD_SIZE)
    {
        if (*(buf + i) == EMPTY_RECORD)
        {
            file.seekp(blockIdx * MAX_BLOCK_SIZE + i, ios::beg);
            // insert record
            file.write(writeBuf, 38);
            // add dynamic hash
            hash_table[*ID] = blockIdx + 1;
            // add bptree
            bptreeInsert(score, blockIdx);

            file.close();
            return true;
        }
    }

    // insert record to end of file (no empty space exist)
    // last block is not enough to insert
    if (readBlockSize >= MAX_BLOCK_SIZE - INTER_BLOCK_GAP_SIZE)
    {
        // add inter-block-gap
        file.seekp(0, ios::end);
        file.write(INTER_BLOCK_GAP, INTER_BLOCK_GAP_SIZE); 
        blockIdx += 1;
    }

    file.seekp(0, ios::end);
    // insert record
    file.write(writeBuf, 38);
    // add dynamic hash
    hash_table[*ID] = blockIdx + 1;
    // add bptree
    bptreeInsert(score, blockIdx);

    file.close();
    return true;
};


// searchByID using Hash Table
bool searchByID(char* name, unsigned ID, float *score, char* dept){
    int blockIdx = hash_table[ID] - 1;
    if(blockIdx == -1)
    {
        return false;
    }

    char buf[MAX_BLOCK_SIZE];
    
    fstream file("./data/Student.dat", ios::in | ios::binary);
    file.seekg(blockIdx * MAX_BLOCK_SIZE);
    file.read(buf, MAX_BLOCK_SIZE);
    int readBlockSize = file.gcount();
    file.close();

    int recordID;
    for (int i = 0; i < readBlockSize; i += RECORD_SIZE)
    {
        memcpy(&recordID, buf+i+20, sizeof(int));
        if (recordID == ID)
        {
            memcpy(name, buf + i, 20);
            memcpy(score, buf + i + 24, sizeof(float));
            memcpy(dept, buf + i + 28, 10);
            return true;
        }
    }

    return false;
}

// searchByScore using B+-tree
bool searchByScore(unsigned *number, unsigned *ID, float scoreMin, float scoreMax){

    *number = 0;
    Node* node = new Node();
    // root_node_blockNum = 2; // in test dataset

    fstream idxfile("Score.idx", ios::in | ios::binary);
    idxfile.seekg(root_node_blockNum * MAX_BLOCK_SIZE, ios::beg);
    idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));


    int k=0;
    while(!node->is_leaf) // traverse to leaf
    {
        k = lower_bound(node->values, node->values + node->size, scoreMin) - node->values;
        idxfile.seekg(node->blockNum[k] * MAX_BLOCK_SIZE, ios::beg);
        idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));
    }
    k = lower_bound(node->values, node->values + node->size, scoreMin) - node->values;
    
    set<int> blockToSearch;
    
    while(k < node->size && node->values[k] <= scoreMax)
    {
        blockToSearch.insert(node->blockNum[k]); //search here
        k++;
    }
    while (node->nextNode != -1)
    {
        idxfile.seekg(node->nextNode * MAX_BLOCK_SIZE, ios::beg);
        idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));
        k=0;
        while(k < node->size && node->values[k] <= scoreMax)
        {
            blockToSearch.insert(node->blockNum[k]); //search here
            k++;
        }
        if(k < node->size && node->values[k] > scoreMax)
        {
            break;
        }
    }
    
    if(blockToSearch.empty())
    {
        idxfile.close();
        return false;
    }

    fstream file("./data/Student.dat", ios::in | ios::binary);
    char buf[MAX_BLOCK_SIZE];
    int readBlockSize;
    float recordscore;
    
    for(auto it = blockToSearch.begin(); it!= blockToSearch.end(); it++)
    {
        file.seekg(*it * MAX_BLOCK_SIZE, ios::beg);
        file.read(buf, MAX_BLOCK_SIZE);
        readBlockSize = file.gcount();
        for (int i = 0; i < readBlockSize; i += RECORD_SIZE)
        {
            memcpy(&recordscore, buf+i+24, sizeof(float));
            
            if (scoreMin <= recordscore && recordscore <= scoreMax)
            {
                memcpy(ID+*number, buf+i+20, sizeof(int));
                
                *number += 1;
            }
        }
        file.clear();
    }

    file.close();
    idxfile.close();

    return true;
}



// B+-tree Insert 
bool bptreeInsert(float *score, int blockIdx)
{
    Node* node = new Node();
    
    fstream idxfile("./data/Score.idx", ios::in | ios::out | ios::binary);
    int idxblockNum = root_node_blockNum;
    idxfile.seekg(idxblockNum * MAX_BLOCK_SIZE, ios::beg);
    idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));

    vector<int> traverse;
    traverse.push_back(idxblockNum);

    int k = 0;
    while(!node->is_leaf) // traverse to leaf
    {
        k = lower_bound(node->values, node->values + node->size, *score) - node->values;
        idxblockNum = node->blockNum[k];
        idxfile.seekg(idxblockNum * MAX_BLOCK_SIZE, ios::beg);
        idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));
        traverse.push_back(idxblockNum);
        
    }

    // insert into leaf
    traverse.pop_back();
    k = lower_bound(node->values, node->values + node->size, *score) - node->values;
    for(int j = node->size; j > k ; j--)
    {
        node->values[j] = node->values[j-1];
        node->blockNum[j] = node->blockNum[j-1];
    }
    node->values[k] = *score;
    node->blockNum[k] = blockIdx;
    node->size += 1;
    
    idxfile.seekp(idxblockNum * MAX_BLOCK_SIZE, ios::beg);
    idxfile.write(reinterpret_cast<char*>(node), sizeof(*node));


        
    // split occured
    while(node->size == NODE_MAX_SIZE)
    {
        float mediumValue = node->values[NODE_MAX_SIZE/2];
        int left_idxblockNum = idxblockNum;
        int right_idxblockNum = 0;
        if(node->is_leaf)
        {    
            Node* node_right = new Node(true);
            memcpy(node_right->values, node->values + NODE_MAX_SIZE / 2, (NODE_MAX_SIZE / 2) * sizeof(*(node->values)));
            memcpy(node_right->blockNum, node->blockNum + NODE_MAX_SIZE / 2, (NODE_MAX_SIZE / 2 + 1) * sizeof(*(node->blockNum)));
            fill_n(node->values + NODE_MAX_SIZE/2, NODE_MAX_SIZE/2, 0.0);
            fill_n(node->blockNum + NODE_MAX_SIZE/2, NODE_MAX_SIZE/2 + 1, -1.0);
            node->size = NODE_MAX_SIZE/2;
            node_right->size = NODE_MAX_SIZE/2;

            idxfile.seekp(0, ios::end);
            right_idxblockNum = idxfile.tellp() / MAX_BLOCK_SIZE;

            node_right->nextNode = node->nextNode;
            node->nextNode = right_idxblockNum;
            
            
            idxfile.write(reinterpret_cast<char*>(node_right), sizeof(*node_right));
            idxfile.seekp(left_idxblockNum * MAX_BLOCK_SIZE, ios::beg);
            idxfile.write(reinterpret_cast<char*>(node), sizeof(*node));

        }
        else
        {
            Node* node_right = new Node(false);
            memcpy(node_right->values, node->values + NODE_MAX_SIZE / 2 + 1, (NODE_MAX_SIZE / 2 - 1) * sizeof(*(node->values)));
            memcpy(node_right->blockNum, node->blockNum + NODE_MAX_SIZE / 2 + 1, (NODE_MAX_SIZE / 2) * sizeof(*(node->blockNum)));
            fill_n(node->values + NODE_MAX_SIZE/2, NODE_MAX_SIZE/2, 0.0);
            fill_n(node->blockNum + NODE_MAX_SIZE/2 + 1, NODE_MAX_SIZE/2 - 1, -1.0);
            node->size = NODE_MAX_SIZE/2;
            node_right->size = NODE_MAX_SIZE/2 - 1;

            idxfile.seekp(0, ios::end);
            right_idxblockNum = idxfile.tellp() / MAX_BLOCK_SIZE;
            
            node_right->nextNode = node->nextNode;
            node->nextNode = right_idxblockNum;
            
            idxfile.write(reinterpret_cast<char*>(node_right), sizeof(*node_right));
            idxfile.seekp(left_idxblockNum * MAX_BLOCK_SIZE, ios::beg);
            idxfile.write(reinterpret_cast<char*>(node), sizeof(*node));

        }
        
        // insert into parent node
        if(traverse.empty())
        {
            node = new Node(false);
            idxfile.seekp(0, ios::end);
            root_node_blockNum = idxfile.tellp() / MAX_BLOCK_SIZE;
            
        }
        else
        {
            idxblockNum = traverse.back();
            traverse.pop_back();
            idxfile.seekg(idxblockNum * MAX_BLOCK_SIZE, ios::beg);
            idxfile.read(reinterpret_cast<char*>(node), sizeof(*node));
            idxfile.seekp(idxblockNum * MAX_BLOCK_SIZE, ios::beg);
        }
        
        k = lower_bound(node->values, node->values + node->size, mediumValue) - node->values;
        for(int j = node->size; j > k ; j--)
        {
            node->values[j] = node->values[j-1];
            node->blockNum[j+1] = node->blockNum[j];
        }
        node->values[k] = mediumValue;
        node->blockNum[k] = left_idxblockNum;
        node->blockNum[k+1] = right_idxblockNum;
        node->size += 1;

        idxfile.write(reinterpret_cast<char*>(node), sizeof(*node));
    }

    idxfile.close();
    // end of insert b+-tree

    return true;
}