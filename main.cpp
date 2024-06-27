#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>

#include "bptree.h"

using namespace std;

#define NumberRecords   20000
#define NumberQueries   100

class Student {
public:
    string      name;
    unsigned    ID;
    float       score;
    string      dept;
};

int main()
{
    ifstream    ifs("./data/dataset.txt", ios::in);
    unsigned    ID;
    string      name;
    float       score;
    string      dept;
    char        cname[21];
    char        cdept[11];
    Student  stud[NumberRecords];
    clock_t     ts, te;
    double      durationInsertion;


    // system("del ./data/student.dat");
    // system("del ./data/Score.idx");
    // system("del ./data/IDAnswer-gen.txt");
    // system("del ./data/ScoreAnswer-gen.txt");

    // insert records

    for(int i=0;i<NumberRecords;i++) {
        ifs>>ID>>name>>score>>dept;
        stud[i].ID=ID; stud[i].name=name; stud[i].dept=dept; stud[i].score=score;
    }

    ts=clock();

    for(int i=0;i<NumberRecords;i++) {
        strcpy(cname,stud[i].name.c_str());
        strcpy(cdept,stud[i].dept.c_str());
        if(insert(cname,&stud[i].ID,&stud[i].score,cdept)!=true) cout<<"In Insert "<<i<<": Error\n";

    }

    ifs.close();

    te=clock();
    durationInsertion=(double)(te-ts)/CLOCKS_PER_SEC;
    cout<<"Insertion time: "<<durationInsertion<<endl;

    // search by ID

    ts=clock();

    ifstream     ifs_ID_Query("./data/IDQuery.txt", ios::in);
    fstream     ifs_Score_Query2("./data/IDAnswer-gen.txt", ios::out);
    for(int i=0;i<NumberQueries;i++) {
        ifs_ID_Query>>ID;
        if(searchByID(cname, ID, &score, cdept)){
            ifs_Score_Query2<<ID<<" "<<cname<<" "<<score<<" "<<cdept<<endl;
        } 
        else cout<<"ID: not found\n";
        

    }

    ifs_ID_Query.close();
    ifs_Score_Query2.close();

    te=clock();
    float durationSearchByID=(double)(te-ts)/CLOCKS_PER_SEC;
    cout<<"search by ID time: "<<durationSearchByID<<endl;

    ts=clock();

    // search by score

    ifstream     ifs_Score_Query("./data/ScoreQuery.txt", ios::in);
    fstream     ifs_Score_Query3("./data/ScoreAnswer-gen.txt", ios::out);

    float        scoreMin, scoreMax;
    unsigned     number;
    unsigned     IDs[NumberRecords];
    for(int i=0;i<NumberQueries;i++) {
        ifs_Score_Query>>scoreMin>>scoreMax;
        
        ifs_Score_Query3<<i<<endl;
        if(searchByScore(&number, IDs, scoreMin, scoreMax)){
            for(unsigned m=0;m<number;m++) ifs_Score_Query3<<IDs[m]<<endl;
        }
        else cout<<"ID: not found\n";
    }

    ifs_ID_Query.close();
    ifs_Score_Query3.close();

    te=clock();
    float durationSearchByScore=(double)(te-ts)/CLOCKS_PER_SEC;
    cout<<"search by score time: "<<durationSearchByScore<<endl;

    float total_time=durationInsertion+durationSearchByID+durationSearchByScore;
    cout<<"total time: "<<total_time<<endl;
    return 0;

}

