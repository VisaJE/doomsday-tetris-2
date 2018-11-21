/*
 * Highscorer.cpp
 *
 *  Created on: 6.6.2018
 *      Author: Eemil
 */
#include <iostream>
#include <string>
#include <sstream>
#include "Highscorer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <fstream>
#include <vector>
#include <climits>
using namespace std;
using namespace rapidjson;
namespace tet {

Highscorer::Highscorer() {
	for (int i = 0; i < 10; i++) {
		currentScore[i] = 0;
	}
	Document scoreBoard;
	if (!fileExists(".hs.json")) setFile(prototype);
	else {
		try {
			string t = readFile();
			scoreBoard.Parse(t.c_str());
			if (scoreBoard.HasParseError()) {
				cout << "Parsing error. Leaderboard reset." << endl;
				setFile(prototype);
				readFile();

			}
			else if (validate(&scoreBoard)) {
				updateLists(&scoreBoard);
			} else {
				setFile(prototype);
			}
		} catch (int i) { cout << "Failure in setting the leaderboard." << endl;}
	}

}

Highscorer::~Highscorer() {
}


void Highscorer::getHighscore(string name[10], int score[10]) {
	for (int i = 0; i < 10; i++) {
			name[i] = currentBoard[i];
			score[i] = currentScore[i];
	}
	sort(name, score);

}

int Highscorer::getHighest() {
	int highest = 0;
	for (int i = 0; i < 10; i++) if (currentScore[i] > highest) highest = currentScore[i];
	return highest;
}

int Highscorer::getLowest() {
	int lowest = INT_MAX;
	for (int i = 0; i < 10; i++) if (currentScore[i] < lowest) lowest = currentScore[i];
	return lowest;
}
void Highscorer::sort(string name[10], int score[10]) {
	int i = 0;
	while (i < 8) {
		while (score[i+1] <= score[i] && i < 9) {
			++i;
		}
		if (i < 9 && score[i+1] > score[i]) {
			string tempn = name[i];
			int temps = score[i];
			name[i] = name[i+1];
			score[i] = score[i+1];
			name[i+1] = tempn;
			score[i+1] = temps;
			i = 0;
		}
	}
}


bool Highscorer::addScore(std::string name, int score) {
	int putHere = -1;
	for (int i = 0; i < 10; i++) {
		if (currentScore[i] == 0) putHere = i;
	}
	if (putHere != -1) {
		currentScore[putHere] = score;
		currentBoard[putHere] = name;
		refreshFile();
		return true;
	}
	else {
		int smallest = score;
		for (int i = 0; i < 10; i++) {
				if (currentScore[i] < smallest) {
					smallest = currentScore[i];
					putHere = i;
				}
		}
		if (putHere != -1) {
			currentScore[putHere] = score;
			currentBoard[putHere] = name;
			refreshFile();
			return true;
		}
	}
	return false;
}


void Highscorer::refreshFile() {
	Document scoreBoard;
	if (!fileExists(".hs.json")) setFile(prototype);
	else {
		scoreBoard.Parse(readFile().c_str());
		if (!validate(&scoreBoard)) setFile(prototype);
	}
	scoreBoard.Parse(readFile().c_str());

	Value::MemberIterator names = scoreBoard.FindMember("name");
	Value::MemberIterator scores = scoreBoard.FindMember("score");
	for (int i = 0; i < 10; i++) {
		const char* thisname = currentBoard[i].c_str();
		names->value[i] = StringRef(thisname);
		scores->value[i] = currentScore[i];
	}
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	scoreBoard.Accept(writer);
	setFile(buffer.GetString());

}


void Highscorer::updateLists(Document *d) {
	for (int i = 0; i < 10; i++) {
		currentBoard[i] = "";
		currentScore[i] = 0;
		Value::MemberIterator names = d->FindMember("name");
		Value::MemberIterator scores = d->FindMember("score");
		if (names->value[i].IsString()) {
			currentBoard[i] = names->value[i].GetString();
			currentScore[i] = scores->value[i].GetInt();
		}
	}
}


#include <sys/stat.h>
// Function: fileExists
/**
    Check if a file exists
@param[in] filename - the name of the file to check

@return    true if the file exists, else false

*/
bool Highscorer::fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

std::string encrypt(std::string msg, std::string const& key)
{
    // Side effects if the following is not written:
    // In my case, division by 0.
    // In the other case, stuck in an infinite loop.
    if(!key.size())
        return msg;

    for (std::string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i%key.size()];
    return msg;
}

// Rewritten to use const& on both parameters
std::string decrypt(std::string const& msg, std::string const& key)
{
    return encrypt(msg, key); // lol
}


void Highscorer::setFile(string t) {
	remove (".hs.json");
	ofstream out(".hs.json");
	string enc = encrypt(t, key);
	out << enc;
	out.close();
}


string Highscorer::readFile() {
	file = fopen(".hs.json", "rb");
	stringstream s;
	char c;
	do {
		c = fgetc(file);
		s << c;
	} while (c != EOF );
	string out = decrypt(s.str(), key);
	while(out.back() != '}' && out.size() > 0){
		out.pop_back();
	}
	fclose(file);
	return out;
}

bool Highscorer::validate(Document *d) {
	bool valid = true;
	if (!d->IsObject()) valid = false;

	Value::MemberIterator here = d->FindMember("name");
	if (here == d->MemberEnd()) valid = false;
	if (!(here->value.IsArray())) valid = false;
	for (int i = 0; i < 10; i++) {
		if (!(here->value[i].IsString())) valid = false;
	}

	here = d->FindMember("score");
	if (here == d->MemberEnd()) valid = false;
	if (!(here->value.IsArray())) valid = false;
	for (int i = 0; i < 10; i++) {
		if (!(here->value[i].IsNumber())) valid = false;
	}
	return valid;
}

} /* namespace tet */
