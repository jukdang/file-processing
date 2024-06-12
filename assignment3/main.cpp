#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>

using namespace std;

#define NumberRecords   20000
#define NumberQueries   100

bool    insert(char* a, unsigned* b, float* c, char* d) { return true; }
bool    searchByID(char* a, unsigned b, float* c, char* d) {return true; }
bool    searchByScore(unsigned* a, unsigned* b, float c, float d) {return true; }

class LIKStudent {
public:
    string      name;
    unsigned    ID;
    float       score;
    string      dept;
};

int main()
{
    ifstream    ifs("file.txt", ios::in);
    unsigned    ID;
    string      name;
    float       score;
    string      dept;
    char        cname[21];
    char        cdept[11];
    LIKStudent  stud[NumberRecords];
    clock_t     ts, te;
    double      durationInsertion;


    system("del student.dat");

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

    ifstream     ifs_ID_Query("IDQuery.txt", ios::in);
    for(int i=0;i<NumberQueries;i++) {
        ifs_ID_Query>>ID;
        if(!searchByID(cname, ID, &score, cdept)) cout<<"ID: not found\n";
    }

    ifs_ID_Query.close();

    te=clock();
    float durationSearchByID=(double)(te-ts)/CLOCKS_PER_SEC;
    cout<<"search by ID time: "<<durationSearchByID<<endl;

    ts=clock();

    // search by score

    ifstream     ifs_Score_Query("ScoreQuery.txt", ios::in);

    float        scoreMin, scoreMax;
    unsigned     number;
    unsigned     IDs[NumberRecords];
    for(int i=0;i<NumberQueries;i++) {
        ifs_Score_Query>>scoreMin>>scoreMax;
        if(searchByScore(&number, IDs, scoreMin, scoreMax))
            for(unsigned m=0;m<number;m++) cout<<IDs[m]<<endl;
        else cout<<"ID: not found\n";
    }

    te=clock();
    float durationSearchByScore=(double)(te-ts)/CLOCKS_PER_SEC;
    cout<<"search by score time: "<<durationSearchByScore<<endl;

    float total_time=durationInsertion+durationSearchByID+durationSearchByScore;
    cout<<"total time: "<<total_time<<endl;
    return 0;

}

