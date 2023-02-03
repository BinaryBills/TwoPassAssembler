#pragma once
#include <vector>
#include <string>
#include <iterator>
#include<algorithm>
using namespace std;

struct entry
{
	string mc;
	string label;
	string op1;
	string op2;
	string operand;
	string instruct;

	entry()
	{
		mc = "";
		label = "";
		op1 = "";
		op2 = "";
		instruct = "";
	}

};

//Struct that models an address
struct address
{
	char loc[8];
	int size = 8;
	address() { for (int i = 0; i < 8; i++) loc[i] = (0 + '0'); loc[7] = '\0'; } //Initialization
};

string convertVectorToString(vector<string> v)
{
	string temp = v[0];

	for (int i = 1; i < v.size(); i++)
	{
		temp += " " + v[i];
	}
	return temp;
}


string getAddress(string instruction)
{
	string memAddress = "";
	bool found = false;

	for (int i = 0; i < instruction.size(); i++)
	{
		if (found == true)
			memAddress += instruction[i];


		else
		{
			if (instruction[i] == '$')
				found = true;
			else
				found = false;
		}


		if (instruction[i] == ' ')
			found = false;
	}
	return memAddress;
}

template<typename T>
T convertToArray(T& a, int size, string memAddress)
{
	T temp = a;
	int bits = size;
	for (int i = memAddress.size(); i >= 0; i--)
	{
		temp.loc[bits] = memAddress[i];
		bits--;
	}
	return temp;
}

address computeNextAddress(address C, int value)
{
	int y;
	std::stringstream ss;
	std::stringstream sstream;
	
	//Converts current address to integer
	//Adds it by value passed in.
	ss << C.loc;
	ss >> std::hex >> y ;
	y += value;
	
	//Converts Sum back to Hex
	sstream << std::hex << std::uppercase << y;
	std::string result = sstream.str();

	return convertToArray(C, 8, sstream.str());
}

int getSmc68000Bytes(vector<string> currLine)
{
	int sum = 0;
	string currStr = "";
	bool found = false;
	
	//#N is 2 bytes
	for (int i = 0; i < currLine.size(); i++)
	{
		if (found == true)
			break;

		currStr = currLine[i];
		for (int j = 0; j < currStr.size(); j++)
			if (currStr[j] == '#') { sum += 2; break; found = true; }
	}

	found = false;

	//<address is 4 bytes
	for (int i = 0; i < currLine.size(); i++)
	{
		if (found == true)
			break;

		currStr = currLine[i];
		for (int j = 0; j < currStr.size(); j++)
			if (currStr[j] == '$') { sum += 4; break; found = true; }
	}

	return sum;	
}


int getInstructionByte(vector<string> currLine, int currIndex)
{
	if (currLine[currIndex] == "ORG" || currLine[currIndex] == "END" || 
		currLine[currIndex] == "TRAP")
		return 0;
	else
		return 2;
}


void setMachineCode(entry & e)
{
	if (e.instruct == "MOVE")
	{
		//Just Number
		if (e.op1[0] == '#' && e.op2[0] == 'D')
			e.mc = "303C";

		//Number and Address
		if (e.op1[0] == '#' && e.op2[0] == '$')
			e.mc = "33FC";

		//Just Address (Left Side)
		else if (e.op1[0] == '$' && e.op2[0] == 'D')
			e.mc = "3039";

		//Just Address (Right Side)
		else if (e.op1[0] == 'D' && e.op2[0] == '$')
			e.mc = "33CO";
	}

	if (e.instruct == "TRAP")
	{
		if (e.op1 == "#0")
			e.mc = "4E40";
		else if (e.op1 == "#1")
			e.mc = "4E41";
		else if (e.op1 == "#2")
			e.mc = "4E42";
	}

	if (e.instruct == "ADD")
	{
	  if (e.op1[0] == '$' && e.op2[0] == 'D')
		e.mc = "D079";
	
	 else if (e.op1[0] == 'D' && e.op2[0] == '$')
		e.mc = "D179";
	}

	if (e.instruct == "ADDI")
	{
		if (e.op1[0] == '#' && e.op2[0] == 'D')
			e.mc = "0640";
		else if (e.op1[0] == '#' && e.op2[0] == '$')
			e.mc = "0679";
	}
	
}

void convertNumToHex(entry& e)
{
	string hex;
	int num;
	bool hasComma = false;
	
	if (e.op1[0] == '#')
	{
		for (int i = 0; i < e.op1.size(); i++)
		{
			if (e.op1[i] != '#' && e.op1[i] != ',')
				hex += e.op1[i];

			if (e.op1[i] == ',')
				hasComma = true;
		}
	}



	if (hex.size() > 0)
	{
		num = stoi(hex);
		std::stringstream sstream;
		sstream << std::hex << std::uppercase << num;
		std::string result = sstream.str();

		if (hasComma == true)
		{
				e.operand = sstream.str() + ',' + e.op2;
			
			
		}
		else
			e.operand = sstream.str();
	}
	else
	{
		if (e.op1[0] != 'D')
			e.operand = e.op1 + e.op2;
		else
			e.operand = e.op2;
	}


	if (e.instruct == "TRAP")
		e.operand = "";


	if (e.op2[0] == 'D')
	{
		e.operand = e.op1;
		e.operand.erase(std::prev(e.operand.end()));
	}
}