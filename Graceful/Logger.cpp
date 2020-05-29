#include "stdio.h"
#include "string"
#include "time.h"

void LogSolution(int* solution, int numberOfNodes)
{
	for (int i = 0; i < numberOfNodes; i++)
		printf("%d ", solution[i]);
	printf("\n");
}

void LogSolutionToFile(int currentTask, int* solution, int numberOfNodes)
{
	FILE* solutionsFile;
	char outputFileName[32];
	sprintf_s(outputFileName, "..\\Logs\\solutions_%d.txt", numberOfNodes);
	fopen_s(&solutionsFile, outputFileName, "a");
	
	fprintf(solutionsFile, "%d,", currentTask);

	for (int i = 0; i < numberOfNodes - 1; i++)
		fprintf(solutionsFile, "%d,", solution[i]);
	fprintf(solutionsFile, "%d\n", solution[numberOfNodes - 1]);

	fclose(solutionsFile);
}

void LogReceiptOfResult(bool isSolved, int source, int taskNumber)
{
	printf("Recieved result %d from rank %d solving problem %d\n", isSolved, source, taskNumber);
}

void LogNumberOfSolvedProblems(int numberOfSolvedProblems, int numberOfProblems)
{
	printf("Solved %d of %d problems\n", numberOfSolvedProblems, numberOfProblems);
}

void LogSendingOfProblemToProcess(int firstIndex, int secondIndex, int numberOfNodes, int currentTask, int destination)
{
	FILE* statusFile;

	char statusFileName[32];
	sprintf_s(statusFileName, "..\\Logs\\status_%d_%d.txt", numberOfNodes, destination);

	fopen_s(&statusFile, statusFileName, "w");

	fprintf(statusFile, "%d %d %d", currentTask, firstIndex, secondIndex);
	fclose(statusFile);
}

void LogNumberOfSolvedProblemsToFile(int currentTask, int numberOfSolvedProblems, int numberOfProblems, int numberOfNodes)
{
	FILE* statusFile;

	char statusFileName[32];
	sprintf_s(statusFileName, "..\\Logs\\status_%d.txt", numberOfNodes);

	fopen_s(&statusFile, statusFileName, "a");

	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	char timeString[32];
	asctime_s(timeString, &timeinfo);
	str:

	fprintf(statusFile, "Solved problem %d, %d of %d problems %s", currentTask, numberOfSolvedProblems, numberOfProblems, timeString);
	fclose(statusFile);
}

void LogCurrentIndicesToFile(int firstIndex, int secondIndex, int numberOfNodes)
{
	FILE* statusFile;
	
	char statusFileName[32];
	sprintf_s(statusFileName, "..\\Logs\\status_%d.txt", numberOfNodes);

	fopen_s(&statusFile, statusFileName, "a");

	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	char timeString[32];
	asctime_s(timeString, &timeinfo);

	fprintf(statusFile, "Indices have incremented to %d and %d %s", firstIndex, secondIndex, timeString);
	fclose(statusFile);
}

void LogHelperCommencingSolve(int rank, int currentTask, int firstIndex, int secondIndex)
{
	printf("Rank %d solving problem %d, with indices %d and %d\n", rank, currentTask, firstIndex, secondIndex);
}
