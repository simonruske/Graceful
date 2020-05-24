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