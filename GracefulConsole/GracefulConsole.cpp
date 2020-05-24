#include <stdlib.h>
#include <windows.h>
#include "../Graceful/Graceful.h"
#include "../Graceful/Reader.h"
#include "mpi.h"
#include "stdio.h"
#include <time.h>

#pragma region Initialise
bool* InitialiseIsSolvedStatus(int numberOfProblems)
{
	bool* isSolved = new bool[numberOfProblems];
	for (int i = 0; i < numberOfProblems; i++)
	{
		isSolved[i] = false;
	}
	return isSolved;
}
#pragma endregion

#pragma region LeaderMethods
void LeaderSendProblem(int* arcs, int firstIndex, int secondIndex, int numberOfNodes, int* currentTask, int destination)
{
	bool terminate = false;
	int* currentArcs = &arcs[*currentTask * (numberOfNodes - 1)];
	MPI_Send(&terminate  , 1                , MPI_C_BOOL, destination, 0, MPI_COMM_WORLD);
	MPI_Send(currentTask , 1                , MPI_INT   , destination, 1, MPI_COMM_WORLD);
	MPI_Send(currentArcs , numberOfNodes - 1, MPI_INT   , destination, 2, MPI_COMM_WORLD);
	MPI_Send(&firstIndex , 1                , MPI_INT   , destination, 3, MPI_COMM_WORLD);
	MPI_Send(&secondIndex, 1                , MPI_INT   , destination, 4, MPI_COMM_WORLD);
}

void LeaderRecieveResult(bool* isSolved, MPI_Status* status, int numberOfNodes)
{
	int taskNumberForResult;
	int* solution = new int[numberOfNodes];

	MPI_Recv(&taskNumberForResult, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, status);
	MPI_Recv(&isSolved[taskNumberForResult], 1, MPI_C_BOOL, status->MPI_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Recieved result %d from rank %d solving problem %d\n", isSolved[taskNumberForResult], status->MPI_SOURCE, taskNumberForResult);

	if (!isSolved[taskNumberForResult]) return;

	MPI_Recv(solution, numberOfNodes, MPI_INT, status->MPI_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for (int i = 0; i < numberOfNodes; i++)
		printf("%d ", solution[i]);
	printf("\n");
}

void LeaderSendOutInitialProblems(int* arcs, int firstIndex, int secondIndex, int numberOfNodes, int* currentTask, int* pendingTasks, int* sentTasks, int worldSize)
{
	for (*currentTask = 0; *currentTask < worldSize - 1; (*currentTask)++)
	{
		LeaderSendProblem(arcs, firstIndex, secondIndex, numberOfNodes, currentTask, (*currentTask) + 1);
		(*pendingTasks)++;
		(*sentTasks)++;
	}
}

void LeaderSendOutNewTasksWhenRequestedUntilTheWorkIsDone(int* arcs, int firstIndex, int secondIndex, int numberOfNodes, int numberOfProblems, int* currentTask, int* pendingTasks, int* sentTasks)
{
	bool* isSolved = InitialiseIsSolvedStatus(numberOfNodes);

	MPI_Status status;
	while (true)
	{
		LeaderRecieveResult(isSolved, &status, numberOfNodes);
		if (*sentTasks < numberOfProblems)
		{
			LeaderSendProblem(arcs, firstIndex, secondIndex, numberOfNodes, currentTask, status.MPI_SOURCE);
			(*currentTask)++;
			(*sentTasks)++;
		}
		else
		{
			(*pendingTasks)--;
		}
		if (*pendingTasks == 0)
			break;
	}
}

void LeaderSendOutTerminationRequest(int worldSize)
{
	bool terminate = true;
	for (int i = 1; i < worldSize; i++)
	{
		MPI_Send(&terminate, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
	}
}

void ExecuteLeaderTasks(int worldSize)
{
	int numberOfNodes = 7;
	int numberOfProblems = 11;

	if (numberOfProblems + 1 < worldSize)
	{
		printf("The number of processes must be at most 1 more than the number of problems.");
		LeaderSendOutTerminationRequest(worldSize);
		return;
	}

	char filename[100] = "..\\TestFiles\\trees_7.csv";
	int* arcs = ReadProblems(filename, 7);

	int pendingTasks = 0;
	int sentTasks = 0;
	int currentTask = 0;

	LeaderSendOutInitialProblems(arcs, 0, 1, numberOfNodes, &currentTask, &pendingTasks, &sentTasks, worldSize);
	LeaderSendOutNewTasksWhenRequestedUntilTheWorkIsDone(arcs, 0, 1, numberOfNodes, numberOfProblems, &currentTask, &pendingTasks, &sentTasks);
	LeaderSendOutTerminationRequest(worldSize);
}
#pragma endregion

#pragma region HelperMethods
void HelperRecieveProblem(int* currentTask, int* currentArcs, int* firstIndex, int* secondIndex, int numberOfNodes)
{
	MPI_Recv(currentTask, 1                , MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(currentArcs, numberOfNodes - 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(firstIndex , 1                , MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(secondIndex, 1                , MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void HelperSendResult(int* currentTask, bool* isSolved, int* solution, int numberOfNodes)
{
	MPI_Send(currentTask, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	MPI_Send(isSolved, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD);

	if (*isSolved)
		MPI_Send(solution, numberOfNodes, MPI_INT, 0, 2, MPI_COMM_WORLD);
}

bool TerminationCodeHasBeenRecieved()
{
	bool terminate;
	MPI_Recv(&terminate, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return terminate;
}

void ExecuteHelperTasks(int rank)
{
	int numberOfNodes = 7;
	
	int currentTask;
	int* currentArcs = new int[numberOfNodes];
	int firstIndex;
	int secondIndex;

	int* solution = new int[numberOfNodes];
	bool hasBeenSolved = false;
	bool isSolved = false;

	while (!TerminationCodeHasBeenRecieved())
	{
		HelperRecieveProblem(&currentTask, currentArcs, &firstIndex, &secondIndex, numberOfNodes);

		printf("Rank %d solving problem %d\n", rank, currentTask);
		solution = Solve(firstIndex, secondIndex, currentArcs, numberOfNodes, &hasBeenSolved);

		HelperSendResult(&currentTask, &hasBeenSolved, solution, numberOfNodes);
	}
}
#pragma endregion

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank;
	int worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		ExecuteLeaderTasks(worldSize);
	}

	else
	{
		ExecuteHelperTasks(rank);
	}

	MPI_Finalize();
}
