#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <stage2.h>
#include <vector>
#include <ctime>
#include <cctype>
#include <stack>
#include <iomanip>


//fix 142 143 150 155 165 114 113 120
using namespace std;

bool beginEndCheck = false;
int  beginEndCount = 0;
// Methods implementing the grammar productions
 /*void prog(); // stage 0, production 1
 void progStmt(); // stage 0, production 2
 void consts(); // stage 0, production 3
 void vars(); // stage 0, production 4
 void beginEndStmt(); // stage 0, production 5
 void constStmts(); // stage 0, production 6
 void varStmts(); // stage 0, production 7
 string ids(); // stage 0, production 8*/

 //map<string, SymbolTableEntry>::iterator i;

 // Constructor
Compiler::Compiler(char** argv)
{
	// open sourceFile using argv[1]				
	sourceFile.open(argv[ 1 ]);					//<<<<<<<<<<<<<<<<ios				
	// open listingFile using argv[2]
	listingFile.open(argv[ 2 ]);					//<<<<<<<<<<<<<<<<ios
	// open objectFile using argv[3]
	objectFile.open(argv[ 3 ]);					//<<<<<<<<<<<<<<<<ios
}

// Destructor
Compiler::~Compiler()
{
	// close all open files and Print the symbol table
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	// line numbers and source statements should be aligned under the headings
	time_t now = time(NULL);
	listingFile << "STAGE2:  " << "Kangmin Kim, Jeff Caldwell   " << ctime(&now) << endl;
	listingFile << "LINE NO." << setw(30) << "SOURCE STATEMENT" << endl;			//<<<<<<<setw, work on it
	listingFile << endl;
}

void Compiler::parser()
{
	nextChar();
	// ch must be initialized to the first character of the source file

	// set the character to the first character in the file
	if (nextToken() != "program")
	{
		processError("keyword \"program\" expected");
	}
	// a call to nextToken() has two effects
	// (1) the variable, token, is assigned the value of the next token
	// (2) the next token is read from the source file in order to make
	// the assignment. The value returned by nextToken() is also
	// the next token.

	prog();
	//parser implements the grammar rules, calling first rule
}

void Compiler::createListingTrailer()
{
	// print "COMPILATION TERMINATED", "# ERRORS ENCOUNTERED"
	listingFile << endl << "COMPILATION TERMINATED" << right << setw(7) << errorCount << " ERRORS ENCOUNTERED" << endl;
}
//token should be "program"
void Compiler::prog()
{
	if (token != "program")
	{
		processError("keyword \"program\" expected");
	}

	progStmt();

	if (token == "const")
	{
		consts();
	}

	if (token == "var")
	{
		vars();
	}

	if (token != "begin")
	{
		processError("keyword \"begin\" expected");
	}

	beginEndStmt();
}
void Compiler::processError(string err)
{
	// for processing errors outputs at the bottom with the specified error
	//Output err to listingFile
	//Call exit() to terminate program
	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl;
	errorCount += 1;
	listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
	exit(-1);
}


//token should be "program"
void Compiler::progStmt()
{
	string x;

	if (token != "program")
	{
		processError("keyword \"program\" expected");
	}

	x = nextToken(); //may need to change to nextToken()

	// if token is not a non_key_id
	if (!isNonKeyId(token))
	{
		processError("program name expected");
	}

	if (nextToken() != ";")
	{
		processError("semicolon expected");
	}

	nextToken();
	code("program", x);
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);

}
//token should be "const"
void Compiler::consts()
{
	if (token != "const")
	{
		processError("keyword \"const\" expected");
	}
	//change from (!isNonKeyId(nextToken()))
	if (!(isNonKeyId(nextToken())))
	{
		processError("non-keyword identifier must follow \"const\"");
	}

	constStmts();
}
//token should be "var"
void Compiler::vars()
{
	if (token != "var")
	{
		processError("keyword \"var\" expected");
	}

	if (!(isNonKeyId(nextToken()))) // change from (!isNonKeyId(nextToken()))
	{
		processError("non-keyword identifier must follow \"var\"");
	}

	varStmts();
}

//token should be "begin"
//process begin-end statement
 //stage 1 production 1
void Compiler::beginEndStmt()
{
	if (token != "begin")
	{
		processError("begin expected");
	}

	nextToken();
	execStmts();

	if (token != "end")
	{
		processError("error");
	}
	nextToken();


	if (token == ".")
	{
		code("end", ".");
	}
	else if (token == ";" && beginEndCheck == false && beginEndCount == 0)
	{
		processError("{Final end lacks period, has semicolon instead.}");
	}
	else if (token == ";")
	{
		return;
	}

	else
	{
		processError("'.' or ';' expected following \"end\"");
	}
}
// token should be a non_key_id
void Compiler::constStmts()
{
	string x, y;
	// if token is a non_key_id
	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}

	x = token;

	// if (nextToken() != "=")
	if (nextToken() != "=")
	{
		processError("\"=\" expected");
	}

	y = nextToken();

	//if (y is not one of "+","-","not",NON_KEY_ID,"true","false",INTEGER)
	if (y != "+" && y != "-" && y != "not" && y != "true" && y != "false" && !(isNonKeyId(y)) && whichType(y) != INTEGER) //maybe not include isInterger //double check
	{
		processError("token to right of \"=\" illegal");
	}

	//if (y is one of "+","-")
	if (y == "+" || y == "-")
	{
		//if (nextToken() is not an INTEGER)
		if (whichType(nextToken()) != INTEGER || !isInteger(token))
		{
			//cout << token;
			processError("integer expected after sign");
		}
		y = y + token;

	}

	if (y == "not")
	{
		//if (nextToken() is not a BOOLEAN)
		if (whichType(nextToken()) != BOOLEAN)
		{
			processError("boolean expected after \"not\"");
		}

		if (whichValue(token) == "true")
		{
			y = "false";
		}
		else
		{
			y = "true";
		}
	}

	if (nextToken() != ";")
	{
		processError("semicolon expected after type");
	}

	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN) //fixed
	{
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	}
	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

	x = nextToken();

	//if (x is not one of "begin","var",NON_KEY_ID)
	if (x != "begin" && x != "var" && !isNonKeyId(x))
	{
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	}

	//if (x is a NON_KEY_ID)
	if (isNonKeyId(x))
	{
		constStmts();
	}
}

//token should be NON_KEY_ID //double check
void Compiler::varStmts()
{
	string x, y;

	//if (token is not a NON_KEY_ID)
	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}

	x = ids();

	if (token != ":")
	{
		processError("\":\" expected");
	}
	nextToken();
	//if (nextToken() is not one of "integer","boolean")
	if (token != "integer" && token != "boolean") //isInteger && isBoolean
	{
		processError("illegal type follows \":\"");
	}

	y = token;

	if (nextToken() != ";")
	{
		processError("semicolon expected");
	}

	//insert(x,y,VARIABLE,"",YES,1)
	//insert(x,(y == "integer"? INTEGER : BOOLEAN ), VARIABLE, "", YES, 1); 

	if (y == "integer")
	{
		insert(x, INTEGER, VARIABLE, "1", YES, 1);
	}
	else
	{
		insert(x, BOOLEAN, VARIABLE, "1", YES, 1);
	}

	nextToken();
	//if (nextToken() is not one of "begin",NON_KEY_ID)
	if (token != "begin" && !isNonKeyId(token)) //CHANGE FROM TOKEN TO NEXT TOKEN
	{
		processError("non-keyword identifier or \"begin\" expected");
	}

	//if (token is a NON_KEY_ID)
	if (isNonKeyId(token))
	{
		varStmts();
	}

}
//token should be NON_KEY_ID
string Compiler::ids()
{
	string temp, tempString;

	//if (token is not a NON_KEY_ID)
	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}

	tempString = token;
	temp = token;

	if (nextToken() == ",")
	{
		//if (nextToken() is not a NON_KEY_ID)
		if (!isNonKeyId(nextToken()))
		{
			processError("non-keyword identifier expected");
		}

		tempString = temp + "," + ids();
	}
	return tempString;
}
//double check	//stage 1 production 2
void Compiler::execStmts()
{
	if (token[ 0 ] == '$')
	{

	}
	if (isNonKeyId(token) || token == "read" || token == "write" || token == "begin" || token == "if" || token == "while" || token == "repeat" || token == ";")
	{
		execStmt();
		execStmts();
	}

	else if (token == "end")
	{

	}

	else if (token == "until")
	{

	}

	else
	{
		processError("one of \";\", \"begin\", \"if\", \"read\", \"repeat\", \"while\", \"write\", \"end\", or \"until\" expected");		//error here
	}


}
// production 3
void Compiler::execStmt()
{
	if (isNonKeyId(token))
	{
		assignStmt();
	}
	else if (token == "begin")
	{
		beginEndCount++;
		beginEndCheck = true;
		beginEndStmt();
	}
	else if (token == "end")
	{
		beginEndCount--;
		if (beginEndCount == 0)
			beginEndCheck = false;
		return;
	}
	else if (token == "read")
	{
		readStmt();
	}
	else if (token == "write")
	{
		writeStmt();
	}
	if (token == "if")
	{
		ifStmt();
	}
	else if (token == "while")
	{
		whileStmt();
	}
	else if (token == "repeat")
	{
		repeatStmt();
	}
	else if (token == ";")
	{
		nullStmt();
	}
	else
	{
		processError("non-keyword id, \"read\", or \"write\" expected ");
	}
}
//stage 1 production 4
void Compiler::assignStmt()
{
	string first, second;

	if (!isNonKeyId(token))
	{
		processError("non - keyword identifier expected");
	}

	if (symbolTable.count(token) == 0)	//if the token is not defined in our symbol table
	{
		processError("reference to undefined variable");
	}

	pushOperand(token);
	nextToken();

	if (token != ":=")
	{
		processError("':=' expected assignment statement");
	}

	pushOperator(":=");	//push the operator ":=" onto the stack
	nextToken();

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
	{
		processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
	}

	express();
	second = popOperand();
	first = popOperand();

	code(popOperator(), second, first);

}
//production 5
void Compiler::readStmt()
{
	string x;
	if (token != "read")
	{
		processError("read expected " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected after \"read\"");
	}

	nextToken();
	x = ids();

	if (token != ")")
	{
		processError("',' or ')' expected are non_key_id in \"read\" ");
	}

	code("read", x);
	nextToken();

	if (token != ";")
	{
		processError("';' expected ");
	}
}
//production 7
void Compiler::writeStmt()
{
	string x;
	if (token != "write")
	{
		processError("write expected " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected after \"write\"");
	}

	nextToken();
	x = ids();

	if (token != ")")
	{
		processError("',' or ')' expected after non-keyword identifier");
	}

	code("write", x);
	nextToken();

	if (token != ";")
	{
		processError("';' expected");
	}
}

//done //stage 1 production 9
void Compiler::express()
{

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non - keyword identifier or integer expected" + token);
	}

	term();

	expresses();

}
//done
void Compiler::expresses()
{
	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		pushOperator(token);
		nextToken();
		term();
		string one = popOperand();
		string two = popOperand();
		code(popOperator(), one, two);
		expresses();
	}
}

//done
//stage 1 production 11
void Compiler::term()
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
	}

	factor();

	if (token == "-" || token == "+" || token == "or")
	{
		terms();
	}
}
//done 
void Compiler::terms()
{
	if (token == "+" || token == "-" || token == "or")
	{
		pushOperator(token);
		nextToken();
		factor();
		string one = popOperand();
		string two = popOperand();
		code(popOperator(), one, two);
		terms();
	}
}
//stage 1 production 13
void Compiler::factor()
{
	// FACTOR {'not','true','false','(','+','-',INTEGER,NON_KEY_ID}
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", INTEGER, or NON_KEY_ID expected");
	}

	// PART 
	part();

	// FACTORS {'*','div','mod','and'}
	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}
	//{'<>','=','<=','>=','<','>',')',';','-','+','or'}
	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == "," || token == ";" || token == "-" || token == "+" || token == "or" || token == "begin" || token == "then" || token == "do")
	{

	}
	else
	{
		processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
	}

}
//stage 1 production 14
void Compiler::factors()
{
	string first, second;

	if (token != "*" && token != "div" && token != "mod" && token != "and")
	{
		processError("\"*\", \"div\", \"mod\", or \"and\" expected");
	}

	pushOperator(token);
	nextToken();

	if (token != "not" && token != "(" && !isInteger(token) && !isNonKeyId(token) && token != "+" && token != "-" && token != "true" && token != "false")
	{
		processError("expected '(', integer, or non-keyword id");
	}

	part();

	second = popOperand();
	first = popOperand();

	code(popOperator(), second, first);

	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}

}
//100% part is done
//stage 1 production 15
void Compiler::part()
{
	string x = "";
	if (token == "not")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}

			express();

			if (token != ")")
			{
				processError("')' expected" + token);
			}

			nextToken();
			code("not", popOperand());
		}

		else if (isBoolean(token))
		{
			if (token == "true")
			{
				pushOperand("false");
				nextToken();
			}
			else
			{
				pushOperand("true");
				nextToken();
			}
		}

		else if (isNonKeyId(token))
		{
			code("not", token);
			nextToken();
		}
	}

	else if (token == "+")
	{
		//cout << token << __LINE__ << endl;
		nextToken();
		//cout << token << __LINE__ << endl;
		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}

			express();

			if (token != ")")
			{
				processError("')' expected");
			}

			nextToken();
		}

		else if (isInteger(token) || isNonKeyId(token))
		{
			pushOperand(token);
			nextToken();
		}

		else
		{
			processError("expected '(', integer, or non-keyword id; found " + token);
		}
	}

	else if (token == "-")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}

			express();

			if (token != ")")
			{
				processError(" ')' expected");
			}

			nextToken();
			code("neg", popOperand());
		}
		else if (isInteger(token))
		{
			pushOperand("-" + token);
			nextToken();
		}
		else if (isNonKeyId(token))
		{
			code("neg", token);
			nextToken();
		}
		else
		{
			processError("expected '(', integer, or non_key_id");
		}
	}
	else if (token == "(")
	{
		nextToken();

		if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
		{
			processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
		}

		express();

		if (token != ")")
		{
			processError(") expected " + token);
		}

		nextToken();
	}
	else if (isInteger(token) || isBoolean(token) || isNonKeyId(token))
	{
		pushOperand(token);
		nextToken();
	}
	else
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, boolean, or non - keyword identifier expected");
	}
}

// stage 2, production 3
void Compiler::ifStmt()
{
	if (token != "if")
	{
		processError("if expected; found " + token);
	}
	nextToken();
	express();

	if (token != "then")
	{
		processError("then expected; found " + token);
	}

	string temp = popOperand();
	code("then", temp);
	nextToken();

	if (isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin")
	{
		execStmt();
	}

	elsePt();
}


// stage 2, production 4
void Compiler::elsePt()
{
	if (token == "else")
	{
		string temp = popOperand();
		code("else", temp);
		nextToken();

		execStmt();
		code("post_if", popOperand());
	}
	else if (isNonKeyId(token) || token == "end" || token == "write" || token == "read" || token == "repeat" || token == "if" || token == "while" || token == "begin" || token == "until" || token == ";")
	{
		code("post_if", popOperand());
	}
	else
	{
		processError("illegal character");
	}
}

// stage 2, production 5
void Compiler::whileStmt()
{
	if (token != "while")
	{
		processError("while expected");
	}

	code("while");
	nextToken();
	express();

	if (token != "do")
	{
		processError("do expected");
	}

	code("do", popOperand());

	nextToken();

	execStmt();

	string second = popOperand();

	string first = popOperand();

	code("post_while", second, first);

}

void Compiler::repeatStmt()
{
	if (token != "repeat")
	{
		processError("received " + token + " expected repeat");
	}

	code("repeat");
	nextToken();

	if (!isNonKeyId(token) && token != "read" && token != "write" && token != "end" && token != "write" && token != "read" && token != "repeat" && token != "if" && token != "while" && token != "begin" && token != "until" && token != ";")
	{
		processError("error1");
	}

	execStmts();

	if (token != "until")
	{
		processError("error2" + token);
	}

	nextToken();
	express();
	string second = popOperand();
	string first = popOperand();

	code("until", second, first);

	if (token != ";")
	{
		processError("received " + token + " expected ;");
	}
}

// stage 2, production 7
void Compiler::nullStmt()
{
	if (token != ";")
	{
		processError("received " + token + " expected ;");
	}

	nextToken();
}


bool Compiler::isKeyword(string s) const
{
	if (s == "program"
		|| s == "begin"
		|| s == "end"
		|| s == "var"
		|| s == "const"
		|| s == "integer"
		|| s == "boolean"
		|| s == "true"
		|| s == "false"
		|| s == "not"
		|| s == "mod"
		|| s == "div"
		|| s == "and"
		|| s == "or"
		|| s == "read"
		|| s == "write"
		|| s == "until"
		|| s == "if"
		|| s == "do"
		|| s == "else"
		|| s == "while"
		|| s == "repeat"
		|| s == "then"

		)
	{
		return true;
	}
	else
		return false;
}

bool Compiler::isSpecialSymbol(char c) const
{
	if (c == '='
		|| c == ':'
		|| c == ','
		|| c == ';'
		|| c == '.'
		|| c == '+'
		|| c == '-'
		|| c == '*'
		|| c == '<'
		|| c == '>'
		|| c == '('
		|| c == ')')
	{
		return true;
	}
	return false;
}

//determines if s is a non_key_id
bool Compiler::isNonKeyId(string s) const
{
	if (!isKeyword(s) && !isInteger(s) && !isSpecialSymbol(s[ 0 ]))
	{
		return true;
	}

	return false;
}

// allow for a + or - followed by one or more digits
bool Compiler::isInteger(string s) const
{
	for (uint i = 0; i < s.length(); i++)
	{
		if (i == 0)
		{
			if (s[ 0 ] == '+' || s[ 0 ] == '-')
				i++;
		}
		if (s[ i ] != '0' && s[ i ] != '1' && s[ i ] != '2' && s[ i ] != '3' && s[ i ] != '4' && s[ i ] != '5' && s[ i ] != '6' && s[ i ] != '7' && s[ i ] != '8' && s[ i ] != '9')
			return false;
	}
	return true;
}

// determines if s is a boolean
bool Compiler::isBoolean(string s) const
{
	if (s == "true" || s == "false")
	{
		return true;
	}
	else
	{
		return false;
	}
}

//determines if s is a literal
bool Compiler::isLiteral(string s) const
{
	//bool integer = isInteger(s);
	if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')// s[0] == "+" || s[0] == "-")
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	//create symbol table entry for each identifier in list of external names
	//Multiply inserted names are illegal
	string name;
	uint listName = 0;

	/* while the name broken from the list of external names
		is put into name != "" */
	while (listName < externalName.length())
	{
		name = "";
		while (name == "")
		{
			while (listName < externalName.length() && externalName[ listName ] != ',')
			{
				name += externalName[ listName ];
				listName += 1;
			}

			// move to the next character
			listName += 1;

			// if the name holds a value
			// a maximum of 15 characters are allowed
			name = name.substr(0, 15);

			// if (symbolTable[name] is defined)
			if (symbolTable.count(name) > 0)
			{
				processError("symbol x is multiply defined");
			}

			else if (isKeyword(name) && name != "true" && name != "false")
			{
				processError("illegal use of keyword");
			}
			// create table entry
			else
			{
				// if name begins with an uppercase letter
				if (name == "true" || name == "TRUE")
				{
					symbolTable.insert({"true", SymbolTableEntry("TRUE", inType, inMode, inValue, inAlloc, inUnits)});
				}
				else if (name == "false" || name == "FALSE")
				{
					symbolTable.insert({"false", SymbolTableEntry("FALSE", inType, inMode, inValue, inAlloc, inUnits)});
				}
				else if (isupper(name[ 0 ]))
				{
					symbolTable.insert({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
				}
				else
				{
					symbolTable.insert({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
				}
			}
		}

		if (symbolTable.size() > 256)
		{
			processError("symbol table cannot exceed 256");
		}
	}
}

//tells which data type a name has
storeTypes Compiler::whichType(string name)
{
	storeTypes type;

	//if (name is a literal)
	if (isLiteral(name))
	{
		//if (name is a boolean literal)
		if (name == "not" || name == "false" || name == "true")
		{
			type = BOOLEAN;
		}
		else
		{
			type = INTEGER;
		}
	}
	else //name is an identifier and hopefully a constant
	{
		// if (symbolTable[name] is defined)
		if (symbolTable.find(name) != symbolTable.end())
		{
			type = symbolTable.at(name).getDataType();
		}

		else
		{
			processError("reference to undefined constant");
		}
	}
	return type;
}

//tells which value a name has
string Compiler::whichValue(string name)
{
	string value;

	//if (name is a literal)
	if (isLiteral(name))
	{
		//value = name;
		/* if the value is false the boolean value is 0 */
		if (name == "false")
		{
			name = "0";
		}
		/* if the value is true the boolean value is -1 */
		else if (name == "true")
		{
			name = "-1";
		}
		else
		{
			value = name;
		}
	}
	else //name is an identifier and hopefully a constant
	{
		if (symbolTable.count(name) > 0)
		{
			//(symbolTable[name] is defined and has a value)
			name = symbolTable.at(name).getValue();
		}
		else
		{
			processError("reference to undefined constant");
		}
	}
	return name;
}

//done
void Compiler::code(string op, string operand1, string operand2)
{
	if (op == "program")
	{
		emitPrologue(operand1);
	}
	else if (op == "end")
	{
		emitEpilogue();
	}
	else if (op == "read")
	{
		emitReadCode(operand1, operand2);
	}
	else if (op == "write")
	{
		emitWriteCode(operand1, operand2);
	}
	else if (op == "+")
	{
		emitAdditionCode(operand1, operand2);// this must be binary '+'
	}
	else if (op == "-")
	{
		emitSubtractionCode(operand1, operand2);// this must be binary '-'
	}
	else if (op == "neg")
	{
		emitNegationCode(operand1, operand2);// this must be unary '-'
	}
	else if (op == "not")
	{
		emitNotCode(operand1, operand2);
	}
	else if (op == "*")
	{
		emitMultiplicationCode(operand1, operand2);
	}
	else if (op == "div")
	{
		emitDivisionCode(operand1, operand2);
	}
	else if (op == "mod")
	{
		emitModuloCode(operand1, operand2);
	}
	else if (op == "and")
	{
		emitAndCode(operand1, operand2);
	}
	else if (op == "or")
	{
		emitOrCode(operand1, operand2);
	}
	else if (op == "<")
	{
		emitLessThanCode(operand1, operand2);
	}
	else if (op == ">")
	{
		emitGreaterThanCode(operand1, operand2);
	}
	else if (op == "<=")
	{
		emitLessThanOrEqualToCode(operand1, operand2);
	}
	else if (op == ">=")
	{
		emitGreaterThanOrEqualToCode(operand1, operand2);
	}
	else if (op == "<>")
	{
		emitInequalityCode(operand1, operand2);
	}
	else if (op == "=")
	{
		emitEqualityCode(operand1, operand2);
	}
	else if (op == ":=")
	{
		emitAssignCode(operand1, operand2);
	}
	else if (op == "then")
	{
		emitThenCode(operand1, "");
	}
	else if (op == "else")
	{
		emitElseCode(operand1, "");
	}
	else if (op == "post_if")
	{
		emitPostIfCode(operand1, "");
	}
	else if (op == "while")
	{
		emitWhileCode("", "");
	}
	else if (op == "do")
	{
		emitDoCode(operand1, "");
	}
	else if (op == "post_while")
	{
		emitPostWhileCode(operand1, operand2);
	}
	else if (op == "repeat")
	{
		emitRepeatCode("", "");
	}
	else if (op == "until")
	{
		emitUntilCode(operand1, operand2);
	}
	else
	{
		processError("undefined constant");
	}
}

void Compiler::pushOperator(string name)
{
	operatorStk.push(name);
}

void Compiler::pushOperand(string name) //push name onto operandStk
//if name is a literal, also create a symbol table entry for it
{
	if (symbolTable.count(name) == 0)
	{
		if (isInteger(name) || name == "true" || name == "false")
		{
			insert(name, whichType(name), CONSTANT, whichValue(name), YES, 1);		//insert symbol table entry, call whichType to determine the data type of the literal
			// may want to be like this instead insert(x,whichType(y),CONSTANT,whichValue(y),YES,1); 																																	
		}
	}

	operandStk.push(name);
}

//pop name from operatorStk
string Compiler::popOperator()
{
	string name;
	if (!operatorStk.empty())
	{
		name = operatorStk.top();
		operatorStk.pop();
	}

	else
	{
		processError("compiler error; operator stack underflow");
	}

	return name;
}
//pop name from operandStk
string Compiler::popOperand()
{
	string name;

	if (!operandStk.empty())
	{
		name = operandStk.top();
		operandStk.pop();
	}
	else
	{
		processError("compiler error; operand stack underflow");
	}

	return name;
}

void Compiler::emit(string label, string instruction, string operands, string comment)
{
	//Turn on left justification in objectFile
	//Output label in a field of width 8
	//Output instruction in a field of width 8
	//Output the operands in a field of width 24
	//Output the comment
	objectFile << left << setw(8) << label << setw(8) << instruction << setw(24) << operands << comment << endl;
}
//done
void Compiler::emitPrologue(string progName, string operand2)
{
	time_t now = time(NULL);
	// Output identifying comments at beginning of objectFile
	objectFile << "; Kangmin Kim, Jeff Caldwell       " << ctime(&now);

	//Output the %INCLUDE directives
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl;
	objectFile << endl;
	emit("SECTION ", ".text");
	emit("global", "_start", "", "; program " + progName.substr(0, 15));
	objectFile << endl;
	emit("_start:");
}
//done
void Compiler::emitEpilogue(string operand1, string operand2)
{
	emit("", "Exit", "{0}");
	objectFile << endl;
	emitStorage();
}

void Compiler::emitStorage()
{
	map<string, SymbolTableEntry>::iterator i;

	emit("SECTION", ".data");

	// Use i from-> map<string, SymbolTableEntry>::iterator i;
	for (i = symbolTable.begin(); i != symbolTable.end(); ++i)
	{
		// for those entries in the symbolTable that have
		// an allocation of YES and a storage mode of CONSTANT
		if (i->second.getAlloc() == YES && i->second.getMode() == CONSTANT)
		{
			//call emit to output a line to objectFile
			emit(i->second.getInternalName(), "dd", i->second.getValue(), "; " + i->first);
		}

	}
	objectFile << endl;
	emit("SECTION", ".bss");

	// for those entries in the symbolTable that have
	for (i = symbolTable.begin(); i != symbolTable.end(); ++i)
	{
		// an allocation of YES and a storage mode of VARIABLE
		if (i->second.getAlloc() == YES && i->second.getMode() == VARIABLE)
		{
			//call emit to output a line to objectFile
			emit(i->second.getInternalName(), "resd", i->second.getValue(), "; " + i->first);
		}
	}
}

void Compiler::emitReadCode(string operand, string operand2)
{
	string name;
	unsigned int numeOfCurrentOperand = 0;

	while (numeOfCurrentOperand < operand.length())
	{
		name = "";
		while (name == "")
		{
			while (numeOfCurrentOperand < operand.length() && operand[ numeOfCurrentOperand ] != ',')
			{
				name = name + operand[ numeOfCurrentOperand ];
				numeOfCurrentOperand = numeOfCurrentOperand + 1;
			}

			numeOfCurrentOperand += 1;
			name = name.substr(0, 15);

			if (symbolTable.count(name) == 0)
			{
				processError("reference to undefined variable '" + name + "'");
			}
			if (symbolTable.at(name).getDataType() != false)
			{
				processError("can't read variables of this type");
			}
			if (symbolTable.at(name).getMode() != VARIABLE)
			{
				processError("reading in of read-only location");
			}
			emit("", "call", "ReadInt", "; read int; value placed in eax");
			emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
			contentsOfAReg = symbolTable.at(name).getInternalName();
		}
	}
}

void Compiler::emitWriteCode(string operand, string operand2)
{
	string name;
	unsigned int numeOfCurrentOperand = 0;

	while (numeOfCurrentOperand < operand.length())
	{
		name = "";
		while (name == "")
		{
			while (numeOfCurrentOperand < operand.length() && operand[ numeOfCurrentOperand ] != ',')
			{
				name = name + operand[ numeOfCurrentOperand ];
				numeOfCurrentOperand = numeOfCurrentOperand + 1;
			}
			numeOfCurrentOperand += 1;
			name = name.substr(0, 15);

			if (symbolTable.count(name) == 0)
			{
				processError("reference to undefined variable '" + name + "'");
			}
			if (symbolTable.at(name).getInternalName() != contentsOfAReg)
			{
				emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
				contentsOfAReg = name;
			}

			if (whichType(name) == INTEGER || whichType(name) == BOOLEAN)
			{
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			}
			emit("", "call", "Crlf", "; write \\r\\n to standard out");
		}
	}
}

//if operand2 is equal to operand1, need fixes
void Compiler::emitAssignCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types for operator ':='");
	}


	if (symbolTable.at(operand2).getMode() != VARIABLE)
	{
		processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
	}

	if (operand1 == operand2)
	{
		return;
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{

		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
	}

	emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg");

	contentsOfAReg = symbolTable.at(operand2).getInternalName();

	if (isTemporary(operand1))
		freeTemp();

}

//FINE // add operand1 by operand2
void Compiler::emitAdditionCode(string operand1, string operand2)
{
	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("binary '+' requires integer operands");

	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");		//do we need commas in here?
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
		contentsOfAReg = "";

	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);		//CHECK THIS
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
	else
		emit("", "add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}

//need fixes
void Compiler::emitSubtractionCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("binary '-' requires integer operands");

	//if the A Register holds a temp not operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
		contentsOfAReg = "";

	if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}

//need fixes
void Compiler::emitMultiplicationCode(string operand1, string operand2)
{
	if (symbolTable.count(operand1) == 0)
		processError("reference to undefined symbol " + operand1);
	else if (symbolTable.count(operand2) == 0)
		processError("reference to undefined symbol " + operand2);

	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("binary '*' requires integer operands");

	//if the A Register holds a temp not operand2 then
	//emit code to store that temp into memory
	//change the allocate entry for the temp in the symbol table to yes
	//deassign it
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
		contentsOfAReg = "";

	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}


	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
	else
		emit("", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);


	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}

//done //divide operand2 by operand1
void Compiler::emitDivisionCode(string operand1, string operand2)
{

	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary 'div' requires integer operands");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit(" ", "mov", '[' + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		if (!isTemporary(contentsOfAReg))
		{
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
//fine // divide operand2 by operand1
void Compiler::emitModuloCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
		processError("binary 'mod' requires integer operands");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		if (!isTemporary(contentsOfAReg))
		{
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	emit(" ", "cdq", " ", "; sign extend dividend from eax to edx:eax");

	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	emit(" ", "xchg", "eax,edx", "; exchange quotient and remainder");

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}

//fine // change operand 1 to negative
void Compiler::emitNegationCode(string operand1, string op1)
{
	if (whichType(operand1) != INTEGER)
	{
		processError("unary '-' requires an integer operand");
	}
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (isTemporary(contentsOfAReg))
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (!isTemporary(contentsOfAReg))
		{
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to load operand1 into the A register
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}

	emit(" ", "neg", "eax", "; AReg = -AReg");

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);

	pushOperand(contentsOfAReg);
}

void Compiler::emitNotCode(string operand1, string op1)
{
	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
		processError("unary 'not' requires a boolean operand");

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (isTemporary(contentsOfAReg))
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && !isTemporary(contentsOfAReg))
		contentsOfAReg = "";

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}

	emit(" ", "not", "eax", "; AReg = !AReg");

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

//operand1 && operand2 == true //done
void Compiler::emitAndCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
		processError("binary 'and' requires boolean operands");

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (isTemporary(contentsOfAReg))
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
		contentsOfAReg = "";

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "and", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "and", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

//operand1 || operand2 == true
void Compiler::emitOrCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
		processError("illegal type");

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (isTemporary(contentsOfAReg))
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}


	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "or", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " or " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " or " + operand2);

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
//done       
void Compiler::emitEqualityCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("binary '=' requires operands of the same type");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);

	string firstlabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "je", "." + firstlabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "je", "." + firstlabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);

	string secondlabel = getLabel();
	emit("", "jmp", "." + secondlabel, "; unconditionally jump");

	emit("." + firstlabel + ":");

	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondlabel + ":");

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
//operand1 not equal to operand2
void Compiler::emitInequalityCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("binary '<>' requires operands of the same type");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			//deassign it 
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);

	string firstlabel = getLabel();
	string secondlabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "jne", "." + firstlabel, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	else
		emit("", "jne", "." + firstlabel, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	emit("", "jmp", "." + secondlabel, "; unconditionally jump");

	emit("." + firstlabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondlabel + ":");

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);

}
// operand2 less than operand1
void Compiler::emitLessThanCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("binary '<' requires integer operands");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
			contentsOfAReg = symbolTable.at(operand2).getInternalName();
		}
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string firstlabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "jl", "." + firstlabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "jl", "." + firstlabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");


	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);

	string secondLabel = getLabel();
	emit("", "jmp", "." + secondLabel, "; unconditionally jump");

	emit("." + firstlabel + ":");

	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

//operand less or equal to operand1
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
		processError("binary '<=' requires integer operands");

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
			symbolTable.at(contentsOfAReg).setAlloc(YES);
			//deassign it
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != operand2 && !isTemporary(contentsOfAReg))
	{
		if (contentsOfAReg != operand1)
		{
			contentsOfAReg = "";
		}
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
		{
			emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
			contentsOfAReg = symbolTable.at(operand2).getInternalName();
		}
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);

	string firstlabel = getLabel();
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "jle", "." + firstlabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "jle", "." + firstlabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);

	string secondLabel = getLabel();

	emit("", "jmp", "." + secondLabel, "; unconditionally jump");

	emit("." + firstlabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
		freeTemp();

	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
//need fixes
void Compiler::emitGreaterThanCode(string operand1, string operand2)
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}

	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("binary '>' requires integer operands");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "jg", "." + newLabel, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);

	string secondLabel = getLabel();
	emit("", "jmp", "." + secondLabel, "; unconditionally jump");

	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
		freeTemp();
	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

//need fixes // op2 >= op1
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2)
{
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}

	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("binary '>=' requires integer operands");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
		emit("", "jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
		emit("", "jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);

	string secondLabel = getLabel();
	emit("", "jmp", "." + secondLabel, "; unconditionally jump");

	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
		freeTemp();

	if (isTemporary(operand2))
		freeTemp();

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}


////emit code that follows 'then' and statement predicate
void Compiler::emitThenCode(string operand1, string operand2)
{
	string tempLabel;

	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
	{
		processError("if predicate must be of type boolean");
	}

	tempLabel = getLabel();

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);	// instruction to move operand1 to the A register
	}

	emit("", "cmp", "eax,0", "; compare eax to 0");	// instruction to compare the A register to zero (false)
	emit("", "je", "." + tempLabel, "; if " + operand1 + " is false then jump to end of if");	// code to branch to tempLabel if the compare indicates equality

	pushOperand(tempLabel);

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = "";
}

//emit code that follows else clause of if statement
void Compiler::emitElseCode(string operand1, string operand2)
{
	string tempLabel;

	tempLabel = getLabel();

	emit("", "jmp", "." + tempLabel, "; jump to end if");
	emit("." + operand1 + ":", "", "", "; else");

	pushOperand(tempLabel);
	contentsOfAReg = "";

}

//emit code that follows end of if statement
void Compiler::emitPostIfCode(string operand1, string)
{
	emit("." + operand1 + ":", "", "", "; end if");

	contentsOfAReg = "";
}

// emit code following 'while'
void Compiler::emitWhileCode(string operand1, string operand2)
{
	string tempLabel;

	tempLabel = getLabel();

	emit("." + tempLabel + ":", "", "", "; while");

	pushOperand(tempLabel);

	contentsOfAReg = "";

}

//emit code that follows do
void Compiler::emitDoCode(string operand1, string operand2)
{
	string tempLabel;

	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
		processError("while predicate must be of type boolean");

	tempLabel = getLabel();

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);	// instruction to move operand1 to the A register

	emit("", "cmp", "eax,0", "; compare eax to 0");
	emit("", "je", "." + tempLabel, "; if " + operand1 + " is false then jump to end while");

	pushOperand(tempLabel);

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = "";
}


//emit code at end of while loop, operand2 is the label of the beginning of the loop,
//operand1 is the label which should follow the end of the loop
void Compiler::emitPostWhileCode(string operand1, string operand2)
{
	emit("", "jmp", "." + operand2, "; end while");

	emit("." + operand1 + ":", "", "", "");

	contentsOfAReg = "";

}

//emit code that follows repeat
void Compiler::emitRepeatCode(string operand1, string operand2)
{
	string tempLabel;

	tempLabel = getLabel();

	emit("." + tempLabel + ":", "", "", "; repeat");

	pushOperand(tempLabel);

	contentsOfAReg = "";
}


//emit code that follows until and the predicate of loop. operand1 is the value of the
//predicate. operand2 is the label that points to the beginning of the loop
void Compiler::emitUntilCode(string operand1, string operand2)
{
	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
		processError("predicate of until must be of type boolean");

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + symbolTable.at(operand1).getInternalName());
		contentsOfAReg = symbolTable.at(operand1).getInternalName(); // reassign
	}

	emit("", "cmp", "eax,0", "; compare eax to 0");
	emit("", "je", "." + operand2, "; until " + operand1 + " is true");

	if (isTemporary(operand1))
		freeTemp();

	contentsOfAReg = "";
}


//returns the next character or end of file marker
char Compiler::nextChar()
{
	static int prevChar = '\n';
	//read in next character
	sourceFile.get(ch);
	// if  end of file
	// return ture if stream has reached end of file
	if (sourceFile.eof())
	{
		ch = END_OF_FILE; //use a special character to designate end of file
		return ch;
	}
	else
	{
		if (prevChar == '\n')
		{
			listingFile << setw(5) << ++lineNo << '|';
			//listingFile << ch;
		}
	}
	// print to listing file (starting new line if necessary)
	listingFile << ch;
	prevChar = ch;
	return ch;
}

//returns the next token or end of file marker {         
string Compiler::nextToken()
{
	token = "";
	while (token == "")
	{
		if (ch == '{')
		{
			while (nextChar() != END_OF_FILE && ch != '}')
			{

			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file: '}' expected");
			}
			else
			{
				nextChar();
			}
		}

		else if (ch == '}')
		{
			processError("'}' cannot begin token");
		}

		else if (isspace(ch))
		{
			nextChar();
		}

		else if (isSpecialSymbol(ch))
		{
			token = ch;
			nextChar();

			//Assignment operation :=
			if (token == ":" && ch == '=')
			{
				token += ch;
				nextChar();
			}

			if ((token == "<" && ch == '=') || (token == "<" && ch == '>') || (token == ">" && ch == '='))
			{
				token += ch;
				nextChar();
			}
		}

		else if (islower(ch))
		{
			token = ch;

			while ((nextChar() == '_' || islower(ch) || isupper(ch) || isdigit(ch)) && ch != END_OF_FILE)
			{
				token = token + ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}

		else if (isdigit(ch))
		{
			token = ch;

			while (nextChar() != END_OF_FILE && isdigit(ch))
			{
				token += ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}

		else if (ch == END_OF_FILE)
		{
			token = ch;
		}

		else
		{
			processError("illegal symbol");
		}
	}

	token = token.substr(0, 15);

	return token;
}

void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1)
		processError("compiler error, currentTempNo should be = �1");
}

string Compiler::getTemp()
{
	string temp;
	currentTempNo++;
	temp = "T" + to_string(currentTempNo);
	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "1", NO, 1);
		maxTempNo++;
	}
	return temp;
}

string Compiler::getLabel()
{
	static int count = -1;
	count += 1;
	return "L" + to_string(count);
}

bool Compiler::isTemporary(string s) const
{
	if (s[ 0 ] == 'T')
	{
		return true;
	}
	return false;
}

string Compiler::genInternalName(storeTypes stype) const
{
	static int counterB = 0, counterI = 0;
	string inputName;

	if (stype == PROG_NAME)
	{
		inputName = "P";
	}
	if (stype == BOOLEAN)
	{
		inputName = "B" + to_string(counterB);
		++counterB;
	}
	if (stype == INTEGER)
	{
		inputName = "I" + to_string(counterI);
		++counterI;
	}

	return inputName;
}
