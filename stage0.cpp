#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stage0.h>
#include <ctime>
#include <cstring>

# A test comment!

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

}

void Compiler::parser()
{

}

void Compiler::createListingTrailer()
{

}

void prog()           // stage 0, production 1
{

}

void progStmt()       // stage 0, production 2
{

}

void consts()         // stage 0, production 3
{

}