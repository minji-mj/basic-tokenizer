//============================================================================
// Name        : TestPart5.cpp
// Description : Testing Code for Course Project Part 5 (Interpeting/Evaluation)
// Original Version Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include <iostream>
#include <sstream>
#include "tokenClassifier.hpp"
using namespace std;

#define DONT_RUN_MAIN

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 5
#include <boost/test/unit_test.hpp>
//#include <boost/algorithm/string.hpp>

using namespace boost;

void check(std::string input, std::vector<std::string> ids, std::vector<std::string> values)
{
	std::cout <<  "\n*** " << boost::unit_test::framework::current_test_case().p_name << " ***\n";
	istringstream inputStream(input);
	ensc251::Token *ast_root = nullptr;
	try{
		ensc251::tokenObjectPs = tokenClassifier(inputStream);
		ast_root = ensc251::recursive_parser();
		if(ast_root)
		{
			ast_root->print_ast();
			ast_root->eval();
			ensc251::print_evaluation_map();
			for(unsigned i=0; i < ids.size(); i++)
			{
				std::string userValue="";
				userValue=ensc251::evalMap.find(ids[i])->second.sptc->getStringValue();
				if(userValue != values[i])
					BOOST_ERROR("Error in the id ("+ids[i]+"): the user value ("+userValue+") != expected value ("+values[i]+").");
			}
		}
		else
			BOOST_ERROR("Syntax error in input.");
	}
	catch(std::exception & e) {
		if(values[0]=="Exception during evaluation")
			std::cout<<e.what();
		else
			BOOST_ERROR(e.what());
	}
	if(ast_root) { // destroy remaining tokens
		delete ast_root;
		ast_root = nullptr;
	}

	ensc251::evalMap.clear(); // Clear the map for the next test case!
	ensc251::tracker = 0; // Reset the tracker for the next case!
}

// assignment
BOOST_AUTO_TEST_CASE(test0)
{
	std::string input("int i; i=100;");
	std::vector<std::string> ids  = {"i"};
	std::vector<std::string> values  = {"100"};
	check(input, ids, values);
}

// assignment
BOOST_AUTO_TEST_CASE(test1)
{
	std::string input("float f; char c; errno=100; f=0.45; c='a';");
	std::vector<std::string> ids  = {"errno", "f","c"};
	std::vector<std::string> values  = {"100","0.450000","97"};
	check(input, ids, values);
}

// additive
BOOST_AUTO_TEST_CASE(test2)
{
	std::string input("int num; char chr; float flt; num = 6; flt=1; chr=1; num=num-10; flt = num + 20.15; chr='a'+1;");
	std::vector<std::string> ids  = {"num","flt","chr"};
	std::vector<std::string> values  = {"-4","16.150000","98"};
	check(input, ids, values);
}

// postfix
BOOST_AUTO_TEST_CASE(test3)
{
	std::string input("int num; char chr; float flt; num = 10; flt=19.99; chr='t'; num++; chr=flt--; chr=chr++;");
	std::vector<std::string> ids  = {"num","flt","chr"};
	std::vector<std::string> values  = {"11","18.990000","19"};
	check(input, ids, values);
}

// conditional
BOOST_AUTO_TEST_CASE(test4a)
{
	std::string input("float res; errno=0; res = errno ? 3.5 : 14.3;");
	std::vector<std::string> ids  = {"res"};
	std::vector<std::string> values  = {"14.300000"};
	check(input, ids, values);
}

// conditional 2
BOOST_AUTO_TEST_CASE(test4b)
{
	std::string input("int res; char c; res=0; c='0'; res = res ? c ? c : 30 : c++;");
	std::vector<std::string> ids  = {"res"};
	std::vector<std::string> values  = {"48"};
	check(input, ids, values);
}

// shift
BOOST_AUTO_TEST_CASE(test5)
{
	std::string input("int s; char v; int n; n=2; v=64>>5; s=v<<n;");
	std::vector<std::string> ids  = {"v","s", "n"};
	std::vector<std::string> values  = {"2","8", "2"};
	check(input, ids, values);
}

// shift
BOOST_AUTO_TEST_CASE(test5b)
{
	std::string input("int s; char v; int n; n=2; v=64>>5; s=n=v<<n;");
	std::vector<std::string> ids  = {"v","s", "n"};
	std::vector<std::string> values  = {"2","8", "8"};
	check(input, ids, values);
}

// div
BOOST_AUTO_TEST_CASE(test6)
{
	std::string input("int m; float f1; float f2; f2=10.5; m=100; f1=50.0/f2; m=m%50;");
	std::vector<std::string> ids  = {"f1","m"};
	std::vector<std::string> values  = {"4.761905","0"};
	check(input, ids, values);
}

// combined 1
BOOST_AUTO_TEST_CASE(test7a)
{
	std::string input("int a; float f; char c; int s; c=0; a=100+5-40; f=a/12.0-(a%13); c=a+++f---~c; s=a>>5<<c;");
	std::vector<std::string> ids  = {"a","f","c","s"};
	std::vector<std::string> values  = {"66","4.416667","71","256"};
	check(input, ids, values);
}

// divide by zero (float)
BOOST_AUTO_TEST_CASE(test8)
{
	std::string input("float i; i=1.5/0;");
	std::vector<std::string> ids  = {"i"};
	//  the floating value "Inf" (Infinity)
	std::vector<std::string> values  = {"inf"};
	check(input, ids, values);
}

// divide by zero (int or char)
BOOST_AUTO_TEST_CASE(testE2)
{
	std::string input("int j; j = 10 / 0;");
	std::vector<std::string> ids  = {"j"};
	std::vector<std::string> values  = {"Exception during evaluation"};
	check(input, ids, values);
}

// divide by zero (int or char)
BOOST_AUTO_TEST_CASE(testE3	)
{
	std::string input("char c; int a; a=10; c=0; a = a / c;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"Exception during evaluation"};
	check(input, ids, values);
}



// test cases added

// gen_assignment_operator

BOOST_AUTO_TEST_CASE(testA1	) 	// /=
{
	std::string input("int a; int b; a=5; b=10; b/=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"5","2"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(testA2	) 	// +=
{
	std::string input("int a; int b; a=5; b=10; b+=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"5","15"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(testA3	) 	// -=
{
	std::string input("int a; int b; a=5; b=10; b-=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"5","5"};
	check(input, ids, values);
}

// int_assignment_operator

BOOST_AUTO_TEST_CASE(testA4	) 	// <<=
{
	std::string input("int a; int b; a=2; b=16; b<<=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"2","64"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(testA5	) 	// >>=
{
	std::string input("int a; int b; a=2; b=16; b>>=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"2","4"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(testA6	) 	// %=
{
	std::string input("int a; int b; a=2; b=17; b%=a;");
	std::vector<std::string> ids  = {"a","b"};
	std::vector<std::string> values  = {"2","1"};
	check(input, ids, values);
}

//BOOST_AUTO_TEST_CASE(testA7	) 	// string ... nvm ..
//{
//	std::string input("char *s; s = \"Hello world\";");
//	std::vector<std::string> ids  = {"s"};
//	std::vector<std::string> values  = {"Hello world"};
//	check(input, ids, values);
//}
