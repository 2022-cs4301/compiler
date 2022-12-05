// Kangmin Kim and Jeff Caldwell
// CS 4301
// Compiler - Stage2

#include "stage2.h"
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

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

void Compiler::createListingHeader() // destructor
{
  // print "STAGE1:", name(s), DATE, TIME OF DAY
  // print "LINE NO:", "SOURCE STATEMENT"
  time_t now = time(0);

  // line numbers and source statements should be aligned under the headings
  listingFile << "STAGE2:  "
    << "Kangmin Kim, Jeff Caldwell       " << ctime(&now) << "\n";
  listingFile << "LINE NO."
    << "               SOURCE STATEMENT\n\n";
}

void Compiler::parser()
{
  nextChar(); // returns the next character or end of file marker

  if (nextToken() != "program") // nextToken() returns next token or EOF
  {
    processError("keyword \"program\" expected");
  }

  prog();
}

void Compiler::createListingTrailer()
{
  listingFile << "\nCOMPILATION TERMINATED" << setw(6) << "" << right << errorCount
    << (errorCount != 1 ? " ERRORS " : " ERROR ") << "ENCOUNTERED\n";
}

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

  // END_OF_FILE = '$'
  if (token[ 0 ] != END_OF_FILE)
  {
    processError("no text may follow \"end\"");
  }
}


void Compiler::progStmt()
{
  string x;

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


void Compiler::consts()
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

void Compiler::vars()
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

void Compiler::beginEndStmt()
{
  if (token != "begin")
  {
    processError("keyword \"begin\" expected");
  }

  nextToken();

  execStmts();

  if (token != "end")
  {
    processError("keyword \"end\" expected");
  }

  nextToken();

  if (token == ".")
  {
    code("end", ".");
    token[ 0 ] = '$';
  }
  else if (token == ";")
  {
    // do nothing
  }
  else
  {
    processError("'.' or ';' expected after \"end\"");
  }
}


void Compiler::execStmts()
{
  if (isNonKeyId(token) ||
      token == "read" ||
      token == "write" ||
      token == "begin" ||
      token == "if" ||
      token == "while" ||
      token == "repeat" ||
      token == ";")
  {
    execStmt();
    execStmts();
  }


  else if (token == "end");

  else if (token == "until");

  else
  {
    processError("one of \";\", \"begin\", \"if\", \"read\", \"repeat\", \"while\", \"write\", \"end\", or \"until\" expected");		//error here
  }
}

void Compiler::execStmt()
{
  if (isNonKeyId(token))
  {
    assignStmt();
  }
  else if (token == "begin")
  {
    beginEndStmt();
  }
  else if (token == "end")
  {
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


void Compiler::assignStmt()
{
  string op2, op1;
  if (!isNonKeyId(token))
  {
    processError("non - keyword identifier expected");
  }

  // Token must be defined
  if (symbolTable.count(token) == 0)
  {
    processError("reference to undefined variable");
  }

  pushOperand(token);

  nextToken();

  if (token != ":=")
  {
    processError("':=' expected; found " + token);
  }

  // token is ":="
  pushOperator(token);
  nextToken();

  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token) &&
      token != ";")
  {
    processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
  }

  express();

  op2 = popOperand();
  op1 = popOperand();
  code(popOperator(), op2, op1);
}


void Compiler::readStmt()
{
  // read list
  string list;
  // list item
  string listItem = "";
  // list counter
  uint i;

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
  string x;
  if (token != "write")
  {
    processError("write expected; found " + token);
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
    processError("',' or ')' expected; found " + token);
  }

  code("write", x);
  nextToken();

  if (token != ";")
  {
    processError("';' expected");
  }
}

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

  if (isNonKeyId(token) ||
      token == "read" ||
      token == "write" ||
      token == "if" ||
      token == "while" ||
      token == "repeat" ||
      token == ";" ||
      token == "begin")
  {
    execStmt();
  }

  elsePt();
}

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
  else if (isNonKeyId(token) ||
           token == "end" ||
           token == "write" ||
           token == "read" ||
           token == "repeat" ||
           token == "if" ||
           token == "while" ||
           token == "begin" ||
           token == "until" ||
           token == ";")
  {
    code("post_if", popOperand());
  }
  else
  {
    processError("illegal character");
  }
}

void Compiler::whileStmt() // stage 2, production 5
{
  if (token != "while")
  {
    processError("received " + token + " expected while");
  }

  code("while");
  nextToken();
  express();

  if (token != "do")
  {
    processError("received " + token + " expected do");
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

  if (!isNonKeyId(token) &&
      token != "read" &&
      token != "write" &&
      token != "end" &&
      token != "write" &&
      token != "read" &&
      token != "repeat" &&
      token != "if" &&
      token != "while" &&
      token != "begin" &&
      token != "until" &&
      token != ";")
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



void Compiler::nullStmt() // stage 2, production 7
{
  if (token != ";")
  {
    processError("received " + token + " expected ;");
  }
  nextToken();
}


void Compiler::express()	//stage 1 production 9
{
  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non - keyword identifier or integer expected" + token);
  }

  term();

  if (token == "<>" ||
      token == "=" ||
      token == "<=" ||
      token == ">=" ||
      token == "<" ||
      token == ">")
  {
    expresses();
  }
}


void Compiler::expresses()	//stage 1 production 10
{
  string op2, op1;

  if (token != "=" &&
      token != "<>" &&
      token != "<=" &&
      token != ">=" &&
      token != "<" &&
      token != ">")
  {
    processError("\"=\", \"<>\", \"<=\", \">=\", \"<\", or \">\" expected");
  }

  pushOperator(token);
  nextToken();

  //error checking here
  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  term();

  op2 = popOperand();
  op1 = popOperand();

  code(popOperator(), op2, op1);

  if (token == "=" ||
      token == "<>" ||
      token == "<=" ||
      token == ">=" ||
      token == "<" ||
      token == ">")
  {
    expresses();
  }
}


void Compiler::term()
{
  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  factor();

  if (token == "-" || token == "+" || token == "or")
  {

    terms();
  }
}



void Compiler::terms() //stage 1 production 12
{
  string op1, op2;

  if (token != "+" && token != "-" && token != "or")
  {
    processError("\"+\", \"-\", or \"or\" expected");
  }

  pushOperator(token);
  nextToken();

  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token))
  {
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  factor();
  op2 = popOperand();
  op1 = popOperand();

  code(popOperator(), op2, op1);

  if (token == "+" || token == "-" || token == "or")
  {
    terms();
  }
}

void Compiler::factor()
{
  if (token != "not" &&
      token != "true" &&
      token != "false" &&
      token != "(" &&
      token != "+" &&
      token != "-" &&
      !isInteger(token) &&
      !isNonKeyId(token))
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
  else if (isNonKeyId(token) ||
           token == "<>" ||
           token == "=" ||
           token == "<=" ||
           token == ">=" ||
           token == "<" ||
           token == ">" ||
           token == ")" ||
           token == ";" ||
           token == "-" ||
           token == "+" ||
           token == "or" ||
           token == "begin" ||
           token == "do" ||
           token == "then")
  {

  }

  else
  {
    processError("expected '(', integer, or non_key_id" + token);
  }
}


void Compiler::factors()
{
  string op1, op2;

  if (token != "*" && token != "div" && token != "mod" && token != "and")
  {
    processError("\"*\", \"div\", \"mod\", or \"and\" expected");
  }

  pushOperator(token);
  nextToken();

  if (token != "not" &&
      token != "(" &&
      !isInteger(token) &&
      !isNonKeyId(token) &&
      token != "+" &&
      token != "-" &&
      token != "true" &&
      token != "false")
  {
    processError("expected '(', integer, or non-keyword id " + token);
  }

  part();

  op2 = popOperand();
  op1 = popOperand();

  code(popOperator(), op2, op1);

  if (token == "*" || token == "div" || token == "mod" || token == "and")
  {
    factors();
  }
}


void Compiler::part()
{
  if (token == "not")
  {
    nextToken();

    if (token == "(")
    {
      nextToken();

      if (token != "not" &&
          token != "true" &&
          token != "false" &&
          token != "(" &&
          token != "+" &&
          token != "-" &&
          !isInteger(token) &&
          !isNonKeyId(token))
      {
        processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
      }

      express();

      if (token != ")")
      {
        processError(") expected; found " + token);
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
        processError("expected ')'; found " + token);
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
        processError("expected ')'; found " + token);
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

    if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
    {
      processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
    }

    express();

    if (token != ")")
    {
      processError(") expected; found " + token);
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

void Compiler::constStmts()
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
    nextToken();

    if (isBoolean(token) || isNonKeyId(token))
    {
      if (token == "true")
      {
        y = "false";
      }
      else if (token == "false")
      {
        y = "true";
      }
      else if (isNonKeyId(token))
      {
        if (symbolTable.at(token).getDataType() != BOOLEAN)
        {
          processError("boolean expected after \"not\"");
        }
        else
        {
          y = (symbolTable.at(token).getValue() == "0") ? "true" : "false";
        }
      }
    }
    else
    {
      processError("boolean expected after \"not\"");
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

void Compiler::varStmts()
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


string Compiler::ids()
{
  string temp, tempString;

  if (!(isNonKeyId(token)))
  {
    processError("non-keyword identifier expected");
  }

  tempString = token;
  temp = token;

  if (nextToken() == ",")
  {
    if (!(isNonKeyId(nextToken())))
    {
      processError("non-keyword identifier expected");
    }

    tempString = temp + "," + ids();
  }

  return tempString;
}


bool Compiler::isKeyword(string s) const
{
  if (s == "program" || s == "const" || s == "var" || s == "integer" || s == "boolean"
  || s == "begin" || s == "end" || s == "true" || s == "false" || s == "not" || s == "mod"
  || s == "div" || s == "and" || s == "or" || s == "read" || s == "write" || s == "if"
  || s == "then" || s == "else" || s == "while" || s == "do" || s == "repeat" || s == "until")
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool Compiler::isSpecialSymbol(char c) const
{
  if (c == ':' || c == ',' || c == ';' || c == '=' || c == '+' || c == '-' || c == '.' || c == '*' || c == '<' || c == '>' || c == '(' || c == ')')
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool Compiler::isNonKeyId(string s) const
{
  if (!isKeyword(s) && !isInteger(s) && !isSpecialSymbol(s[ 0 ]))
  {
    return true;
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



bool Compiler::isLiteral(string s) const
{
  if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')// s[0] == "+" || s[0] == "-")
  {
    return true;
  }
  else
  {
    return false;
  }
}



string Compiler::genInternalName(storeTypes storeType) const
{                                 // this function returns initialized alphabet characters with numbers
  static int I = 0, B = 0, U = 0; // integer, boolean, unknown

  string internalName;

  if (storeType == PROG_NAME)
  {
    internalName = "P0";
  }
  else if (storeType == INTEGER)
  {
    internalName = "I" + to_string(I);
    I++;
  }

  else if (storeType == BOOLEAN)
  {
    internalName = "B" + to_string(B);
    B++;
  }
  else if (storeType == UNKNOWN)
  {
    internalName = "U" + to_string(U);
    U++;
  }

  return internalName;
}



void Compiler::insert(string externalName,
                      storeTypes inType,
                      modes inMode,
                      string inValue,
                      allocation inAlloc,
                      int inUnits)
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



storeTypes Compiler::whichType(string name)
{
  storeTypes type;

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



string Compiler::whichValue(string name)
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



void Compiler::code(string op, string operand1, string operand2)	//Calls emitPrologue when our op is "program" and calls emitEpilogue when our op is "end"
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
    emitReadCode(operand1, "");
  }

  else if (op == "write")
  {
    emitWriteCode(operand1, "");
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
    emitNegationCode(operand1, op);
  }

  else if (op == "not")
  {
    emitNotCode(operand1, op);
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

  else if (op == ":=")
  {
    emitAssignCode(operand1, operand2);
  }

  else if (op == "then")
  {
    emitThenCode(operand1);
  }

  else if (op == "else")
  {
    emitElseCode(operand1);
  }

  else if (op == "while")
  {
    emitWhileCode();
  }

  else if (op == "do")
  {
    emitDoCode(operand1);
  }
  else if (op == "repeat")
  {
    emitRepeatCode();
  }

  else if (op == "until")
  {
    emitUntilCode(operand1, operand2);
  }
  else if (op == "post_if")
  {
    emitPostIfCode(operand1);
  }

  else if (op == "post_while")
  {
    emitPostWhileCode(operand1, operand2);
  }
  else
  {
    processError("compiler error; function code called with illegal arguments" + op);
  }
}


void Compiler::emit(string label, string instruction, string operands, string comment)
{
  objectFile << left << setw(8) << label;
  objectFile << setw(8) << instruction;
  objectFile << setw(24) << operands;
  objectFile << setw(8) << comment << '\n';
}



void Compiler::emitPrologue(string progName, string operand2)
{
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
  emit("", "Exit", "{0}");
  objectFile << endl;
  emitStorage();
}



void Compiler::emitStorage()
{

  map<string, SymbolTableEntry>::iterator itr = symbolTable.begin();

  emit("SECTION", ".data");

  for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr)
  {

    if (itr->second.getAlloc() == YES)
    {
      if (itr->second.getMode() == CONSTANT)
      {
        emit(itr->second.getInternalName(), "dd", itr->second.getValue(), "; " + itr->first);
      }
    }
  }

  objectFile << "\n";
  emit("SECTION", ".bss");

  for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr)
  {

    if (itr->second.getAlloc() == YES)
    {
      if (itr->second.getMode() == VARIABLE)
      {
        emit(itr->second.getInternalName(), "resd", itr->second.getValue(), "; " + itr->first);
      }
    }
  }
}



void Compiler::emitWriteCode(string operand, string operand2)
{
  string name;

  for (uint i = 0; i < operand.size(); ++i)
  {

    if (operand[ i ] != ',' && i < operand.size()) // get operand string and assign it to name
    {
      name += operand[ i ];
      continue;
    }
    if (name.length() > 15)
    {
      name = name.substr(0, 15);
    }

    if (name != "") // if name is not empty
    {
      if (symbolTable.count(name) == 0) // if name is not defined in symbolTable
      {
        processError("reference to undefined symbol " + name); // processError
      }
      if (symbolTable.at(name).getInternalName() != contentsOfAReg) // if name != AReg;
      {
        emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]",
             "; load " + name + " in eax");                      // emit "mov eax, [name]; load name in eax"
        contentsOfAReg = symbolTable.at(name).getInternalName(); // AReg = name
      }
      emit("", "call", "WriteInt",
           "; write int in eax to standard out"); // emit "call WriteInt; write int in eax to standard out"

      emit("", "call", "Crlf", "; write \\r\\n to standard out"); // emit "call Crlf; write \r\n to standard out"
    }
    name = ""; // deassign name
  }

  if (symbolTable.count(name) == 0) // if name is not defined in symbolTable
  {
    processError("reference to undefined symbol " + name);
  }
  if (symbolTable.at(name).getInternalName() != contentsOfAReg) // if name != AReg
  {
    emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]",
         "; load " + name + " in eax");                      // emit "mov eax,[name]; load name in eax"
    contentsOfAReg = symbolTable.at(name).getInternalName(); // AReg = name
  }
  emit("", "call", "WriteInt",
       "; write int in eax to standard out"); // emit "call WriteInt; write int in eax to standard out"

  emit("", "call", "Crlf", "; write \\r\\n to standard out"); // emit "call Crlf; write \r\n to standard out"
}



void Compiler::emitWriteCode(string operand, string)
{
  string name;
  static bool definedStorage = false;
  unsigned int size = operand.size();

  for (unsigned int loopC = 0; loopC < size; ++loopC)
  {
    if (operand[ loopC ] != ',' && loopC < size)
    {
      name += operand[ loopC ];
      continue;
    }

    if (name != "")
    {
      if (symbolTable.count(name) == 0)
      {
        processError("reference to undefined symbol " + name);
      }

      if (symbolTable.at(name).getInternalName() != contentsOfAReg)
      {
        emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
        contentsOfAReg = symbolTable.at(name).getInternalName();
      }

      if (symbolTable.at(name).getDataType() == storeTypes::INTEGER)
      {
        emit("", "call", "WriteInt", "; write int in eax to standard out");
      }

      else
      {
        //emit code to compare the A register to 0
        emit("", "cmp", "eax,0", "; compare to 0");
        //acquire a new label Ln
        string firstL = getLabel();
        //emit code to jump if equal to the acquired label Ln
        emit("", "je", "." + firstL, "; jump if equal to print FALSE");
        //emit code to load address of TRUE literal in the D register
        emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
        //acquire a second label L(n + 1)
        string secondL = getLabel();
        //emit code to unconditionally jump to label L(n + 1)
        emit("", "jmp", "." + secondL, "; unconditionally jump to ." + secondL);
        //emit code to label the next line with the first acquired label Ln
        emit("." + firstL + ":");
        //emit code to load address of FALSE literal in the D register
        emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
        //emit code to label the next line with the second acquired label L(n + 1)
        emit("." + secondL + ":");
        //emit code to call the Irvine WriteString function
        emit("", "call", "WriteString", "; write string to standard out");

        //if static variable definedStorage is false
        if (definedStorage == false)
        {
          //set definedStorage to true
          definedStorage = true;
          //output an endl to objectFile
          objectFile << endl;
          //emit code to begin a .data SECTION
          emit("SECTION", ".data");
          //emit code to create label TRUELIT, instruction db, operands 'TRUE',0
          emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
          //emit code to create label FALSELIT, instruction db, operands 'FALSE',0
          emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
          //output an endl to objectFile
          objectFile << endl;
          //emit code to resume .text SECTION
          emit("SECTION", ".text");
        }

      }

      //emit code to call the Irvine Crlf function
      emit("", "call", "Crlf", "; write \\r\\n to standard out");
    }
    name = "";
  }
  //check if anything is left inside name
  if (name != "")
  {
    //if name is not in symbol table
    if (symbolTable.count(name) == 0)
    {
      //processError(reference to undefined symbol)
      processError("reference to undefined symbol " + name);
    }

    //if name is not in the A register
    if (symbolTable.at(name).getInternalName() != contentsOfAReg)
    {
      //emit the code to load name in the A register
      emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
      //set the contentsOfAReg = name
      contentsOfAReg = symbolTable.at(name).getInternalName();
    }

    //if data type of name is INTEGER
    if (symbolTable.at(name).getDataType() == storeTypes::INTEGER)
    {
      //emit code to call the Irvine WriteInt function
      emit("", "call", "WriteInt", "; write int in eax to standard out");
    }

    //data type is BOOLEAN
    else
    {
      //emit code to compare the A register to 0
      emit("", "cmp", "eax,0", "; compare to 0");
      //acquire a new label Ln
      string firstL = getLabel();
      //emit code to jump if equal to the acquired label Ln
      emit("", "je", "." + firstL, "; jump if equal to print FALSE");
      //emit code to load address of TRUE literal in the D register
      emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
      //acquire a second label L(n + 1)
      string secondL = getLabel();
      //emit code to unconditionally jump to label L(n + 1)
      emit("", "jmp", "." + secondL, "; unconditionally jump to ." + secondL);
      //emit code to label the next line with the first acquired label Ln
      emit("." + firstL + ":");
      //emit code to load address of FALSE literal in the D register
      emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
      //emit code to label the next line with the second acquired label L(n + 1)
      emit("." + secondL + ":");
      //emit code to call the Irvine WriteString function
      emit("", "call", "WriteString", "; write string to standard out");

      //if static variable definedStorage is false
      if (definedStorage == false)
      {
        //set definedStorage to true
        definedStorage = true;
        //output an endl to objectFile
        objectFile << endl;
        //emit code to begin a .data SECTION
        emit("SECTION", ".data");
        //emit code to create label TRUELIT, instruction db, operands 'TRUE',0
        emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
        //emit code to create label FALSELIT, instruction db, operands 'FALSE',0
        emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
        //output an endl to objectFile
        objectFile << endl;
        //emit code to resume .text SECTION
        emit("SECTION", ".text");
      }
    }

    //emit code to call the Irvine Crlf function
    emit("", "call", "Crlf", "; write \\r\\n to standard out");
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



void Compiler::emitAdditionCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  // check that neither operand is empty
  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not integer
  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("binary '+' requires integer operands");
  }

  //if the A Register holds a temp not operand1 nor operand2
  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
  {
    //emit code to store that temp into memory (store contentsofareg? - Z)
    //store contentsofAReg into eax by emitting assembly code
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");		//do we need commas in here?

    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);

    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand1 nor operand2
  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if neither operand is in the A register then
  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);		//CHECK THIS

    // set A reg == operand 2 
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    //emit code to perform register-memory addition with operand 1
    emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
  }
  else
  {
    //emit code to perform register-memory addition with operand 2
    emit("", "add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
  }

  //deassign all temporaries involved in the addition and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to integer
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitSubtractionCode(string operand1, string operand2) // op2 - op1
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  // check that neither operand is empty
  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not integer
  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("illegal type. binary '-' requires integer operands");
  }

  //if the A Register holds a temp not operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory (store contentsofareg? - Z)
    //store contentsofAReg into eax by emitting assembly code
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand1 nor operand2
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    contentsOfAReg = "";
  }

  //if operand2 is NOT in the A register then
  if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    // A Reg == operand2
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform register-memory subtraction with operand1
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);
  }

  //deassign all temporaries involved in the addition and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to integer
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitMultiplicationCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not integer
  //processError(illegal type)
  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("Illegal type");
  }

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

  // if the A register holds a non-temp not operand2 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    contentsOfAReg = "";
  }


  // if operand2 is not in the A register
  // emit instruction to do a register-memory load of operand2 into the A register
  if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }


  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
  }

  else
  {
    emit("", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);
  }

  // deassign all temporaries involved in the addition and free those names for reuse
  // A Register = next available temporary name and change type of its symbol table entry to integer
  // push the name of the result onto operandStk
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



void Compiler::emitDivisionCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not integer
  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("binary 'div' requires integer operands");
  }

  //if the A Register holds a temp not operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if operand2 is not in the A register
  if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
  {
    //emit instruction to do a register-memory load of operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to extend sign of dividend from the A register to edx:eax
  emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

  //emit code to perform a register-memory division
  //emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
  emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);	//not sure if this is right. we will need to add a comment here as well

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to integer	(this needs to be looked at further)
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  //push the name of the result onto operandStk	(this needs to be looked at further)
  pushOperand(contentsOfAReg);
}



void Compiler::emitModuloCode(string operand1, string operand2)
{

  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not integer
  if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
  {
    processError("binary 'mod' requires integer operands");
  }

  //if the A Register holds a temp not operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if operand2 is not in the A register
  if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
  {
    //emit instruction to do a register-memory load of operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to extend sign of dividend from the A register to edx:eax
  emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

  //emit code to perform a register-memory division
  emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);	//not sure if this is right. we will need to add a comment here as well
  emit("", "xchg", "eax,edx", "; exchange quotient and remainder");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to integer	(this needs to be looked at further)
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  //push the name of the result onto operandStk	(this needs to be looked at further)
  pushOperand(contentsOfAReg);
}



void Compiler::emitNegationCode(string operand1, string)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  //if type of either operand is not boolean
  if (symbolTable.at(operand1).getDataType() != INTEGER)
  {
    //processError(illegal type)
    processError("illegal type");
  }
  //if the A Register holds a temp not operand1
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }
  //if the A register holds a non-temp not operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //emit code to load operand1 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
    contentsOfAReg = symbolTable.at(operand1).getInternalName();
  }

  //emit code to perform register-memory NOT
  emit("", "neg", "eax", "; AReg = -AReg");

  //deassign all temporaries involved in the and operation and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to INTEGER
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(INTEGER);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitNotCode(string operand1, string)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  //if type of either operand is not boolean
  if (symbolTable.at(operand1).getDataType() != BOOLEAN)
  {
    //processError(illegal type)
    processError("illegal type");
  }
  //if the A Register holds a temp not operand1
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }
  //if the A register holds a non-temp not operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    //emit code to load operand1 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
    contentsOfAReg = symbolTable.at(operand1).getInternalName();
  }

  //emit code to perform register-memory NOT
  emit("", "not", "eax", "; AReg = !AReg");

  //deassign all temporaries involved in the and operation and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitAndCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not boolean
  if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
  {
    //processError(illegal type)
    processError("binary 'and' requires boolean operands");
  }
  //if the A Register holds a temp not operand1 nor operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }
  //if the A register holds a non-temp not operand1 nor operand2 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform register-memory and
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "and", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "and", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);
  }

  //deassign all temporaries involved in the and operation and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitOrCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if type of either operand is not boolean
  if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
  {
    //processError(illegal type)
    processError("illegal type");
  }
  //if the A Register holds a temp not operand1 nor operand2
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for the temp in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }
  //if the A register holds a non-temp not operand1 nor operand2 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  //if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform register-memory OR
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "or", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " or " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " or " + operand2);
  }

  //deassign all temporaries involved in the and operation and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitEqualityCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if types of operands are not the same
  if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
  {
    processError("incompatible types");
  }

  //if the A Register holds a temp not operand1 nor operand2 then 
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2 nor operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  // if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform a register-memory compare
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
  }

  //emit code to jump if equal to the next available Ln (call getLabel)
  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
  }

  //emit code to load FALSE into the A register
  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  //insert FALSE in symbol table with value 0 and external name false
  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  string secondLabel = getLabel();
  //emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");
  //emit code to load TRUE into A register
  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  //insert TRUE in symbol table with value -1 and external name true
  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  //emit code to label the next instruction with the second acquired label L(n+1)
  emit("." + secondLabel + ":");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitInequalityCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }
  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if types of operands are not the same
  if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
  {
    processError("incompatible types for operator '<>'");
  }

  //if the A Register holds a temp not operand1 nor operand2 then 
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2 nor operand1 then deassign it
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it 
    contentsOfAReg = "";
  }

  // if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    ////emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform a register-memory compare
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }
  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
  }

  //emit code to jump if equal to the next available Ln (call getLabel)
  string label_1 = getLabel(), label_2 = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
  }
  else
  {
    emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
  }

  //emit code to load FALSE into the A register
  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  //insert FALSE in symbol table with value 0 and external name false
  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
    symbolTable.at("false").setInternalName("FALSE");
  }

  //emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
  emit("", "jmp", "." + label_2, "; unconditionally jump");

  emit("." + label_1 + ":");
  //emit code to load TRUE into A register
  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  //insert TRUE in symbol table with value -1 and external name true
  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
    symbolTable.at("true").setInternalName("TRUE");
  }

  //emit code to label the next instruction with the second acquired label L(n+1)
  emit("." + label_2 + ":");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }
  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}


void Compiler::emitLessThanCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if types of operands are not the same
  if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
  {
    processError("incompatible types");
  }

  //if the A Register holds a temp not operand1 nor operand2 then 
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2 nor operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  // if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform a register-memory compare
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }

  //emit code to jump if NOT equal to the next available Ln (call getLabel)
  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
  }

  //emit code to load FALSE into the A register
  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  //insert FALSE in symbol table with value 0 and external name false
  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
  }

  string secondLabel = getLabel();
  //emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");

  //emit code to load TRUE into A register
  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  //insert TRUE in symbol table with value -1 and external name true
  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
  }

  //emit code to label the next instruction with the second acquired label L(n+1)
  emit("." + secondLabel + ":");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if types of operands are not the same
  if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
  {
    processError("incompatible types");
  }

  //if the A Register holds a temp not operand1 nor operand2 then 
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2 nor operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  // if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform a register-memory compare
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
  }

  //emit code to jump if NOT equal to the next available Ln (call getLabel)
  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
  }

  //emit code to load FALSE into the A register
  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  //insert FALSE in symbol table with value 0 and external name false
  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
  }

  string secondLabel = getLabel();
  //emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");
  //emit code to load TRUE into A register
  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  //insert TRUE in symbol table with value -1 and external name true
  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
  }

  //emit code to label the next instruction with the second acquired label L(n+1)
  emit("." + secondLabel + ":");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



void Compiler::emitGreaterThanCode(string operand1, string operand2)
{
  // check that neither operand is empty
  if (symbolTable.count(operand1) == 0)
  {
    processError("reference to undefined symbol " + operand1);
  }

  else if (symbolTable.count(operand2) == 0)
  {
    processError("reference to undefined symbol " + operand2);
  }

  //if types of operands are not the same
  if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
  {
    processError("incompatible types");
  }

  //if the A Register holds a temp not operand1 nor operand2 then 
  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to store that temp into memory
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    //change the allocate entry for it in the symbol table to yes
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    //deassign it
    contentsOfAReg = "";
  }

  //if the A register holds a non-temp not operand2 nor operand1 then deassign it
  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //deassign it
    contentsOfAReg = "";
  }

  // if neither operand is in the A register then
  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    //emit code to load operand2 into the A register
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  //emit code to perform a register-memory compare
  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
  }

  //emit code to jump if NOT equal to the next available Ln (call getLabel)
  string newLabel = getLabel();

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "jg", "." + newLabel, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");
  }

  //emit code to load FALSE into the A register
  emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

  //insert FALSE in symbol table with value 0 and external name false
  if (symbolTable.count("false") == 0)
  {
    insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
  }

  string secondLabel = getLabel();
  //emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
  emit("", "jmp", "." + secondLabel, "; unconditionally jump");

  emit("." + newLabel + ":");
  //emit code to load TRUE into A register
  emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

  //insert TRUE in symbol table with value -1 and external name true
  if (symbolTable.count("true") == 0)
  {
    insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
  }

  //emit code to label the next instruction with the second acquired label L(n+1)
  emit("." + secondLabel + ":");

  //deassign all temporaries involved and free those names for reuse
  if (isTemporary(operand1))
  {
    freeTemp();
  }

  if (isTemporary(operand2))
  {
    freeTemp();
  }

  //A Register = next available temporary name and change type of its symbol table entry to boolean
  contentsOfAReg = getTemp();
  symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

  //push the name of the result onto operandStk
  pushOperand(contentsOfAReg);
}



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
    processError("incompatible types");
  }

  if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
    symbolTable.at(contentsOfAReg).setAlloc(YES);
    contentsOfAReg = "";
  }

  if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    contentsOfAReg = "";
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
    contentsOfAReg = symbolTable.at(operand2).getInternalName();
  }

  if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
  }

  else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
  {
    emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
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



void Compiler::emitThenCode(string operand1, string)
{
  string tempLabel;

  if (symbolTable.at(operand1).getDataType() != BOOLEAN)
  {
    processError("the predicate of \"if\" must be of type BOOLEAN");
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
  {
    freeTemp();
  }

  contentsOfAReg = "";
}


void Compiler::emitElseCode(string operand1, string)
{
  string tempLabel;

  tempLabel = getLabel();

  emit("", "jmp", "." + tempLabel, "; jump to end if");

  emit("." + operand1 + ":", "", "", "; else");

  pushOperand(tempLabel);

  contentsOfAReg = "";
}


void Compiler::emitPostIfCode(string operand1, string)
{
  emit("." + operand1 + ":", "", "", "; end if");

  contentsOfAReg = "";
}


void Compiler::emitWhileCode(string, string)
{
  string tempLabel;

  tempLabel = getLabel();

  emit("." + tempLabel + ":", "", "", "; while");

  pushOperand(tempLabel);

  contentsOfAReg = "";
}


void Compiler::emitDoCode(string operand1, string)
{
  string tempLabel;

  if (symbolTable.at(operand1).getDataType() != BOOLEAN)
  {
    processError("while predicate must be of type boolean");
  }

  tempLabel = getLabel();

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);	// instruction to move operand1 to the A register
  }

  emit("", "cmp", "eax,0", "; compare eax to 0");	// instruction to compare the A register to zero (false)
  emit("", "je", "." + tempLabel, "; if " + operand1 + " is false then jump to end while");	// code to branch to tempLabel if the compare indicates equality

  pushOperand(tempLabel);

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  contentsOfAReg = "";
}


void Compiler::emitPostWhileCode(string operand1, string operand2)
{
  emit("", "jmp", "." + operand2, "; end while");

  emit("." + operand1 + ":", "", "", "");

  contentsOfAReg = "";
}


void Compiler::emitRepeatCode(string, string)
{

  string tempLabel;

  tempLabel = getLabel();

  emit("." + tempLabel + ":", "", "", "; repeat");

  pushOperand(tempLabel);

  contentsOfAReg = "";
}


void Compiler::emitUntilCode(string operand1, string operand2)
{
  if (symbolTable.at(operand1).getDataType() != BOOLEAN)
  {
    processError("the predicate of \"if\" must be of type BOOLEAN");
  }

  if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
  {
    emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + symbolTable.at(operand1).getInternalName());
    contentsOfAReg = symbolTable.at(operand1).getInternalName();
  }

  emit("", "cmp", "eax,0", "; compare eax to 0");

  emit("", "je", "." + operand2, "; until " + operand1 + " is true");

  if (isTemporary(operand1))
  {
    freeTemp();
  }

  contentsOfAReg = "";
}



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



char Compiler::nextChar()
{
  sourceFile.get(ch);

  static char prev = '\n';

  if (sourceFile.eof())
  {
    ch = END_OF_FILE;
  }

  else
  {
    if (prev == '\n')
    {
      lineNo += 1;
      listingFile << right << setw(5) << lineNo << '|';
    }

    listingFile << ch;
  }

  prev = ch;

  return ch;
}


void Compiler::pushOperator(string name)
{
  operatorStk.push(name);
}


void Compiler::pushOperand(string name) //push name onto operandStk
{
  if (symbolTable.count(name) == 0)
  {
    if (isInteger(name) || name == "true" || name == "false")
    {
      insert(name, whichType(name), CONSTANT, whichValue(name), YES, 1);
    }
  }

  operandStk.push(name);
}



string Compiler::popOperator() //pop name from operatorStk
{
  string top;

  if (!operatorStk.empty())
  {
    top = operatorStk.top();
    operatorStk.pop();
  }

  else
  {
    processError("compiler error; operator stack underflow");
  }

  return top;
}



string Compiler::popOperand() //pop name from operandStk
{
  string top;

  if (!operandStk.empty())
  {
    top = operandStk.top();
    operandStk.pop();
  }

  else
  {
    processError("compiler error; operand stack underflow");
  }

  return top;
}

void Compiler::freeTemp()
{
  currentTempNo--;
  if (currentTempNo < -1)
  {
    processError("compiler error, currentTempNo should be >= –1");
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
    symbolTable.at(temp).setInternalName(temp);
    maxTempNo++;
  }

  return temp;
}

string Compiler::getLabel()
{
  string label;
  static int count = 0;

  label = "L" + to_string(count);

  count++;

  return label;
}


bool Compiler::isTemporary(string s) const
{
  if (s[ 0 ] == 'T')
  {
    return true;
  }
  return false;
}
