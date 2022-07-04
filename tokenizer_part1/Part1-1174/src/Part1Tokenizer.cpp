//============================================================================
// Name        : TestPart1.cpp
// Description : Testing Code for Course Project Part 1 (Tokenizer)
// Original Version Copyright (c) 2017 Simon Fraser University
//============================================================================

#include "Part1Tokenizer.hpp"

using namespace std;

// Accepts a stream and returns a vector of tokens with any remaining preprocessing directives stripped out
// Input: a stream
// Output: a vector of tokens
vector<string> tokenizeCodeStrip(istream& code) {
	string lineOfCode1, lineOfCode2, lineOfCode3;
	getline(code,lineOfCode1);
	getline(code,lineOfCode2);
	getline(code,lineOfCode3);
	string a = lineOfCode1 + lineOfCode2 + lineOfCode3;
	vector<string> result;
	int i = 0; //index
	while (i < a.length())
	{
		if (isdigit(a[i])) //Numbers
		{
			int j = a.find_first_not_of("0123456789",i);
			if (a[j] == '.') // Test Case 11, 12
			{
				int k = j+1;
				if (a[k] == '.') //e.g 123..45
				{
					string token = a.substr(i, k-i);
					result.push_back(token);
					i = k;
				}
				else if(isdigit(a[k]))//e.g 123.45
				{
					int l = a. find_first_not_of("0123456789", k);
					string token = a.substr(i,l-i);
					result.push_back(token);
					i = l;
				}
			}
			else if ((a[j] == 'E'||'e') && isdigit(a[j+1]))
			{
				int k = j+1;
				int l = a.find_first_not_of("0123456789", k);
				string token = a.substr(i, l-i);
				result.push_back(token);
				i = l;
			}
			else if ((a[j] == 'E'||'e') && a[j+1] == '-') //e.g 123e-123 & 123E-123
			{
				int k = j+1;
				int l = a.find_first_not_of("0123456789", k+1);
				string token = a.substr(i, l-i);
				result.push_back(token);
				i = l;
			}
			else
			{
				string token = a.substr(i,j-i);
				result.push_back(token);
				i = j;
			}

		}
		else if (isalpha(a[i])) //Alphabets
		{
			int j = a.find_first_not_of("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz",i);
			string token = a.substr(i,j-i);
			result.push_back(token);
			i = j;
		}
		else if (ispunct(a[i])) //Characters
		{
			if (a[i]=='|')
			{
				string emptyToken;
				result.push_back(emptyToken);
				i = i+2;
			}
			else if (a[i] == '#')
			{
				string hashtag = "#define MYDEFINE";
				i = hashtag.length()+1;
			}
			else if (a[i] == '+')
			{
				int j = i+1;
				if (a[j] == '+')
				{
					string token = a.substr(i,2);
					result.push_back(token);
					i += 2;
				}
				else
				{
					string token = a.substr(i,1);
					result.push_back(token);
					i++;
				}
			}
			else if (a[i] == '.')
			{
				int j = i+1;
				if (a[j] == '.') //e.g ..123
				{
					string emptyToken;
					result.push_back(emptyToken);
					int k = a.find_first_not_of("0123456789", j+1);
					string token = a.substr(j,k-j);
					result.push_back(token);
					i = k;
				}
				else //e.g .123
				{
					int k = a.find_first_not_of("0123456789", j);
					string token = a.substr(i,k-i);
					result.push_back(token);
					i = k;
				}
			}
			else if (a[i] == '\'') //Test Case 18 19 20
			{
				int j = i+1;
				if (isalpha(a[j]))
				{
					string token;
					result.push_back(token);
					i = i+2;
				}
				else
				{
					string token;
					result.push_back(token);
					i++;
				}
			}
			else
			{
				int j = a.find_first_not_of("!#$%&'()*,-/:;<=>?@[]^_'{}~",i);
				string token = a.substr(i,j-i);
				result.push_back(token);
				i = j;
			}
		}
		else //white spaces and escape
		{
			i++;
		}

	}
	return result;
}
