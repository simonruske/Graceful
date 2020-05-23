#include "SolutionState.h"
#include "Initialise.h"
#include <stdlib.h>

void SetCurrentNodeOnSolution(SolutionState solutionState, int valueIndex, int differenceIndex, int currentPosition)
{
	solutionState.IsValueRemaining[valueIndex] = false;
	solutionState.IsDifferenceRemaining[differenceIndex] = false;
	solutionState.Values[currentPosition] = valueIndex;
	solutionState.Differences[currentPosition] = differenceIndex;
}

void UpdateDifference(int* difference, bool *increasing)
{
	if (*increasing)
		(*difference)++;
	else
		(*difference)--;
	if (*difference == 0)
		*increasing = true;
}

bool DifferenceIsIncreasing(SolutionState solutionState, int* arcs, int positionOnSolution, int valueIndex)
{
	return (solutionState.Values[arcs[positionOnSolution - 1]] <= valueIndex);
}

void IncrementPreviousNode(SolutionState solutionState, int positionOnSolution, int* arcs)
{
	int valueIndex = solutionState.Values[positionOnSolution];
	int differenceIndex = solutionState.Differences[positionOnSolution];
	
	solutionState.IsValueRemaining[valueIndex] = true;
	solutionState.IsDifferenceRemaining[differenceIndex] = true;
	
	bool differenceIsIncreasing = DifferenceIsIncreasing(solutionState, arcs, positionOnSolution, valueIndex);
	
	valueIndex++;
	solutionState.Values[positionOnSolution] = valueIndex;
	UpdateDifference(&differenceIndex, &differenceIsIncreasing);
	solutionState.Differences[positionOnSolution] = differenceIndex;
}

void SearchForNextValidIndices(SolutionState solutionState, int* arcs, int* positionOnSolution, int numberOfNodes)
{
	int valueIndex = solutionState.Values[*positionOnSolution];
	int differenceIndex = solutionState.Differences[*positionOnSolution];
	bool differenceIsIncreasing = DifferenceIsIncreasing(solutionState, arcs, *positionOnSolution, valueIndex);

	while (valueIndex < numberOfNodes)
	{
		if (solutionState.IsValueRemaining[valueIndex] && solutionState.IsDifferenceRemaining[differenceIndex])
		{
			SetCurrentNodeOnSolution(solutionState, valueIndex, differenceIndex, *positionOnSolution);
			(*positionOnSolution)++;

			if (*positionOnSolution < numberOfNodes)
			{
				InitialiseNextNode(solutionState, arcs, *positionOnSolution);
			}
			return;
		}
		valueIndex++;
		UpdateDifference(&differenceIndex, &differenceIsIncreasing);
	}
	(*positionOnSolution)--;
	IncrementPreviousNode(solutionState, *positionOnSolution, arcs);
}

bool TraverseSolutions(SolutionState solutionState, int* arcs, int* positionOnSolution, int numberOfNodes)
{
	while ((*positionOnSolution) > 1 && (*positionOnSolution) < numberOfNodes)
	{
		SearchForNextValidIndices(solutionState, arcs, positionOnSolution, numberOfNodes);
	}

	bool hasBeenSolved = *positionOnSolution == numberOfNodes;
	return hasBeenSolved;
}

int* Solve(int firstIndex, int secondIndex, int* arcs, int numberOfNodes, bool* hasBeenSolved)
{
	SolutionState solutionState;

	solutionState.Values = new int[numberOfNodes];
	solutionState.Differences = new int[numberOfNodes];
	solutionState.IsValueRemaining = new bool[numberOfNodes];
	solutionState.IsDifferenceRemaining = new bool[numberOfNodes];

	InitialiseSolutionState(solutionState, firstIndex, secondIndex, numberOfNodes);
	
	int positionOnSolution = 2;
	InitialiseNextNode(solutionState, arcs, positionOnSolution);
	*hasBeenSolved = TraverseSolutions(solutionState, arcs, &positionOnSolution, numberOfNodes);
	return solutionState.Values;
}