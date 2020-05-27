void LogSolution(int* solution, int numberOfNodes);
void LogSolutionToFile(int currentTask, int* solution, int numberOfNodes);
void LogReceiptOfResult(bool isSolved, int source, int taskNumber);
void LogNumberOfSolvedProblems(int numberOfSolvedProblems, int numberOfProblems);
void LogNumberOfSolvedProblemsToFile(int currentTask, int numberOfSolvedProblems, int numberOfProblems, int numberOfNodes);
void LogCurrentIndicesToFile(int firstIndex, int secondIndex, int numberOfNodes);
void LogHelperCommencingSolve(int rank, int currentTask, int firstIndex, int secondIndex);
