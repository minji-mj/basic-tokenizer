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
// Name        : TestPart2.cpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 2
#include <vector>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/join.hpp>

#include "tokenClassifier.hpp"

using namespace boost::algorithm;
using namespace ensc251;
using namespace std;

#ifndef ENSC251_CHECK_EQUAL
#define ENSC251_CHECKER(A, B) BOOST_CHECK(A == B)
#else
#define ENSC251_CHECKER(A, B) BOOST_CHECK_EQUAL(A, B)
#endif

void check(std::string input, vector<std::string> tokens, vector<std::string> expectedTypes)
{
	istringstream code(input);
	std::vector<ensc251::Token*> tokenObjects = tokenClassifier(code);

	for(unsigned int i =0;i<expectedTypes.size();i++)
	{
		std::string type = ensc251::demangle(typeid(*tokenObjects.at(i)).name());
		if(type==expectedTypes.at(i)) {
			if(type=="ensc251::numeric_int_const<int>")
			{
				ensc251::numeric_const<int> *intToken = dynamic_cast<ensc251::numeric_int_const<int> *>(tokenObjects.at(i));
				ENSC251_CHECKER(intToken->getValue(), atoi(tokens.at(i).c_str()));
			}
			else if(type=="ensc251::numeric_const<float>")
			{
				ensc251::numeric_const<float> *floatToken = dynamic_cast<ensc251::numeric_const<float> *>(tokenObjects.at(i));
				ENSC251_CHECKER(floatToken->getValue(), (float)atof(tokens.at(i).c_str()) );
			}
			else if(type=="ensc251::numeric_int_const<char>")
			{
				ensc251::numeric_const<char> *charToken = dynamic_cast<ensc251::numeric_int_const<char> *>(tokenObjects.at(i));
				ENSC251_CHECKER(charToken->getValue(),  tokens.at(i).at(1) );
			}
			else // string const
			{
				ensc251::StringBasedToken *strToken = dynamic_cast<ensc251::StringBasedToken *>(tokenObjects.at(i));
				ENSC251_CHECKER(strToken->getStringValue(), tokens.at(i));
			}
		}
		else {
			std::cout << "For below failure, expected tokenValue: " << tokens.at(i) << std::endl;
			BOOST_CHECK_EQUAL(type, expectedTypes.at(i));
		}
	}
}

BOOST_AUTO_TEST_CASE( test1 ){
	std::string input= "int a = 2 ;";
	vector<std::string> tokens = {"int", "a", "=", "2", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec","ensc251::int_id", "ensc251::gen_assignment_operator","ensc251::numeric_int_const<int>","ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test2 ){
	std::string input= "char c = 'c' ;";
	vector<std::string> tokens = {"char", "c", "=", "'c'", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec","ensc251::int_id", "ensc251::gen_assignment_operator","ensc251::numeric_int_const<char>","ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test3 ){
	std::string input= "float _fnumber; _fnumber += 10.5 % 5;";
	vector<std::string> tokens = {"float","_fnumber",";","_fnumber", "+=", "10.5", "%", "5", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec", "ensc251::numeric_id", "ensc251::punctuator", "ensc251::numeric_id","ensc251::gen_assignment_operator", "ensc251::numeric_const<float>","ensc251::mod_operator","ensc251::numeric_int_const<int>", "ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}
BOOST_AUTO_TEST_CASE( test4 ){
	std::string input= "char Int = errno ? 10 : 20 ;";
	vector<std::string> tokens = {"char","Int", "=", "errno", "?", "errno", ":", "20", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec", "ensc251::int_id","ensc251::gen_assignment_operator", "ensc251::int_id","ensc251::conditional_operator","ensc251::numeric_int_const<int>","ensc251::punctuator","ensc251::numeric_int_const<int>","ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test5 ){
	std::string input= "errno <<= '2' >> 5 ;";
	vector<std::string> tokens = {"a", "<<=", "'2'",">>","5", ";"};
	vector<std::string> expectedTypes = {"ensc251::int_id" , "ensc251::int_assignment_operator", "ensc251::numeric_int_const<char>", "ensc251::shift_operator", "ensc251::numeric_int_const<int>","ensc251::punctuator",};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test6 ){
	std::string input= "float _f; float f = 39E-2 - ( _f ++ ) ;";
	vector<std::string> tokens = {"float", "_f", ";", "float", "f", "=", "39E-2", "-" , "(" , "_f", "++", ")", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec", "ensc251::numeric_id", "ensc251::punctuator", "ensc251::type_spec", "ensc251::numeric_id","ensc251::gen_assignment_operator","ensc251::numeric_const<float>", "ensc251::additive_operator", "ensc251::punctuator", "ensc251::numeric_id", "ensc251::postfix_operator","ensc251::punctuator", "ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test7 ){
	std::string input= "char* _str = \"Hello World\" ;";
	vector<std::string> tokens = {"char", "*", "_str", "=", "\"Hello World\"", ";"};
	vector<std::string> expectedTypes = {"ensc251::type_spec","ensc251::punctuator","ensc251::pointer_id", "ensc251::gen_assignment_operator","ensc251::string","ensc251::punctuator"};
	check(input, tokens, expectedTypes);
}

BOOST_AUTO_TEST_CASE( test8 ){
	std::string input= "char* err &= \"test ;";
	vector<std::string> tokens = {"char", "*", "err", "&", "=", "\"test ;"};
	vector<std::string> expectedTypes = {"ensc251::type_spec","ensc251::punctuator", "ensc251::pointer_id", "ensc251::incorrect", "ensc251::gen_assignment_operator", "ensc251::incorrect"};
	check(input, tokens, expectedTypes);
}

