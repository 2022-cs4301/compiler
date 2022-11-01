#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stage0.h>
#include <stage0main.C>
#include <ctime>
#include <cstring>

Compiler::Compiler (char **argv)
{
	sourcFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

Compiler::~Compiler()
{
	souceFile.close();
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