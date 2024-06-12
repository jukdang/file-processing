#include <iostream>
#include "assignment1.h"

using namespace std;

int main(){
    printf("## Start Main.\n");

    char* iname = new char[20];
    unsigned* iID = new unsigned;
    float* iscore = new float;
    char* idept = new char[10];


    // #1 insert test 1
    printf("\n\n############# #1 insert test 1 ###########\n");
    for(int i=0; i<300; i++){
        sprintf(iname, "namenamenamenamename");
        *iID = i;
        *iscore = i + 0.1;
        sprintf(idept, "deptdeptdeptd");
        
        if(insert(iname, iID, iscore, idept)){
            printf("Success: %d inserted.\n", i);
        }
        else{
            printf("Fail: %d already exist.\n", i);
        }
    }


    // #2 duplicate insert test 1
    printf("\n\n############# #2 duplicate insert test 1 ###########\n");
    for(int i=0; i<300; i++){
        sprintf(iname, "namenamenamenamename");
        *iID = i;
        *iscore = i + 0.1;
        sprintf(idept, "deptdeptdeptd");
        
        if(insert(iname, iID, iscore, idept)){
            printf("Success: %d inserted.\n", i);
        }
        else{
            printf("Fail: %d already exist.\n", i);
        }
    }


    // #3 read test 1
    printf("\n\n############# #3 read test 1 ###########\n");
    for(int i=0; i<400; i++){
        if(read(i ,iname, iID, iscore, idept)){
            printf("Success: %dth element: %s, %d, %f, %s\n",i, iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: %dth element not exist\n", i);
        }   
    }


    // #4 search test 1
    printf("\n\n############# #4 search test 1 ###########\n");
    for(int i=0; i<400; i++){
        *iID = i;
        if(search(iname, *iID, iscore, idept)){
            printf("Success: searched: %s, %d, %f, %s\n", iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: ID==%d not exist\n", *iID);
        }   
    }


    // #5 drop test 1
    printf("\n\n############# #5 drop test 1 ###########\n");
    for(int i=0;i<150;i++){
        *iID = (i*2)+1; 
        if(drop(iname, *iID , iscore, idept)){
            printf("Success: %d droppped.\n", *iID);
        }
        else{
            printf("Fail: %d drop failed.\n", *iID);
        }
    }


    // #6 read test 2
    printf("\n\n############# #6 read test 2 ###########\n");
    for(int i=0; i<400; i++){
        if(read(i ,iname, iID, iscore, idept)){
            printf("Success: %dth element: %s, %d, %f, %s\n",i, iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: %dth element not exist\n", i);
        }   
    }


    // #7 search test 2
    printf("\n\n############# #7 search test 2 ###########\n");
    for(int i=0; i<400; i++){
        *iID = i;
        if(search(iname, *iID, iscore, idept)){
            printf("Success: searched: %s, %d, %f, %s\n", iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: ID==%d not exist\n", *iID);
        }   
    }

    // #8 insert test 2
    printf("\n\n############# #8 insert test 2 ###########\n");
    for(int i=200; i<600; i++){
        sprintf(iname, "namenamenamenamename");
        *iID = i;
        *iscore = i + 0.1;
        sprintf(idept, "deptdeptdeptd");

       if(insert(iname, iID, iscore, idept)){
            printf("Success: %d inserted.\n", i);
        }
        else{
            printf("Fail: %d already exist.\n", i);
        }
    }

    // #9 read test 3
    printf("\n\n############# #9 read test 3 ###########\n");
    for(int i=0; i<700; i++){
        if(read(i ,iname, iID, iscore, idept)){
            printf("Success: %dth element: %s, %d, %f, %s\n",i, iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: %dth element not exist\n", i);
        }   
    }


    // #10 search test 3
    printf("\n\n############# #10 search test 3 ###########\n");
    for(int i=0; i<700; i++){
        *iID = i;
        if(search(iname, *iID, iscore, idept)){
            printf("Success: searched: %s, %d, %f, %s\n", iname, *iID, *iscore, idept);
        }
        else{
            printf("Fail: ID==%d not exist\n", *iID);
        }   
    }
    

    printf("## End Main.\n\n");
    return 0;

}