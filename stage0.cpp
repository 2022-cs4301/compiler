#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <stage0.h>
#include <ctime>
#include <cstring>



Compiler::Compiler(char **argv) // constructor
{
  sourceFile.open(argv[ 1 ]); // open sourceFile using argv[1] (input from argv[1])
  listingFile.open(argv[ 2 ]);// open listingFile using argv[2] (generate a listing to argv[2])
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

  //line numbers and source statements should be aligned under the headings
  listingFile << "STAGE0:  " << "Jeff Caldwell, Kangmin Kim       " << ctime(&now) << "\n";
  listingFile << "LINE NO." << "              SOURCE STATEMENT\n\n";
}
// private: uint lineNo = 0; // line numbers for the listing

void Compiler::parser()
{
  nextChar(); //returns the next character or end of file marker

  //ch must be initialized to the first character of the source file
  if (nextToken() != "program") // string nextToken() returns the next token or END_OF_FILE marker
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
  //parser implements the grammar rules, calling first rule
  cout << "Made it to the end of parser!!\n";
}

void Compiler::createListingTrailer()
{
  listingFile << "\nCOMPILATION TERMINATED" << setw(6) << "" << right << errorCount << " ERRORS ENCOUNTERED\n";
  cout << "Made it to the end of listing trailer!!\n";
}
// private: uint errorCount = 0; // total number of errors encountered

void Compiler::processError(string error)
{
  listingFile << "\n" << "Error: Line " << lineNo << ": " << error << "\n";
  errorCount++;
  listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED\n";
  exit(0);
}
/*
  Note that insert() calls genInternalName(), a function that has one argument, the type of the name being
  inserted. genInternalName() returns a unique internal name each time it is called, a name that is known to
  be a valid symbolic name. As a visual aid, we use different forms of internal names for each data-type of interest.
  The general form is:
  dn
  where d denotes the data-type of the name ("I" for integer, "B" for boolean) and n is a non-negative integer
  starting at 0. The generated source code for 001.dat clearly shows the effects of calling
  genInternalName(). The compiler itself will also need to generate names to appear in the object code, but
  since the compiler is defining these itself, there is no need to convert these names into any other form. The
  external and internal forms will be the same. The code for insert() treats any external name beginning with
  an uppercase character as defined by the compiler.
*/
string Compiler::genInternalName(storeTypes stype) const
{
  static int I = 0;
  static int B = 0;
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



/** PRODUCTIONS **/

void Compiler::prog()           // stage 0, production 1
{								// 1. PROG → PROG_STMT CONSTS VARS BEGIN_END_STMT
  if (token != "program")
  {
    processError("keyword \"program\" expected");
  }

  progStmt();

  if (token == "const")
  {
    consts();
    cout << "Made it past consts() call\n";
  }

  if (token == "var")
  {
    vars();
    cout << "Made it past vars() call\n";
  }

  if (token != "begin")
  {
    processError("keyword \"begin\" expected");


  }
  cout << "Made it past beginEndStmt() call\n";

  beginEndStmt();

  if (token[ 0 ] != END_OF_FILE)
  {
    processError("no text may follow \"end\"");
  }
  cout << "Made it to end of prog()\n";
}

void Compiler::progStmt()       //2. PROG_STMT → 'program' NON_KEY_IDx ';'
{								                //   code(’program’, x); insert(x,PROG_NAME,CONSTANT,x,NO,0)
  string x, y;						        //   → 'program' NON_KEY_IDx ';'

  if (token != "program")
  {
    processError("keyword \"program\" expected");
  }

  x = nextToken();


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

void Compiler::consts() //3. CONSTS → 'const' CONST_STMTS
{						//   → ε
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

void Compiler::vars() //4. VARS → 'var' VAR_STMTS
{					  //   → ε
  if (token != "var")
  {
    cout << "Made it to error of vars\n";
    processError("keyword \"var\" expected");
  }

  if (!isNonKeyId(nextToken()))
  {
    processError("non-keyword identifier must follow \"var\"");
  }
  varStmts();
}

void Compiler::beginEndStmt() //5. BEGIN_END_STMT → 'begin' 'end' '.' code(‘end’, ‘.’)
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

void Compiler::constStmts() //6. CONST_STMTS → NON_KEY_IDx '='( NON_KEY_IDy | 'not' NON_KEY_IDy | LITy ) ';' 
{							//   insert(x,whichType(y),CONSTANT,whichValue(y),YES,1)
  string x, y;				//   ( CONST_STMTS | ε )

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
  if (!isNonKeyId(token))
  {
    processError("non-keyword identifier expected");
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
string Compiler::ids() //8. IDS → NON_KEY_ID ( ',' IDS | ε )
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
  try
  {
    stoi(s);
  }
  catch (invalid_argument&)
  {
    return false;
  }

  return true;
}

bool Compiler::isBoolean(string s) const // Jeff - (better test this one!) //11. BOOLEAN → 'true' | 'false'
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

bool Compiler::isLiteral(string s) const // Test me! - Jeff
{										 //10. LIT → INTEGER | BOOLEAN | 'not' BOOLEAN | '+' INTEGER | '-' INTEGER
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
  allocation inAlloc,  //create symbol table entry for each identifier in list of external names
  int inUnits)         //Multiple inserted names are illegal
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
          symbolTable.insert({name.substr(0, 15), SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
        }
        else
        {
          symbolTable.insert({name.substr(0.15), SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
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


// Needs testing! - Jeff
storeTypes Compiler::whichType(string name) //tells which data type a name has
{											//9. TYPE → 'integer'
  storeTypes type;							//		  → 'boolean'


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
    processError("compiler error: function code called with illegal arguments " + op);
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
  objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n\n";

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
  map<string, SymbolTableEntry>::iterator i = symbolTable.begin();
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
    if (i->second.getAlloc() == YES && i-> second.getMode() == VARIABLE)
    {
      emit(i->second.getInternalName(), "resd", i->second.getValue(), "; " + i->first);
    }
  }

}


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
        }	// empty body, skip

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

        while (nextChar() && ((islower(ch) || isdigit(ch) || ch == '_')
          && ch != END_OF_FILE))
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

  return token;
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
    {
      listingFile << setw(5) << ++lineNo << '|';
    }
    listingFile << ch;
  }

  prevChar = ch;
  return ch;
}

/*
#include <stage0.h>
int main(int argc, char **argv)
{
 if (argc != 4) // Check to see if pgm was invoked correctly
 {
 // No; print error msg and terminate program
 cerr << "Usage: " << argv[0] << " SourceFileName ListingFileName "
 << "ObjectFileName" << endl;
 exit(EXIT_FAILURE);
 }
 Compiler myCompiler(argv);
 myCompiler.createListingHeader();
 myCompiler.parser();
 myCompiler.createListingTrailer();
 return 0;
}
*/