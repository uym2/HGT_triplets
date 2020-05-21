#include <iostream>
#include <fstream>
#include <cstring>

#include "int_stuff.h"
#include "TripletPairing.h"
#include "newick_parser.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif

void usage(char *programName) {
  std::cout << "Usage: " << programName << " <tree1> <tree2>" << std::endl; 
}

int main(int argc, char** argv) {
  if(argc == 1) {
    usage(argv[0]);
    return 0;
  }
  
  char *refTreeFile = argv[1];
  char *myTreeFile = argv[2];
  
  NewickParser parser;
  
  RootedTreeFactory *rFactory = new RootedTreeFactory();
  RootedTreeFactory *tFactory = new RootedTreeFactory();
  RootedTree *rRef = rFactory->getRootedTree();
  RootedTree *rTree = tFactory->getRootedTree();

  rRef->factory = rFactory;
  rTree->factory = tFactory;
  
  //cout << "Allocated memory" << endl;

  // read reference tree
  rRef->read_newick_file(refTreeFile);

  // read input tree
  rTree->read_newick_file(myTreeFile);    

  //cout << "Read input trees" << endl;

  TripletPairing tripPairs;
  tripPairs.initialize(rRef,rTree);
  //cout << "Initialized" << endl;
  if (! tripPairs.pair_to_ref())
      cout << "Failed to pair the input trees!" << endl;
  else {    
      //cout << "Paired two trees" << endl;
      tripPairs.count();
      //cout << "Counted triplet pairs" << endl;
      for (vector<PairCounter>::iterator p = tripPairs.tripPairs.begin(); p != tripPairs.tripPairs.end(); p++){
            string taxon1 = p->taxon1;
            string taxon2 = p->taxon2;
            INTTYPE_REST count = p->counter;
            cout << taxon1 << " " << taxon2 << " " << count << endl;
      }
  }

  delete rFactory;
  delete tFactory;
  return 0;
}
