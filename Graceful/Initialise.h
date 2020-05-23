struct SolutionState;
void InitialiseSolutionStateValues(int* solutionStateValues, int firstIndex, int secondIndex, int numberOfNodes);
void InitialiseSolutionStateDifferences(int* currentSolutionDifferenceIndices, int firstIndex, int secondIndex, int numberOfNodes);
void InitialiseIsValueRemaining(bool* isValueRemaining, int firstIndex, int secondIndex, int numberOfNodes);
void InitialiseIsDifferenceRemaining(bool* isDifferenceRemaining, int firstIndex, int secondIndex, int numberOfNodes);
void InitialiseSolutionState(SolutionState solutionState, int firstIndex, int secondIndex, int numberOfNodes);
void InitialiseNextNode(SolutionState solutionState, int* arcs, int positionOnSolution);