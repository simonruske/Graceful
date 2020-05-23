#include "pch.h"
#include "CppUnitTest.h"
#include "..\Graceful\Initialise.h"
#include "..\Graceful\SolutionState.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GracefulUnitTests
{
	TEST_CLASS(InitialiseTests)
	{
	public:
		
		TEST_METHOD(InitialiseTests_InitialiseSolutionStateValues_SetsCorrectly)
		{
			//Arrange
			int numberOfNodes = 10;
			int* solutionStateValues = new int[numberOfNodes];
			int firstIndex = 1;
			int secondIndex = 2;

			//Act
			InitialiseSolutionStateValues(solutionStateValues, firstIndex, secondIndex, numberOfNodes);

			//Assert
			Assert::AreEqual(1, solutionStateValues[0]);
			Assert::AreEqual(2, solutionStateValues[1]);

			for (int i = 2; i < numberOfNodes; i++)
			{
				Assert::AreEqual(0, solutionStateValues[i]);
			}
		}

		TEST_METHOD(InitialiseTests_InitialiseSolutionStateDifferences_SetsCorrectly)
		{
			//Arrange
			int numberOfNodes = 10;
			int* solutionStateDifferences = new int[numberOfNodes];
			int firstIndex = 1;
			int secondIndex = 3;

			//Act
			InitialiseSolutionStateDifferences(solutionStateDifferences, firstIndex, secondIndex, numberOfNodes);

			//Assert
			Assert::AreEqual(-1, solutionStateDifferences[0]);
			Assert::AreEqual(2, solutionStateDifferences[1]);
			for (int i = 2; i < numberOfNodes; i++)
			{
				Assert::AreEqual(0, solutionStateDifferences[i]);
			}
		}

		TEST_METHOD(InitialiseTests_InitialiseIsValueRemaining_SetsCorrectly)
		{
			//Arrange
			int numberOfNodes = 10;
			bool* isValueRemaining = new bool[numberOfNodes];
			int firstIndex = 1;
			int secondIndex = 3;

			//Act
			InitialiseIsValueRemaining(isValueRemaining, firstIndex, secondIndex, numberOfNodes);
			
			//Assert
			for (int i = 0; i < numberOfNodes; i++)
			{
				if (i == 1 || i == 3)
				{
					Assert::AreEqual(false, isValueRemaining[i]);
				}
				else
				{
					Assert::AreEqual(true, isValueRemaining[i]);
				}
			}
		}

		TEST_METHOD(InitialiseTests_InitialiseIsDifferenceRemaining_SetsCorrectly)
		{
			//Arrange
			int numberOfNodes = 10;
			bool* isDifferenceRemaining = new bool[numberOfNodes];
			int firstIndex = 1;
			int secondIndex = 3;

			//Act
			InitialiseIsDifferenceRemaining(isDifferenceRemaining, firstIndex, secondIndex, numberOfNodes);

			//Assert
			for (int i = 0; i < numberOfNodes; i++)
			{
				if (i == 2)
				{
					Assert::AreEqual(false, isDifferenceRemaining[i]);
				}
				else
				{
					Assert::AreEqual(true, isDifferenceRemaining[i]);
				}
			}
		}

		TEST_METHOD(InitialiseTests_InitialiseSolutionState_SetsCorrectly)
		{
			//Arrange
			int numberOfNodes = 10;
			int firstIndex = 2;
			int secondIndex = 5;

			SolutionState solutionState;

			solutionState.Values = new int[numberOfNodes];
			solutionState.Differences = new int[numberOfNodes];
			solutionState.IsValueRemaining = new bool[numberOfNodes];
			solutionState.IsDifferenceRemaining = new bool[numberOfNodes];

			//Act
			InitialiseSolutionState(solutionState, firstIndex, secondIndex, numberOfNodes);

			//Assert - Values
			Assert::AreEqual(2, solutionState.Values[0]);
			Assert::AreEqual(5, solutionState.Values[1]);
			for (int i = 2; i < numberOfNodes; i++)
			{
				Assert::AreEqual(0, solutionState.Values[i]);
			}

			// Assert - Values remaining
			for (int i = 0; i < numberOfNodes; i++)
			{
				if (i == 2 || i == 5)
				{
					Assert::AreEqual(false, solutionState.IsValueRemaining[i]);
				}
				else
				{
					Assert::AreEqual(true, solutionState.IsValueRemaining[i]);
				}
			}

			//Assert - Differences
			Assert::AreEqual(-1, solutionState.Differences[0]);
			Assert::AreEqual(3 , solutionState.Differences[1]);
			for (int i = 2; i < numberOfNodes; i++)
			{
				Assert::AreEqual(0, solutionState.Differences[i]);
			}

			//Assert - IsDifferenceRemainings
			for (int i = 0; i < numberOfNodes - 1; i++)
			{
				if (i == 3)
				{
					Assert::AreEqual(false, solutionState.IsDifferenceRemaining[i]);
				}
				else
				{
					Assert::AreEqual(true, solutionState.IsDifferenceRemaining[i]);
				}
			}
		}

		TEST_METHOD(InitialiseTests_InitialiseNextNode_SetsValueAndDifferenceCorrectly)
		{
			//Arrange
			int arcs[3]{ 0, 1, 1 };
			int positionOnSolution = 2;

			SolutionState solutionState;
			solutionState.Values      = new int[4]{ 0 , 1, 2, 0 };
			solutionState.Differences = new int[4]{ -1, 1, 0, 0 };

			//Act
			InitialiseNextNode(solutionState, arcs, positionOnSolution);

			//Assert
			int expectedValues[4]{ 0, 1, 0, 0 };
			for (int i = 0; i < 4; i++)
			{
				Assert::AreEqual(expectedValues[i], solutionState.Values[i]);
			}

			int expectedDifferences[4]{ -1, 1, 1, 0 };
			for (int i = 0; i < 4; i++)
			{
				Assert::AreEqual(expectedDifferences[i], solutionState.Differences[i]);
			}
		}
	};
}
