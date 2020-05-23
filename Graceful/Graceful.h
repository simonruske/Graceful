struct SolutionState;
void SetCurrentNodeOnSolution(SolutionState solutionState, int valueIndex, int differenceIndex, int currentPosition);
void UpdateDifference(int* difference, bool* increasing);
bool DifferenceIsIncreasing(SolutionState solutionState, int* arcs, int positionOnSolution, int valueIndex);
void IncrementPreviousNode(SolutionState solutionState, int positionOnSolution, int* arcs);
void SearchForNextValidIndices(SolutionState solutionState, int* arcs, int* positionOnSolution, int numberOfNodes);
void TraverseSolutions(SolutionState solutionState, int* arcs, int* positionOnSolution, int numberOfNodes);
int* Solve(int firstIndex, int secondIndex, int* arcs, int numberOfNodes, bool* hasBeenSolved);