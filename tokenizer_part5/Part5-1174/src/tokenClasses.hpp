//============================================================================
// Name        : tokenClasses.hpp
// Description : Course Project Part 5 (Interpreting/Evaluation)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSES_HPP_
#define TOKENCLASSES_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include "TreeNode.hpp"

namespace ensc251 {

char *demangle(const char *typeName);
void print_evaluation_map();

// forward declarations
class constant;
class Token;

typedef std::shared_ptr<constant> shared_ptr_to_constant;

// global variable external references (evalMap below)

extern std::vector<Token*> tokenObjectPs; 	// objects for abstract syntax tree
extern unsigned int tracker; 				// tracker index


class Token : public TreeNode {				// The Token class, parent to all types of tokens

private:
	int lineNumber=0;
	int indexNumber=0;

public:
	int getLineNumber() { return lineNumber; }
	int getIndexNumber() { return indexNumber; }

	Token();
	// default -1 ...
	Token(int lineNum, int indexNum) : lineNumber(lineNum), indexNumber(indexNum) {}

	virtual std::string getStringValue() const = 0;
	virtual bool has_string_value(const std::string& aString) { return getStringValue() == aString; };

	void print_ast() { print_traverse(); OUT << ";" << std::endl;}

	friend std::ostream& operator <<(std::ostream& outs, const Token& token){
		token.print(outs);
		return outs;
	}

	void print(std::ostream& str) const
	{
		str << demangle(typeid(*this).name()) <<
				" (line: " << lineNumber <<", index: " << indexNumber <<")" <<
				" : " << this->getStringValue();
	}

	virtual Token* process_punctuator(const std::string &puncValue) { return nullptr; }
	virtual Token* process_declaration() { return nullptr; }

	virtual Token* process_id() { return nullptr; }
	virtual Token* process_primary_exp() { return nullptr; }

//	Token* process_pointer_id() { return process_id(); }
//	virtual Token* process_pointer_primary_exp() { return nullptr; }

	Token* process_numeric_id()	{ return process_id(); }
	virtual Token* process_numeric_primary_exp() { return nullptr; }

	Token* process_int_id()	{ return process_id(); }
	virtual Token* process_int_primary_exp() { return nullptr; }

	virtual Token* process_int_comp() { return nullptr; }
	virtual Token* process_postfix_operator() { return nullptr; }

	virtual Token* advance_past_conditional_operator() { return nullptr; }
	virtual Token* advance_past_shift_operator() { return nullptr; }
	virtual Token* advance_past_additive_operator() { return nullptr; }

	virtual Token* advance_past_div_operator() { return nullptr; }
	virtual Token* advance_past_mod_operator() { return nullptr; }

	virtual Token* advance_past_gen_assignment_operator() { return nullptr; }
	virtual Token* advance_past_int_assignment_operator() { return nullptr; }

	virtual shared_ptr_to_constant eval() = 0;

	Token* getChildToken(unsigned childNum) const { 
		return static_cast<Token*>(this->childPVector[childNum]); 
	}

	shared_ptr_to_constant evalChildToken(unsigned childNum) {
		return getChildToken(childNum)->eval();
	}

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

	virtual void print_action() { OUT << getStringValue() << " "; }

	virtual void print_traverse()
	{
		if(childPVector.size() == 2)
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
	TokenException(Token* tkn=nullptr, std::string msg=""){
		if(tkn)
			ErrMsg = "\nSyntax Error around {" + tkn->getStringValue() +
					"}, line " + std::to_string(tkn->getLineNumber()) +
					", index " + std::to_string(tkn->getIndexNumber()) +
					"\n" + msg + "\n";
		else
			ErrMsg = "Syntax Error!";
	}
	const char * what () const throw () {
		return ErrMsg.c_str();
	}
};

#define MULTI_OP_IFACE(RNAME, OP, TYPE) \
		virtual shared_ptr_to_constant RNAME(TYPE v) = 0;

#define NUMERIC_OP_IFACE(NAME, OP) \
		MULTI_OP_IFACE(r##NAME, OP, float) \
		MULTI_OP_IFACE(r##NAME, OP, int) \
		MULTI_OP_IFACE(r##NAME, OP, char) \
		\
		virtual shared_ptr_to_constant NAME(constant& obj) = 0;\
		virtual shared_ptr_to_constant operator OP (constant& obj) = 0;

template<typename T>
class numeric_const;

template<typename T>
class numeric_int_const;

class constant : public Token {							// subclasses: numeric_const<T>, ...
public:

	/* ***** this class needs work including for multiple operations supported on constants such as postfix_decrement, addition, subtraction, rightShift. */

	constant(int lineNum, int indexNum): Token(lineNum, indexNum) {};

	virtual shared_ptr_to_constant putValueIntoInt() = 0;
	// added
	virtual shared_ptr_to_constant putValueIntoChar() = 0;
	virtual shared_ptr_to_constant putValueIntoFloat() = 0;

	virtual bool asBool() = 0;  // do we need both asBool and isNotZero?  Could use Macro.
	virtual bool isNotZero() = 0;

	NUMERIC_OP_IFACE(div, /)
	NUMERIC_OP_IFACE(add, +)
	NUMERIC_OP_IFACE(sub, -)

	/* throw errors unless overridden */
	virtual shared_ptr_to_constant complement() { throw std::domain_error("Requires numeric_int_const"); }
	virtual shared_ptr_to_constant mod(constant& rval) { throw std::domain_error("Requires numeric_int_const"); }
	virtual shared_ptr_to_constant leftShift(constant& rval) { throw std::domain_error("Requires numeric_int_const"); }
	virtual shared_ptr_to_constant rightShift(constant& rval) { throw std::domain_error("Requires numeric_int_const"); } 	// added

	virtual int getIntValue() { throw std::domain_error("Requires numeric_int_const"); }

	virtual shared_ptr_to_constant postfix_increment() = 0;
	virtual shared_ptr_to_constant postfix_decrement() = 0;

	Token* process_primary_exp() { return process_token(); }
};

// the below functions are defined in tokenClasses.cpp
Token* recursive_parser();
Token* assignment_exp();
Token* int_assignment_exp();
Token* int_comp_exp();

class StringBasedToken:public Token {
protected:
	std::string stringValue;
public:
	StringBasedToken(const std::string &aStringValue, int lineNum, int indexNum): Token(lineNum, indexNum), stringValue (aStringValue) {};
	std::string getStringValue() const { return stringValue; }
};

class punctuator:public StringBasedToken {
	// e.g., ';', '*', '(', ')', ':', etc.
public:
	punctuator(const std::string &puncValue, int lineNum, int indexNum): StringBasedToken(puncValue, lineNum, indexNum){ };

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
					delete tokenObjectPs[tracker - 1];
					tokenObjectPs[tracker - 1] = nullptr;
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

	Token* process_punctuator(const std::string &puncValue)
	{
		if (puncValue == stringValue)
			return process_token();
		return nullptr;
	}

	shared_ptr_to_constant eval()
	{
		if(stringValue == "(")
			return this->evalChildToken(0);
		// ";" node evaluation handled below
		// we could consider catching exceptions here
		this->evalChildToken(0);
		this->evalChildToken(1);
		return shared_ptr_to_constant(); // ";" node has no result
	}
};

class type_spec:public StringBasedToken { // 'char' | 'int' | 'float'
public:
	type_spec(): StringBasedToken("int", 0, 0) { };
	type_spec(const std::string &typeValue, int lineNum, int indexNum): StringBasedToken(typeValue, lineNum, indexNum){ };
	Token* process_declaration()
	{
		Token* subTreeP = process_token();
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->process_punctuator("*"))) {
			tObjP->add_childP(subTreeP);
			subTreeP = tObjP;
		}
		if ((tObjP = tokenObjectPs[tracker]->process_id())) {
			subTreeP->add_childP(tObjP);
			return subTreeP;
		}
		return nullptr;
	}

	shared_ptr_to_constant ofTypeCreateConstant(shared_ptr_to_constant constToCreateFromSP)
	{
		switch (this->stringValue[0]) {
		/* ***** this needs work for other supported data types i.e. char and float */
		case 'i': // int
			return constToCreateFromSP->putValueIntoInt();

		// added
		case 'c': // char
			return constToCreateFromSP->putValueIntoChar();
		case 'f': // float
			return constToCreateFromSP->putValueIntoFloat();
		}
		throw std::logic_error ("Should never get to this line of code");
	}

	shared_ptr_to_constant  eval();  // please see definition of this function in tokenClasses.cpp
};

struct varInfo {
	shared_ptr_to_constant sptc;
	type_spec ts;
};

typedef std::map<std::string, varInfo > evalMapType;
extern
evalMapType evalMap;  // map holds the evaluation results for AST.

class id:public StringBasedToken {	// identifiers -- example:  sum
public:
	id(const std::string &idValue, int lineNum, int indexNum): StringBasedToken(idValue, lineNum, indexNum) {  };
	Token* process_id()	{ return process_token(); }
	Token* process_primary_exp() { return process_id(); }
	shared_ptr_to_constant eval() { return evalMap[getStringValue()].sptc; }
};

class pointer_id:public id {	// id of type char*
public:
	pointer_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
};

class numeric_id:public id {	// id of type float (and see below)
public:
	numeric_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
	Token* process_numeric_primary_exp() { return process_numeric_id(); }
};

class int_id:public numeric_id {	// id of types char and int
public:
	int_id(const std::string &idValue, int lineNum, int indexNum): numeric_id(idValue, lineNum, indexNum) {  };
	Token* process_int_primary_exp() { return process_int_id(); }
};

class gen_assignment_operator:public StringBasedToken
{
	// '/=' | '+=' | '-=' | '='
public:
	gen_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_gen_assignment_operator() { return process_token();	}

	/* ***** BONUS MARKS: Evaluation of string assignments would be considered for the bonus points.
	 * for example 's = "Hello world";'  */

	/*
	 * ***** So far we have only considered '=' in evaluation.
	 * Implement  '/=' | '+=' | '-='
	 *
	*/

	shared_ptr_to_constant eval()
	{
		// 		 '/=', '+=', '-=', '='

		auto right = evalChildToken(1);
		auto key = getChildToken(0)->getStringValue();
		auto mapEntry = evalMap[key];


		// added
		// checks for each gen_assignment_operator, then evaluates (performs assignment) accordingly using available functions
				// e.g. a/=b; is the same as a=a/b; therefore, a would be assigned to the operation done between a and b

		if (has_string_value("/="))
			return evalMap[key].sptc = mapEntry.sptc->div(*mapEntry.ts.ofTypeCreateConstant(right));	// gen_assignment_operator "/="

		else if (has_string_value("+="))
			return evalMap[key].sptc = mapEntry.sptc->add(*mapEntry.ts.ofTypeCreateConstant(right));	// gen_assignment_operator "+="

		else if (has_string_value("-="))
			return evalMap[key].sptc = mapEntry.sptc->sub(*mapEntry.ts.ofTypeCreateConstant(right));	// gen_assignment_operator "-="

		else
			return evalMap[key].sptc = evalMap[key].ts.ofTypeCreateConstant(right);						// gen_assignment_operator "="
	};

};

class int_assignment_operator:public StringBasedToken
{
	// '<<=' | '>>=' | '%='
public:
	int_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_int_assignment_operator() { return process_token();	}

	/*
	 * ***** So far we have only considered '<<=' in evaluation.
	 * Implement  '%=' | '>>='
	 *
	*/

	shared_ptr_to_constant eval()
	{
		// '<<=', '>>=', '%='

		auto right = evalChildToken(1);
		auto key = getChildToken(0)->getStringValue();
		auto mapEntry = evalMap[key];

		// added
		// checks for each int_assignment_operator, then evaluates (performs assignment) accordingly using available functions
		if (has_string_value("<<="))
			return evalMap[key].sptc = mapEntry.sptc->leftShift(*mapEntry.ts.ofTypeCreateConstant(right));	 	// int_assignment_operator "<<="

		else if (has_string_value(">>="))
			return evalMap[key].sptc = mapEntry.sptc->rightShift(*mapEntry.ts.ofTypeCreateConstant(right)); 	// int_assignment_operator ">>="

		else
			return evalMap[key].sptc = mapEntry.sptc->mod(*mapEntry.ts.ofTypeCreateConstant(right));			// int_assignment_operator "%="
	};
};

class conditional_operator:public StringBasedToken
{
	// '?'
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

	shared_ptr_to_constant eval()
	{
		/* ***** this needs work */

		// added
		// evalChildToken(0) is automatically a bool value, where '1' if true and '0' if false
		// if true, evalchildToken(1) is returned (in this code, true if isNotZero())
		// if false, evalchildToken(2) is returned
				// e.g. from test case 4a:
					// res=errno?3.5:14.3; will assign res to 3.5 if errno is '1' and 14.3 if errno is '0'

		if (evalChildToken(0)->isNotZero())
			return evalChildToken(1);
		else
			return evalChildToken(2);
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

	shared_ptr_to_constant eval()
	{
		/* ***** this needs work */

		// added
		// checks for either '<<' or '>>' and evaluates accordingly

		auto right_eval = evalChildToken(1);
		if (has_string_value("<<"))										// checks for left shift '<<'
			return this->evalChildToken(0)->leftShift(*right_eval);
		else  															// checks for right shift '>>'
			return this->evalChildToken(0)->rightShift(*right_eval);
	}
};

class additive_operator:public StringBasedToken
{
	// '+' | '-'
public:
	additive_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
	Token* advance_past_additive_operator() {
		return process_token();
	}

	shared_ptr_to_constant eval()
	{
		/* ***** this needs work */

		// added
		// checks for either '+' or '-' and evaluates accordingly

		auto right_eval = evalChildToken(1);

		if (has_string_value("+"))								// checks for "+"
			return evalChildToken(0)->add(*right_eval);					// if there exists "+", then performs addition
		else													// checks for "-"
			return evalChildToken(0)->sub(*right_eval);					// if there exists "-", then performs subtraction
	}
};

class div_operator:public StringBasedToken {	// '/'
public:
	div_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_div_operator() { return process_token(); }

	shared_ptr_to_constant eval()
	{
		auto right_eval = evalChildToken(1);
		return evalChildToken(0)->div(*right_eval);
	}
};

class mod_operator:public StringBasedToken {
	// 	'%'
public:
	mod_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
	Token* advance_past_mod_operator() { return process_token(); }

	shared_ptr_to_constant eval()
	{
		auto right_eval = evalChildToken(1);

		if (!(right_eval->isNotZero()))
			throw(std::overflow_error("Mod by zero"));
		else
		{
			return evalChildToken(0)->mod(*right_eval);
		}
	}
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

	shared_ptr_to_constant eval()
	{
		return this->evalChildToken(0)->complement();
	}
};

class postfix_operator:public StringBasedToken
{
	// '++' | '--'
public:
	postfix_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
	void print_traverse()
	{
		traverse_children_post_order(
				static_cast<ensc251::traverse_func>(&postfix_operator::print_traverse),
				static_cast<ensc251::action_func>(&postfix_operator::print_action));
	}

	Token* process_postfix_operator() { return process_token(); }

	shared_ptr_to_constant eval()
	{
		/* ***** this needs work */

		// added
		// checks for either '++' or '--' and evaluates accordingly

		if (has_string_value("++"))									// checks for "++"
			return this->evalChildToken(0)->postfix_increment();
		else														// checks for "--"
			return this->evalChildToken(0)->postfix_decrement();

		return evalChildToken(0);
	}
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

	shared_ptr_to_constant eval() { throw std::runtime_error("string evaluation unimplemented"); }

	/* ***** BONUS MARKS: Evaluation of string assignments would be considered for the bonus points.
	 * for example 'char *s; s = "Hello world";'  */
	// :(
};

class incorrect:public StringBasedToken
{
public:
	incorrect(const std::string &aStringValue, int lineNum, int indexNum) : StringBasedToken(aStringValue, lineNum, indexNum) {};
	shared_ptr_to_constant eval() { throw std::runtime_error("Cannot evaluate 'incorrect' token!"); }
};

shared_ptr_to_constant create(float v); // definition in tokenClasses.cpp

// template function.  See beginning of chapter 17.
template<typename T> // needed for int and char
shared_ptr_to_constant create(T v) {
		return shared_ptr_to_constant(new numeric_int_const<T>(v));
}

#define MULTI_OP(RNAME, OP, TYPE) \
		shared_ptr_to_constant RNAME(TYPE v) { return create(v OP value); }

#define NUMERIC_OP(NAME, OP) \
		MULTI_OP(r##NAME, OP, float) \
		MULTI_OP(r##NAME, OP, int) \
		MULTI_OP(r##NAME, OP, char) \
		\
		shared_ptr_to_constant NAME(constant& obj) {return obj.r##NAME(value); } \
		shared_ptr_to_constant operator OP (constant& obj) {return obj.r##NAME(value); }

template<typename T>
class numeric_const:public constant
{
private:
	T value;
public:
	/* ***** this class needs work including for multiple operations supported on constants such as postfix operations, addition, subtraction. */

	numeric_const(const T& constValue, int lineNum=-1, int indexNum=-1): constant(lineNum, indexNum), value(constValue)  {}

	shared_ptr_to_constant putValueIntoInt() { return shared_ptr_to_constant(new numeric_int_const<int>(value)); }
	// added
	shared_ptr_to_constant putValueIntoChar() { return shared_ptr_to_constant(new numeric_int_const<char>(value)); } 	// added
	shared_ptr_to_constant putValueIntoFloat() { return shared_ptr_to_constant(new numeric_const<float>(value)); } 		// added


	std::string getStringValue() const { return std::to_string(value); }
	T& getValue() { return value; }
	bool asBool() { return value; }
	bool isNotZero() { return value; }

	NUMERIC_OP(div, /)
	NUMERIC_OP(add, +) 		// added - for addition operation
	NUMERIC_OP(sub, -)		// added - for subtraction operation

	shared_ptr_to_constant postfix_increment()		// increments value
	{ // a ++;
		T temp = value;
		value++;
		return create(temp);
	}

	// added
	shared_ptr_to_constant postfix_decrement()		// decrements value
	{ // a --;
		T temp = value;
		value--;
		return create(temp);
	}

	shared_ptr_to_constant eval() {	return create(value); }

	Token* process_numeric_primary_exp() { return this->process_primary_exp(); }
};

template<typename T>
class numeric_int_const:public numeric_const<T>
{
public:
	/* ***** this class needs work including for multiple operations supported on int constants such as shifts and complement. */
	numeric_int_const(const T& constValue, int lineNum=-1, int indexNum=-1): numeric_const<T>(constValue, lineNum, indexNum) {}

	int getIntValue() { return this->getValue(); }

	shared_ptr_to_constant rdiv(int v)
	{
		if (this->getValue())
			return create(v / this->getValue());
		else
			throw(std::overflow_error("Divide by zero"));
	}

	// is this needed?  Probably so.
	shared_ptr_to_constant rdiv(char v)
	{
		return rdiv(static_cast<int>(v));
	}

	shared_ptr_to_constant complement()
	{
		/* ***** this  needs work */
		// added
		return create(~this->getIntValue());	// returns complement "~" (bitwise complement) of value
//		return create(0);
	}

	shared_ptr_to_constant mod(constant& right)
	{
		return create(this->getValue() % right.getIntValue());
	}

	shared_ptr_to_constant leftShift(constant& right)
	{
		/* ***** this  needs work */
		// added
		return create(this->getIntValue() << right.getIntValue()); 	// returns leftShift "<<" (bitwise shift to the left) of value
	}

	// added
	shared_ptr_to_constant rightShift(constant& right)
	{
		return create((this->getIntValue()) >> right.getIntValue());	// returns rightShift ">>" (bitwise shift to the right) of value
	}

	Token* process_int_primary_exp() { return this->process_primary_exp(); }
};

} // namespace ensc251
#endif /* TOKENCLASSES_HPP_ */