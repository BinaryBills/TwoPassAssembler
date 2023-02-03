/*Author: Binary Bills
*Creation Date: April 15, 2022
*Modification Date: April 16, 2022
*Purpose: The purpose of this program is to simulate a 2 Pass assembler. 
An assembler is a program that translates assembly code into machine code. 
In pass one, we perform the address computation and the construction of 
the symbol table. In the second pass, the actual translation is done. 
We will read actual MASM assembly code from our input file. 
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include "inputParser.h"
#include "inputHelpers.h"
#include<iomanip>
using namespace std;


void addToSymTable(map<string, string> & s, entry e, string memAddress)
{
	pair <string, string>p1(e.label, memAddress);
	s.insert(p1);
}

void addToMap(map<string, entry> & m, vector<string> v, string memAddress, entry & newEntry)
{
	int currIndex = 0;

	if (newEntry.label == "")
	{
		newEntry.instruct = v[0];
		currIndex = 1;
	}
	else
	{
		newEntry.instruct = v[1];
		currIndex = 2;
	}

	for (int i = currIndex; i < v.size(); i++)
	{
		if (i == currIndex)
			newEntry.op1 = v[i];
		else
			newEntry.op2 = v[i];
	}

	pair <string, entry>p1(memAddress, newEntry);
    m.insert(p1);

	
	
		setMachineCode(newEntry);
		convertNumToHex(newEntry);
}


void passOne(vector<vector <string>> instruction, map <string, entry>& hashTable, map<string, string>& symTable)
{
	vector<string> currLine = instruction[0];
	address LC;
	entry newEntry;
	int currIndex, addressSum = 0;

	for (int i = 0; i < instruction.size(); i++)
	{

		//Gets first memory address
		if (i == 0)
		{
			string temp = getAddress(convertVectorToString(currLine));
			LC = convertToArray(LC, 8, temp);
			continue;
		}

		//Gets current vector in 2D vector
		currLine = instruction[i];
		currIndex = 0;
		addressSum = 0;
		newEntry.label = "";
		newEntry.instruct = "";
		newEntry.op1 = "";
		newEntry.op2 = "";

		//Checks if there is a label in the first column.
		// If so,we save it, and increment the currIndex.
		if (currLine[currIndex] == "LOOP1" || currLine[currIndex] == "LOOP2")
		{
			newEntry.label = currLine[currIndex];
			currIndex++;
		}

		//As demonstrated on handout, if END is next vector. We end execution. 
		if (instruction[i + 1][0] != "END")
		{
			//Append to LOOP1 and LOOP2 to symbol table
			if (newEntry.label != "")
				addToSymTable(symTable, newEntry, LC.loc);

			//Add to hashtable then increment location counter using
			//the bytes of the instructions and SMC68000
		    addToMap(hashTable, currLine, LC.loc, newEntry);
			addressSum = getSmc68000Bytes(currLine) + getInstructionByte(currLine, currIndex);
			LC = computeNextAddress(LC, addressSum);
			//cout << LC.loc << endl;
		}

		//Adds last entry to table and ends the memory address computation phase
		if (instruction[i + 1][0] == "END")
		{
			addToMap(hashTable, currLine, LC.loc, newEntry);
			break;
		}
	}
}


void passTwo(map <string, entry>& hashTable)
{
	entry temp;
	for (auto const& item : hashTable)
	{
		temp = item.second;
		setMachineCode(temp);
		convertNumToHex(temp);
		hashTable[item.first] = temp;
	}
}

void printResult(map <string, entry> hashTable, map <string, string> symTable)
{
	ofstream of("output.txt");
	cout << endl;
	cout << "Address" << setw(20) << "Machine Code" << setw(15) << "Operands" << setw(20) << "Instructions" << endl;

	of << "Address" << setw(20) << "Machine Code" << setw(15) << "Operands" << setw(20) << "Instructions" << endl;

	for (auto const& item : hashTable)
	{
		cout << item.first;
		cout << setw(14) << item.second.mc;
		cout << setw(22) << item.second.operand;
		cout << setw(14) << item.second.instruct;
		cout << setw(20) << item.second.op1 + item.second.op2;
		cout << endl;

		of << item.first;
		of << setw(14) << item.second.mc;
		of << setw(22) << item.second.operand;
		of << setw(14) << item.second.instruct;
		of << setw(20) << item.second.op1 + item.second.op2;
		of << endl;
	}

	cout << endl;
	cout << "Symbol Table" << endl;
	cout << "Label" << setw(20) << "Memory Address" << endl;

	of << endl;
	of << "Symbol Table" << endl;
	of << "Label" << setw(20) << "Memory Address" << endl;
	for (auto const& item : symTable)
	{
		cout << item.first;
		cout << setw(14) << item.second;
		cout << endl;

		of << item.first;
		of << setw(14) << item.second;
		of << endl;
	 }
}




int main()
{
	vector <vector<string>> instructions = readFile();
	map<string, entry> hashTable;
	map <string, string> symTable;
	fstream outputFile("Output.txt");
    passOne(instructions,hashTable,symTable);
	passTwo(hashTable);
	printResult(hashTable,symTable);
	outputFile.close();
}

