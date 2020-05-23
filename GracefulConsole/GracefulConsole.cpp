#include <stdlib.h>
#include <windows.h>
#include "../Graceful/Graceful.h"
#include "../Graceful/Reader.h"
#include "mpi.h"
#include "stdio.h"
#include <time.h>

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

void LeaderRecieveResult(int* result, MPI_Status* status)
{
	MPI_Recv(result, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, status);
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
	int result;
	MPI_Status status;
	while (true)
	{
		LeaderRecieveResult(&result, &status);
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

void HelperSendResult(bool* result)
{
	MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
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
	bool hasBeenSolved = false;

	int currentTask;
	int* currentArcs = new int[numberOfNodes];
	int firstIndex;
	int secondIndex;

	bool result = true;

	while (!TerminationCodeHasBeenRecieved())
	{
		HelperRecieveProblem(&currentTask, currentArcs, &firstIndex, &secondIndex, numberOfNodes);

		printf("Rank %d solving problem %d\n", rank, currentTask);
		Solve(firstIndex, secondIndex, currentArcs, numberOfNodes, &hasBeenSolved);

		HelperSendResult(&result);
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
