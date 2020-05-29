#include "Reader.h"
#include <iostream>
#include <fstream>
#include <string>

int ReadNumberOfProblems(char* filename)
{
	std::string line;
	std::ifstream treeFile(filename);

	int numberOfProblems = 0;
	if (treeFile.is_open())
	{
		while (std::getline(treeFile, line))
		{
			numberOfProblems++;
		}
		treeFile.close();
	}

	if (numberOfProblems == 0)
	{
		throw std::exception("Was unable to read file.");
	}

	return numberOfProblems;
}
int* ReadProblems(char* filename, int numberOfNodes, int numberOfProblems)
{
	int* result = new int[numberOfProblems * (numberOfNodes - 1)];

	std::string line;
	std::ifstream treeFile(filename);

	int currentPosition = 0;
	if (treeFile.is_open())
	{
		for (int i = 0; i < numberOfProblems; i++)
		{
			for (int j = 0; j < numberOfNodes - 2; j++)
			{
				std::getline(treeFile, line, ',');
				result[currentPosition] = std::stoi(line);
				currentPosition++;
			}
			std::getline(treeFile, line, '\n');
			result[currentPosition] = std::stoi(line);
			currentPosition++;
		}
	}

	return result;
}

void ReadCurrentStatus(int rank, int numberOfNodes, int* currentTask, int* firstIndex, int* secondIndex)
{
	FILE* statusFile;

	char statusFileName[32];
	sprintf_s(statusFileName, "..\\Logs\\status_%d_%d.txt", numberOfNodes, rank);

	fopen_s(&statusFile, statusFileName, "r");

	fscanf_s(statusFile, "%d", currentTask);
	fscanf_s(statusFile, "%d", firstIndex); 
	fscanf_s(statusFile, "%d", secondIndex);
	fclose(statusFile);
}

bool AllStatusFilesExist(int worldSize, int numberOfNodes) 
{
	char statusFileName[32];
	for (int rank = 1; rank < worldSize; rank++)
	{
		sprintf_s(statusFileName, "..\\Logs\\status_%d_%d.txt", numberOfNodes, rank);

		FILE* statusFile;
		int successful = fopen_s(&statusFile, statusFileName, "r");

		if (successful == 0) 
		{
			fclose(statusFile);
		}
		else 
		{
			return false;
		}
	}
	return true;
}

bool* InitialiseIsSolvedStatusFromFile(int numberOfNodes, int numberOfProblems)
{
	FILE* statusFile;

	char statusFileName[32];
	sprintf_s(statusFileName, "..\\Logs\\status_%d.txt", numberOfNodes);

	fopen_s(&statusFile, statusFileName, "r");

	bool* isSolved = new bool[numberOfProblems];

	for (int i = 0; i < numberOfProblems; i++)
	{
		isSolved[i] = false;
	}

	char buffer[128];
	int currentIsSolved;
	while (fgets(buffer, 128, statusFile))
	{
		int stat = sscanf_s(buffer, "Solved problem %d", &currentIsSolved);
		if (stat == 1)
		{
			isSolved[currentIsSolved] = true;
		}
	}
	fclose(statusFile);

	return isSolved;
}