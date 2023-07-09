#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;
fstream fileIN, fileOUT;

vector<string> splitString(string line, string delimiters) 
{
	size_t pos = 0;
	std::string item;
	std::vector<std::string> output;

	while ((pos = line.find_first_of(delimiters)) != std::string::npos) {
		item = line.substr(0, pos);
		output.push_back(item);
		line.erase(0, pos + 1);
	}
	output.push_back(line);

	return output;
}

bool loadFile(string name)
{
	fileIN.open(name.c_str(), fstream::in);
	if (!fileIN.is_open()) {
		cout << "Error!\tFile: " << name << " not found!\n";
		return false;
	}
	cout << name << "\tOK!\n";
	string newName = name.substr(0, name.find_first_of(".")) + ".rose";
	fileOUT.open(newName.c_str(), fstream::out | fstream::binary);
	return true;
}

uint16_t findCommand(string line)
{
	string buff = splitString(line, "=")[0];

	if (buff == "[SkipFRAME]") {
		return 0xCC01;
	}
	else if (buff == "[PlayBgm]") {
		return 0xCC02;
	}
	else if (buff == "[CreateBG]") {
		return 0xCC03;
	}
	else if (buff == "[PrintText]") {
		return 0xCC04;
	}
	else if (buff == "[PlayVoice]") {
		return 0xCC05;
	}
	else if (buff == "[PlaySe]") {
		return 0xCC06;
	}
	else if (buff == "[Next]") {
		return 0xCC07;
	}
	else if (buff == "[PlayMovie]") {
		return 0xCC08;
	}
	else if (buff == "[BlackFade]") {
		return 0xCC09;
	}
	else if (buff == "[WhiteFade]") {
		return 0xCC0A;
	}
	else if (buff == "[SetSELECT]") {
		return 0xCC0B;
	}
	else if (buff == "[EndBGM]") {
		return 0xCC0C;
	}
	else if (buff == "[EndRoll]") {
		return 0xCC0D;
	}
	else if (buff == "[MoveSom]") {
		return 0xCC0E;
	}
	else {
		cout << "uknown command:\t\a" << buff << "\n";
		return 0xCC00;
	}
}

uint32_t timeToMiliSec(string line)
{
	uint32_t timems;
	//mlseconds
	string timeMS; int iTimeMS;
	timeMS += line[6];
	timeMS += line[7];
	iTimeMS = stoi(timeMS);
	timems = iTimeMS;

	//seconds
	string timeS; int iTimeS;
	timeS += line[3];
	timeS += line[4];
	iTimeS = stoi(timeS);
	iTimeS *= 1000;
	timems += iTimeS;

	//minutes
	string timeM; int iTimeM;
	timeM += line[0];
	timeM += line[1];
	iTimeM = stoi(timeM);
	iTimeM *= 60000;
	timems += iTimeM;

	return timems;
}

void codeLine(string line)
{
	string buffS; uint32_t buff32; uint16_t buff16; uint8_t buff8; vector<string> buffVS;
	
	buffVS = splitString(line, "\t");
	buff16 = findCommand(buffVS[0]);
	fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16));

	switch (buff16)
	{
	case 0xCC01: // [SkipFRAME]
		buff32 = timeToMiliSec(splitString(buffVS[0], "=")[1]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write start time
		break;
	
	case 0xCC02: // [PlayBgm]
		buff32 = timeToMiliSec(splitString(buffVS[0], "=")[1]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write start time
		
		buff16 = buffVS[1].size();
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write size of path
		fileOUT << buffVS[1]; // write path
		
		buff32 = timeToMiliSec(buffVS[2]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write end time
		break;

	case 0xCC03: // [CreateBG]
		buff32 = timeToMiliSec(splitString(buffVS[0], "=")[1]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write start time

		buff16 = ("BGS" == buffVS[1]) ? 0xBBBB : 0xAAAA;
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write BGS

		buff16 = buffVS[2].size();
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write size of path
		fileOUT << buffVS[3]; // write path
		
		buff32 = timeToMiliSec(buffVS[4]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write end time
		break;

	case 0xCC04: // [PrintText]
		buff32 = timeToMiliSec(splitString(buffVS[0], "=")[1]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write start time
		
		buff16 = buffVS[1].size();
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write size of name
		fileOUT << buffVS[1]; // write name

		buff16 = buffVS[2].size();
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write size of text
		fileOUT << buffVS[2]; // write text

		buff32 = timeToMiliSec(buffVS[3]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write end time
		break;
	case 0xCC05: // [PlayVoice]
		buff32 = timeToMiliSec(splitString(buffVS[0], "=")[1]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write start time

		buff16 = buffVS[1].size();
		fileOUT.write(reinterpret_cast<const char*>(&buff16), sizeof(buff16)); // write size of path
		fileOUT << buffVS[1]; // write path

		buff8 = (buffVS[2] == "1") ? 1 : 0;
		fileOUT.write(reinterpret_cast<const char*>(&buff8), sizeof(buff8)); // write if speeking person is male

		fileOUT << buffVS[3]; // write short name of person
		
		buff32 = timeToMiliSec(buffVS[4]);
		fileOUT.write(reinterpret_cast<const char*>(&buff32), sizeof(buff32)); // write end time
		break;
			
	case 0xCC06: // [PlaySe]
		

		break;
	case 0xCC07: // [Next]
		//noting
		break;
	case 0xCC08: // [PlayMovie]
		fileIN >> sBuff; // path to movie
		usBuff = sBuff.size();
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff)); //size of path
		fileOUT << sBuff;

		fileIN >> sBuff; // layer??? always 0???
		usBuff = stoi(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff));

		fileIN >> sBuff; // end time
		iBuff = timeToMiliSec(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&iBuff), sizeof(iBuff));
		break;
	case 0xCC09: // [BlackFade]
		fileIN >> sBuff; // IN/OUT
		usBuff = fadeIO(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff));

		fileIN >> sBuff; // end time
		iBuff = timeToMiliSec(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&iBuff), sizeof(iBuff));
		break;
	case 0xCC0A: // [WhiteFade]
		fileIN >> sBuff; // IN/OUT
		usBuff = fadeIO(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff));

		fileIN >> sBuff; // end time
		iBuff = timeToMiliSec(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&iBuff), sizeof(iBuff));
		break;
	case 0xCC0B: // [SetSELECT]
		getline(fileIN, sBuff);
		setSelect(sBuff);
		break;
	case 0xCC0C: // [EndBGM]???
		fileIN >> sBuff; // path to bgm
		usBuff = sBuff.size();
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff)); //size of path
		fileOUT << sBuff;

		fileIN >> sBuff; // end time
		iBuff = timeToMiliSec(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&iBuff), sizeof(iBuff));
		break;
	case 0xCC0D: // [EndRoll]??? - at the end of each episode only?
		fileIN >> sBuff; // path to movie
		usBuff = sBuff.size();
		fileOUT.write(reinterpret_cast<const char*>(&usBuff), sizeof(usBuff)); //size of path
		fileOUT << sBuff;

		fileIN >> sBuff; // end time
		iBuff = timeToMiliSec(sBuff);
		fileOUT.write(reinterpret_cast<const char*>(&iBuff), sizeof(iBuff));
		break;
	case 0xCC0E: // [MoveSom] //Maybe it will be useful for censorship xd
		fileIN >> sBuff; //index?
		fileIN >> sBuff; //end time
		break;
	default:
		cout << "Error!\a\tUknown command!\n";
		cout << "0x" << hex << cmd << "\n";
		break;
	}
}

bool makeBin()
{
	uint32_t header = 0x526F736553637269; //RoseScri
	fileOUT.write(reinterpret_cast<const char*>(&header), sizeof(header));

	string line;
	while (!fileIN.eof())
	{
		getline(fileIN, line);
		//cout << line << "\n";
		if (line.size() > 2) {
			codeLine(line);
		}
	}
	fileIN.close();
	fileOUT.close();
	return true;
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		string name(argv[1]);
		if (loadFile(name)) {
			if (makeBin()) {
				cout << "BIN file was successfully created.\n";
			}
			else {
				cout << "The BIN file was not created correctly.\n";
				cout << "An error has been encountered!\n";
			}
		}
	}
	else {
		cout << "Usage:\nSDScript2bin *file name or path to file*\n\n";
	}
	return 0;
}