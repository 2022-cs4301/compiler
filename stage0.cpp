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