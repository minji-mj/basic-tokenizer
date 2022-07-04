//============================================================================
// Name        : tokenClasses.hpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSES_HPP_
#define TOKENCLASSES_HPP_

#include <string>
#include <iostream>
#include <typeinfo>

namespace ensc251 {

	char *demangle(const char *typeName);

	class Token {	// The Base Token Class

	private:
		int lineNumber = 0;
		int indexNumber = 0;

	public:
		int getLineNumber() const { return lineNumber; }
		int getIndexNumber() const { return indexNumber; }

		Token() { std::cout << "Warning:  Do not invoke this constructor1" << std::endl; } // make sure this does not get invoked.

		Token(int lineNum, int indexNum) : lineNumber(lineNum), indexNumber(indexNum) {}

		// we will fix this up when we learn about virtual functions.
		friend std::ostream& operator <<(std::ostream& outs, const Token& token) {
			token.print(outs);
			return outs;
		}

		void print(std::ostream& str) const { str << demangle(typeid(*this).name()) << " (line: " << lineNumber << ", index: " << indexNumber << ")" << ": "; }

		virtual ~Token() {} // You will learn about virtual soon, but for now, don't delete this line.
	};

	class StringBasedToken :public Token {

	private:
		std::string stringValue;
	public:

		// StringBasedToken() { std::cout << "Warning:  Do not invoke this constructor" << std::endl; }; // make sure this does not get invoked.
		StringBasedToken(const std::string &aStringValue, int lineNum, int indexNum) : stringValue(aStringValue) {}
		const std::string& getStringValue() const
		{
			return stringValue;
		} // return the stringValue

		void print(std::ostream& str) const { Token::print(str); str << ": " << stringValue; }
	};

	class punctuator :public StringBasedToken {
		// e.g., ';', '*', '(', ')', ':'
	public:
		punctuator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class type_spec :public StringBasedToken {
		// e.g., 'char' | 'int' | 'float'
	public:
		type_spec(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	// do not instantiate this class.
	class id :public StringBasedToken {
		// identifiers -- example:  sum
	public:
		id(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class pointer_id :public id {
		// e.g., char* identifier
	public:
		pointer_id(const std::string stringValue, int lineNum, int indexNum) : id(stringValue, lineNum, indexNum) {};
	};

	class numeric_id :public id {
		// char, int, and float identifiers
	public:
		numeric_id(const std::string stringValue, int lineNum, int indexNum) : id(stringValue, lineNum, indexNum) {};
	};

	class int_id :public numeric_id {
		// char and int identifiers
	public:
		int_id(const std::string stringValue, int lineNum, int indexNum) : numeric_id(stringValue, lineNum, indexNum) {};
	};

	class gen_assignment_operator :public StringBasedToken
	{
		// '=' | '/=' | '+=' | '-='
	public:
		gen_assignment_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class int_assignment_operator :public StringBasedToken
	{
		// '%=' | '<<=' | '>>='
	public:
		int_assignment_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class conditional_operator :public StringBasedToken
	{
		// ?
	public:
		conditional_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class shift_operator :public StringBasedToken
	{
		// '<<' | '>>'
	public:
		shift_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class additive_operator :public StringBasedToken
	{
		// '+' | '-'
	public:
		additive_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class div_operator :public StringBasedToken {
		// '/'
	public:
		div_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class mod_operator :public StringBasedToken {
		// '%'
	public:
		mod_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class comp_operator :public StringBasedToken
	{
		//  '~'
	public:
		comp_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class postfix_operator :public StringBasedToken
	{
		// '++' | '--'
	public:
		postfix_operator(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class string :public StringBasedToken
	{
		// e.g., "sample string"
	public:
		string(const std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
	};

	class incorrect :public StringBasedToken
	{
		// e.g., "sample string     --> This is incorrect ...
		//		(quotation mark " is missing at the end)
	public:
		incorrect(std::string stringValue, int lineNum, int indexNum) : StringBasedToken(stringValue, lineNum, indexNum) {};
		/* Store the unrecognized characters. */
	};

	template<typename T>
	class numeric_const;

	template<typename T>
	class numeric_int_const;

	class constant :public Token
	{
	public:
		constant() { std::cout << "Warning:  Do not invoke this constructor2" << std::endl; }; // make sure this does not get invoked.
		constant(int lineNum, int indexNum) : Token(lineNum, indexNum) {};
	};

	template<typename T>
	class numeric_const :public constant
	{
		// e.g. 48.5
	private:
		T value;
	public:
		numeric_const(const T& constValue, int lineNum, int indexNum) : constant(lineNum, indexNum) { value = constValue; };
		//Fill in for additional constructors if desired

		const T& getValue() const { return value; }
		void print(std::ostream& str) const { Token::print(str); str << ": " << value; }
	};

	template<typename T>
	class numeric_int_const :public numeric_const<T>
	{
		// e.g. 48 or '0' -- covers just int and char
	public:
		numeric_int_const(const T& constValue, int lineNum, int indexNum) : numeric_const<T>(constValue, lineNum, indexNum) {}
		//Fill in for additional constructors if desired
	};

} // namespace ensc251
#endif /* TOKENCLASSES_HPP_ */
