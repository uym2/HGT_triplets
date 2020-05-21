#include "TripletPairing.h"
#include "hdt.h"
#include "hdt_factory.h"

void TripletPairing::count(){
    // construct HDT for myRef
    myTree->countLeaves();
    hdt = HDT::constructHDT(myRef, myTree->maxDegree + 1, dummyHDTFactory);
    // count
    this->__count__(this->myTree);    
}

bool TripletPairing::pair_to_ref(){
	vector<RootedTree*>* l = myTree->getList();
	map<string, RootedTree*> altWorldLeaves;
	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
		altWorldLeaves[leaf->name] = leaf;
	}
	delete l;

	l = myRef->getList();
	map<string, RootedTree*>::iterator altWorldEnd = altWorldLeaves.end();
	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		RootedTree *leaf = *i;
		map<string, RootedTree*>::iterator j = altWorldLeaves.find(leaf->name);
		if (j == altWorldEnd)
		{
			// This leaf wasn't found in the input tree!
			    cerr << "Leaves don't agree! Aborting! (" << leaf->name << " didn't exist in the second tree)" << endl;
			    delete l;
			    return false;
		}
				
		// If we got this far, we found the match! Setup bidirectional pointers!
		leaf->altWorldSelf = j->second;
		j->second->altWorldSelf = leaf;
        
		// Delete result
		altWorldLeaves.erase(j);
	}

	// Is there results left in altWorldLeaves? If so it had more leaves than we do...
	if (altWorldLeaves.size() > 0)
	{
        cerr << "Leaves don't agree! Aborting! (" << altWorldLeaves.begin()->first << " didn't exist in the first tree)";
        if (altWorldLeaves.size() > 1)
            cerr << " (and " << (altWorldLeaves.size() - 1) << " other leaves missing from first tree!)";
        cerr << endl;
        delete l;
        return false;
	}
	delete l;
    return true;
}

TripletPairing::TripletPairing(){
    this->myRef = this->myTree = NULL;
    this->hdt = NULL;
    //this->tripPairs = NULL;
    this->dummyHDTFactory = NULL;
}

bool TripletPairing::initialize(RootedTree *ref, RootedTree *tree){
    this->myRef = ref;
    this->myTree = tree;
    this->hdt = NULL;
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    return true;
}

TripletPairing::~TripletPairing(){
    //delete tripPairs;
    delete dummyHDTFactory;
}

void TripletPairing::__count__(RootedTree *v) {
  if (v->isLeaf()) {    
    hdt->updateCounters();
    // This will make sure the entire subtree has color 0!
    v->colorSubtree(0);
    return;
  }

  
  /*
  RootedTree* a_leaf = *(left_leaves->begin()); 
  a_leaf->colorSubtree(2);
  hdt->updateCounters();
  INTTYPE_REST ntrip_outside = 2*hdt->getResolvedTriplets(1); 
  a_leaf->colorSubtree(1);
  hdt->updateCounters();

  //INTTYPE_REST nleaf_outside = this->myTree->n - v->n;
  //INTTYPE_REST ntrip_outside = nleaf_outside*(nleaf_outside-1);   
  */
    
  vector<RootedTree*> *leaves = v->getList();  
  map<string,INTTYPE_REST> ntrip_outside; 
  for (vector<RootedTree*>::iterator i=leaves->begin(); i != leaves->end(); i++) {
      RootedTree *l = *i;
      l->colorSubtree(2);
      hdt->updateCounters();
      INTTYPE_REST n = hdt->getResolvedTriplets(1);
      ntrip_outside[l->name] = n;
      l->colorSubtree(1);        
  }  
  
  // v is not a leaf! Assumming v has exactly two children
  RootedTree* left = v->children->data;
  RootedTree* right = v->children->next->data;

  vector<RootedTree*> *left_leaves = left->getList(); 
  vector<RootedTree*> *right_leaves = right->getList();

  // take each pair (l,r) of left and right
  for (vector<RootedTree*>::iterator i=left_leaves->begin(); i != left_leaves->end(); i++) {
      RootedTree *l = *i;
      // make l color 2
      l->colorSubtree(2);
      for (vector<RootedTree*>::iterator j=right_leaves->begin(); j != right_leaves->end(); j++){
        RootedTree *r = *j;
        // make r color 2
        r->colorSubtree(2);
        // update counters in the HDT then query it
        hdt->updateCounters();
        INTTYPE_REST p = this->hdt->getResolvedTriplets(1) - ntrip_outside[l->name] - ntrip_outside[r->name];
        //cout << l->name << " " << r->name << " " << this->hdt->getResolvedTriplets(1) << " " << ntrip_outside[l->name] + ntrip_outside[r->name] << endl;
        PairCounter newPair = PairCounter(l->name,r->name,p);
        this->tripPairs.push_back(newPair);
        // change the color of r back to 1
        r->colorSubtree(1);    
      }
      // change the color of l back to 1
      l->colorSubtree(1);
  }

  delete left_leaves;
  delete right_leaves;

  // color right to 0 and recurse on left
  right->colorSubtree(0);
  __count__(left);
  // after left is counted, its leaves should all have color 0. Now color right to 1 and recurse
  right->colorSubtree(1);
  __count__(right);
}
