//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.cpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P5_<userid1>_<userid2>" (eg. P5_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClassifier.cpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include <vector>
#include <sstream>
#include <algorithm>
#include "tokenClasses.hpp"

#include "Part1Tokenizer.hpp"
#include <string>

using namespace std;
using namespace ensc251;


// \n is also whitespace, but should have been removed by getline()
#define WHITE "\r\t\v\f "

std::string delim3[] = { ">>=", "<<=" };
std::string delim2[] = { "+=","-=", "/=", "%=", "<<", ">>", "++", "--" };
std::string delim1andWhite = WHITE "/()*%:;=+-~?";
std::string white = WHITE;
std::string idEle = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789_";
std::string pointerId = "";

bool processDigits(std::string line, std::string::size_type &index)
{
	bool DigiFound = false;
	while (index < line.length() && isdigit(line.at(index)))
	{
		index++;
		DigiFound = true;
	}
	return DigiFound;
}

bool isInt(std::string sub)
{
	bool dotOrEFound = false;
	std::string::size_type index1 = 0;
	while (index1 < sub.length())
	{
		if (sub[index1] == '.' || sub[index1] == 'e' || sub[index1] == 'E')
		{
			dotOrEFound = true;
			break;
		}
		else
			index1++;
	}
	if (dotOrEFound == false)
		return true;
	else
		return false;
}

// Construct token objects categorized into appropriate types
// Input: a stream
// Output: a vector containing the list of Token objects properly categorized

std::vector<ensc251::Token*> tokenClassifier(istream& code)
{
	std::vector<ensc251::Token*> tokenObjectPs;
	// ensc251::Token* myToken;

	std::string line;

	while (getline(code, line)) {

		int lineNumber = 0;

		if (line.at(0) == '#')
			continue;

		std::string::size_type index2 = 0;
		auto length = line.length();

		while (index2 < length) {
			auto index1 = line.find_first_not_of(white, index2);
			if (index1 == std::string::npos)
				break;

			/// Dealing with int and float consts
			if (isdigit(line.at(index1)) || line.at(index1) == '.')
			{
				index2 = index1;

				// integer consts
				bool preDigitFound = processDigits(line, index2);

				// floats
				if (index2 < length && line.at(index2) == '.')
				{
					index2++;
					bool postDigitFound = processDigits(line, index2);

					if (!preDigitFound && !postDigitFound) // detecting the single "." as an invalid token!
					{
						cout << "Error -- invalid token in input: " << line.substr(index1, index2 - index1) << endl;

						/////
						//std::string tkn = line.substr(index1, index2-index1); //numeric_int_const<char>


						//result.push_back("");
						continue;
					}
				}

				// exponentials
				if (index2 < length && (line.at(index2) == 'e' || line.at(index2) == 'E'))
				{
					if (line.at(index2 + 1) == '+' || line.at(index2 + 1) == '-')
						index2 += 2;
					else
						index2++;

					if (!processDigits(line, index2)) // invalid float -> roll back ...
					{
						if (line.at(index2 - 1) == 'e' || line.at(index2 - 1) == 'E')
							index2--;
						else
							index2 -= 2;
					}
				}

				// suffix f and F
				if (index2 < length && (line.at(index2) == 'f' || line.at(index2) == 'F')) ///// FLOAT
					index2++;

				auto subs = line.substr(index1, index2 - index1);

				if (isInt(subs) == true)
				{
					int tkn = stoi(subs);
					ensc251::numeric_int_const<int>* tokenType = new ensc251::numeric_int_const<int>(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else
				{
					float tkn = stof(subs);
					ensc251::numeric_const<float>* tokenType = new ensc251::numeric_const<float>(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				//result.push_back(subs);
				continue;
			}

			for (auto delim : delim3) {
				if (line.substr(index1, 3) == delim) {

					/////
					//result.push_back(delim);
					std::string tkn = line.substr(index1, index2 - index1);
					if (tkn == "<<=" || tkn == ">>=") // int_assignment_operator
					{
						ensc251::int_assignment_operator* tokenType = new ensc251::int_assignment_operator(tkn, lineNumber, index1);
						tokenObjectPs.push_back(tokenType);
					}

					index2 = index1 + 3;
					break;
				}
			}
			if (index2 > index1) continue;

			// the below follows the pattern from the lines above.
			for (auto delim : delim2) {
				if (line.substr(index1, 2) == delim) {

					/////
					//result.push_back(delim);
					std::string tkn = line.substr(index1, index2 - index1);
					if (tkn == "%=") // int_assignment_operator
					{
						ensc251::int_assignment_operator* tokenType = new ensc251::int_assignment_operator(tkn, lineNumber, index1);
						tokenObjectPs.push_back(tokenType);
					}

					index2 = index1 + 2;
					break;
				}
			}
			if (index2 > index1) continue;

			if (line[index1] == '"') {
				index2 = index1 + 1;
				while (index2 < length) {
					if (line[index2] == '\\') {
						index2++; // skip over escape character
					}
					else
						if (line.at(index2) == '"') {

							/////
							//result.push_back(line.substr(index1, index2 + 1 - index1)); /////STRING
							std::string tkn = line.substr(index1, index2 + 1 - index1);
							ensc251::string* tokenType = new ensc251::string(tkn, lineNumber, index1);
							tokenObjectPs.push_back(tokenType);

							break;
						}
					index2++;
				}
				if (index2 >= length) { // String is not terminated!
					cout << "Error:  string is not terminated!" << endl;

					/////
					//result.push_back("");
					index2 = line.length();
					std::string tkn = line.substr(index1, index2 + 1 - index1); //????? INCORRECT
					ensc251::incorrect* tokenType = new ensc251::incorrect(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);


					break;
				}
				index2++; // non-empty string was pushed on the result vector
				continue;
			}

			if (line[index1] == '\'') {
				index2 = index1 + 1;
				if (index2 < length && line[index2] == '\\') {
					index2++; // skip over escape character
				}
				index2++; // skip over character
				if (index2 >= length || line.at(index2) != '\'') // Character constant is not terminated!
				{
					cout << "Error:  character constant is not terminated!" << endl;
					//result.push_back("");
				}
				else {
					index2++;
					//result.push_back(line.substr(index1, index2 - index1));
				}
				continue;
			}

			if (line[index1] == '_' || isalpha(line[index1])) {
				index2 = index1 + 1;
				while (index2 < length && (line[index2] == '_' || isalnum(line[index2])))
					index2++;

				/////
				std::string tkn = line.substr(index1, index2 - index1);
				if (tkn == "char" || tkn == "int" || tkn == "float") // type_spec
				{
					ensc251::type_spec* tokenType = new ensc251::type_spec(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				if (tokenObjectPs.front() /*or tokenObjectPs.back()*/ == "char" || tokenObjectPs.front() == "int")
				{
					ensc251::int_id* tokenType = new ensc251::int_id(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}
			}


			index2 = line.find_first_of(delim1andWhite, index1);
			if (index2 == index1) {
				index2++; // we found a 1-character token

						  /////
				std::string tkn = line.substr(index1, index2 - index1); // index2-index1 not very necessary because only 1-character
				if (tkn == ";" || tkn == "*" || tkn == "(" || tkn == ")" || tkn == ":")
				{
					ensc251::punctuator* tokenType = new ensc251::punctuator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);

					if (tkn == "*")
					{
						int index1 = line.find_first_not_of(white, index2);
						int index2 = line.find_first_not_of(idEle, index1);
						pointerId = line.substr(index1, index2 - index1);

						ensc251::pointer_id* tokenType = new ensc251::pointer_id(tkn, lineNumber, index1);
						tokenObjectPs.push_back(tokenType);
					}
					else { continue; }
				}

				else if (tkn == "?")
				{
					ensc251::conditional_operator* tokenType = new ensc251::conditional_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else if (tkn == "+" || tkn == "-")
				{
					ensc251::additive_operator* tokenType = new ensc251::additive_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else if (tkn == "/")
				{
					ensc251::div_operator* tokenType = new ensc251::div_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else if (tkn == "%")
				{
					ensc251::mod_operator* tokenType = new ensc251::mod_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else if (tkn == "=")
				{
					ensc251::gen_assignment_operator* tokenType = new ensc251::gen_assignment_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else if (tkn == "~")
				{
					ensc251::comp_operator* tokenType = new ensc251::comp_operator(tkn, lineNumber, index1);
					tokenObjectPs.push_back(tokenType);
				}

				else { continue; }


				//result.push_back(line.substr(index1, index2 - index1));
				continue;
			}
			cout << "Error -- invalid token in input: " <<
				line.substr(index1, index2 - index1) << endl;
			//result.push_back("");
		}
		lineNumber++;
	}
	return tokenObjectPs;
}

