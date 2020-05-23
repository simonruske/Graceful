#ifndef SOLUTIONSTATE_H
#define SOLUTIONSTATE_H

struct SolutionState
{
	int* Values;
	int* Differences;
	bool* IsValueRemaining;
	bool* IsDifferenceRemaining;
};

#endif