#include "SolutionState.h"
struct SolutionState;

void InitialiseSolutionStateValues(int* solutionStateValues, int firstIndex, int secondIndex, int numberOfNodes)
{
	solutionStateValues[0] = firstIndex;
	solutionStateValues[1] = secondIndex;
	for (int i = 2; i < numberOfNodes; i++)
	{
		solutionStateValues[i] = 0;
	}
}

void InitialiseSolutionStateDifferences(int* solutionStateDifferences, int firstIndex, int secondIndex, int numberOfNodes)
{
	solutionStateDifferences[0] = -1;
	solutionStateDifferences[1] = secondIndex - firstIndex;
	for (int i = 2; i < numberOfNodes; i++)
	{
		solutionStateDifferences[i] = 0;
	}
}

void InitialiseIsValueRemaining(bool* isValueRemaining, int firstIndex, int secondIndex, int numberOfNodes)
{
	for (int i = 0; i < numberOfNodes; i++)
	{
		isValueRemaining[i] = (i != firstIndex) && (i != secondIndex);
	}
}

void InitialiseIsDifferenceRemaining(bool* isDifferenceRemaining, int firstIndex, int secondIndex, int numberOfNodes)
{
	int differenceIndex = secondIndex - firstIndex;
	for (int i = 0; i < numberOfNodes; i++)
	{
		isDifferenceRemaining[i] = i != differenceIndex;
	}
}

void InitialiseSolutionState(SolutionState solutionState, int firstIndex, int secondIndex, int numberOfNodes)
{
	InitialiseSolutionStateValues(solutionState.Values, firstIndex, secondIndex, numberOfNodes);
	InitialiseSolutionStateDifferences(solutionState.Differences, firstIndex, secondIndex, numberOfNodes);
	InitialiseIsValueRemaining(solutionState.IsValueRemaining, firstIndex, secondIndex, numberOfNodes);
	InitialiseIsDifferenceRemaining(solutionState.IsDifferenceRemaining, firstIndex, secondIndex, numberOfNodes);
}

void InitialiseNextNode(SolutionState solutionState, int* arcs, int positionOnSolution)
{
	solutionState.Values[positionOnSolution] = 0;
	solutionState.Differences[positionOnSolution] = solutionState.Values[arcs[positionOnSolution-1]];
}