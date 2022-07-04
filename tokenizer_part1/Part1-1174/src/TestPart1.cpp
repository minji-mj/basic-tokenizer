//============================================================================
// Name        : TestPart1.cpp
// Description : Testing Code for Course Project Part 1 (Tokenizer)
// Original Version Copyright (c) 2017 Simon Fraser University
//============================================================================


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 1
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/join.hpp>

#include <sstream>

#include "Part1Tokenizer.hpp"

using namespace std;
using namespace boost::algorithm;

void testTokenStrings(string codeString, vector<string> desiredTokenVector)
{
	string newLineTab = " \n\t ";
	string newLine = " \n ";
	istringstream code(codeString);
	auto vectorToTest = tokenizeCodeStrip(code);
	string joinedVectorToTest = newLineTab + join(vectorToTest, " \n\t ") + newLine;
	string joinedDesiredTokenVector = newLineTab + join(desiredTokenVector, " \n\t ") + newLine;

	BOOST_CHECK_EQUAL(joinedVectorToTest, joinedDesiredTokenVector);
}

BOOST_AUTO_TEST_CASE( test1 )
{
	testTokenStrings("errno = errno + 10 ;",
			{"errno","=","errno","+","10",";"});
}

BOOST_AUTO_TEST_CASE( test2 )
{
	testTokenStrings("errno=errno+10;",
			{"errno","=","errno","+","10",";"});
}

BOOST_AUTO_TEST_CASE( test3 )
{
	testTokenStrings("errno=errno+10\n;",
			{"errno","=","errno","+","10",";"});
}

BOOST_AUTO_TEST_CASE( test4 )
{
	testTokenStrings("errno=errno||\n10\n;",
			{"errno","=","errno","","10",";"});
}

BOOST_AUTO_TEST_CASE( test5 )
{
	testTokenStrings("char* str; str = \"string test.\";",
			{"char", "*", "str",";", "str","=","\"string test.\"",";"});
}

BOOST_AUTO_TEST_CASE( test6 )
{
	testTokenStrings("char* str; str    =    \"  x  \"     \"  +  \"     \"  \"    ; ",
			         {"char", "*", "str",";", "str", "=", "\"  x  \"",  "\"  +  \"",  "\"  \"", ";"});
}

BOOST_AUTO_TEST_CASE( test7 )
{
	testTokenStrings("errno+++errno;",
			{"errno","++","+","errno",";"});
}


BOOST_AUTO_TEST_CASE( test8 )
{
	testTokenStrings("#define MY_DEFINE \n int i;",
										{"int","i",";"});
}

BOOST_AUTO_TEST_CASE( test9 )
{
    // the comment below shows what's in memory (but with extra whitespace
    //      to align things as much as possible) delimited by '[' and ']' and
    //      using ? to indicate control characters:
    //                 [#define MYDEFINE ? ?  str    =     """       "+"      ""     ; ]
	testTokenStrings(  "#define MYDEFINE \r\n str    =    \"\\\"\"    \"+\"    \"\"    ; ",
										  {  "str", "=", "\"\\\"\"", "\"+\"", "\"\"", ";"});
}

BOOST_AUTO_TEST_CASE( test10 )
{
    // the comment below shows what's in memory (but with extra whitespace
    //      to align things as much as possible) delimited by '[' and ']' and
    //      using ? to indicate control characters:
    //                 [#define MYDEFINE ? ?  str    =     "x"      "+"      ""     ; ]
	testTokenStrings(  "#define MYDEFINE \r\n str    =    \"x\"    \"+\"    \"\"    ; ",
										  {  "str", "=", "\"x\"", "\"+\"", "\"\"", ";"});
}

// below demonstrates a bug in one of our libraries on at least some platforms
/*
BOOST_AUTO_TEST_CASE( testBug )
{
    // the comment below shows what's in memory (but with extra whitespace
    //      to align things as much as possible) delimited by '[' and ']' and
    //      using ? to indicate control characters:
    //                 [#define MYDEFINE ? ?  str    =     "\""        "+"      ""     ; ]
	testTokenStrings(  "#define MYDEFINE \r\n str    =    \"\\\"\"    \"+\"    \"\"    ; ",
										  {  "str", "=", "\"\\\"\"", "\"+\"", "\"\"", ";"});
}
*/

BOOST_AUTO_TEST_CASE( test11 )
{
	testTokenStrings("a = 50001.1123 + .25;", {"a", "=", "50001.1123","+",".25", ";"});
}

BOOST_AUTO_TEST_CASE( test12 )
{
	testTokenStrings("a = 50001..1123 + .25;", {"a", "=", "50001.", ".1123","+",".25", ";"});
}

BOOST_AUTO_TEST_CASE( test13 )
{
	testTokenStrings("a = ..1123 + .25;", {"a", "=", "", ".1123","+",".25", ";"});
}

BOOST_AUTO_TEST_CASE( test14 )
{
	testTokenStrings("float pi; pi = 314159E-5;",
			{"float","pi",";","pi","=","314159E-5",";"});
}

BOOST_AUTO_TEST_CASE( test15 )
{
	testTokenStrings("sumFloat = 314159E-5 + 12345e-3;",
			{"sumFloat","=","314159E-5","+","12345e-3",";"});
}

BOOST_AUTO_TEST_CASE( test16 )
{
	testTokenStrings("sumF=35e3+4120E009;",
			{"sumF","=","35e3","+","4120E009",";"});
}

BOOST_AUTO_TEST_CASE( test17 )
{
	testTokenStrings("newLineChar = '\\n';",
			{"newLineChar","=","'\\n'",";"});
}

// Below mentioned test case would generate error by C compiler.
// However, we should be able to get tokens for it.
BOOST_AUTO_TEST_CASE( test18 )
{
	testTokenStrings("fNameInitial = 'A ;",
			{"fNameInitial","=","",";"});
}

BOOST_AUTO_TEST_CASE( test19 )
{
	testTokenStrings("fNameInitial = 'A\n ;",
			{"fNameInitial","=","",";"});
}

BOOST_AUTO_TEST_CASE( test20 )
{
	testTokenStrings("fNameInitial = '\n ;",
			{"fNameInitial","=","",";"});
}

BOOST_AUTO_TEST_CASE( test21 )
{
	testTokenStrings("str = \"Hello World out there ; \n str++;",
			{"str","=","","str","++",";"});
}

BOOST_AUTO_TEST_CASE( test22 )
{
	testTokenStrings("float a= 1.2e-errno;",
			{"float","a", "=", "1.2", "e", "-", "errno", ";"});
}

BOOST_AUTO_TEST_CASE( test23 )
{
	testTokenStrings("float a= 1.2eerrno;",
			{"float","a", "=", "1.2", "eerrno", ";"});
}
