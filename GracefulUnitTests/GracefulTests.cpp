#include "pch.h"
#include "CppUnitTest.h"
#include "..\Graceful\Graceful.h"
#include "..\Graceful\SolutionState.h"
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GracefulUnitTests
{
	TEST_CLASS(GracefulTests)
	{
	public:

		TEST_METHOD(GracefulTests_SetCurrentNodeOnSolution_SetsCorrectly)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values                = new int [5] { 0   , 0   , 0   , 0   , 0    };
			solutionState.IsValueRemaining      = new bool[5] { true, true, true, true, true };
			solutionState.Differences           = new int [5] { 0   , 0   , 0   , 0   , 0    };
			solutionState.IsDifferenceRemaining = new bool[5] { true, true, true, true, true };
			
			int valueIndex = 1;
			int differenceIndex = 2;
			int currentPosition = 2;

			//Act
			SetCurrentNodeOnSolution(solutionState, valueIndex, differenceIndex, currentPosition);

			//Assert
			Assert::AreEqual(valueIndex, solutionState.Values[currentPosition]);
			Assert::AreEqual(false, solutionState.IsValueRemaining[valueIndex]);

			Assert::AreEqual(differenceIndex, solutionState.Differences[currentPosition]);
			Assert::AreEqual(false, solutionState.IsDifferenceRemaining[differenceIndex]);
		}

		TEST_METHOD(GracefulTests_UpdateDifference_StartFrom0_AlwaysIncreasing)
		{
			int difference = 0; 
			bool increasing = true;
			
			UpdateDifference(&difference, &increasing);

			Assert::AreEqual(1, difference);
			Assert::AreEqual(true, increasing);

			UpdateDifference(&difference, &increasing);

			Assert::AreEqual(2, difference);
			Assert::AreEqual(true, increasing);
		}

		TEST_METHOD(GracefulTests_UpdateDifferences_StartAbove0_DecreasesUntil0AndThenStartsIncreasing)
		{
			int difference = 1;
			bool increasing = false;
			UpdateDifference(&difference, &increasing);

			Assert::AreEqual(0, difference);
			Assert::AreEqual(true, increasing);

			UpdateDifference(&difference, &increasing);

			Assert::AreEqual(1, difference);
			Assert::AreEqual(true, increasing);
		}

		TEST_METHOD(GracefulTests_DifferenceIsIncreasing_CurrentValueIsLessThanConnectedValue_DifferenceIsDecreasing)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values = new int[4]{ 2, 0, 0, 0 };

			// All other nodes connect into node 0
			int* arcs = new int[4]{ 0, 0, 0, 0 };

			int positionOnSolution = 1;
			int valueIndex = 0;

			//Act
			bool isDifferenceIncreasing = DifferenceIsIncreasing(solutionState, arcs, positionOnSolution, valueIndex);

			//Assert
			Assert::AreEqual(false, isDifferenceIncreasing);
		}

		TEST_METHOD(GracefulTests_DifferenceIsIncreasing_CurrentValueIsGreaterThanConnectedValue_DifferenceIsIncreasing)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values = new int[4]{ 2, 3, 0, 0 };

			//All other nodes connect into node 0
			int* arcs = new int[4]{ 0, 0, 0, 0 };
			
			int positionOnSolution = 1;
			int valueIndex = 3; // current value exceeds the value on the node that it connects to

			//Act
			bool isDifferenceIncreasing = DifferenceIsIncreasing(solutionState, arcs, positionOnSolution, valueIndex);

			//Assert
			Assert::AreEqual(true, isDifferenceIncreasing);
		}

		TEST_METHOD(GracefulTests_DifferenceIsIncreasing_CurrentValueIsEqualToConnectedValue_DifferenceIsIncreasing)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values = new int[4]{ 2, 3, 0, 0 };

			//All other nodes connect into node 0
			int* arcs = new int[4]{ 0, 0, 0, 0 };
			int positionOnSolution = 1;
			int valueIndex = 2;

			//Act
			bool isDifferenceIncreasing = DifferenceIsIncreasing(solutionState, arcs, positionOnSolution, valueIndex);

			//Assert
			Assert::AreEqual(true, isDifferenceIncreasing);
		}

		TEST_METHOD(GracefulTests_DifferenceIsIncreasing_CurrentValuesIs0_DifferenceIsIncreasing)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values = new int[4]{ 0, 0, 0, 0 };

			//All other nodes connect into node 0
			int* arcs = new int[4]{ 0, 0, 0, 0 };
			int positionOnSolution = 1;
			int valueIndex = 0;

			//Act
			bool isDifferenceIncreasing = DifferenceIsIncreasing(solutionState, arcs, positionOnSolution, valueIndex);

			//Assert
			Assert::AreEqual(true, isDifferenceIncreasing);
		}

		TEST_METHOD(GracefulTests_IncrementPreviousNode_IncreasingDifference_SetsCurrentSelectedValueDifferencePairToAvailableAndIncrements)
		{
			//Arrange
			SolutionState solutionState;

			solutionState.Values = new int[4]{ 2, 3, 0, 0};
			solutionState.IsValueRemaining = new bool[4]{ false, true, false, false };
			solutionState.Differences = new int[4]{ -1, 1, 3, 0 };
			solutionState.IsDifferenceRemaining = new bool[4]{ true, false, true, false };

			int positionOnSolution = 2;

			int* arcs = new int[3]{ 0, 0, 0 };

			//Act
			IncrementPreviousNode(solutionState, positionOnSolution, arcs);

			//Assert
			int* expectedValues = new int[4]{ 2, 3, 1, 0 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);

			bool* expectedIsValueRemaining = new bool[4]{ true, true, false, false };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedIsValueRemaining[i], solutionState.IsValueRemaining[i]);

			int* expectedDifferences = new int[4]{ -1, 1, 2, 0 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);

			bool* expectedDifferenceRemaining = new bool[4]{ true, false, false, true };
		}

		TEST_METHOD(GracefulTests_IncrementPreviousNode_DecreasingDifference_SetsCurrentSelectedValueDifferencePairToAvailableAndIncrements)
		{
			//Arrange
			SolutionState solutionState;

			solutionState.Values = new int[4]{ 0, 2, 1, 0 };
			solutionState.IsValueRemaining = new bool[4]{ false, false, false, true };
			solutionState.Differences = new int[4]{ -1, 2, 1, 0 };
			solutionState.IsDifferenceRemaining = new bool[4]{ true, false, false, true };

			int positionOnSolution = 2;

			int* arcs = new int[3]{ 0, 0, 0 };

			//Act
			IncrementPreviousNode(solutionState, positionOnSolution, arcs);

			//Assert
			int* expectedValues = new int[4]{ 0, 2, 2, 0 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);

			bool* expectedIsValueRemaining = new bool[4]{ false, true, false, true };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedIsValueRemaining[i], solutionState.IsValueRemaining[i]);

			int* expectedDifferences = new int[4]{ -1, 2, 2, 0 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);

			bool* expectedDifferenceRemaining = new bool[4]{ true, true, false, true };
		}

		TEST_METHOD(GracefulTests_SearchForNextValidIndices_NextValueDifferencePairIsAvailable_SetsNextValue)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values                = new int [5]{ 0    , 1    , 0   , 0   , 0    };
			solutionState.IsValueRemaining      = new bool[5]{ false, false, true, true, true };
			solutionState.Differences           = new int [5]{ -1   , 1    , 1   , 0   , 0    };
			solutionState.IsDifferenceRemaining = new bool[5]{ true , false, true, true, true };
			
			int* arcs = new int[4]{ 0, 1, 2, 3 };
			int positionOnsolution = 2;
			int numberOfNodes = 5;

			//Act
			SearchForNextValidIndices(solutionState, arcs, &positionOnsolution, numberOfNodes);

			//Assert - solution state - should have 3 appended in position 2
			int* expectedValues = new int[5]{ 0, 1, 3, 0, 0 };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);

			bool* expectedValuesRemaining = new bool[5]{ false, false, true, false, true };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedValuesRemaining[i], solutionState.IsValueRemaining[i]);

			// Assert - solution state - should use 2 for current difference and have difference set ready for next iteration
			// Next node is node 3 which is connected to node 2 which we have set to value 3 so initial difference should be 3 - 0 = 3
			int* expectedDifferences = new int[5]{ -1, 1, 2, 3, 0 };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);

			// Assert - solution state - expected differences remaining
			bool* expectedDifferencesRemaining = new bool[5]{ true, false, false, true, true};
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedDifferencesRemaining[i], solutionState.IsDifferenceRemaining[i]);

			//Assert - arcs should not change
			int* expectedArcs = new int[4]{ 0, 1, 2, 3 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedArcs[i], arcs[i]);

			//Assert - Position on solution
			Assert::AreEqual(3, positionOnsolution);
		}

		TEST_METHOD(GracefulTests_SearchForNextValidIndices_FinishedSolution_DoesNotSetNextNode)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values                = new int [5]{ 0    , 4    , 1    , 3    , 0     };
			solutionState.IsValueRemaining      = new bool[5]{ false, false, true , false, false };
			solutionState.Differences           = new int [5]{ -1   , 4    , 3    , 2    , 3     };
			solutionState.IsDifferenceRemaining = new bool[5]{ true , true , false, false, false  };

			int* arcs = new int[4]{ 0, 1, 2, 3 };
			int positionOnsolution = 4;
			int numberOfNodes = 5;

			//Act
			SearchForNextValidIndices(solutionState, arcs, &positionOnsolution, numberOfNodes);

			//Assert - solution state values - should have 2 appended in the final position
			int* expectedValues = new int[5]{ 0, 4, 1, 3, 2 };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);

			//Assert - solution state values remaining - everything should be used
			bool* expectedValuesRemaining = new bool[5]{ false, false, false, false, false };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedValuesRemaining[i], solutionState.IsValueRemaining[i]);

			// Assert - solution state differences - 2 should be the last value used
			// Node 4 is connected to 3, therefore the difference should be 2
			int* expectedDifferences = new int[5]{ -1, 4, 3, 2, 1 };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);

			bool* expectedDifferencesRemaining = new bool[5]{ true, false, false, false, false };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedDifferencesRemaining[i], solutionState.IsDifferenceRemaining[i]);

			//Assert - arcs should not change
			int* expectedArcs = new int[4]{ 0, 1, 2, 3 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedArcs[i], arcs[i]);

			//Assert - Position on solution
			Assert::AreEqual(5, positionOnsolution);
		}

		TEST_METHOD(GracefulTests_SearchForNextValidIndices_RunOutOfAvailableValues_MovesBackOneSpace)
		{
			//Arrange
			SolutionState solutionState;
			solutionState.Values                = new int [5]{ 0    , 1    , 3    , 0    , 0    };
			solutionState.IsValueRemaining      = new bool[5]{ false, false, true , false, true };
			solutionState.Differences           = new int [5]{ -1   , 1    , 2    , 3    , 0    };
			solutionState.IsDifferenceRemaining = new bool[5]{ true , false, false, true , true };

			int* arcs = new int[4]{ 0, 1, 2, 3 };
			int positionOnsolution = 3;
			int numberOfNodes = 5;

			//Act
			SearchForNextValidIndices(solutionState, arcs, &positionOnsolution, numberOfNodes);

			//Assert - we should have moved the position back one 
			Assert::AreEqual(2, positionOnsolution);

			//Assert - should have moved onto 4 on the previous node
			int* expectedValues = new int[3]{ 0, 1, 4 };
			for (int i = 0; i < 2; i++)
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);

			//Assert - solution state values only 0 and 1 used (4 needs to be tested next so should not be used yet)
			bool* expectedValuesRemaining = new bool[5]{ false, false, true, true, true };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedValuesRemaining[i], solutionState.IsValueRemaining[i]);

			// Assert - solution state differences - 2 should be the last value used
			// Node 3 is connected to node 2 
			int* expectedDifferences = new int[3]{ -1, 1, 3 };
			for (int i = 0; i < 3; i++)
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);

			bool* expectedDifferencesRemaining = new bool[5]{ true, false, true, true, true };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedDifferencesRemaining[i], solutionState.IsDifferenceRemaining[i]);

			//Assert - arcs should not change
			int* expectedArcs = new int[4]{ 0, 1, 2, 3 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(expectedArcs[i], arcs[i]);
		}

		TEST_METHOD(GracefulTests_Solve_StarWith5_Nodes_GivesCorrectSolution)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 1;
			int numberOfNodes = 5;
			int* arcs = new int[numberOfNodes]{ 0, 0, 0, 0, 0 };
			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			Assert::AreEqual(true, hasBeenSolved);

			int* expectedSolution = new int[5]{ 0, 1, 2, 3, 4 };
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedSolution[i], actualSolution[i]);
		}

		TEST_METHOD(GracefulTests_Solve_ChainWithPossibleSolution_GivesCorrectSolution)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 4;
			int numberOfNodes = 5;
			int* arcs = new int[numberOfNodes]{0, 1, 2, 3};
			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			Assert::AreEqual(true, hasBeenSolved);

			int* expectedSolution = new int[5]{ 0, 4, 1, 3, 2};
			for (int i = 0; i < 5; i++)
				Assert::AreEqual(expectedSolution[i], actualSolution[i]);
		}

		TEST_METHOD(GracefulTests_Solve_ChainWithoutPossibleSolution_SetsHasBeenSolvedToFalse)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 1;
			int numberOfNodes = 5;
			int* arcs = new int[4] {0, 1, 2, 3 };
			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			Assert::AreEqual(false, hasBeenSolved);
		}

		TEST_METHOD(GracefulTests_Solve_3A)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 1;
			int numberOfNodes = 3;

			int* arcs = new int[2]{ 0, 0 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);
			
			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve_4A)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 2;
			int numberOfNodes = 4;

			int* arcs = new int[3]{ 0, 1, 0 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve_4B)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 1;
			int numberOfNodes = 4;

			int* arcs = new int[3]{ 0, 0, 0 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve_5A)
		{

			//Arrange
			int firstIndex = 0;
			int secondIndex = 2;
			int numberOfNodes = 5;

			int* arcs = new int[4]{ 0, 1, 0, 3 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve_5B)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 2;
			int numberOfNodes = 5;

			int* arcs = new int[4]{ 0, 1, 0, 0 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve5C)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 1;
			int numberOfNodes = 5;


			int* arcs = new int[4]{ 0, 0, 0, 0 };

			bool hasBeenSolved;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		TEST_METHOD(GracefulTests_Solve_14_Nodes_SolvesCorrectly)
		{
			//Arrange
			int firstIndex = 0;
			int secondIndex = 7;
			int numberOfNodes = 14;
			int* arcs = new int[13]{ 0, 1, 2, 3, 4, 5, 2, 0, 8, 9, 10, 11, 11 };
			
			bool hasBeenSolved = false;

			//Act
			int* actualSolution = Solve(firstIndex, secondIndex, arcs, numberOfNodes, &hasBeenSolved);

			//Assert
			AssertSolutionIsSolved(actualSolution, hasBeenSolved, arcs, numberOfNodes);
		}

		void AssertSolutionIsSolved(int* actualSolution, bool hasBeenSolved, int* arcs, int32_t numberOfNodes)
		{
			Assert::AreEqual(true, hasBeenSolved);

			int* differences = new int[numberOfNodes - 1];

			for (int i = 0; i < numberOfNodes - 1; i++)
				differences[i] = abs(actualSolution[i + 1] - actualSolution[arcs[i]]);

			std::sort(differences, differences + numberOfNodes - 1);

			for (int i = 0; i < numberOfNodes - 1; i++)
				Assert::AreEqual(i + 1, differences[i]);

			std::sort(actualSolution, actualSolution + numberOfNodes);
			for (int i = 0; i < numberOfNodes; i++)
				Assert::AreEqual(i, actualSolution[i]);
		}

	};
}