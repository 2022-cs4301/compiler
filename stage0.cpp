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

  if (token[ 0 ] != END_OF_FILE)
  {
    processError("no text may follow \"end\"");
  }
}

void Compiler::progStmt()       // token should be program
{
  string x;

  if (token != "program")
  {
    processError("keyword \"program\" expected");
    x = nextToken();
  }

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

void Compiler::consts() //token should be "const"
{
  if (token != "const")
  {
    processError("keyword \"const\" expected");
  }
  if (!isNonKeyId(nextToken()))
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

  if (!isNonKeyId(nextToken()))
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

  if (nextToken() != "end")
  {
    processError("keyword \"end\" expected");
  }

  if (nextToken() != ".")
  {
    processError("period expected");
  }

  nextToken();

  code("end", ".");
}

void Compiler::constStmts() //token should be NON_KEY_ID
{
  string x, y;

  if (!isNonKeyId(token))
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

  // instead of using a crazy, long string of conditional operators (||),
  // just make an array and loop through that
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

bool Compiler::isSpecialSymbol(char c) const // - Jeff - all the tests need to be done!
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

bool Compiler::isLiteral(string s) const // Test me! - Jeff
{
  if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')
  {
    return true;
  }

  return false;
}

bool Compiler::isNonKeyId(string s) const // Test me! - Jeff
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
  //Multiple inserted names are illegal
{
  string name;
  uint i;

  while (i < externalName.length())
  {
    name = "";

    while (i < externalName.length() && externalName[ i ] != ',')
    {
      name = name + externalName[ i ];
      i++;
    }

    if (!name.empty())
    {
      name = name.substr(0, 15);

      if (symbolTable.find(name) != symbolTable.end())
      {
        processError("symbol " + name + " is defined multiple times");
      }
      else if (isKeyword(name))
      {
        processError("illegal use of " + name + " keyword"); // do we need to look out for booleans?
      }
      else
      {
        if (isupper(name[ 0 ]))
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
      processError("symbol table overflow");
    }

    i++;
  }
}

// Needs testing! - Jeff
storeTypes Compiler::whichType(string name) //tells which data type a name has
{
  storeTypes type;

  if (isLiteral(name))
  {
    if (isBoolean(name))
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
    if (symbolTable.find(name) != symbolTable.end())
    {
      type = symbolTable.find(name)->second.getDataType();
    }
    else
    {
      processError("variable " + name + " is undefined");
    }
  }
  return type;
}

// needs work and testing! - Jeff
string Compiler::whichValue(string name) //tells which value a name has
{
  string value;

  if (isLiteral(name))
  {
    value = name;
  }

  else //name is an identifier and hopefully a constant
  {
    if (symbolTable.find(name) != symbolTable.end())
    {
      value = symbolTable.at(name).getValue();
    }
    else
    {
      processError("constant " + name + " is undefined");
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
    processError("compiler error: function code should not be called with illegal arguments" + op);
  }
}

/** EMIT FUNCTIONS **/

void Compiler::emit(string label, string instruction, string operands, string comment)
{
  // Turn on left justification in objectFile
  //   Output label in a field of width 8
  //   Output instruction in a field of width 8
  //   Output the operands in a field of width 24
  //   Output the comment
}

void Compiler::emitPrologue(string progName, string operand2)
{
  // Output identifying comments at beginning of objectFile
  //   Output the %INCLUDE directives
  //   emit("SECTION", ".text")
  //   emit("global", "_start", "", "; program" + progName)
  //   emit("_start:")
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
  emit("", "Exit", "{0}");
  emitStorage();
}

void Compiler::emitStorage()
{
  // emit("SECTION", ".data")
  //   for those entries in the symbolTable that have
  //     an allocation of YES and a storage mode of CONSTANT
  //   {call emit to output a line to objectFile}
  //     emit("SECTION", ".bss")
  //     for those entries in the symbolTable that have
  //       an allocation of YES and a storage mode of VARIABLE
  //     {call emit to output a line to objectFile}
}


// We're going to have to untangle this crazy switch statement
string Compiler::nextToken() //returns the next token or end of file marker
{
  token = "";
  while (token == "")
  {
    {
      if (ch == '{')
      {
        while (nextChar() && ch != END_OF_FILE && ch != '}')
        {
        }	// do nothing, just skip the words

        if (ch == END_OF_FILE)
          processError("unexpected end of file");
        else
          nextChar();
      }

      else if (ch == '}')
        processError("'}' cannot begin token");

      else if (isspace(ch))
        nextChar();

      else if (isSpecialSymbol(ch))
      {
        token = ch;
        nextChar();

        // token now represent the first char in operators
        // ch represent the second one
        if (token == ":" && ch == '=')
        {
          token += ch;
          nextChar();
        }
        else if (token == "<" && (ch == '>' || ch == '='))
        {
          token += ch;
          nextChar();
        }
        else if (token == ">" && ch == '=')
        {
          token += ch;
          nextChar();
        }
      }

      else if (islower(ch))
      {
        token += ch;

        while (nextChar() && ((islower(ch) || isdigit(ch) || ch == '_')
          && ch != END_OF_FILE))
          token += ch;

        if (ch == END_OF_FILE)
          processError("unexpected end of file");
      }

      else if (isdigit(ch))
      {
        token = ch;

        // build up the number or characters
        while (isdigit(nextChar()) && ch != END_OF_FILE
          && !isSpecialSymbol(ch))
          token += ch;

        if (ch == END_OF_FILE)
          processError("unexpected end of file");
      }

      else if (ch == END_OF_FILE)
        token = ch;

      else
        processError("illegal symbol");
    }

    return token;
  }
}

char Compiler::nextChar() //returns the next character or end of file marker
{
  // get the next character
  sourceFile.get(ch);

  static char prevChar = '\n';

  if (sourceFile.eof())
  {
    ch = END_OF_FILE;
    return ch;
  }
  else
  {
    if (prevChar == '\n')
      listingFile << setw(5) << ++lineNo << '|';

    listingFile << ch;
  }

  prevChar = ch;
  return ch;
}