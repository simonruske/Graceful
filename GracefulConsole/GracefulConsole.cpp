#define LOG_EXECUTION_TIME 1;

#include "../Graceful/Graceful.h"
#include "../Graceful/Reader.h"
#include "../Graceful/Logger.h"
#include <stdlib.h>
#include <windows.h>
#include "mpi.h"
#include "stdio.h"

#if LOG_EXECUTION_TIME
#include <chrono>
#endif


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
	LogSendingOfProblemToProcess(firstIndex, secondIndex, numberOfNodes, *currentTask, destination);

	bool terminate = false;
	int* currentArcs = &arcs[*currentTask * (numberOfNodes - 1)];
	MPI_Send(&terminate  , 1                , MPI_C_BOOL, destination, 0, MPI_COMM_WORLD);
	MPI_Send(currentTask , 1                , MPI_INT   , destination, 1, MPI_COMM_WORLD);
	MPI_Send(currentArcs , numberOfNodes - 1, MPI_INT   , destination, 2, MPI_COMM_WORLD);
	MPI_Send(&firstIndex , 1                , MPI_INT   , destination, 3, MPI_COMM_WORLD);
	MPI_Send(&secondIndex, 1                , MPI_INT   , destination, 4, MPI_COMM_WORLD);
}

void LeaderRecieveResult(bool* isSolved, MPI_Status* status, int* numberOfSolvedProblems, int numberOfNodes, int numberOfProblems)
{
	int taskNumberForResult;
	int* solution = new int[numberOfNodes];

	MPI_Recv(&taskNumberForResult, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, status);
	
	if (isSolved[taskNumberForResult])
	{
		bool dummy;
		int* dummySolution = new int[numberOfNodes];

		MPI_Recv(&dummy, 1, MPI_C_BOOL, status->MPI_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (!dummy) return;

		MPI_Recv(dummySolution, numberOfNodes, MPI_INT, status->MPI_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else
	{
		MPI_Recv(&isSolved[taskNumberForResult], 1, MPI_C_BOOL, status->MPI_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

#if DEBUG
		LogReceiptOfResult(isSolved[taskNumberForResult], status->MPI_SOURCE, taskNumberForResult);
#endif

		if (!isSolved[taskNumberForResult]) return;

		MPI_Recv(solution, numberOfNodes, MPI_INT, status->MPI_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		(*numberOfSolvedProblems)++;

#if DEBUG
		LogSolution(solution, numberOfNodes);
		LogNumberOfSolvedProblems(*numberOfSolvedProblems, numberOfProblems);
#else
		LogSolutionToFile(taskNumberForResult, solution, numberOfNodes);
		LogNumberOfSolvedProblemsToFile(taskNumberForResult, *numberOfSolvedProblems, numberOfProblems, numberOfNodes);
#endif
		

	}	
}

void LeaderSendOutInitialProblems(int* arcs, int firstIndex, int secondIndex, int numberOfNodes, int* currentTask, int* pendingTasks, int worldSize)
{
	for (*currentTask = 0; *currentTask < worldSize - 1; (*currentTask)++)
	{
		LeaderSendProblem(arcs, firstIndex, secondIndex, numberOfNodes, currentTask, (*currentTask) + 1);
		(*pendingTasks)++;
	}
}

void LeaderSendOutInitialProblemsUsingStatusFile(int* arcs, int* firstIndex, int* secondIndex, int numberOfNodes, int* currentTask, int* pendingTasks, int worldSize)
{
	int currentRankTask;
	for (int rank = 1; rank < worldSize; rank++)
	{
		ReadCurrentStatus(rank, numberOfNodes, &currentRankTask, firstIndex, secondIndex);
		LeaderSendProblem(arcs, *firstIndex, *secondIndex, numberOfNodes, &currentRankTask, rank);
		(*pendingTasks)++;
		*currentTask = max(currentRankTask, *currentTask);
	}
}

bool AllProblemsAreSolved(int numberOfSolvedProblems, int numberOfProblems)
{
	return numberOfSolvedProblems >= numberOfProblems;
}

bool HaveRunOutOfPossibleSolutions(int firstIndex, int numberOfNodes)
{
	return (firstIndex == numberOfNodes);
}

bool LeaderFinishedSendingProblems(int numberOfSolvedProblems, int numberOfProblems, int firstIndex, int numberOfNodes)
{
	return AllProblemsAreSolved(numberOfSolvedProblems, numberOfProblems) || HaveRunOutOfPossibleSolutions(firstIndex, numberOfNodes);
}

void LeaderIncrementIndices(int* firstIndex, int* secondIndex, int numberOfNodes)
{
	(*secondIndex)++;
	if (*firstIndex == *secondIndex) (*secondIndex)++;

	if (*secondIndex == numberOfNodes)
	{
		*secondIndex = 0;
		(*firstIndex)++;
	}
	LogCurrentIndicesToFile(*firstIndex, *secondIndex, numberOfNodes);
}

void LeaderIncrementCurrentTask(int* currentTask, int numberOfProblems, int* firstIndex, int* secondIndex, int numberOfNodes)
{
	(*currentTask)++;
	if (*currentTask == numberOfProblems)
	{
		(*currentTask) = 0;
		LeaderIncrementIndices(firstIndex, secondIndex, numberOfNodes);
	}
}

void LeaderSendOutNewTasksWhenRequestedUntilTheWorkIsDone(int* arcs, int firstIndex, int secondIndex, int numberOfNodes, int numberOfProblems, int* currentTask, int* pendingTasks, bool* isSolved)
{
	int numberOfSolvedProblems = 0;
	for (int i = 0; i < numberOfProblems; i++)
		if (isSolved[i] == true)
			numberOfSolvedProblems++;

	MPI_Status status;
	while (true)
	{
		LeaderRecieveResult(isSolved, &status, &numberOfSolvedProblems, numberOfNodes, numberOfProblems);
		if (!LeaderFinishedSendingProblems(numberOfSolvedProblems, numberOfProblems, firstIndex, numberOfNodes))
		{
			LeaderSendProblem(arcs, firstIndex, secondIndex, numberOfNodes, currentTask, status.MPI_SOURCE);

			while (true)
			{
				LeaderIncrementCurrentTask(currentTask, numberOfProblems, &firstIndex, &secondIndex, numberOfNodes);
				if (!isSolved[*currentTask])
					break;	
			}
		}
		else
		{
			(*pendingTasks)--;
		}
		if (*pendingTasks == 0)
			break;
	}
#if DEBUG
	LogNumberOfSolvedProblems(numberOfSolvedProblems, numberOfProblems);
#endif 
}

void LeaderSendOutTerminationRequest(int worldSize)
{
	bool terminate = true;
	for (int i = 1; i < worldSize; i++)
	{
		MPI_Send(&terminate, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
	}
}

void ExecuteLeaderTasks(int worldSize, int numberOfNodes, char* filename)
{
	int numberOfProblems = ReadNumberOfProblems(filename);

	if (numberOfProblems < worldSize)
	{
		printf("The number of processes must less than the number of problems.");
		MPI_Abort(MPI_COMM_WORLD, 0);
		return;
	}

	int* arcs = ReadProblems(filename, numberOfNodes, numberOfProblems);
	bool* isSolved;

	int pendingTasks = 0;
	int currentTask = 0;

	int firstIndex = 0;
	int secondIndex = 1;

	if (AllStatusFilesExist(worldSize, numberOfNodes))
	{
		LeaderSendOutInitialProblemsUsingStatusFile(arcs, &firstIndex, &secondIndex, numberOfNodes, &currentTask, &pendingTasks, worldSize);
		isSolved = InitialiseIsSolvedStatusFromFile(numberOfNodes, numberOfProblems);
	}
	else
	{
		LeaderSendOutInitialProblems(arcs, firstIndex, secondIndex, numberOfNodes, &currentTask, &pendingTasks, worldSize);
		isSolved = InitialiseIsSolvedStatus(numberOfProblems);
	}
	
	LeaderSendOutNewTasksWhenRequestedUntilTheWorkIsDone(arcs, firstIndex, secondIndex, numberOfNodes, numberOfProblems, &currentTask, &pendingTasks, isSolved);
	MPI_Abort(MPI_COMM_WORLD, 0);
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

void ExecuteHelperTasks(int rank, int numberOfNodes)
{	
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

#if DEBUG
		LogHelperCommencingSolve(rank, currentTask, firstIndex, secondIndex);
#endif
		solution = Solve(firstIndex, secondIndex, currentArcs, numberOfNodes, &hasBeenSolved);

		HelperSendResult(&currentTask, &hasBeenSolved, solution, numberOfNodes);
	}
}
#pragma endregion

int main(int argc, char* argv[])
{
#if LOG_EXECUTION_TIME
	auto start = std::chrono::high_resolution_clock::now();
#endif

	if (argc != 3)
	{
		printf("You must include the number of nodes followed by the name of the file to read the trees from\n");
		return 0;
	}

	int numberOfNodes = atoi(argv[1]);

	MPI_Init(&argc, &argv);

	int rank;
	int worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		ExecuteLeaderTasks(worldSize, numberOfNodes, argv[2]);
	}

	else
	{
		ExecuteHelperTasks(rank, numberOfNodes);
	}
#if LOG_EXECUTION_TIME
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	printf("Rank %d : Execution time in seconds %d", rank, duration.count());
#endif 
	MPI_Finalize();
}
