int ReadNumberOfProblems(char* filename);
int* ReadProblems(char* filename, int numberOfNodes, int numberOfProblems);
void ReadCurrentStatus(int rank, int numberOfNodes, int* currentTask, int* firstIndex, int* secondIndex);
bool AllStatusFilesExist(int worldSize, int numberOfNodes);
bool* InitialiseIsSolvedStatusFromFile(int numberOfNodes, int numberOfProblems);