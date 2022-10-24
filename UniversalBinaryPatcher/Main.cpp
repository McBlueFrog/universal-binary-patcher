#include <windows.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

typedef unsigned char BYTE;
int offset = 0;
std::string data;
char instruction = ' ';
std::vector<BYTE> fileData;

/// instruction var
uint64_t iOffset = 0;
int icHex;
std::vector<BYTE> iHex;


void finishPatch(const char* file)
{
	std::ofstream mainFile(file, std::ios::binary);
	for (int i = 0; i < fileData.size(); i++)
	{
		mainFile << fileData.at(i);
	}
}


void patch()
{
	for (int i = 0; i < iHex.size(); i++)
	{
		fileData.at(iOffset + i) = iHex.at(i);
	}
	iHex.clear();
}




void executeInstructions()
{
	std::ifstream inst("code.UBPI");
	char ch;
	while (inst >> std::noskipws >> ch) {
		
		if (ch == ' ' || ch == '\n')
		{
			if (instruction == 'h')
			{
				if (data == "end")
				{
					patch();
					instruction = ' ';
				}
				if ((ch == ' ' || ch == '\n') && instruction == 'h')
				{
					std::istringstream buffer(data);
					buffer >> std::hex >> icHex;
					iHex.push_back(icHex);
				}
			}
			if (instruction == 'o')
			{
				std::istringstream buffer(data);
				buffer >> std::hex >> iOffset;
				instruction = 'h';
			}
			if (data == "offset")
			{
				instruction = 'o';
			}
			data = "";
		}
		else
		{
			data = data + ch;
		}
	}
	inst.close();
}


std::vector<BYTE> processFile(const char* filename)
{
	// open the file:
	std::streampos fileSize;
	std::ifstream file(filename, std::ios_base::binary);
	std::ofstream("backup", std::ios_base::binary) << file.rdbuf();
	std::cout << "File was successfuly backed up\n";

	// get its size:
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// read the data:
	std::vector<BYTE> fileData(fileSize);
	file.read((char*)&fileData[0], fileSize);
	file.close();
	return fileData;
}


void readFile(int from, size_t to)
{
	std::cout << std::showbase << std::internal << std::setfill('0');
	if (from >= 0 && to <= fileData.size())
	{
		std::cout << std::setw(8) << std::hex << (unsigned int)offset << ' ';
		for (int i = from; i < to; i++)
		{
			if ((i + 1) % 8 == 0)
			{
				std::cout << std::hex << (unsigned int)fileData.at(i) << '\t';
				for (int x = (i - 7); x <= i; x++)
				{
					if (fileData.at(x) >= 33 && fileData.at(x) != 127 && fileData.at(x) != 129 && fileData.at(x) != 141 && fileData.at(x) != 143 && fileData.at(x) != 144 && fileData.at(x) != 157 && fileData.at(x) != 160 && fileData.at(x) != 173 && fileData.at(x) != 255)
					{
						std::cout << fileData.at(x);
					}
					else
					{
						std::cout << '.';
					}
				}
				std::cout << std::endl;
				offset = offset + 8;
				std::cout << std::setw(8) << std::hex << (unsigned int)offset << ' ';
			}
			else
			{
				std::cout << std::hex << (unsigned int)fileData.at(i) << '\t';
			}
		}
	}
	else
	{
		std::cout << "Error the observable hex values were outside the scope of the file";
	}
	std::cout << std::endl;
	offset = 0;
}


int main(int argc, char** argv)
{
	SetConsoleTitle("Universal Binary Patcher");
	if (argc > 0)
	{
		fileData = processFile(argv[1]);
		//readFile(0, fileData.size());
		executeInstructions();
		finishPatch(argv[1]);
		//readFile( 0, fileData.size());
	}
	else
	{
		std::cout << "Drop the patchable file on the executable to patch it\n";
	}
	std::cout << std::endl;
	system("pause");
	return 0;
}