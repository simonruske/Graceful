#include "pch.h"
#include "CppUnitTest.h"
#include "..\Graceful\Reader.h"
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GracefulUnitTests
{
	TEST_CLASS(ReaderTests)
	{
	public:
		TEST_METHOD(ReaderTests_ReadNumberOfProblems_TestFile_Returns11)
		{
			//Act
			int numberOfProblems = ReadNumberOfProblems("..\\TestFiles\\trees_7.csv");

			//Assert
			Assert::AreEqual(11, numberOfProblems);
		}

		TEST_METHOD(ReaderTest_ReadNumberOfProblem_FileDoesNotExist_ThrowsException)
		{
			auto func = [] { ReadNumberOfProblems("..\\Test Files\\nonExistintantFile.csv"); };
			Assert::ExpectException<std::exception> (func);
		}

		TEST_METHOD(ReaderTest_ReadProblems_TestFile_ReturnsCorrectResult)
		{
			//Act
			int* actualResult = ReadProblems("..\\TestFiles\\trees_7.csv", 7, 11);

			//Assert
			int* expectedResult = new int[66]
			{
				0,	1,	2,	0,	4,	5,
				0,	1,	2,	0,	4,	4,
				0,	1,	2,	0,	4,	0,
				0,	1,	1,	1,	0,	5,
				0,	1,	1,	0,	4,	4,
				0,	1,	1,	0,	4,	0,
				0,	1,	1,	0,	0,	0,
				0,	1,	0,	3,	0,	5,
				0,	1,	0,	3,	0,	0,
				0,	1,	0,	0,	0,	0,
				0,	0,	0,	0,	0,	0
			};

			for (int i = 0; i < 66; i++)
				Assert::AreEqual(expectedResult[i], actualResult[i]);
		}
	};
}