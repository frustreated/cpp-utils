//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _num_eval_.cpp - implementation file for the
// recursive descent numeric expression parser.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_num_eval_.h"

namespace soige {

// helper rounder
#define ROUNDED_LONG(x)  ( \
							( (x)<0 ) ? \
							( ((x)-((long)(x)))<=-0.5 ? ((long)(x)-1) : ((long)(x)) ) : \
							( ((x)-((long)(x)))>= 0.5 ? ((long)(x)+1) : ((long)(x)) ) \
						 )

_num_eval_::_num_eval_()
{
	_expression = "";
	_curtok = "";
	_prevtok = "";
	_curtoktype = _prevtoktype = TOK_END;
	_curptr = NULL;
}

_num_eval_::~_num_eval_()
{
	reset();
}

double _num_eval_::evaluate(LPCSTR expr)
{
	double result = 0.0;

	_expression = expr;
	if( !_expression.endsWith(";") ) _expression += ";";
	_curptr = &_expression[0];

	// eval
	return _expr();
}

void _num_eval_::reset()
{
	_expression = "";
	_curtok = "";
	_prevtok = "";
	_curtoktype = _prevtoktype = TOK_END;
	_curptr = NULL;
	_vars.clear();
}

//----------------------------------------------
// This func evaluates the following production:
//	expr   ::=	[expr1] ; [expr]
double _num_eval_::_expr()
{
	double result = 0.0;
	bool all_empty = true;

	while( _gettoken() != TOK_END )
	{
		if( _curtok[0] == ';' )
			continue;  // empty expression
		all_empty = false;
		_ungettoken();
		_expr1( result );
		_match( ";" );
	}

	if(all_empty)
	{
		// all the expressions were empty; at least one has to be
		// non-empty for the whole expression to be evaluated
		// throw exception( "Syntax error: specified expression is empty" );
		return 0.0;
	}

	return result;
}

//----------------------------------------------
// This func evaluates the following production:
//	expr1  ::=	var = expr1 |
//				expr2
void _num_eval_::_expr1(double& result)
{
	_gettoken();
	if( _curtoktype == TOK_VAR )
	{
		_string_ var = _curtok;
		// could be first or second production in expr1
		// figure it out by checking if the next token is =
		_gettoken();
		if( _curtok[0] == '=' )
		{
			if( !_vars.containsKey(var) )
				_vars.put(var, 0.0);
			_expr1(result);
			_vars.get(var) = result;
		}
		else
		{
			_ungettoken();
			_expr2(result);
		}
	}
	else
	{
		// second production in expr1
		_expr2(result);
	}
}

//----------------------------------------------
// This func evaluates the following production:
//	expr2  ::=	expr3 [+ expr3 | - expr3]
void _num_eval_::_expr2(double& result)
{
	double temp = 0.0;

	_expr3(result);
	_gettoken();
	while( _curtok[0] == '+' || _curtok[0] == '-' )
	{
		if( _curtok[0] == '+' )
		{
			_gettoken();
			_expr3(temp);
			result += temp;
		}
		else if( _curtok[0] == '-' )
		{
			_gettoken();
			_expr3(temp);
			result -= temp;
		}
		_gettoken();
	}
	_ungettoken();
}

//----------------------------------------------
// This func evaluates the following production:
//	expr3  ::=	expr4 [* expr4 | / expr4 | % expr4]
void _num_eval_::_expr3(double& result)
{
	double temp = 0.0;

	_expr4(result);
	_gettoken();
	while( _curtok[0] == '*' || _curtok[0] == '/' || _curtok[0] == '%' )
	{
		if( _curtok[0] == '*' )
		{
			_gettoken();
			_expr4(temp);
			result *= temp;
		}
		else if( _curtok[0] == '/' )
		{
			_gettoken();
			_expr4(temp);
			result /= temp;
		}
		else if( _curtok[0] == '%' )
		{
			_gettoken();
			_expr4(temp);
			result = ROUNDED_LONG(result) % ROUNDED_LONG(temp);
		}
		_gettoken();
	}
	_ungettoken();
}

//----------------------------------------------
// This func evaluates the following production:
//	expr4  ::=	[+ | -] ( expr1 )	[^ expr4] |
//				[+ | -] var			[^ expr4] |
//				[+ | -] factor		[^ expr4] 
// The exponentiation op is right-associative,
// so recursively evaluate what's to the right.
void _num_eval_::_expr4(double& result)
{
	int sign = 1;
	
	if( _curtok[0] == '+' )
		_gettoken();
	else if( _curtok[0] == '-' )
		_gettoken(), sign = -1;
	
	if( _curtok[0] == '(' )
	{
		_expr1(result);
		_match( ")" );
	}
	else if( _curtoktype == TOK_NUM )
	{
		result = _curtok.doubleVal();
	}
	else if( _curtoktype == TOK_VAR )
	{
		if( _vars.containsKey(_curtok) )
			result = _vars.get(_curtok);
		else
			_vars.put( _curtok, result = 0.0 );
	}
	else
	{
		// error
		throw exception( "Syntax error; expected: (, or number, or variable" );
	}

	if(sign < 0) result = -result;
	
	// see if the optional [^ expr4] is present
	_gettoken();
	if( _curtok[0] == '^' )
	{
		double temp = 0.0;
		_gettoken();
		_expr4(temp);
		result = pow( result, temp );
	}
	else
		_ungettoken();
}

_num_eval_::TOKTYPE _num_eval_::_gettoken()
{
	_prevtok = _curtok;
	_prevtoktype = _curtoktype;
	_curtok = "";

	while( isspace(*_curptr) ) _curptr++;
	if( *_curptr == 0 ) return _curtoktype = TOK_END;
	
	if( isdigit(*_curptr) || *_curptr == '.' )
	{
		_curtoktype = TOK_NUM;
		while( *_curptr == '.' || isdigit(*_curptr) )
			_curtok += *_curptr++;
	}
	else if( isalpha(*_curptr) )
	{
		_curtoktype = TOK_VAR;
		while( !_isdelim(*_curptr) )
			_curtok += *_curptr++;
		_curtok.upper();
	}
	else if( _isspecial(*_curptr) )
	{
		_curtoktype = TOK_SPEC;
		_curtok += *_curptr++;
	}
	else
	{
		// error
		throw exception( "Syntax error: unexpected token" );
	}

	return _curtoktype;
}

void _num_eval_::_ungettoken()
{
	if( _curtok.length() == 0 ) return;
	_curptr -= _curtok.length();
	_curtok = _prevtok;
	_curtoktype = _prevtoktype;
	_prevtok = "";
	_prevtoktype = TOK_END;
}

void _num_eval_::_match(LPCSTR to_match)
{
	_gettoken();

	char _error[128] = "Syntax error; expected: ";
	
	if(_curtok != to_match)
	{
#if defined(UNICODE) || defined(_UNICODE)
		char* p = (char*) malloc( lstrlenA(to_match)+1 );
		wcstombs(p, to_match, lstrlenA(to_match)+1);
		lstrcatA(_error, p);
		free(p);
#else
		lstrcatA(_error, to_match);
#endif  // UNICODE
		throw exception(_error);
	}
}

bool _num_eval_::_isdelim(char c)
{
	// delimiter = isspace | isspecial
	return ( strchr(" \x09\x0a\x0b\x0c\x0d;+-*/%^()=", c) != NULL );
}

bool _num_eval_::_isspecial(char c)
{
	return ( strchr(";+-*/%^()=", c) != NULL );
}


};	// namespace soige

