// (1) Record 입력:
bool insert(char* name, unsigned *ID, float *score, char* dept);
// (2) n-번째 Record의 검색:
bool read(int n, char* name, unsigned *ID, float *score, char* dept);
// (3) ID로 검색:
bool search(char* name, unsigned ID, float *score, char* dept);
// (4) ID로 검색한 데이터를 삭제:
bool drop(char* name, unsigned ID, float *score, char* dept);