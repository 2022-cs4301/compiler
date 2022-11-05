#include "stage0.h"

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    cerr << "Usage: "
      << argv[ 0 ]
      << " SourceFileName ListingFileName "
      << "ObjectFileName\n";

    exit(EXIT_FAILURE);
  }

  Compiler compiler(argv);

  cout << argv[ 0 ] << "\n";
  compiler.createListingHeader();
  compiler.parser();
  compiler.createListingTrailer();

  return 0;
}