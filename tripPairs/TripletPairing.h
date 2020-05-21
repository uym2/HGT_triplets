#ifndef TRIPLET_PAIRING_H
#define TRIPLET_PAIRING_H

#include "int_stuff.h"
#include "hdt.h"

class PairCounter {
public:
    string taxon1; // the name of the first taxon
    string taxon2; // the name of the second taxon
    INTTYPE_REST counter;
    PairCounter(string t1, string t2, INTTYPE_REST c) { taxon1 = t1; taxon2 = t2; counter=c; }
};

class TripletPairing {
 public:
  TripletPairing();
  bool initialize(RootedTree *ref, RootedTree *tree);
  ~TripletPairing();
  
  bool pair_to_ref(); // match the leaves of myRef and myTree
  void count();

  HDTFactory *dummyHDTFactory;
  RootedTree *myRef, *myTree;
  HDT *hdt;
  vector<PairCounter> tripPairs;
 
 private:
  void __count__(RootedTree *v);
};

#endif
