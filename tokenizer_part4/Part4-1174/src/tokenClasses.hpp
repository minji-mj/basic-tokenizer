//============================================================================
// Name        : tokenClasses.hpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSES_HPP_
#define TOKENCLASSES_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include "TreeNode.hpp"

namespace ensc251 {

class Token;
char *demangle(const char *typeName);
class constant;
extern std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree
extern unsigned int tracker; // tracker index

class Token : public TreeNode {	// The Base Token Class
private:
	int lineNumber;
	int indexNumber;
public:
	int getLineNumber() { return lineNumber; }
	int getIndexNumber() { return indexNumber; }

	Token();
	Token(int lineNum, int indexNum) : lineNumber(lineNum), indexNumber(indexNum) {}

	virtual bool has_string_value(const std::string& aString) { return false; };
	virtual std::string getStringValue() = 0;
	void print_ast() { print_traverse(); OUT << ";" << std::endl;}

	friend std::ostream& operator <<(std::ostream& outs, const Token& token){
		token.print(outs);
		return outs;
	}

	void print(std::ostream& str) const { str << demangle(typeid(*this).name()) << ": "; }

	virtual Token* process_punctuator(const std::string &puncValue) { return nullptr; }
	virtual Token* process_declaration() { return nullptr; }

	virtual Token* process_id() { return nullptr; }
	virtual Token* process_primary_exp() { return nullptr; }

	virtual Token* process_pointer_primary_exp() { return nullptr; }

	virtual Token* process_numeric_id() { return nullptr; }
	virtual Token* process_numeric_primary_exp() { return nullptr; }

	virtual Token* process_int_id() { return nullptr; }
	virtual Token* process_int_primary_exp() { return nullptr; }

	virtual Token* process_int_comp() { return nullptr; }
	virtual Token* process_postfix_operator() { return nullptr; }
	virtual Token* advance_past_conditional_operator() { return nullptr; }
	virtual Token* advance_past_shift_operator() { return nullptr; }

	// ***** you may need to add code here *****
	virtual Token* advance_past_additive_operator() { return nullptr;} 	// added

	virtual Token* advance_past_div_operator() { return nullptr; }

	virtual Token* advance_past_mod_operator() { return nullptr; }

	virtual Token* advance_past_gen_assignment_operator() { return nullptr; }
	virtual Token* advance_past_int_assignment_operator() { return nullptr; }

	Token* process_token()
	{
		childPVector.clear();

		tracker++;
		if (tracker < tokenObjectPs.size()){
			return this;
		}
		else
			// our grammar must end in ';'. If *this is the end
			//  then it is a premature end which we will report now.
			return nullptr;
	}

	virtual void print_action() = 0;
	virtual void print_traverse()
	{
		if (childPVector.size() > 2)
			traverse_children_post_order(
					static_cast<ensc251::traverse_func>(&Token::print_traverse),
					static_cast<ensc251::action_func>(&Token::print_action));
		else if(childPVector.size() == 2)
			traverse_children_in_order(
					static_cast<ensc251::traverse_func>(&Token::print_traverse),
					static_cast<ensc251::action_func>(&Token::print_action));
		else
			traverse_children_pre_order(
					static_cast<ensc251::traverse_func>(&Token::print_traverse),
					static_cast<ensc251::action_func>(&Token::print_action));
	}

	virtual ~Token() {}
};

class TokenException : public std::exception {
	std::string ErrMsg;
public:
	TokenException(Token* tkn = nullptr, std::string msg=""){
		if(tkn)
			ErrMsg = "\nSyntax Error around {" + tkn->getStringValue() +
				"}, line " + std::to_string(tkn->getLineNumber()) +
				", index " + std::to_string(tkn->getIndexNumber()) +
				"\n" + msg + "\n";
		else
			ErrMsg = "Syntax Error!\n" + msg + "\n";
	}
	const char * what () const throw () {
		return ErrMsg.c_str();
	}
};

template<typename T>
class numeric_const;

template<typename T>
class numeric_int_const;

class constant : public Token {
public:
	constant(int lineNum, int indexNum): Token(lineNum, indexNum) {};

	virtual std::string getStringValue() = 0;
	Token* process_primary_exp() { return process_token(); }
};

// the below functions are defined in tokenClasses.cpp
Token* recursive_parser();
Token* assignment_exp();
Token* int_assignment_exp();
Token* int_comp_exp();

class StringBasedToken:public Token
{
protected:
	std::string stringValue;
public:
	StringBasedToken(const std::string &aStringValue, int lineNum, int indexNum): Token(lineNum, indexNum), stringValue (aStringValue) {};
	void print(std::ostream& str) const { Token::print(str); str << ": " << stringValue; }
	bool has_string_value(const std::string& aString) { return stringValue == aString; }
	std::string getStringValue() { return stringValue; }
	void print_action() { OUT << stringValue << " "; };
};

class punctuator:public StringBasedToken {
	// e.g., ';', '*', '(', ')', ':', etc.
public:
	punctuator(const std::string &puncValue, int lineNum, int indexNum): StringBasedToken(puncValue, lineNum, indexNum){ };

	void print_traverse()
	{
		if (stringValue == "(") {
			traverse_children_pre_order(
					static_cast<ensc251::traverse_func>(&punctuator::print_traverse),
					static_cast<ensc251::action_func>(&punctuator::print_action));

			OUT << ") ";
		}
		else
			traverse_children_in_order(
					static_cast<ensc251::traverse_func>(&punctuator::print_traverse),
					static_cast<ensc251::action_func>(&punctuator::print_action));
	}

	void print_action()
	{
		StringBasedToken::print_action();
		if (stringValue==";")
			OUT << std::endl;
	};

	Token* process_primary_exp()
	{
		if(stringValue == "(" && process_token())
		{
			Token* tObjP = assignment_exp();
			if(tObjP)
			{
				if(tokenObjectPs[tracker]->process_punctuator(")"))
					{
					add_childP(tObjP);
					return this;
					}
			}

		}
		return nullptr;
	}

	Token* process_int_primary_exp()
	{
		if(stringValue == "(" && process_token())
		{
			Token* tObjP = int_assignment_exp();
			if(tObjP)
			{
				if(tokenObjectPs[tracker]->process_punctuator(")"))
					{
					add_childP(tObjP);
					return this;
					}
			}

		}
		return nullptr;
	}
/*
	// added for type_spec-pointer
	Token* process_pointer_primary_exp()
	{
		if(stringValue == "*" && process_token())
		{
			Token* tObjP = int_assignment_exp();
			if(tObjP)
			{
				if(tokenObjectPs[tracker]->process_punctuator("*"))
					{
					add_childP(tObjP);
					return this;
					}
			}
		}
		return nullptr;
	}
*/
	Token* process_punctuator(const std::string &puncValue)
	{
		if (puncValue == stringValue)
			return process_token();
		return nullptr;
	}

};

class type_spec:public StringBasedToken 	// added
{ // 'char' | 'int' | 'float'
public:
	type_spec(): StringBasedToken("int", 0, 0) { };
	type_spec(const std::string &typeValue, int lineNum, int indexNum): StringBasedToken(typeValue, lineNum, indexNum){ };

	// ***** Complete this class

	Token* process_declaration()
	{
		/* Checks if type_spec is followed by id.
		 * If type spec is followed by '*',
		 * then type_spec object is added as a child to punctuator object representing '*'
		 * and pointer is returned to '*'.
		 * Otherwise, pointer is returned to type_spec object */

		auto typeS = process_token(); 	// type_spec object

		// 1. production: type_spec '*' id
			// e.g. char *a;

		if(auto ptr = tokenObjectPs[tracker]->process_punctuator("*"))	// check for punctuator object representing '*'
		{
			auto ID = tokenObjectPs[tracker]->process_id();				// check for id
			if(ID)
			{
				ptr->add_childP(typeS);			// add type_spec as child of '*'
				ptr->add_childP(ID);			// add id as child of '*'
				return ptr;						// return '*' as root with children type_spec and id
			}
			return nullptr;						// return null pointer if production rule is not satisfied
		}

		// 2. production: type_spec id
			// e.g. int a;

		if(auto ID = tokenObjectPs[tracker]->process_id())	// check for id
		{
			typeS->add_childP(ID);		// add id as child of type_spec
			return typeS;				// return type_spec as root with id as child
		}
		return nullptr;					// return null pointer if production rule is not satisfied
	}
};

class id:public StringBasedToken
{	// identifiers -- example:  sum
public:
	id(const std::string &idValue, int lineNum, int indexNum): StringBasedToken(idValue, lineNum, indexNum) {  };
	Token* process_id()	{ return process_token(); }
	Token* process_primary_exp() { return process_id(); }
};

class pointer_id:public id {
	// char* identifier
public:
	pointer_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
};

class numeric_id:public id
{	// char, int, and float identifiers
public:
	numeric_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
	Token* process_numeric_id()	{ return process_token(); }
	Token* process_numeric_primary_exp() { return process_numeric_id(); }
};

class int_id:public numeric_id
{	// char and int identifiers
public:
	int_id(const std::string &idValue, int lineNum, int indexNum): numeric_id(idValue, lineNum, indexNum) {  };
	Token* process_int_id()	{ return process_id(); } // a different pattern is being followed here.
	Token* process_int_primary_exp() { return process_int_id(); }
};

class gen_assignment_operator:public StringBasedToken
{
	// '=' | '/=' | '+=' | '-='
public:
	gen_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_gen_assignment_operator() { return process_token();	}
};

class int_assignment_operator:public StringBasedToken
{
	// '%=' | '<<=' | '>>='
public:
	int_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_int_assignment_operator() { return process_token();	}
};

class conditional_operator:public StringBasedToken
{	// '?'
public:
	conditional_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
	Token* advance_past_conditional_operator() {
		return process_token();
	}
	void print_traverse()
	{
		static_cast<ensc251::Token*>(this->childPVector[0])->print_traverse();
		this->print_action();
		static_cast<ensc251::Token*>(this->childPVector[1])->print_traverse();
		OUT << ": " ;
		static_cast<ensc251::Token*>(this->childPVector[2])->print_traverse();
	}
};

class shift_operator:public StringBasedToken
{
	// '<<' | '>>'
public:
	shift_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
	Token* advance_past_shift_operator()
	{
		return process_token();
	}
};

class additive_operator:public StringBasedToken
{
	// '+' | '-'
public:
	additive_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };

	// ***** Complete this class if needed

	Token* advance_past_additive_operator() { return process_token(); } 	// added
};

class div_operator:public StringBasedToken
{	// '/'
public:
	div_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_div_operator() { return process_token(); }
};

class mod_operator:public StringBasedToken
{	// '%'
public:
	mod_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_mod_operator() { return process_token(); }
};

class comp_operator:public StringBasedToken
{
	//  '~'
public:
	comp_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };

	Token* process_int_comp()
	{
		Token* subTreeP;
		if(process_token() && (subTreeP = int_comp_exp()))
		{
			add_childP(subTreeP);
			return this;
		}
		return nullptr;
	}
};

class postfix_operator:public StringBasedToken
{	// '++' | '--'
public:
	postfix_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };

	// ***** Complete this class if needed

	void print_traverse() // added for syntax (runs as pre-order by default)
	{
		traverse_children_post_order(
				static_cast<ensc251::traverse_func>(&Token::print_traverse),
				static_cast<ensc251::action_func>(&Token::print_action));
	}
	Token* process_postfix_operator() { return process_token(); }
};

class string:public StringBasedToken
{
	// e.g., "sample string"
public:
	string(const std::string &aStringValue, int lineNum, int indexNum) : StringBasedToken(aStringValue, lineNum, indexNum) {};

	Token* process_primary_exp()
	{
		return process_token();
	}
};

class incorrect:public StringBasedToken
{	// e.g., &
public:
	incorrect(const std::string &aStringValue, int lineNum, int indexNum) : StringBasedToken(aStringValue, lineNum, indexNum) {
	};
};

template<typename T>
class numeric_const:public constant
{	// e.g., 123 | 'a' | 12.5
private:
	T value;
public:
	numeric_const(const T& constValue, int lineNum, int indexNum): constant(lineNum, indexNum), value(constValue)  {}

	std::string getStringValue() { return std::to_string(value); }
	T& getValue() { return value; }
	void print(std::ostream& str) const {Token::print(str); str << ": " << value; }
	void print_action() {
		if (std::is_same<T, char>::value)
			OUT << "'" << value << "'" << " ";
		else
			OUT << value << " ";
	};

	Token* process_numeric_primary_exp() { return this->process_primary_exp(); }
};

template<typename T>
class numeric_int_const:public numeric_const<T>
{	// e.g., 123 | 'a'
public:
	numeric_int_const(const T& constValue, int lineNum, int indexNum): numeric_const<T>(constValue, lineNum, indexNum) {}

	Token* process_int_primary_exp() { return this->process_primary_exp(); }
};

} // namespace ensc251
#endif /* TOKENCLASSES_HPP_ */