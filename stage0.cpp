#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stage0.h>
#include <ctime>
#include <cstring>

Compiler::Compiler(char **argv)
{
  sourceFile.open(argv[ 1 ]);
  listingFile.open(argv[ 2 ]);
  objectFile.open(argv[ 3 ]);
}

Compiler::~Compiler()
{
  sourceFile.close();
  listingFile.close();
  objectFile.close();
}

void Compiler::createListingHeader()
{
  // print "STAGE0:", name(s), DATE, TIME OF DAY
  // print "LINE NO:", "SOURCE STATEMENT"
  //line numbers and source statements should be aligned under the headings
  time_t now = time(0);
  char* currentTime = ctime(&now);
  cout << "STAGE0\t Jeff Caldwell and Kangmin Kim," << time << "\n\n";
  cout << "LINE NO:\t\t\t\t" << "SOURCE STATEMENT\n\n";
}

void Compiler::parser()
{
  nextChar();
  //ch must be initialized to the first character of the source file
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
  cout << "COMPILATION TERMINATED, # ERRORS ENCOUNTERED\n";
}

void Compiler::processError(string error)
{
  ofstream cout("error has occured");
  exit(0);
}

/** PRODUCTIONS **/

void Compiler::prog()           // stage 0, production 1
{
  if (token != "program")
  {
    processError("keyword \"program\" expected");
    progStmt();
  }

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
    beginEndStmt();
  }

  if (token != END_OF_FILE)
  {
    processError("no text may follow \"end\"");
  }
}

void Compiler::progStmt()       // token should be program
{
  if (token != "program")
  {
    processError("keyword \"program\" expected");
    x = nextToken();

  }

  if (token is not a NON_KEY_ID)
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

void Compiler::consts() //token should be "const"
{
  if (token != "const")
  {
    processError("keyword "\const\" expected");
  }
  if (nextToken() is not a NON_KEY_ID)
  {
    processError("non-keyword identifier must follow \"const\"");
  }
  constStmts();
}

void Compiler::vars() //token should be "var"
{
  if (token != "var")
  {
    processError("keyword \"var\" expected");
  }

  string tkn = nextToken();

  if (nextToken() is not a NON_KEY_ID)
  {
    processError("non-keyword identifier must follow \"var\"");
  }
  varStmts();
}

void Compiler::beginEndStmt() //token should be "begin"
{
  if (token != "begin")
  {
    processError("keyword \"begin\" expected");
  }

  string tkn = nextToken();

  if (tkn != "end")
  {
    processError("keyword \"end\" expected");
  }

  if (tkn != ".")
  {
    processError("period expected");
  }

  tkn = nextToken();

  code("end", ".");
}

void Compiler::constStmts() //token should be NON_KEY_ID
{
  string x, y;

  if (token is not a NON_KEY_ID)
  {
    processError("non-keyword identifier expected");
  }

  x = token;

  if (nextToken() != "=")
  {
    processError("\"=\" expected");
  }

  y = nextToken();

  if (y != "+" && y != "-" && y != "not" && !(isNonKeyId(y)) && !(isBoolean(y)) && !(isInteger(y)))
  {
    processError("token to right of \"=\" illegal");
  }

  if (y == "+" || y == "-")
  {
    string next = nextToken();

    if (!isInteger(next))
    {
      processError("integer expected after sign");
    }
    y = y + token;
  }

  if (y == "not")
  {
    string next = nextToken();

    if (!isBoolean(next))
    {
      processError("boolean expected after \"not\"");
    }

    if (token == "true")
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
    processError("semicolon expected");
  }

  storeTypes currentType = whichType(y);

  if (currentType != INTEGER && currentType != BOOLEAN)
  {
    processError("data type of token on the right - hand side must be INTEGER or BOOLEAN");
  }

  insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

  x = nextToken();

  if (x != "begin" && x != "var" && !(isNonKeyId(x)))
  {
    processError("non - keyword identifier, \"begin\", or \"var\" expected");
  }

  if (isNonKeyId(x))
  {
    constStmts();
  }
}

void Compiler::varStmts() //token should be NON_KEY_ID
{
  string x, y;

  if (!(isNonKeyId(token)))
  {
    processError("non-keyword identifier expected");
  }

  x = ids();

  if (token != ":")
  {
    processError("\":\" expected");
  }

  if (nextToken() != "integer" && nextToken() != "boolean")
  {
    processError("illegal type follows \":\"");
  }

  y = token;

  if (nextToken() != ";")
  {
    processError("semicolon expected");
  }

  insert(x, INTEGER, VARIABLE, "1", YES, 1);

  if (nextToken() != "begin" && !(isNonKeyId(nextToken())))
  {
    processError("non - keyword identifier or \"begin\" expected");
  }

  if (isNonKeyId(token))
  {
    varStmts();
  }
}

string Compiler::ids() //token should be NON_KEY_ID
{
  string temp, tempString;

  if (!isNonKeyId(token))
  {
    processError("non-keyword identifier expected");
  }

  tempString = token;
  temp = token;

  if (nextToken() == ",")
  {
    if (!isNonKeyId(nextToken()))
    {
      processError("non-keyword identifier expected");
    }

    tempString = temp + "," + ids();
  }
  return tempString;
}

/** TYPE CHECKING FUNCTIONS **/
bool Compiler::isKeyword(string s) const // - Jeff
{
  string keywords[ 23 ] = {
    "program", "const", "var",
    "integer", "boolean", "begin",
    "end", "true", "false",
    "not", "mod", "div", "and",
    "or", "read", "write", "if",
    "then", "else", "while",
    "do", "repeat", "until"
  };

  int len = *(&keywords + 1) - keywords;

  for (int i = 0; i < len; i++)
  {
    if (keywords[ i ] == s)
    {
      return true;
    }
  }

  return false;
}

bool Compiler::isSpecialSymbol(char c) const // - Jeff
{
  char symbols[ 12 ] = {':', ',', ';', '=', '+', '-', '.', '*', '<', '>', '(', ')'};

  int len = *(&symbols + 1) - symbols;

  for (int i = 0; i < len; i++)
  {
    if (symbols[ i ] == c)
    {
      return true;
    }
  }

  return false;
}

bool Compiler::isInteger(string s) const // Jeff - (needs testing)
{
  for (int i = 0; i < s.length(); i++)
  {
    if (!isdigit(s[ i ]))
    {
      return false;
    }
  }

  return true;
}

bool Compiler::isBoolean(string s) const // Jeff - (better test this one!)
{
  return s == "true" || "false";
}

bool Compiler::isLiteral(string s) const // Jeff
{
  if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')
  {
    return true;
  }

  return false;
}

bool Compiler::isNonKeyId(string s) const // Jeff
{
  if (!isInteger(s) && !isKeyword(s) && !isSpecialSymbol(s[ 0 ]))
  {
    return true;
  }

  return false;
}

/** ACTION ROUTINES **/

void Compiler::insert(
  string externalName,
  storeTypes inType,
  modes inMode,
  string inValue,
  allocation inAlloc,
  int inUnits)
  //create symbol table entry for each identifier in list of external names
  //Multiply inserted names are illegal
{
  string name;
  while (name broken from list of external names and put into name != "")
  {
    if (symbolTable[ name ] is defined)
    {
      processError("multiple name definition");
    }
    else if (name is a keyword)
    {
      processError("illegal use of keyword")
    }
    else //create table entry
    {
      if (name begins with uppercase)
      {
        symbolTable[ name ] = (name, inType, inMode, inValue, inAlloc, inUnits);
      }
      else
      {
        symbolTable[ name ] = (genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits);
      }
    }
  }
}

storeTypes Compiler::whichType(string name) //tells which data type a name has
{
  if (name is a literal)
  {
    if (name is a boolean literal)
    {
      data type = BOOLEAN;
    }
    else
    {
      data type = INTEGER;
    }
  }
  else //name is an identifier and hopefully a constant
  {
    if (symbolTable[ name ] is defined)
    {
      data type = type of symbolTable[ name ];
    }
    else
    {
      processError("reference to undefined constant");
    }
  }
  return data type;
}

string Compiler::whichValue(string name) //tells which value a name has
{
  if (name is a literal)
  {
    value = name;
  }
  else //name is an identifier and hopefully a constant
  {
    if (symbolTable[ name ] is defined and has a value)
    {
      value = value of symbolTable[ name ];
    }
    else
    {
      processError("reference to undefined constant");
    }
  }
  return value;
}

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
  else
  {
    processError("compiler error since function code should not be called with illegal arguments");
  }
}

/** EMIT FUNCTIONS **/

void Compiler::emit(string label, string instruction, string operands, string comment)
{
  Turn on left justification in objectFile
    Output label in a field of width 8
    Output instruction in a field of width 8
    Output the operands in a field of width 24
    Output the comment
}

void Compiler::emitPrologue(string progName, string operand2)
{
  Output identifying comments at beginning of objectFile
    Output the %INCLUDE directives
    emit("SECTION", ".text")
    emit("global", "_start", "", "; program" + progName)
    emit("_start:")
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
  emit("", "Exit", "{0}");
  emitStorage();
}

void Compiler::emitStorage()
{
  emit("SECTION", ".data")
    for those entries in the symbolTable that have
      an allocation of YES and a storage mode of CONSTANT
    {call emit to output a line to objectFile}
      emit("SECTION", ".bss")
      for those entries in the symbolTable that have
        an allocation of YES and a storage mode of VARIABLE
      {call emit to output a line to objectFile}
}

string Compiler::nextToken() //returns the next token or end of file marker
{
  token = "";
  while (token == "")
  {
    switch (ch)
    {
      case '{': //process comment
        while (nextChar() is not one of END_OF_FILE, '}')
        {
        } //empty body

        if (ch == END_OF_FILE)
        {
          processError("unexpected end of file");
        }
        else
        {
          nextChar();
        }
      case '}':
        processError("'}' cannot begin token");
        break;
      case isspace(ch):
        nextChar();
        break;
      case isSpecialSymbol(ch):
        token = ch;
        nextChar();
        break;
      case islower(ch):
        token = ch;
        break;
      case isDigit(ch):
        while (nextChar is digit but not end of file)
        {
          token += ch;
        }

        if (ch is END_OF_FILE)
        {
          processError("unexpected end of file");
        }
        break;
      case END_OF_FILE:
        token = ch;
        break;
      default:
        processError("illegal symbol");
    }
    return token;
  }

  char Compiler::nextChar() //returns the next character or end of file marker
  {
    // get the next character
    sourceFile.get(ch);

    // read in next character
    if (sourceFile.eof())
    {
      ch = END_OF_FILE;
    }
    else
    {
      ch = next character;
    }

    print to listing file;
    return ch;
  }