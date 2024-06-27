bool insert(char *name, unsigned *ID, float *score, char *dept);
bool searchByID(char* name, unsigned ID, float *score, char* dept);
bool searchByScore(unsigned *number, unsigned *ID, float scoreMin, float scoreMax);