#include "stdio.h"

void LogSolution(int* solution, int numberOfNodes)
{
	for (int i = 0; i < numberOfNodes; i++)
		printf("%d ", solution[i]);
	printf("\n");
}

void LogReceiptOfResult(bool isSolved, int source, int taskNumber)
{
	printf("Recieved result %d from rank %d solving problem %d\n", isSolved, source, taskNumber);
}

void LogNumberOfSolvedProblems(int numberOfSolvedProblems, int numberOfProblems)
{
	printf("Solved %d of %d problems\n", numberOfSolvedProblems, numberOfProblems);
}

void LogHelperCommencingSolve(int rank, int currentTask, int firstIndex, int secondIndex)
{
	printf("Rank %d solving problem %d, with indices %d and %d\n", rank, currentTask, firstIndex, secondIndex);
}
