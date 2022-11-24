#include "stage1.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// Kangmin Kim and Jeff Caldwell
// CS 4301
// Compiler - Stage0

Compiler::Compiler(char **argv) // constructor
{
  sourceFile.open(argv[ 1 ]);  // open sourceFile using argv[1] (input from argv[1])
  listingFile.open(argv[ 2 ]); // open listingFile using argv[2] (generate a
  // listing to argv[2])
  objectFile.open(argv[ 3 ]); // open objectFile using argv[3] (write object code to argv[3])
}

Compiler::~Compiler() //  close all open files
{
  sourceFile.close();
  listingFile.close();
  objectFile.close();
}

void Compiler::createListingHeader() // destructor
{
  // print "STAGE0:", name(s), DATE, TIME OF DAY
  // print "LINE NO:", "SOURCE STATEMENT"
  time_t now = time(0);

  // line numbers and source statements should be aligned under the headings
  listingFile << "STAGE0:  "
    << "Jeff Caldwell, Kangmin Kim       " << ctime(&now) << "\n";
  listingFile << "LINE NO."
    << "               SOURCE STATEMENT\n\n";
}
// private: uint lineNo = 0; // line numbers for the listing

void Compiler::parser()
{
  nextChar(); // returns the next character or end of file marker

  // ch must be initialized to the first character of the source file
  if (nextToken() != "program") // string nextToken() returns the next token or
    // END_OF_FILE marker
  {
    processError("keyword \"program\" expected"); // Output err to listingFile
    // Call exit() to terminate program
  }
  // a call to nextToken() has two effects
  // (1) the variable, token, is assigned the value of the next token
  // (2) the next token is read from the source file in order to make
  // the assignment. The value returned by nextToken() is also
  // the next token.
  prog();
  // parser implements the grammar rules, calling first rule
}

void Compiler::createListingTrailer()
{
  listingFile << "\nCOMPILATION TERMINATED" << setw(6) << "" << right << errorCount
    << (errorCount != 1 ? " ERRORS " : " ERROR ") << "ENCOUNTERED\n";
}
// private: uint errorCount = 0; // total number of errors encountered

void Compiler::processError(string error)
{
  listingFile << "\n"
    << "Error: Line " << lineNo << ": " << error << "\n";
  errorCount++;
  createListingTrailer();
  // close files to ensure output will be written
  // calling exit() before closing the files seems
  // to preempt writing to them
  listingFile.close();
  objectFile.close();
  exit(EXIT_FAILURE);
}
/*
  Note that insert() calls genInternalName(), a function that has one argument,
  the type of the name being inserted. genInternalName() returns a unique
  internal name each time it is called, a name that is known to be a valid
  symbolic name. As a visual aid, we use different forms of internal names for
  each data-type of interest. The general form is: dn where d denotes the
  data-type of the name ("I" for integer, "B" for boolean) and n is a
  non-negative integer starting at 0. The generated source code for 001.dat
  clearly shows the effects of calling genInternalName(). The compiler itself
  will also need to generate names to appear in the object code, but since the
  compiler is defining these itself, there is no need to convert these names
  into any other form. The external and internal forms will be the same. The
  code for insert() treats any external name beginning with an uppercase
  character as defined by the compiler.
*/
string Compiler::genInternalName(storeTypes stype) const
{
  static int I = 0; // integer
  static int B = 0; // boolean
  string iName;
  if (stype == PROG_NAME)
  {
    iName = "P0";
  }
  else if (stype == INTEGER)
  {
    iName = "I" + to_string(I);
    I++;
  }

  else if (stype == BOOLEAN)
  {
    iName = "B" + to_string(B);
    B++;
  }

  return iName;
}

/** STAGE 0 PRODUCTIONS **/

void Compiler::prog() // stage 0, production 1
{                     // 1. PROG → PROG_STMT CONSTS VARS BEGIN_END_STMT
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

  if (token[ 0 ] != END_OF_FILE)
  {
    processError("no text may follow \"end\"");
  }
}

void Compiler::progStmt() // 2. PROG_STMT → 'program' NON_KEY_IDx ';'
{                         //    code(’program’, x); insert(x,PROG_NAME,CONSTANT,x,NO,0)
  string x;               //    → 'program' NON_KEY_IDx ';'

  if (token != "program")
  {
    processError("keyword \"program\" expected");
  }

  x = nextToken(); // program name

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

void Compiler::consts() // 3. CONSTS → 'const' CONST_STMTS
{                       //   → ε
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

void Compiler::vars() // 4. VARS → 'var' VAR_STMTS
{                     //   → ε
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

void Compiler::beginEndStmt() // 5. BEGIN_END_STMT → 'begin' 'end' '.'
// code(‘end’, ‘.’)
{
  string x;

  if (token != "begin")
  {
    processError("keyword \"begin\" expected");
  }

  // advance
  nextToken();

  if (token != "end")
  {
    // check for exec statements
    if (isNonKeyId(token) || token == "read" || token == "write")
    {
      execStmts();
    }
    else
    {
      processError("keyword \"end\" expected");
    }

    if (nextToken() != ".")
    {
      processError("period expected");
    }

    // check for tokens after .end
    if (nextToken() != "$")
    {
      processError("no tokens may appear after \"end.\"");
    }

    else
    {
      code("end", ".");
    }
  }
}

void Compiler::constStmts() // 6. CONST_STMTS → NON_KEY_IDx '='( NON_KEY_IDy |
// 'not' NON_KEY_IDy | LITy ) ';'
{              //   insert(x,whichType(y),CONSTANT,whichValue(y),YES,1)
  string x, y; //   ( CONST_STMTS | ε )

  if (!isNonKeyId(token))
  {
    processError("non-keyword identifier expected");
  }

  // check for illegal '_' at end of variable name
  if (token.back() == '_')
  {
    processError("illegal character '_'");
  }

  // check for '__' in middle of var/const
  if (token.find("__") != string::npos)
  {
    processError("illegal character '_'");
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
    processError("data type of token on the right - hand side must be INTEGER "
                 "or BOOLEAN");
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

void Compiler::varStmts() // token should be NON_KEY_ID
{
  string x, y;
  if (!isNonKeyId(token))
  {
    processError("non-keyword identifier expected");
  }

  // check for illegal '_' at end of variable name
  if (token.back() == '_')
  {
    processError("illegal character '_'");
  }

  // check for '__' in middle of var/const
  if (token.find("__") != string::npos)
  {
    processError("illegal character '_'");
  }

  x = ids();

  if (token != ":")
  {
    processError("\":\" expected");
  }

  if (nextToken() != "integer" && token != "boolean")
  {
    processError("illegal type follows \":\"");
  }

  y = token;

  if (nextToken() != ";")
  {
    processError("semicolon expected");
  }

  if (y == "integer")
  {
    insert(x, INTEGER, VARIABLE, "1", YES, 1);
  }
  else
  {
    insert(x, BOOLEAN, VARIABLE, "1", YES, 1);
  }

  if (nextToken() != "begin" && !(isNonKeyId(token)))
  {
    processError("non-keyword identifier or \"begin\" expected");
  }

  if (isNonKeyId(token))
  {
    varStmts();
  }
}

string Compiler::ids() // 8. IDS → NON_KEY_ID ( ',' IDS | ε )
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

/** STAGE 1 PRODUCTIONS **/
void Compiler::execStmts() // -> EXEC_STMT | EXEC_STMTS
{                          // -> ε
  if (isNonKeyId(token) || token == "read" || token == "write" || token == ";")
  {
    execStmt();  // token will be at end of last exec statement
    nextToken(); // advance token
    execStmts(); // recurse
  }
  else if (token == "end")
  {
    return;
  }
  else
  {
    processError("\";\", \"begin\", \"read\",\"write\", \"end\" expected");
  }
}

void Compiler::execStmt()
{
  if (isNonKeyId(token)) // assignment statement
  {
    // cout << "assignment token: " << token << "\n";
    assignStmt();
  }

  else if (token == "read") // read statement
  {
    // cout << "read token: " << token << "\n";
    readStmt();
  }

  else if (token == "write") // write statement
  {
    // cout << "write token: " << token << "\n";
    writeStmt();
  }

  else
  { // error
    processError("non-keyword id, \"read\", or \"write\" expected");
  }
}

void Compiler::assignStmt()
{
  // Redundant check for non-key id.
  // We could probably just remove this, honestly.
  if (!isNonKeyId(token))
  {
    processError("non-keyword id expected");
  }
  if (symbolTable.count(token) == 0)
  {
    processError("reference to undefined variable");
  }

  pushOperand(token);

  nextToken();

  if (token != ":=")
  {
    processError("\":=\" expected");
  }

  pushOperator(token); // push the operator ":=" onto the stack
  nextToken();

  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token) && token != ";")
  {
    processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
  }

  express(); // EXPRESS

  string tmp1 = popOperand();
  string tmp2 = popOperand();

  code(popOperator(), tmp1, tmp2);
}

void Compiler::readStmt()
{
  string list;          // read list
  string listItem = ""; // list item
  uint i;               // list counter

  // double check for "read" token
  if (token != "read")
  {
    processError("keyword \"read\" expected");
  }

  // We have a read token. Advance to next token.
  nextToken();

  // Make sure it's a "("
  if (token != "(")
  {
    processError("\"(\" expected");
  }

  else
  {
    // We have a left paren. Advance token.
    nextToken();

    // collect non token ids, will advance token
    list = ids();

    // loop through the characters of the list
    for (i = 0; i < list.length(); i++)
    {
      if (list[ i ] == ',')
      {
        // if we have a ',', code current list item
        code("read", listItem);

        // reset list item for next characters
        listItem = "";
      }
      else
      {
        // if we don't have a ',', add characters to the list item
        listItem += list[ i ];
      }
    }

    // code current list item
    code("read", listItem);

    // look for a right paren
    // call to ids() advanced token, so no need to advance now
    if (token != ")")
    {
      processError("',' or ')' expected after non-keyword identifier");
    }

    // advance token
    nextToken();

    // check for semicolon
    if (token != ";")
    {
      processError("';' expected");
    }
  }
}

void Compiler::writeStmt()
{
  string list;          // read list
  string listItem = ""; // list item
  uint i;               // list counter

  // double check for "write" token
  if (token != "write")
  {
    processError("keyword \"write\" expected");
  }

  // We have a write token. Advance to next token.
  nextToken();

  // Make sure it's a "("
  if (token != "(")
  {
    processError("\"(\" expected");
  }

  else
  {
    // We have a left paren. Advance token.
    nextToken();

    // collect non token ids, will advance token
    list = ids();

    // loop through the characters of the list
    for (i = 0; i < list.length(); i++)
    {
      if (list[ i ] == ',')
      {
        // if we have a ',', code current list item
        code("write", listItem);
        contentsOfAReg = listItem;

        // reset list item for next characters
        listItem = "";
      }
      else
      {
        // if we don't have a ',', add characters to the list item
        listItem += list[ i ];
      }
    }

    // code current list item
    code("write", listItem);
    contentsOfAReg = listItem;

    // look for a right paren
    // call to ids() advanced token, so no need to advance now
    if (token != ")")
    {
      processError("',' or ')' expected after non-keyword identifier");
    }

    // advance token
    nextToken();

    // check for semicolon
    if (token != ";")
    {
      processError("';' expected");
    }
  }
}

void Compiler::express() // stage 1 production 9
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non - keyword identifier or integer expected" +
                 token);
  }

  term(); // TERM

  if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
  {
    expresses(); // EXPRESSES
  }
}

void Compiler::expresses()
{
  if (token != "=" && token != "<>" && token != "<=" && token != ">=" && token != "<" && token != ">")
  {
    processError("\"=\", \"<>\", \"<=\", \">=\", \"<\", or \">\" expected");
  }
  pushOperator(token); // pushOperator(x)
  nextToken();

  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }
  term(); // TERM

  string tmp1 = popOperand();
  string tmp2 = popOperand();

  code(popOperator(), tmp1, tmp2); // code

  if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
  {
    expresses(); // EXPRESSES
  }
}

void Compiler::term()
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }
  factor(); // FACTOR

  if (token == "-" || token == "+" || token == "or")
  {

    terms(); // TERMS
  }
}

void Compiler::terms()
{

  if (token != "+" && token != "-" && token != "or")
  {
    processError("\"+\", \"-\", or \"or\" expected");
  }

  pushOperator(token); // ADD_LEVEL_OPx
  nextToken();

  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  factor(); // FACTOR

  string tmp1 = popOperand();
  string tmp2 = popOperand();

  code(popOperator(), tmp1, tmp2);

  if (token == "+" || token == "-" || token == "or")
  {
    terms(); // TREMS
  }
}

void Compiler::factor()
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
      !isInteger(token) && !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", INTEGER, or NON_KEY_ID expected");
  }

  part(); // PART

  if (token == "*" || token == "div" || token == "mod" || token == "and")
  {
    factors(); // FACTORS
  }

  //{'<>','=','<=','>=','<','>',')',';','-','+','or'}
  else if (isNonKeyId(token) || token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" ||
           token == ">" || token == ")" || token == ";" || token == "-" || token == "+" || token == "or" ||
           token == "begin" || token == "do" || token == "then")
  {
    return;
  }

  else
  {
    processError("'(', integer, or non_key_id" + token + "expected");
  }
}

void Compiler::factors()
{

  if (token != "*" && token != "div" && token != "mod" && token != "and")
  {
    processError("\"*\", \"div\", \"mod\", or \"and\" expected");
  }

  pushOperator(token); // pushOperator(x)
  nextToken();

  if (token != "not" && token != "(" && !isInteger(token) && !isNonKeyId(token) && token != "+" && token != "-" &&
      token != "true" && token != "false")
  {
    processError("expected '(', integer, or non-keyword id " + token);
    ;
  }

  part(); // PART

  string tmp1 = popOperand();
  string tmp2 = popOperand();

  code(popOperator(), tmp1, tmp2); // code

  if (token == "*" || token == "div" || token == "mod" || token == "and")
  {
    factors(); // FACTORS
  }
}

void Compiler::part()
{
  if (token == "not") // 'not'
  {
    nextToken();

    if (token == "(") // (
    {
      nextToken();

      if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
          !isInteger(token) && !isNonKeyId(token))
      {
        processError(
            "\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
      }
      express(); // EXPRESS

      if (token != ")") // )
      {
        processError(") expected");
      }

      nextToken();
      code("not", popOperand()); // code
    }

    else if (isBoolean(token)) // BOOLEAN
    {
      if (token == "true")
      {
        pushOperand("false"); // if x = true, then x = false
        nextToken();
      }
      else if (token == "false")
      {
        pushOperand("true");
        nextToken();
      }
    }

    else if (isNonKeyId(token)) // NON_KEY_IDx
    {
      code("not", token); // code
      nextToken();
    }
  }

  else if (token == "+") // +
  {

    nextToken();
    if (token == "(") // (
    {
      nextToken();

      if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
          !isInteger(token) && !isNonKeyId(token))
      {
        processError(
            "\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
      }

      express(); // EXPRESS

      if (token != ")") // )
      {
        processError("expected ')'");
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
      processError("expected '(', integer, or non-keyword id");
    }
  }

  else if (token == "-")
  {
    nextToken();

    if (token == "(")
    {
      nextToken();

      if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
          !isInteger(token) && !isNonKeyId(token))
      {
        processError(
            "\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
      }

      express();

      if (token != ")")
      {
        processError("expected ')'");
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
  }

  else if (token == "(")
  {
    nextToken();

    if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" &&
        !isInteger(token) && !isNonKeyId(token))
    {
      processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
    }

    express();

    if (token != ")")
    {
      processError(") expected");
    }

    nextToken();
  }

  else if (isInteger(token) || isBoolean(token) || isNonKeyId(token))
  {
    // we reach this point with int & nonkeyid
    // what happens in pushOperand?

    pushOperand(token);
    nextToken();
  }

  else
  {
    processError(
        "\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, boolean, or non - keyword identifier expected");
  }
}

/** END PRODUCTIONS **/

/** STACK FUNCTIONS **/

void Compiler::pushOperator(string op)
{
  operatorStk.push(op);
}

string Compiler::popOperator() // pop name from operatorStk
{
  string op;

  if (!operatorStk.empty())
  {
    op = operatorStk.top();
    operatorStk.pop();
  }
  else
  {
    processError("Compiler error: operator stack underflow");
  }
  return op;
}

void Compiler::pushOperand(string operand) // push name onto operatorStk
{
  if (symbolTable.count(operand) == 0)
  {
    if (isInteger(operand))
    {
      insert(operand, INTEGER, CONSTANT, whichValue(operand), YES, 1);
    }
    else if (operand == "true")
    {
      insert("true", BOOLEAN, CONSTANT, whichValue(operand), YES, 1);
    }
    else if (operand == "false")
    {
      insert("false", BOOLEAN, CONSTANT, whichValue(operand), YES, 1);
    }
  }
  operandStk.push(operand);
}

string Compiler::popOperand() // pop name from operandStk
{
  string op;
  if (!operandStk.empty())
  {
    op = operandStk.top();
    operandStk.pop();
  }
  else
  {
    processError("Compiler error: operand stack underflow");
  }

  return op;
}

void Compiler::freeTemp()
{
  currentTempNo--;
  if (currentTempNo < -1)
  {
    processError("compiler error: currentTempNo should be greater than or equal to –1");
  }
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
  string iName; // Label
  static int L = 0;

  iName = "L" + to_string(L);

  L++;

  return iName;
}
bool Compiler::isTemporary(string s) const // determines if s represents a temporary
{
  if (s[ 0 ] == 'T')
    return true;
  else
    return false;
}

/** TYPE CHECKING FUNCTIONS **/
bool Compiler::isKeyword(string s) const
{

  // instead of using a crazy, long string of conditional operators (||),
  // just make an array and loop through that
  string keywords[ 16 ] = {"program", "const", "var", "integer", "boolean", "begin", "end", "true",
    "false", "not", "mov", "div", "and", "or", "read", "write"};

  int len = *(&keywords + 1) - keywords; // length of keywords

  for (int i = 0; i < len; i++)
  {
    if (keywords[ i ] == s)
    {
      return true;
    }
  }

  return false;
}

bool Compiler::isSpecialSymbol(char c) const
{
  char symbols[ 12 ] = {':', ',', ';', '=', '+', '-', '.', '*', '(', ')', '>', '<'};

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

bool Compiler::isInteger(string s) const
{
  // Check for '+' or '-' without digits
  if (s.length() == 1)
  {
    if (s == "+" || s == "-")
    {
      return false;
    }
  }

  for (uint i = 0; i < s.length(); i++)
  {
    // if the first character is not a '+' or a '-'
    // of if any character is not a digit, it is not an integer
    if (!(isdigit(s[ i ]) || s[ 0 ] == '+' || s[ 0 ] == '-'))
    {
      return false;
    }
  }

  // if we made it this far we have an integer
  return true;
}

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

bool Compiler::isLiteral(string s) const // 10. LIT → INTEGER | BOOLEAN | 'not'
// BOOLEAN | '+' INTEGER | '-' INTEGER
{
  if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')
  {
    return true;
  }

  return false;
}

bool Compiler::isNonKeyId(string s) const
{
  if (!isInteger(s) && !isKeyword(s) && !isSpecialSymbol(s[ 0 ]))
  {
    return true;
  }

  return false;
}

/** ACTION ROUTINES **/

void Compiler::insert(string externalName, // create symbol table entry for each identifier in list
                      storeTypes inType,   // of external names
                      modes inMode,        // Multiple inserted names are illegal
                      string inValue, allocation inAlloc, int inUnits)
{
  string name;
  uint i = 0;

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
      if (symbolTable.find(name) != symbolTable.end())
      {
        processError("symbol " + name + " is multiply defined");
      }
      else if (isKeyword(name) && !isBoolean(name))
      {
        processError("illegal use of " + name + " keyword"); // do we need to look out for booleans?
      }
      else
      {
        if (isupper(name[ 0 ]))
        {
          symbolTable.insert({name.substr(0, 15), SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
        }
        else if (name == "true")
        {
          symbolTable.insert({name.substr(0, 15), SymbolTableEntry("TRUE", inType, inMode, inValue, inAlloc, inUnits)});
        }
        else if (name == "false")
        {
          symbolTable.insert(
              {name.substr(0, 15), SymbolTableEntry("FALSE", inType, inMode, inValue, inAlloc, inUnits)});
        }
        else
        {
          symbolTable.insert({name.substr(0, 15),
                              SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
        }
      }
    }

    if (symbolTable.size() > 256)
    {
      processError("symbol table overflow");
    }
    if (i == externalName.length())
    {
      break;
    }
    i++;
  }
}

storeTypes Compiler::whichType(string name) // tells which data type a name has
{                                           // 9. TYPE → 'integer'
  storeTypes type;                          //		  → 'boolean'

  if (isLiteral(name))
  {
    if (isInteger(name))
    {
      type = INTEGER;
    }
    else if (isBoolean(name))
    {
      type = BOOLEAN;
    }
  }
  else // name is an identifier and hopefully a constant
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

string Compiler::whichValue(string name) // tells which value a name has
{
  string value;

  if (isLiteral(name))
  {
    if (name == "false")
    {
      value = "0";
    }
    else if (name == "true")
    {
      value = "-1";
    }
    else
      value = name;
  }
  else // name is an identifier and hopefully a constant
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

/** EMIT FUNCTIONS **/

void Compiler::code(string op, string operand1, string operand2)
{
  if (op == "program")
  {
    emitPrologue(operand1);
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
    emitAdditionCode(operand1, operand2);
  }

  else if (op == "-")
  {
    emitSubtractionCode(operand1, operand2);
  }
  else if (op == "neg")
  {
    emitNegationCode(operand1, operand2);
  }

  else if (op == ":=")
  {
    emitAssignCode(operand1, operand2);
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

  else if (op == "<=")
  {
    emitLessThanOrEqualToCode(operand1, operand2);
  }

  else if (op == ">")
  {
    emitGreaterThanCode(operand1, operand2);
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

  else if (op == "end")
  {
    emitEpilogue();
  }

  else
  {
    processError("compiler error: function code called with illegal arguments " + op);
  }
}

void Compiler::emit(string label, string instruction, string operands, string comment)
{
  // Turn on left justification in objectFile
  //   Output label in a field of width 8
  //   Output instruction in a field of width 8
  //   Output the operands in a field of width 24
  //   Output the comment
  objectFile << left << setw(8) << label;
  objectFile << setw(8) << instruction;
  objectFile << setw(24) << operands;
  objectFile << setw(8) << comment << '\n';
}

void Compiler::emitPrologue(string progName, string operand2)
{
  // Output identifying comments at beginning of objectFile
  //   Output the %INCLUDE directives
  //   emit("SECTION", ".text")
  //   emit("global", "_start", "", "; program" + progName)
  //   emit("_start:")

  time_t now = time(0);
  objectFile << "; Kangmin Kim, Jeff Caldwell       " << setw(8) << right << ctime(&now);
  objectFile << "%INCLUDE \"Along32.inc\"\n"
    << "%INCLUDE \"Macros_Along.inc\"\n\n";

  emit("SECTION", ".text");
  emit("global", "_start", "", "; program " + progName + "\n");
  emit("_start:");
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
  emit("", "Exit", "{0}\n");
  emitStorage();
}

void Compiler::emitStorage()
{
  map<string, SymbolTableEntry>::iterator i;
  // emit("SECTION", ".data")
  //   for those entries in the symbolTable that have
  //     an allocation of YES and a storage mode of CONSTANT
  //   {call emit to output a line to objectFile}
  //     emit("SECTION", ".bss")
  //     for those entries in the symbolTable that have
  //       an allocation of YES and a storage mode of VARIABLE
  //     {call emit to output a line to objectFile}

  emit("SECTION", ".data");

  for (i = symbolTable.begin(); i != symbolTable.end(); i++)
  {
    if (i->second.getAlloc() == YES && i->second.getMode() == CONSTANT)
    {
      emit(i->second.getInternalName(), "dd", i->second.getValue(), "; " + i->first);
    }
  }

  objectFile << "\n";
  emit("SECTION", ".bss");

  for (i = symbolTable.begin(); i != symbolTable.end(); i++)
  {
    if (i->second.getAlloc() == YES && i->second.getMode() == VARIABLE)
    {
      emit(i->second.getInternalName(), "resd", i->second.getValue(), "; " + i->first);
    }
  }
}

void Compiler::emitReadCode(string operand, string operand2)
{
  string name;
  uint i = 0;

  while (i < operand.length())
  {
    name = "";

    while (i < operand.length() && operand[ i ] != ',')
    {
      name = name + operand[ i ];
      i++;
    }

    if (!name.empty())
    {
      if (symbolTable.find(name) == symbolTable.end())
      {
        processError("symbol " + name + " is undefined");
      }

      if (symbolTable.at(name).getDataType() != INTEGER)
      {
        processError("Can't read variables of this type");
      }

      if (symbolTable.at(name).getMode() != VARIABLE)
      {
        processError("Attempting to read to a read-only location");
      }

      emit("", "call", "ReadInt", "; read int; value placed in eax");

      contentsOfAReg = symbolTable.at(name).getInternalName();

      emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
    }
  }
}

void Compiler::emitWriteCode(string operand, string operand2)
{
  string name;
  uint i = 0;

  while (i < operand.length())
  {
    name = "";

    while (i < operand.length() && operand[ i ] != ',')
    {
      name = name + operand[ i ];
      i++;
    }

    if (!name.empty())
    {
      if (symbolTable.find(name) == symbolTable.end())
      {
        processError("symbol " + name + " is undefined");
      }

      if (contentsOfAReg != name)
      {
        emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "] ", "; load " + name + " in eax ");
        contentsOfAReg = name;
      }

      if (symbolTable.at(name).getDataType() == INTEGER)
      {
        emit("", "call", "WriteInt", "; write int in eax to standard out");
      }
      else if (symbolTable.at(name).getDataType() == BOOLEAN)
      {
        emit("", "call", "WriteInt", "; write boolean in eax to standard out");
      }

      emit("", "call", "Crlf", "; write \\r\\n to standard out");
    }
  }
}

void Compiler::emitAssignCode(string operand1, string operand2) // op2 = op1
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
  {
    freeTemp();
  }
}

void Compiler::emitAdditionCode(string operand1, string operand2) // op2 + op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER ||
      symbolTable.at(operand2).getDataType() != INTEGER) // check both DataType is integer
  {
    processError("binary '+' requires integer operands");
  }

  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg &&
      symbolTable.at(operand2).getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; AReg = " + operand2 + " + " + operand1);
  }

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  pushOperand(contentsOfAReg);
}

void Compiler::emitSubtractionCode(string operand1, string operand2) // op2 - op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("illegal type: binary '-' requires integer operands");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }


  if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; AReg = " + operand2 + " - " + operand1);
  }

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  pushOperand(contentsOfAReg);
}

void Compiler::emitNegationCode(string operand1, string operand2)
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER)
  {
    processError("illegal type; must be integer");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);

    contentsOfAReg = symbolTable.at(operand1).getInternalName();
  }

  emit("", "neg", "eax", "; AReg = -AReg");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  pushOperand(contentsOfAReg);
}

void Compiler::emitNotCode(string operand1, string operand2) // !op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  if (symbolTable.at(operand1).getDataType() != BOOLEAN)
  {
    processError("illegal type");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }


  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);

    contentsOfAReg = symbolTable.at(operand1).getInternalName();
  }

  emit("", "not", "eax", "; AReg = !AReg");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 * op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("Illegal type");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    contentsOfAReg = "";
  }

  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]",
         "; AReg = " + operand2 + " * " + operand1);
  }

  else
  {
    emit("", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]",
         "; AReg = " + operand1 + " * " + operand2);
  }

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);
  pushOperand(contentsOfAReg);
}

void Compiler::emitDivisionCode(string operand1, string operand2) // op2 / op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("binary 'div' requires integer operands");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  // emit code to extend sign of dividend from the A register to edx:eax
  emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

  emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]",
       "; AReg = " + operand2 + " div " +
           operand1); // not sure if this is right. we will need to add a comment here as well

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();

  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  pushOperand(contentsOfAReg);
}

void Compiler::emitModuloCode(string operand1, string operand2) // op2 % op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("binary 'mod' requires integer operands");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

  emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]",
       "; AReg = " + operand2 + " div " + operand1);

  emit("", "xchg", "eax,edx", "; exchange quotient and remainder");

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();

  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  pushOperand(contentsOfAReg);
}

void Compiler::emitAndCode(string operand1, string operand2) // op2 && op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
  {
    processError("binary 'and' requires boolean operands");
  }
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "and", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; AReg = " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "and", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; AReg = " + operand1 + " and " + operand2);
  }

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();

  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitOrCode(string operand1, string operand2) // op2 || op1
{
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
  {
    processError("illegal type");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "or", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; AReg = " + operand2 + " or " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; AReg = " + operand1 + " or " + operand2);
  }

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitEqualityCode(string operand1, string operand2) // op2 == op1
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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; compare " + operand1 + " and " + operand2);
  }

  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  string secondLabel = getLabel();

  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  emit("." + secondLabel + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();

  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitInequalityCode(string operand1, string operand2) // op2 != op1
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
    processError("incompatible types for operator '<>'");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; compare " + operand1 + " and " + operand2);
  }

  string label_1 = getLabel(), label_2 = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
  }
  else
  {
    emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  emit("", "jmp", "." + label_2, "; unconditionally jump");

  emit("." + label_1 + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  emit("." + label_2 + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();

  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanCode(string operand1, string operand2) // op2 < op1
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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);

    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }

  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
  }

  string secondLabel = getLabel();

  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
  }

  emit("." + secondLabel + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; compare " + operand1 + " and " + operand2);
  }

  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  string secondLabel = getLabel();

  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  emit("." + secondLabel + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanCode(string operand1, string operand2) // op2 > op1
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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; compare " + operand1 + " and " + operand2);
  }

  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jg", "." + newLabel, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
  }

  string secondLabel = getLabel();

  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
  }

  emit("." + secondLabel + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {

    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");

    symbolTable.at(contentsOfAReg).setAlloc(YES);

    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() &&
      contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]",
         "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]",
         "; compare " + operand1 + " and " + operand2);
  }

  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
  }

  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  string secondLabel = getLabel();

  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  emit("." + secondLabel + ":");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  pushOperand(contentsOfAReg);
}

/** LEXER FUNCTIONS **/

string Compiler::nextToken() // returns the next token or end of file marker
{
  token = "";
  while (token == "")
  {
    {
      if (ch == '{')
      {
        while (nextChar() && ch != END_OF_FILE && ch != '}')
        {
          // this is a comment
        } // empty body, skip

        if (ch == END_OF_FILE)
        {
          processError("unexpected end of file");
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

        while (nextChar() && ((islower(ch) || isdigit(ch) || ch == '_') && ch != END_OF_FILE))
          token += ch;

        if (ch == END_OF_FILE)
          processError("unexpected end of file");
      }

      else if (isdigit(ch))
      {
        token = ch;

        while (isdigit(nextChar()) && ch != END_OF_FILE && !isSpecialSymbol(ch))
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
  }
  token = token.substr(0, 15);

  // cout << "Token: " << token << "\n";

  return token;
}

char Compiler::nextChar() // returns the next character or end of file marker
{
  // get the next character
  sourceFile.get(ch);

  // set prevChar at the beginning so we know when to start
  // a new line number. Needs to be static because this fn is
  // called from outside repeatedly.
  static char prevChar = '\n';

  if (!sourceFile) // works like if(sourcefile.eof())
  {                // see Dr Motl's lecture from 11/01
    ch = END_OF_FILE;
    return ch;
  }

  else
  {
    if (prevChar == '\n')
    {
      // we have a new line, increment the
      // line number and add it to the left
      // with a separator
      listingFile << setw(5) << ++lineNo << '|';
    }
    // add the current character
    listingFile << ch;
  }

  // now set the previous character (static!) to the
  // current character. When we reach a new line again,
  // it will be reset to '\n'
  prevChar = ch;

  // done
  return ch;
}
