#pragma once
#include <iostream>
#include <sstream>
using namespace std;

vector<string> splitString(string line)
{
	vector<string> result;
	string temp = "";
	string code = "";
	istringstream iss(line);

	//We take the information from the input file
	//and parse them together in such a way that
	//allows it to used to make the symbol table easily.
	while (iss >> temp)
	{
		if (temp[0] == ';')
			break;
		else if (code == "")
			code = temp;
		else
			code = code + " " + temp;
	}

	string tmp;
	stringstream ss(code);

	while (getline(ss, tmp, ' ')) 
		result.push_back(tmp);

	return result;
}

vector <vector<string>> readFile()
{
	vector <vector <string>> data;
	string line;
	fstream inputFile("input.txt");

	//We ignore the first line of the input.txt
	//since it cannot be used in any capacity
	getline(inputFile, line);
	cout << line << endl;

	//We read all of the relevant lines and
	//insert them into a vector.
	while (getline(inputFile, line))
	{
		data.push_back(splitString(line));
		cout << line << endl;
	}
	inputFile.close();
	return data;
}