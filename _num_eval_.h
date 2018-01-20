//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _num_eval_.h - header file for
//  recursive descent numeric expression evaluator.
//
// Evaluates numeric expressions with the following operators:
//    + - * / % ^ (exponent), parenthesized exprs, and
//    case-insensitive variables (including assigning to them).
// Multiple statements can be specified for evaluation if
// separated by ; (semicolon). The result of the last non-empty
// expression is returned. At least one non-empty expression
// has to be present. Variable values are preserved across
// the subexpressions (expressions separated by ';') within
// the main expression.
//
// Note that while all the arithmetic operators are left-
// associative, the assignment and exponentiation operators
// are right-associative. The procedures for left-associative
// operators eat their productions in a while loop, and
// the right-associative ones call themselves recursively.
//
// Here is the grammar of accepted expressions:
//	expr   ::=	[expr1] ; [expr]
//	expr1  ::=	var = expr1 |
//				expr2
//	expr2  ::=	expr3 [+ expr3 | - expr3]
//	expr3  ::=	expr4 [* expr4 | / expr4 | % expr4]
//	expr4  ::=	[+ | -] ( expr1 )	[^ expr4] |
//				[+ | -] var			[^ expr4] |
//				[+ | -] factor		[^ expr4] 
//	var    ::=	string
//	factor ::=	number [. number] | [number] . number
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __num_eval_already_included_vasya__
#define __num_eval_already_included_vasya__

#include <math.h>
#include "_dictionary_.h"
#include "_string_.h"

namespace soige {

class _num_eval_
{
public:
	_num_eval_();
	virtual ~_num_eval_();
	double evaluate(LPCSTR expr);
	void   reset();

protected:
	typedef enum _TOKTYPE {
		TOK_VAR, TOK_NUM, TOK_SPEC, TOK_END
	} TOKTYPE;

	_string_	_expression;
	_string_	_curtok;
	_string_	_prevtok;
	TOKTYPE		_curtoktype;
	TOKTYPE		_prevtoktype;
	const char* _curptr;  // pointer to current position in _expr
	_dictionary_<_string_, double> _vars;

protected:
	double _expr();
	void   _expr1(double& result);
	void   _expr2(double& result);
	void   _expr3(double& result);
	void   _expr4(double& result);

	TOKTYPE _gettoken();
	void	_ungettoken();
	void	_match(LPCSTR to_match);
	bool	_isdelim(char c);
	bool	_isspecial(char c);

private:
	// no byval operations
	_num_eval_(const _num_eval_& rne) { }
	_num_eval_& operator=(const _num_eval_& rne) { }
};

};	// namespace soige

#endif  // __num_eval_already_included_vasya__
