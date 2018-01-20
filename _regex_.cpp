//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _regex_.cpp - impl file for the regular expression string
// search class.
// See _regex_.h for accepted vocabularies.
//
// Algorithms for building the regex parse tree, building and
// manipulating DFA states from the parse tree, and building
// the match-time transition table are taken from Red Dragon.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_regex_.h"

namespace soige {

int streq(const char* s1, const char* s2) { return lstrcmp(s1, s2) == 0; }

const char* _regex_::re_error_msg[] =
{
	"",										/* REG_NOERROR */
	"Invalid regular expression",			/* REG_BADPAT */
	"Invalid collation character",			/* REG_ECOLLATE */
	"Invalid character class name",			/* REG_ECTYPE */
	"Trailing backslash",					/* REG_EESCAPE */
	"Invalid back reference",				/* REG_ESUBREG */
	"Unmatched [ or [^",					/* REG_EBRACK */
	"Unmatched ( or \\(",					/* REG_EPAREN */
	"Unmatched { or \\{",					/* REG_EBRACE */
	"Invalid contents of {} or \\{\\}",		/* REG_BADBR */
	"Invalid range end in []",				/* REG_ERANGE */
	"Out of memory",						/* REG_ESPACE */
	"Invalid preceding regular expression",	/* REG_BADRPT */
	"Regular expression too big",			/* REG_ESIZE */
	"Invalid hex char value after \\x"		/* REG_EHEX */
};

_regex_::_regex_()
{
	_originalExpr = 0;
	_regexExpr = 0;
	_fullRegex = false;
	_caseSensitive = true;
	_entireString = false;
	_parseTree = 0;
	_reError = REG_NOERROR;
}

_regex_::~_regex_()
{
	clear();
}

void _regex_::clear()
{
	if( _parseTree && _parseTree != (node*)-1 )
		tree_destroy( _parseTree );
	if( _regexExpr )
		free( _regexExpr );
	if( _originalExpr )
		free( _originalExpr );

	_originalExpr = 0;
	_regexExpr = 0;
	_fullRegex = false;
	_caseSensitive = true;
	_entireString = false;
	_parseTree = 0;
	_reError = REG_NOERROR;

	_transitions.clear();
	_accepting_states.clear();
	_running_states.clear();
}

//------------------------------------------------------------
// Compile the regex pattern into a DFA to prepare for
// string matching. Returns true if pattern was successfully
// compiled, false otherwise. In the latter case call errstr()
// to retrieve the error message.
//
// Params:
//  @pattern			- the regular expression pattern that
//		conforms to the rules for either basic or full regular
//		expressions (depending on the @fullRegex param).
//  @fullRegex			- is @pattern a basic regular expr
//		(pass false) or a full one (true)? Default is false.
//  @caseSensitive		- is matching to be case sensitive?
//		Default is true.
//  @matchEntireString	- this flag is used by grep to limit
//		matches to entire strings. This restriction can be
//		expressed in the pattern itself by bounding it with
//		^$ (in both basic and full regular expressions), but
//		grep has a separate flag for this. Default is false.
//------------------------------------------------------------
bool _regex_::initPattern ( const char* pattern, bool fullRegex,
							bool caseSensitive, bool matchEntireString )
{
	int alloclen = 0;
	char* p = 0;
	char* porig = 0;

	clear();
	
	if( !pattern )
		pattern = "";

	alloclen = strlen(pattern);
	
	// validate
	if( (alloclen == 1 && pattern[alloclen-1] == '\\') ||
		(alloclen > 1 && pattern[alloclen-1] == '\\' && pattern[alloclen-2] != '\\') )
	{
		// "trailing backslash" error
		_reError = REG_EESCAPE;
		return false;
	}

	_fullRegex = fullRegex;
	_caseSensitive = caseSensitive;
	_entireString = matchEntireString;
	
	alloclen += 7; // allow for: .*( original_expression ).*
	_originalExpr = strdup( pattern );
	_regexExpr = (char*) malloc(alloclen);
	strcpy( _regexExpr, pattern );

	if( streq(_regexExpr, "") || streq(_regexExpr, "^") || streq(_regexExpr, "$") )
	{
		_parseTree = 0;  // null expression - null tree - matches every line
		return true;
	}
	else if( streq(_regexExpr, "^$") || (_entireString && streq(_regexExpr, "")) )
	{
		_parseTree = (node*) -1;  // null expr matching entire line - i.e., empty lines only
		return true;
	}
	
	// Consider the ^ at the beginning of the pattern:
	// If pattern starts with ^, just remove it; the resulting
	// DFA will automatically match only strings starting with
	// the first char in the pattern.
	// If pattern does not start with ^, prepend .* to it,
	// which will result in a DFA matching every string in
	// which some substring matches the original pattern.
	// Also, the expression has to be enclosed in () for
	// full regex expressions to avoid incorrect matching.
	porig = p = _regexExpr;
	if( p[0] == '^' )
	{
		if( _fullRegex )
			p[0] = '(';
		else
			p++;
	}
	else if( !(p[0] == '.' && p[1] == '*') && !_entireString )
	{
		// prepend .* to the beginning of the RE
		if( _fullRegex )
		{
			memmove( p+3, p, alloclen-3 );
			p[0] = '.'; p[1] = '*'; p[2] = '(';
		}
		else
		{
			memmove( p+2, p, alloclen-2 );
			p[0] = '.'; p[1] = '*';
		}
	}
	else
	{
		if( _fullRegex )
		{
			memmove( p+1, p, alloclen-1 );
			p[0] = '(';
		}
	}

	// Consider the $ at the end of the pattern
	// (see the explanation for dealing with ^ above)
	int lenp = strlen(p);
	if( p[lenp-1] == '$' && p[lenp-2] != '\\')
	{
		if( _fullRegex )
			p[lenp-1] = ')';
		else
			p[lenp-1] = 0;
	}
	else if( !(p[lenp-2] == '.' && p[lenp-1] == '*') && !_entireString )
	{
		if( _fullRegex )
		{
			p[lenp] = ')'; p[lenp+1] = '.'; p[lenp+2] = '*';
			p[lenp+3] = 0;
		}
		else
		{
			p[lenp] = '.'; p[lenp+1] = '*';
			p[lenp+2] = 0;
		}
	}
	else
	{
		if( _fullRegex )
		{
			p[lenp] = ')';
			p[lenp+1] = 0;
		}
	}
	
	_regexExpr = (char*) malloc( strlen(p)+2 );
	strcpy(_regexExpr, p);
	free(porig);
	p = porig = 0;

	// if not case-sensitive, convert to lower
	if( !_caseSensitive )
		strlwr( _regexExpr );

	// find a char that does not occur in the pattern and
	// use it as the unique endmarker to append to the expression
	// (needed for building the parse tree); start from 255 and go down
	for(int c=255; c>=0; c--)
		if( !strchr(_regexExpr, c) )
			break;
	char em[] = { c, 0 };
	strcat( _regexExpr, em );  // append the end-marker
	
	// parse and build the tree
	eat_expression( _regexExpr, &_parseTree );
	if( _reError )
	{
		// error occurred while parsing;
		// use errstr() to get the error description
		re_error_code errtemp = _reError;
		clear();
		_reError = errtemp;
		return false;
	}

	// assign a numbered position to each non-empty leaf node
	mark_nodes_position_in_tree( _parseTree, true );
	if( _reError )
	{
		// error occurred while computing node positions;
		// use errstr() to get the error description
		re_error_code errtemp = _reError;
		clear();
		_reError = errtemp;
		return false;
	}

	// compute the transition funcs which are then used
	// to build the DFA and transition table;
	// the order of computing the transition functions has to be
	// followed exactly because later funcs depend on the earlier ones
	compute_nullable( _parseTree );
	compute_firstpos( _parseTree );
	compute_lastpos( _parseTree );
	compute_followpos( _parseTree );
	// build the actual DFA & transition table
	build_transition_table();
	
	// preset the size of the running set of states to the max node pos,
	// which is the treepos of endmarker - root's right child;
	// this will avoid unnecessary relocations at match-time
	_running_states.reserve(_parseTree->c2->treepos);

	// voila! ...pfui!
	return true;
}

//------------------------------------------------------------
// Performs the matching of the passed string against
// the previously compiled regular expression pattern.
// Returns true if match is found, false otherwise.
// The starting index of the match in @pString is returned
// in @pMatchStart, the length of the matching substring
// (in chars) - in @pMatchLength.
// Matching is done according to the two options:
// case sensitivity, and entire string matching.
// Since the string length is specified by @stringLen, it
// can contain embedded null characters without the danger
// of stopping the matcher before end of string is reached.
//------------------------------------------------------------
bool _regex_::match( const char* pString, long stringLen,
					 long* pMatchStart, long* pMatchLength, bool fastReturn )
{
#define eos(ptr)	((ptr)-pString >= stringLen)

	const char* p = pString;

	if(pMatchStart) *pMatchStart = 0;
	if(pMatchLength) *pMatchLength = 0;
	
	// two values of the _parseTree ptr are very dear to me:
	if( _parseTree == 0 )
		// null expression - matches every line
		return true;
	else if( _parseTree == ((node*)-1) )
		// null expression matching entire line - i.e. line has to be empty
		return (stringLen == 0);

	// init the running set of states with the start states,
	// which is the set in firstpos(root-node)
	_running_states.clear_size();
	_running_states.insert_all_from( _parseTree->firstpos );

	while( _running_states.size() && !eos(p) )
	{
		next_states( &_running_states, (_caseSensitive? *p : tolower(*p)) );
		// when fastReturn is specified, we do not wait for
		// the entire string to match; as long as an accepting
		// state is reached, there is a match. This also results
		// in the shortest possible match instead of the longest
		// one (which is the default in RE matching).
		if( fastReturn && is_accepting_set(&_running_states) )
			return true;
		p++;
	}

	return is_accepting_set( &_running_states );
}

const char* _regex_::errstr() const
{
	return re_error_msg[_reError];
}


#if defined(DEBUG) || defined(_DEBUG)

void _regex_::printDebugInfo()
{
	printf( "\n--- REGEX ---\n%s\n", _regexExpr );
	printf( "\n--- PARSE TREE ---\n" );
	if( _parseTree && _parseTree != (node*)-1 )
		debug_walk_tree( _parseTree );
	printf( "\n--- TRANSITIONS ---\n" );
	debug_print_transitions();
}

// does a post-order traversal (left subtree, right subtree, then root, recursively)
void _regex_::debug_walk_tree(node* pnode, int level)
{
	if( pnode )
	{
		debug_print_node( pnode, level+1 );
		debug_walk_tree( pnode->c1, level+1 );
		debug_walk_tree( pnode->c2, level+1 );
	}
}

void _regex_::debug_print_node(node* pnode, int level)
{
	int i;
	// address of current node
	for(i=0;i<level; i++) printf("\t");
	printf("this: 0x%08lX\n", pnode);
	// address of left child
	for(i=0;i<level; i++)  printf("\t");
	printf("left: 0x%08lX\n", pnode->c1);
	// address of right child
	for(i=0;i<level; i++)  printf("\t");
	printf("rght: 0x%08lX\n", pnode->c2);
	// node type
	for(i=0;i<level; i++)  printf("\t");
	if( pnode->type == type_none )
		printf("type: none\n");
	else if( pnode->type == type_cat )
		printf("type: cat\n");
	else if( pnode->type == type_or )
		printf("type: or\n");
	else if( pnode->type == type_star )
		printf("type: star\n");
	else if( pnode->type == type_char )
	{
		printf("type: char\n");
		for(i=0;i<level; i++)  printf("\t");
		printf("char: \'%c\'\n", pnode->chr);
	}
	else if( pnode->type == type_dot )
		printf("type: dot\n");
	else if( pnode->type == type_class )
		printf("type: class\n");
	else if( pnode->type == type_empty )
		printf("type: empty\n");
	// treepos
	for(i=0;i<level; i++)  printf("\t");
	printf("tpos: %d\n", (int)pnode->treepos);
	// nullable
	for(i=0;i<level; i++)  printf("\t");
	printf("null: %s\n", pnode->nullable? "yes" : "no" );
	// firstpos
	for(i=0;i<level; i++)  printf("\t");
	printf("fpos: { ");
	for(i=0; i<pnode->firstpos.size(); i++)
		printf( (i==pnode->firstpos.size()-1)? "%d " : "%d, ", pnode->firstpos[i] );
	printf("}\n");
	// lastpos
	for(i=0;i<level; i++)  printf("\t");
	printf("lpos: { ");
	for(i=0; i<pnode->lastpos.size(); i++)
		printf( (i==pnode->lastpos.size()-1)? "%d " : "%d, ", pnode->lastpos[i] );
	printf("}\n");
	// followpos
	for(i=0;i<level; i++)  printf("\t");
	printf("wpos: { ");
	for(i=0; i<pnode->followpos.size(); i++)
		printf( (i==pnode->followpos.size()-1)? "%d " : "%d, ", pnode->followpos[i] );
	printf("}\n");
	printf("\n");
	return;
}

void _regex_::debug_print_transitions()
{
	int i;
	printf( "Start state(s): { " );
	if( (int)_parseTree > 0 )
		for(i=0; i<_parseTree->firstpos.size(); i++)
			printf( (i==_parseTree->firstpos.size()-1)? "%d " : "%d, ", _parseTree->firstpos[i] );
	printf("}\n");

	printf( "Accepting state(s): { " );
	for(i=0; i<_accepting_states.size(); i++)
		printf( (i==_accepting_states.size()-1)? "%d " : "%d, ", _accepting_states[i] );
	printf("}\n");

	for(i=0; i<_transitions.size(); i++)
	{
		printf( "Transition #%d: from_state: %d, any_char/on_char: %d/%u, to_state: %d\n",
				(i+1),
				_transitions[i]->from_state,
				(int)_transitions[i]->on_any_char,
				(int)_transitions[i]->on_char,
				_transitions[i]->to_state );
	}
	printf( "\n" );
}

#endif // DEBUG


//------------------------------------------------------------
// INTERNAL PRIVATE FUNCTIONS
//------------------------------------------------------------
int _regex_::eat_expression(const char* substr, node** ppnode)
{
	const char* p = substr;

	if( _fullRegex )
		p += eat_alternation( p, ppnode );
	else
		p += eat_catenation( p, ppnode );

	if( _reError )
		return -1;
	return (p - substr);
}

int _regex_::eat_alternation(const char* substr, node** ppnode)
{
	const char* p = substr;
	node* pchild = 0; // currently worked on child node, created by eat_repetition()
	node* pcurnode = 0;

	*ppnode = 0;

	while( *p )
	{
		pchild = 0;
		p += eat_catenation( p, &pchild );
		if( _reError )
		{
			tree_destroy( pcurnode );
			return -1;
		}

		if( *p == '\r' && *(p+1) == '\n' )
			p++;

		if( *p == '|' || *p == '\n' )
		{
			p++;
			if( !pcurnode )
				pcurnode = new node(type_or);
			
			if( !pchild )
				pchild = new node(type_empty);
		}
		
		if( !pchild )
			break;
		
		if( !pcurnode )
			pcurnode = pchild;
		else
		{
			if( pcurnode->type != type_or )
			{
				node* palt = new node(type_or);
				palt->c1 = pcurnode;
				palt->c2 = pchild;
				pcurnode = palt;
			}
			else
			{
				if( !pcurnode->c1 )
					pcurnode->c1 = pchild;
				else if( !pcurnode->c2 )
					pcurnode->c2 = pchild;
				else
				{
					node* palt = new node(type_or);
					palt->c1 = pcurnode;
					palt->c2 = pchild;
					pcurnode = palt;
				}
			}
		}
	}

	// make sure that if curnode is alt, then it has both leaves
	if( pcurnode && pcurnode->type == type_or &&
		(!pcurnode->c1 || !pcurnode->c2) )
	{
		if( !pcurnode->c1 && !pcurnode->c2 )
		{
			delete pcurnode;
			pcurnode = 0;
		}
		else if( !pcurnode->c1 )
			pcurnode->c1 = new node(type_empty);
		else
			pcurnode->c2 = new node(type_empty);
	}

	*ppnode = pcurnode;
	return (p - substr);
}

int _regex_::eat_catenation(const char* substr, node** ppnode)
{
	const char* p = substr;
	node* pchild = 0; // currently worked on child node, created by eat_repetition()
	node* pcurnode = 0;

	*ppnode = 0;

	while( *p )
	{
		pchild = 0;
		p += eat_repetition( p, &pchild );
		if( _reError )
		{
			tree_destroy( pcurnode );
			return -1;
		}
		if( !pchild )
			break;
		pcurnode = tree_catenate( pcurnode, pchild );
	}
	
	// make sure that if curnode is cat, then it has both leaves
	if( pcurnode && pcurnode->type == type_cat &&
		(!pcurnode->c1 || !pcurnode->c2) )
	{
		node* ptemp = pcurnode;
		pcurnode = pcurnode->c1? pcurnode->c1 : pcurnode->c2;
		delete ptemp;
	}

	*ppnode = pcurnode;
	return (p - substr);
}

int _regex_::eat_repetition(const char* substr, node** ppnode)
{
	const char* p = substr;
	node* pcurnode = 0;

	if( !*p )
		return 0;
	
	*ppnode = 0;

	if( _fullRegex )
	{
		if( *p == '*' || *p == '?' || *p == '+' || *p == '{' )
		{
			// in full regex, repetition ops have to be preceeded by a regex string;
			// RE error in '': ?, *, +, or { } not preceded by valid regular expression
			_reError = REG_BADRPT;
			return -1;
		}
		else if( *p == ')' || *p == '|' || *p == '\r' || *p == '\n' )
		{
			// regex operators - do not process
			return (p - substr);
		}
		else if( *p == '\\' && (*(p+1) == '<' || *(p+1) == '>') )
		{
			// process beginning- or end-of-word operators
		}

		// eat the expression/char
		if( *p == '(' )
		{
			// start of a parenthesized expression
			p++;
			p += eat_expression( p, &pcurnode );
			if( _reError )
				return -1;
			if( *p++ != ')' )
			{
				tree_destroy( pcurnode );
				_reError = REG_EPAREN;
				return -1; // RE error: unbalanced ()
			}
		}
		else
		{
			p += eat_simple_char( p, &pcurnode );
			if( _reError )
				return -1;
		}

		if( pcurnode )
		{
			// check for rep ops
			if( *p == '*' )
			{
				// create a new "0 or more repetitions" node (star-node)
				// and attach the current one as its left leaf
				node* pstar = new node(type_star);
				pstar->c1 = pcurnode;
				pcurnode = pstar;
				p++;
			}
			else if( *p == '?' )
			{
				// create a new "0 or one repetition" node;
				// basically, it's a '|' node with the current node
				// as the left child and an e-leaf as the right
				node* pquestion = new node(type_or);
				pquestion->c1 = pcurnode;
				pquestion->c2 = new node(type_empty);
				pcurnode = pquestion;
				p++;
			}
			else if( *p == '+' )
			{
				// create a new "1 or more repetitions" node;
				// basically, something like this:
				/*		"ab+c"
								o
							   / \
							  o  'c'
							 / \
							o   *
						   / \   \
						 'a' 'b' 'b'
				  i.e., a cat-node with left child == current node
				  and the right child a star node with the current node
				  as the leaf
				*/
				node* pcat = new node(type_cat);
				node* pstar = new node(type_star);
				node* pcopy = tree_copy( pcurnode );
				pstar->c1 = pcopy;
				pcat->c1 = pcurnode;
				pcat->c2 = pstar;
				pcurnode = pcat;
				p++;
			}
			else if( *p == '{' )
			{
				node* ptemp = 0;
				p += eat_m_to_n( p, pcurnode, &ptemp );
				if( _reError )
				{
					tree_destroy( pcurnode );
					return -1;
				}
				pcurnode = ptemp;
			}
		} // if(pcurnode)
	}
	else  // not full regex
	{
		bool was_group = false;

		if( *p == '\\' && *(p+1) == ')' )
		{
			// regex operators - do not process
			return (p - substr);
		}
		else if( *p == '\\' && (*(p+1) == '<' || *(p+1) == '>') )
		{
			// process beginning- or end-of-word operators \< \>
		}
		else if( *p == '\\' && isdigit(*(p+1)) )
		{
			// process "repeat previous find" operator \num
			/*
			if( does not exist group_num )
			{
				_reError = REG_ESUBREG; // invalid back reference
				return -1;
			}
			*/
		}

		if( *p == '\\' && *(p+1) == '(' )
		{
			// start of a \( \) sub-expression to be repeated later
			was_group = true;
			p += 2;
			p += eat_expression( p, &pcurnode );
			if( _reError )
				return -1;
			if( !(*p == '\\' && *(p+1) == ')') )
			{
				tree_destroy( pcurnode );
				_reError = REG_EPAREN;
				return -1; // RE error: unbalanced \( \)
			}
			p += 2;
		}
		else
		{
			p += eat_simple_char( p, &pcurnode );
			if( _reError )
				return -1;
		}

		// if \( \) group preceeded, do not process repetition
		// operators as such, but rather as literals, which will
		// be done inside the next call to eat_repetition()
		if( pcurnode && !was_group )
		{
			// check for rep ops
			if( *p == '*' )
			{
				// create a new "0 or more repetitions" node (star-node)
				// and attach the current one as its left leaf
				node* pstar = new node(type_star);
				pstar->c1 = pcurnode;
				pcurnode = pstar;
				p++;
			}
			else if( *p == '\\' && *(p+1) == '{' )
			{
				node* ptemp = 0;
				p += eat_m_to_n( p, pcurnode, &ptemp );
				if( _reError )
				{
					tree_destroy( pcurnode );
					return -1;
				}
				pcurnode = ptemp;
			}
		}
	}

	*ppnode = pcurnode;
	return (p - substr);
}

int _regex_::eat_simple_char(const char* substr, node** ppnode)
{
	const char* p = substr;
	int eaten = 0;
	node* pcurnode = 0;
	
	if( !*p )
		return 0;

	*ppnode = 0;

	pcurnode = new node(type_none);

	if( *p == '[' )
	{
		// char class
		p += eat_char_class( p, pcurnode );
		if( _reError )
		{
			delete pcurnode;
			return -1;
		}
	}
	else if( *p == '.' )
	{
		// any char
		pcurnode->type = type_dot;
		p++;
	}
	else if( *p == '\\' )
	{
		// escaped char
		pcurnode->type = type_char;
		p++;
		pcurnode->chr = *p++;
		if( !pcurnode->chr )
		{
			delete pcurnode;
			_reError = REG_EESCAPE;  // "trailing backslash" error
			return -1;
		}
		else if( pcurnode->chr == 'n' ) // linefeed
			pcurnode->chr = '\n';
		else if( pcurnode->chr == 'r' ) // carriage return
			pcurnode->chr = '\r';
		else if( pcurnode->chr == 't' ) // tab
			pcurnode->chr = '\t';
		else if( pcurnode->chr == 'v' ) // vertical tab
			pcurnode->chr = '\v';
		else if( pcurnode->chr == 'f' ) // form-feed
			pcurnode->chr = '\f';
		else if( pcurnode->chr == 'a' ) // alert
			pcurnode->chr = '\a';
		else if( pcurnode->chr == 'x' ) // hex value of char
		{
			if( !ishexdigit(*p) || !ishexdigit(*(p+1)) )
			{
				delete pcurnode;
				_reError = REG_EHEX;  // "invalid hex char value" error
				return -1;
			}
			pcurnode->chr = hextochar(p);
			p += 2;
		}
		// all other escaped chars are left as is, including \, ?, ', "
	}
	else
	{
		// regular char
		pcurnode->type = type_char;
		pcurnode->chr = *p++;
	}

	*ppnode = pcurnode;
	return (p - substr);
}

int _regex_::eat_char_class(const char* substr, node* pcurnode)
{
	const uchar* p = (const uchar*) substr;
	bool reverse = false;
	uchar startchar;
	uchar endchar;

	pcurnode->makecharclass();
	
	p++;  // skip the '['
	if( *p == '^' )
	{
		reverse = true;
		p++;
	}
	
	// prep the array according to ^ or no ^ after [
	memset( pcurnode->cls, reverse, 256*sizeof(bool) );
	
	// these chars after [ (or ^) are accounted for separately
	if( *p == ']' || *p == '-' )
		pcurnode->cls[*p++] = !reverse;
	
	while( *p != ']' )
	{
		if( !*p )
		{
			_reError = REG_EBRACK;
			return -1;  // RE error in [] [ ] imbalance or syntax error
		}
		else if( *p == '-' )
		{
			if( !*(p+1) )
			{
				_reError = REG_EBRACK;
				return -1;  // RE error in [] [ ] imbalance or syntax error
			}
			else if( *(p+1) == ']' )  // '-' before closing ]
			{
				pcurnode->cls[*p] = !reverse;
			}
			else
			{
				startchar = *(p-1);
				endchar = *++p;
				if( startchar > endchar )
				{
					_reError = REG_ERANGE;
					return -1;  // RE error in [c-a] invalid endpoint in range
				}
				for( ; startchar <= endchar; startchar++ )
					pcurnode->cls[startchar] = !reverse;
			}
		}
		else if( *(p+1) != '-' ) // this is a char and next one isn't '-'
			pcurnode->cls[*p] = !reverse; // single char
		
		p++;
	}
	p++;  // skip the ']'

	return ((const char*)p - substr);
}

int _regex_::eat_m_to_n(const char* substr, node* pnode_to_m_n, node** ppnode)
{
	const char* p = substr;
	node* proot = 0;
	node* pcurnode = 0;
	char buf[16];
	int m = 0, n = 0;
	int i;

	*ppnode = 0;

	// p points to \{ in simple regex and { in full regex
	// make sure to account for this difference everywhere
	p += _fullRegex? 1 : 2;

	if( (_fullRegex && !strstr(p, "}")) || (!_fullRegex && !strstr(p, "\\}")) )
	{
		_reError = REG_EBRACE;  // missing } or \}
		return -1;
	}

	i = 0;
	while( isdigit(*p) && (i+1)<sizeof(buf) )
		buf[i++] = *p++;
	buf[i] = 0;
	m = atoi(buf);
	if( m <= 0 || m >= 256 )
	{
		_reError = REG_BADBR;
		return -1;  // RE error in a\{276\} contents of \{ \} invalid
	}

	if( (_fullRegex && *p == '}') ||
		(!_fullRegex && *p == '\\' && *(p+1) == '}') )
	{
		// exactly m reps
		p += _fullRegex? 1 : 2;
		if( m == 1 )
			proot = pnode_to_m_n;
		else
		{
			proot = new node(type_cat);
			proot->c1 = pnode_to_m_n;
			pcurnode = tree_copy( pnode_to_m_n );
			proot->c2 = pcurnode;
			i = 2;
			while( i < m )
			{
				pcurnode = tree_copy( pnode_to_m_n );
				proot = tree_catenate( proot, pcurnode );
				i++;
			}
		}
	}
	else if( *p == ',' &&
			((_fullRegex && *(p+1) == '}') ||
			 (!_fullRegex && *(p+1) == '\\' && *(p+2) == '}')) )
	{
		// at least m reps;
		// cat m nodes and a star node
		p += _fullRegex? 2 : 3;
		if( m == 1 )
			proot = pnode_to_m_n;
		else
		{
			proot = new node(type_cat);
			proot->c1 = pnode_to_m_n;
			pcurnode = tree_copy( pnode_to_m_n );
			proot->c2 = pcurnode;
			i = 2;
			while( i < m )
			{
				pcurnode = tree_copy( pnode_to_m_n );
				proot = tree_catenate( proot, pcurnode );
				i++;
			}
		}
		// cat the star node
		pcurnode = new node(type_star);
		pcurnode->c1 = tree_copy( pnode_to_m_n );
		proot = tree_catenate( proot, pcurnode );
	}
	else if( *p == ',' && isdigit(*(p+1)) )
	{
		// m to n inclusive;
		// add m nodes catenated, and then cat ? nodes up to n
		p++;
		i = 0;
		while( isdigit(*p) && (i+1)<sizeof(buf) )
			buf[i++] = *p++;
		buf[i] = 0;
		n = atoi(buf);
		if( n <= 0 || n >= 256 || m > n )
		{
			_reError = REG_BADBR;
			return -1;  // RE error in a\{276\} contents of \{ \} invalid
		}
		if( (_fullRegex && *p != '}') ||
			(!_fullRegex && (*p != '\\' || *(p+1) != '}')) )
		{
			_reError = REG_BADBR;
			return -1;  // RE error in a\{276\} contents of \{ \} invalid
		}
		p += _fullRegex? 1 : 2;

		if( m == 1 )
			proot = pnode_to_m_n;
		else
		{
			proot = new node(type_cat);
			proot->c1 = pnode_to_m_n;
			pcurnode = tree_copy( pnode_to_m_n );
			proot->c2 = pcurnode;
			i = 2;
			while( i < m )
			{
				pcurnode = tree_copy( pnode_to_m_n );
				proot = tree_catenate( proot, pcurnode );
				i++;
			}
		}
		
		// cat the ? nodes up to n
		i = m;
		while( i < n )
		{
			pcurnode = new node(type_or);
			pcurnode->c1 = tree_copy( pnode_to_m_n );
			pcurnode->c2 = new node(type_empty);
			proot = tree_catenate( proot, pcurnode );
			i++;
		}
	}
	else
	{
		_reError = REG_BADBR;
		return -1;  // RE error in a\{276\} contents of \{ \} invalid
	}

	*ppnode = proot;
	return (p - substr);
}

_regex_::node* _regex_::tree_copy(node* proot)
{
	node* pnew = 0;
	if( proot )
	{
		pnew = new node( *proot );
		pnew->c1 = tree_copy( proot->c1 );
		pnew->c2 = tree_copy( proot->c2 );
	}
	return pnew;
}

_regex_::node* _regex_::tree_catenate(node* pleft, node* pright)
{
	if( !pleft && !pright )
		return 0;
	else if( !pleft )
		return pright;
	else if( !pright )
		return pleft;

	// attach pleft as the left and pright as the right node
	// of a combined cat-node, making sure to descend all the
	// way down the left side of the right tree to reach the leaf
	node* ptree = pright;

	if( pright->type == type_cat && pright->c1 )
		while( pright->c1->type == type_cat )
			pright = pright->c1;

	// ptree is now the bottommost cat-node of ptree;
	// attach the substree as its left branch
	node* pcat = new node(type_cat);
	if( pright->type == type_cat )
	{
		pcat->c1 = pleft;
		pcat->c2 = pright->c1;
		pright->c1 = pcat;
	}
	else
	{
		pcat->c1 = pleft;
		pcat->c2 = pright;
		ptree = pcat;
	}

	return ptree;
}

//------------------------------------------------------------

void _regex_::mark_nodes_position_in_tree(node* pnode, bool reset_counter)
{
	static ushort cur_pos = 1;
	if( reset_counter )
		cur_pos = 1;

	if( cur_pos >= 0xFFFE )
	{
		_reError = REG_ESIZE;
		return;
	}

	if( pnode )
	{
		mark_nodes_position_in_tree( pnode->c1 );
		mark_nodes_position_in_tree( pnode->c2 );
		
		// only non-e leaf nodes have tree-positions
		if( pnode->isleaf() && pnode->type != type_empty )
			pnode->treepos = cur_pos++;
	}
}

// the rules for computing the nullable, firstpos, lastpos,
// and followpos functions are in Red Dragon, 138-139 pp.
void _regex_::compute_nullable(node* pnode)
{
	if( pnode )
	{
		compute_nullable( pnode->c1 );
		compute_nullable( pnode->c2 );
		if( pnode->isleaf() )
		{
			// if the node is e, then nullable is true; otherwise false
			pnode->nullable = (pnode->type == type_empty);
		}
		else // interior node
		{
			bool c1_nullable;
			bool c2_nullable;
			
			if( pnode->type == type_star )
				pnode->nullable = true;
			else if( pnode->type == type_cat )
			{
				// for an interior node, left child always exists
				c1_nullable = pnode->c1->nullable;
				c2_nullable = pnode->c2? pnode->c2->nullable : true;
				pnode->nullable = c1_nullable && c2_nullable;
			}
			else if( pnode->type == type_or )
			{
				c1_nullable = pnode->c1->nullable;
				c2_nullable = pnode->c2? pnode->c2->nullable : false;
				pnode->nullable = c1_nullable || c2_nullable;
			}
		}
	}
}

void _regex_::compute_firstpos(node* pnode)
{
	if( pnode )
	{
		compute_firstpos( pnode->c1 );
		compute_firstpos( pnode->c2 );

		if( pnode->isleaf() )
		{
			// if the node is e, then firstpos does not exist
			if( pnode->type == type_empty )
				pnode->firstpos.clear();
			else
			{
				// for non-e nodes, firstpos consists only of pnode->treepos
				pnode->firstpos.insert( pnode->treepos );
			}
		}
		else // interior node
		{
			if( pnode->type == type_star )
				pnode->firstpos = pnode->c1->firstpos;
			else if( pnode->type == type_cat )
			{
				if( pnode->c1->nullable )
				{
					// if c1 is nullable, this node's firstpos
					// is a union of c1's position set and c2's
					pnode->firstpos.insert_all_from( pnode->c1->firstpos );
					pnode->firstpos.insert_all_from( pnode->c2->firstpos );
				}
				else
					pnode->firstpos = pnode->c1->firstpos;
			}
			else if( pnode->type == type_or )
			{
				// union of c1's position set and c2's
				pnode->firstpos.insert_all_from( pnode->c1->firstpos );
				pnode->firstpos.insert_all_from( pnode->c2->firstpos );
			}
		}
	}
}

void _regex_::compute_lastpos(node* pnode)
{
	if( pnode )
	{
		compute_lastpos( pnode->c1 );
		compute_lastpos( pnode->c2 );

		if( pnode->isleaf() )
		{
			// if the node is e, then lastpos does not exist
			if( pnode->type == type_empty )
				pnode->lastpos.clear();
			else
			{
				// for non-e nodes, lastpos consists only of pnode->treepos
				pnode->lastpos.insert( pnode->treepos );
			}
		}
		else  // interior node
		{
			if( pnode->type == type_star )
				pnode->lastpos = pnode->c1->lastpos;
			else if( pnode->type == type_cat )
			{
				if( pnode->c2->nullable )
				{
					// if c2 is nullable, this node's lastpos
					// is a union of c2's position set and c1's
					pnode->lastpos.insert_all_from( pnode->c1->lastpos );
					pnode->lastpos.insert_all_from( pnode->c2->lastpos );
				}
				else
					pnode->lastpos = pnode->c2->lastpos;
			}
			else if( pnode->type == type_or )
			{
				// union of c1's position set and c2's
				pnode->lastpos.insert_all_from( pnode->c1->lastpos );
				pnode->lastpos.insert_all_from( pnode->c2->lastpos );
			}
		}
	}
}

void _regex_::compute_followpos(node* pnode)
{
	if( pnode )
	{
		compute_followpos( pnode->c1 );
		compute_followpos( pnode->c2 );

		// followpos is computed only on interior nodes
		if( !pnode->isleaf() )
		{
			ushort curpos;
			node* tempnode;
			int i;

			if( pnode->type == type_cat )
			{
				for(i=0; i<pnode->c1->lastpos.size(); i++)
				{
					curpos = pnode->c1->lastpos[i];
					tempnode = node_at_treepos( _parseTree, curpos );
					// first stick all positions in the followpos
					// of the node at position pos, if any
					if( tempnode )
						// add all positions in firstpos(c2) to the existing set
						tempnode->followpos.insert_all_from( pnode->c2->firstpos );
				}
			}
			else if( pnode->type == type_star )
			{
				for(i=0; i<pnode->lastpos.size(); i++)
				{
					curpos = pnode->lastpos[i];
					tempnode = node_at_treepos( _parseTree, curpos );
					if( tempnode )
						// add all positions in firstpos(pnode) to the existing set
						tempnode->followpos.insert_all_from( pnode->firstpos );
				}
			}
		} // if( !pnode->isleaf() )
	} // if(pnode)
}

_regex_::node* _regex_::node_at_treepos(node* proot, ushort treepos)
{
	node* retval = 0;
	if( proot )
	{
		if( proot->treepos == treepos )
			return proot;
		retval = node_at_treepos( proot->c1, treepos );
		if( retval )
			return retval;
		retval = node_at_treepos( proot->c2, treepos );
		if( retval )
			return retval;
	}

	return 0;
}

//------------------------------------------------------------

// the DFA-building algorithm is in Red Dragon, p.141
void _regex_::build_transition_table()
{
	_array_<dfa_state> dfa_states;
	ushort from_state;
	node* pcurnode;
	ushort to_state;
	transition tran;
	int i, c;

	// stick all states in firstpos(root) as the initial
	// unmarked states in _dfa_states
	for(i=0; i<_parseTree->firstpos.size(); i++)
		dfa_states.append( dfa_state(false, _parseTree->firstpos[i]) );

	while( true )
	{
		from_state = 0;
		for(i=0; i<dfa_states.length(); i++)
		{
			if( !dfa_states[i].marked )
			{
				from_state = dfa_states[i].state;
				break;
			}
		}
		
		if( from_state == 0 )
			break;

		dfa_states[i].marked = true;
		pcurnode = node_at_treepos( _parseTree, from_state );
		if( pcurnode->followpos.size() == 0 )
			continue;

		tran.from_state = from_state;
		tran.on_any_char = false;
		tran.on_char = 0;
		if( pcurnode->type == type_dot )
		{
			tran.on_any_char = true;
			for(i=0; i<pcurnode->followpos.size(); i++)
			{
				to_state = pcurnode->followpos[i];
				// "marked" flag is ignored in matching dfa_states
				if( dfa_states.find(dfa_state(true, to_state)) < 0 )
					dfa_states.append( dfa_state(false, to_state) );
				tran.to_state = to_state;
				_transitions.insert( tran );
			}
		}
		else
		{
			for(c=0; c<256; c++)
			{
				if( (pcurnode->type == type_char && pcurnode->chr == c) ||
					(pcurnode->type == type_class && pcurnode->cls[c]) )
				{
					tran.on_char = (uchar)c;
					for(i=0; i<pcurnode->followpos.size(); i++)
					{
						to_state = pcurnode->followpos[i];
						// "marked" flag is ignored in matching dfa_states
						if( dfa_states.find(dfa_state(true, to_state)) < 0 )
							dfa_states.append( dfa_state(false, to_state) );
						tran.to_state = to_state;
						_transitions.insert( tran );
					}
				}
			}
		} // curnode != dot-type
	} // while(1)

	// the start state of the matcher is the set of states
	// in firstpos(root); see match()

	// the accepting set of states includes all sets which contain
	// the position of the endmarker node (the right child of the root);
	// so walk all nodes looking for such sets
	build_accepting_states(_parseTree);
}

void _regex_::build_accepting_states(node* pnode)
{
	if( pnode )
	{
		build_accepting_states( pnode->c1 );
		build_accepting_states( pnode->c2 );

		ushort lastpos = _parseTree->c2->treepos;
		if( pnode->firstpos.contains(lastpos) )
			_accepting_states.insert_all_from( pnode->firstpos );
		else if( pnode->lastpos.contains(lastpos) )
			_accepting_states.insert_all_from( pnode->lastpos );
		else if( pnode->followpos.contains(lastpos) )
			_accepting_states.insert_all_from( pnode->followpos );
	}
}

void _regex_::next_states(short_set* pcurr_states, uchar onchar)
{
	int curr_from_size = pcurr_states->size();
	if( curr_from_size == 0 ) return;
	ushort* curr_from_states = (ushort*) malloc( curr_from_size*sizeof(ushort) );
	const transition* pt;
	const transition* const pend = _transitions.end();
	
	memcpy( curr_from_states, pcurr_states->_array, curr_from_size*sizeof(ushort) );
	pcurr_states->clear_size();

	for(int i=0; i<curr_from_size; i++)
	{
		pt = _transitions.find_first_from_transition( curr_from_states[i] );
		while ( (pt < pend) &&
				(pt->from_state == curr_from_states[i]) &&
				(pt->on_any_char || pt->on_char == onchar) )
		{
			pcurr_states->insert( pt->to_state );
			pt++;
		}
	}

	free( curr_from_states );
}

bool _regex_::is_accepting_set(const short_set* pset)
{
	for(int i=0; i<pset->size(); i++)
		if( _accepting_states.contains( (*pset)[i] ) )
			return true;
	return false;
}

//------------------------------------------------------------

void _regex_::tree_destroy(node* pnode)
{
	if( pnode )
	{
		tree_destroy( pnode->c1 );
		tree_destroy( pnode->c2 );
		pnode->c1 = pnode->c2 = 0;
		delete pnode;
	}
}


};	// namespace soige

