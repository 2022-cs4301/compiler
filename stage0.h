#ifndef STAGE0_H
#define STAGE0_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

const char END_OF_FILE = '$';

enum storeTypes { INTEGER, BOOLEAN, PROG_NAME };
enum modes { VARIABLE, CONSTANT };
enum allocation { YES, NO };

class SymbolTableEntry
{
public:
  SymbolTableEntry(string in, storeTypes st, modes m, string v, allocation a, int u)
  {
    setInternalName(in);
    setDataType(st);
    setMode(m);
    setValue(v);
    setAlloc(a);
    setUnits(u);
  }

  // GETTERS //
  string getInternalName() const
  {
    return internalName;
  }

  storeTypes getDataType() const
  {
    return dataType;
  }

  modes getMode() const
  {
    return mode;
  }

  string getValue() const
  {
    return value;
  }

  allocation getAlloc() const
  {
    return alloc;
  }

  int getUnits() const
  {
    return units;
  }

  void setInternalName(string s)
  {
    internalName = s;
  }

  void setDataType(storeTypes st)
  {
    dataType = st;
  }

  void setMode(modes m)
  {
    mode = m;
  }

  void setValue(string s)
  {
    value = s;
  }

  void setAlloc(allocation a)
  {
    alloc = a;
  }

  void setUnits(int i)
  {
    units = i;
  }

private:
  string internalName;
  storeTypes dataType;
  modes mode;
  string value;
  allocation alloc;
  int units;
};

class Compiler
{
public:
  Compiler(char **argv);
  ~Compiler();

  void createListingHeader();
  void parser();
  void createListingTrailer();

  // Implement grammar productions
  //Note/Jeff: I'm using full words instead of Motl's
  // abbreviated forms to make things easier to read.
  void prog();
  void progStmt();
  void consts();
  void vars();
  void beginEndStmt();
  void constStmts();
  void varStmts();
  string ids();

  // Helper functions for Pascallite lexicon
  bool isKeyword(string s) const;
  bool isSpecialSymbol(char c) const;
  bool isNonKeyId(string s) const;
  bool isInteger(string s) const;
  bool isBoolean(string s) const;
  bool isLiteral(string s) const;

  // Action routines
  void insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits);
  storeTypes whichType(string name);
  void code(string op, string operand1 = ", string operand2 = ");

  // Emit functions
  void emit(string label = "", string instruction = "", string operands = "", string comment = "");
  void emitPrologue(string progName, string = ""); // missing param name in pseudocode?
  void emitEpilogue(string = "", string = ""); // no param names in pseudocode! do they need to be named?
  void emitStorage(); // This one must be up to us!

  // Lexical routines
  char nextChar();    // Returns next character or END_OF_FILE marker
  string nextToken(); // Returns next token or END_OF_FILE marker

  // Other
  string generateInternalName(storeTypes stype) const;
  void processError(string error);

private:
  map<string, SymbolTableEntry> symbolTable;
  ifstream sourceFile;
  ofstream listingFile;
  string token;        // Next token
  char ch;             // Next character of source file
  uint errorCount = 0; // total number of errors encountered
  uint lineNo = 0;     // line numbers for the listing 
};

#endif