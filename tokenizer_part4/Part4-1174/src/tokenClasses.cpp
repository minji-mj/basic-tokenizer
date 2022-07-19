//============================================================================
// Name        : tokenClasses.cpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include "tokenClasses.hpp"
//#include <memory>

// I'm not sure if next line works with other than GNU compiler
#include <cxxabi.h>

using namespace std;

namespace ensc251 {

Token* int_assignment_exp();
Token* shift_exp();
Token* int_div_exp();

unsigned int tracker = 0; // tracker index
std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree


char *demangle(const char *typeName) {
	int status;
	// I'm not sure if the below line works with compilers other than GNU
	return abi::__cxa_demangle(typeName, 0, 0, &status);
}


Token* postfix_exp() 	// id postfix_operator | primary_exp
{
	/* First checks for an id followed by a postfix_operator.
	 * If there exists a postfix operator then it becomes the root of the subtree with id as a child.
	 * If not, checks for the primary expression and retrieves its subtree. */

	// ***** Complete this function

	// 1. Checks for an id, followed by a postfix_operator

	int old_tracker = tracker;									// keep location of current tracker for later use (checking for primary expression)
	Token* subTreeP = tokenObjectPs[tracker]->process_id(); 	// check for id

	if(subTreeP)
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->process_postfix_operator())) 	// check for postfix operator
		{
			tObjP->add_childP(subTreeP); 	// add id as a child of postfix operator
			return tObjP; 					// return postfix operator as root with id as child
		}
	}

	// ***** Complete this function

	// 2. Checks for primary expression, since first production rule was not satisfied (i.e. there wasn't a postfix operator following id)

	tracker = old_tracker; 										// return tracker location to what we had in the beginning of the function
	subTreeP = tokenObjectPs[tracker]-> process_primary_exp(); 	// check for primary expression

	if(subTreeP)
		 return subTreeP; 	// retrieve subtree of the primary expression
							// (more specifically, returns pointer to the primary expression)
	else
		 return nullptr;	// return null pointer if production rule is not satisfied
}

Token* int_postfix_exp() 	// similar to postfix_exp()
							// int_id postfix_operator | int_primary_exp
{
	/* First checks for an int_id followed by a postfix_operator.
	   If there exists a postfix operator then it becomes the root of the subtree with int_id as a child.
	   If not, checks for the int_primary expression and retrieves its subtree. */

	// ***** Complete this function

	// 1. Checks for an int_id, followed by a postfix_operator

	int old_tracker = tracker; 										// keep location of current tracker for later use (checking for int_primary expression)
	Token* subTreeP = tokenObjectPs[tracker]->process_int_id(); 	// check for int_id

	if(subTreeP)
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->process_postfix_operator())) 	// check for postfix_operator
		{
			tObjP->add_childP(subTreeP); 	// add int_id as a child of postfix operator
			return tObjP; 					// return postfix operator as root with int_id as child
		}
	}

	// ***** Complete this function

	// 2. Checks for int_primary expression, since there wasn't a postfix operator following int_id

	tracker = old_tracker;											// return tracker location to what we had in the beginning of the function
	subTreeP = tokenObjectPs[tracker]->process_int_primary_exp(); 	// check for int_primary expression

	if(subTreeP)
		 return subTreeP; 	// retrieve subtree of the int_primary expression
	else
		 return nullptr;	// return null pointer if production rule is not satisfied
}

Token* comp_exp() 	// given
{
	int old_tracker = tracker;

	Token* tObjP;

	if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
	{
		tracker = old_tracker;
		tObjP = postfix_exp();
	}
	return tObjP;
}

Token* int_comp_exp() 	// given
{
	int old_tracker = tracker;

	Token* tObjP;

	if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
	{
		tracker = old_tracker;
		tObjP = int_postfix_exp();
	}
	return tObjP;
}

Token* div_exp() 	// given
{
	int old_tracker = tracker;

	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()))
		{
			if(!(ueTreeP = comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	else
	{
		tracker = old_tracker;
		lowerNodeP = int_div_exp();
	}

	return lowerNodeP;
}

Token* int_div_exp() 	// given
{
	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = int_comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()) || (upperNodeP = tokenObjectPs[tracker]->advance_past_mod_operator()))
		{
			if(!(ueTreeP = int_comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

// ***** Add more functions around here somewhere *****
//     additive_exp() and int_additive_exp() added

Token* additive_exp() 	// div_exp {additive_operator div_exp}*
{
	/* Can also be represented as 'div_exp {additive_operator div_exp}*',
	 * where * means zero or more instances of {additive_operator div_exp} */

	Token* ueTreeP = nullptr; 		// initializations of objects pointing to class Token
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = div_exp())) {															// check for div_exp
																							// loop below checks for zero or more instances of {additive_operator div_exp}

		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator())) 		// check for additive_operator
		{
			if(!(ueTreeP = div_exp()))															// check for div_exp
				return nullptr;																	// returns null pointer if div_exp is not satisfied
			upperNodeP->add_childP(lowerNodeP);													// add lowerNodeP=div_exp() as child to upperNodeP=additive_operator()
			upperNodeP->add_childP(ueTreeP);													// add ueTreeP=div_exp() as child to upperNodeP=additive_operator()
			lowerNodeP = upperNodeP;															// assign lowerNodeP to uppwerNodeP for loop to continue
																								// i.e. lowerNodeP = upperNodeP if condition is satisfied
		}
	}
	return lowerNodeP;		// return lowerNodeP=nullptr as production rule was not satisfied
}

Token* int_additive_exp() 	// similar to additive_exp
							// int_div_exp {additive_operator int_div_exp}*
{
	/* Can also be represented as 'int_div_exp {additive_operator int_div_exp}*',
	 * where * means zero or more instances of {additive_operator int_div_exp} */

	Token* ueTreeP = nullptr;		// initializations of objects pointing to class Token
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = int_div_exp())) {													// check for int_div_exp
																					    // loop below checks for zero or more instances of {additive_operator int_div_exp}

		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))		// check for additive_operator
		{
			if(!(ueTreeP = int_div_exp()))													// check for int_div_exp
				return nullptr;																// returns null pointer if int_div_exp is not satisfied
			upperNodeP->add_childP(lowerNodeP);												// add lowerNodeP=int_div_exp() as child to upperNodeP=additive_operator()
			upperNodeP->add_childP(ueTreeP);												// add ueTreeP=int_div_exp() as child to upperNodeP=additive_operator()
			lowerNodeP = upperNodeP;														// assign lowerNodeP to upperNodeP for loop to continue
																							// i.e. lowerNodeP = upperNodeP if condition is satisfied
		}
	}
	return lowerNodeP;		// return lowerNodeP=nullptr as production rule was not satisfied
}

Token* shift_exp() 	// int_additive_exp {shift_operator int_additive_exp}*
{
	/* Can also be represented as 'int_additive_exp {shift_operator int_additive_exp}*',
	 * where * means zero or more instances of {shift_operator int_additive_exp} */

	Token* subTreeP = nullptr; 		// initializations of objects pointing to class Token
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	// ***** Complete this function

	if ((lowerNodeP = int_additive_exp())) {										// check for int_additive_exp
																					// loop below checks for zero or more instances of {shift_operator int_additive_exp}

		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_shift_operator()))	// check for shift_operator
		{
			if(!(subTreeP = int_additive_exp()))										// check for int_additive_exp
				return nullptr;															// returns null pointer if int_additive_exp is not satisfied
			upperNodeP->add_childP(lowerNodeP);											// add lowerNodeP=int_additive_exp() as child to upperNodeP=shift_operator
			upperNodeP->add_childP(subTreeP);											// add subTreeP=int_additive_exp() as child to upperNodeP=shift_operator
			lowerNodeP = upperNodeP;													// assign lowerNodeP to upperNodeP for loop to continue
																						// i.e. lowerNodeP = uppderNodeP if condition is satisfied
		}
	}
	return lowerNodeP;		// return lowerNodeP = nullptr as production rule was not satisfied
}

Token* ternary_exp() 	// id '?' assignment_exp ':' ternary_exp | shift_exp | additive_exp
{
	/* Subtree created for this production has '?' as the root of the subtree,
	 * with id, assignment_exp, and another ternary_exp as its children.
	 * If first production rule is not satisfied (id '?' assignment_exp ':' ternary_exp),
	 * then it would check for shift_exp,  and if shift_exp is not satisfied,
	 * then it would check for additive_exp. */

	int old_tracker = tracker;		// keep location of current tracker for later use
	Token* subTreeP;

	// ***** Complete this function

	// 1. Checks for '?' as root of subtree, with id, assignment_exp, and another ternary_exp as its children

	if((subTreeP = tokenObjectPs[tracker]->process_id()))	// check for id
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))	// check for conditional_operator "?"
		{
			tObjP->add_childP(subTreeP);	// add id as child of '?'
			subTreeP = assignment_exp();
			if(subTreeP)					// check for subTreeP=assignment_exp()
			{
				tObjP->add_childP(subTreeP);							// add assignment_exp as another child of '?'
				if(tokenObjectPs[tracker]->process_punctuator(":"))		// check for ":"
				{
					subTreeP = ternary_exp();
					if(subTreeP)										// check for ternary_exp
					{
						tObjP->add_childP(subTreeP);					// add the ternary_exp as yet another child of '?'
						return tObjP;									// return '?' as root with id, assignment_exp, and another ternary_exp as its children
					}
				}
			}
			return nullptr;		// return null pointer if any of the production rule is violated
		}
	}

	// ***** Complete this function

	// 2. Checks for shift expression, because the first production rule is not satisfied

	tracker = old_tracker;		// return tracker location to what we had in the beginning of the function
	subTreeP = shift_exp();

	if(!subTreeP)						// check for shift_exp
	{
		tracker = old_tracker;			// if there exists no shift_exp, return tracker to the original location

		// ***** Complete this function

		// 3. Checks for additive_exp under the condition that no shift_exp was found

		subTreeP = additive_exp();		// check for additive_exp
	}
	return subTreeP;					// returns: either shift_exp or additive_exp
}

Token* int_ternary_exp() 	// given
							// id '?' int_assignment_exp ':' int_ternary_exp | shift_exp
{
	/* Subtree created for this production has '?' as the root of the subtree,
	 * with id, assignment_exp, and another ternary_exp as its children.
	 * If first production rule is not satisfied (id '?' assignment_exp ':' ternary_exp),
	 * then it would check for shift_exp.   */

	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = int_assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = int_ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}
	tracker = old_tracker;
	subTreeP = shift_exp();
	if(!subTreeP)
		return nullptr;
	return subTreeP;
}

Token* assignment_exp() 	// given
							// int_id int_assignment_operator_int_assignment_exp | id gen_assignment_operator assignment_exp | ternary_exp
{
	int old_tracker = tracker;

	// int_id int_assignment_operator int_assignment_exp
	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production: id gen_assignment_operator assignment_exp
	tracker = old_tracker;
	if ((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				// we have assignment operator, but not valid assignment expression,
				return nullptr;
		}

		if(tokenObjectPs[tracker]->has_string_value(";"))
			// we have already built a unary expression subtree, which satisfies the ternary_exp production.
			return subTreeP;
	}

	// production:  ternary_exp
	tracker = old_tracker;
	subTreeP = ternary_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* int_assignment_exp() 	// given
								// int_id gen_assignment_operator assignment_exp | int_id int_assignment_operator int_assignment_exp | int_ternary_exp
{
	int old_tracker = tracker;

	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}

		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production:  int_ternary_exp
	tracker = old_tracker;
	subTreeP = int_ternary_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* stat() 	// given
				// declaration | assignment_exp
{
	if(auto subTreeP = tokenObjectPs[tracker]->process_declaration())
		return subTreeP;
	else
		return assignment_exp();
}

Token* stat_list() 	// given
					// stat {';' stat}*
{
	Token* statTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = stat())) {

		while (tokenObjectPs[tracker]->has_string_value(";")) {
			tracker++;
			if (tracker == tokenObjectPs.size())
				return lowerNodeP;
			upperNodeP = tokenObjectPs[tracker - 1];

			if(!(statTreeP = stat()))
				throw ensc251::TokenException(tokenObjectPs[tracker]);

			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(statTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	throw ensc251::TokenException(tokenObjectPs[tracker]);
}

Token* recursive_parser()
{
	if (tokenObjectPs.size())
		return stat_list();
	else
		throw ensc251::TokenException();
}
}
