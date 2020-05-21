#include "TripletRooting.h"
#include "hdt.h"
#include "hdt_factory.h"

bool TripletRooting::pairing(){
    myRef->pairAltWorld(myTree,true,tripCount);
    
    if (myTree->isError()) {
        std::cerr << "The two trees do not have the same set of leaves." << std::endl;
        std::cerr << "Aborting." << std::endl;
        return false;
    }

    myTree->mark_active(tripCount);
    return true;
}

bool TripletRooting::find_optimal_root(){
    if (!this->pairing()){
        cerr << "Error: could not pair the two trees. Aborting!" << endl;
        return false;
    }

    // construct HDT for myRef
    countChildren(myTree);
    std::cout << "Degree: " << myTree->maxDegree + 1 << std::endl;
    hdt = HDT::constructHDT(myRef, myTree->maxDegree + 1, dummyHDTFactory);
    
    count(myTree);

    // HDT is deleted in count if extracting and contracting!
/*
#ifndef doExtractAndContract
    delete hdt->factory;
#endif
*/    
    this->compute_tA(this->myTree);
    

    unsigned int r = myTree->idx;

    this->optimalRoot = NULL;
    this->optimalTripScore = -1;
    
    this->downroot(myTree,tripCount->tA[r],false);

    return true;
}

void TripletRooting::downroot(RootedTree *t, INTTYPE_REST parent_score, bool parent_active){
    unsigned int u = t->idx;
    
    for(TemplatedLinkedList<RootedTree*> *current = t->children; current != NULL; current = current->next) {
        unsigned int v = current->data->idx;
        if (!tripCount->isActive[v]){
            continue;
        }
        INTTYPE_REST current_score = parent_score - tripCount->tI[u] + tripCount->tO[v] + tripCount->tR[v];
        bool sister_active = false;
        for(TemplatedLinkedList<RootedTree*> *sis = t->children; sis != NULL; sis = sis->next) {
            if (sis != current){
                unsigned int v1 = sis->data->idx;
                if (tripCount->isActive[v1]){
                    sister_active = true;
                    break;
                }
            }
        }
        if (parent_active || sister_active){
            if (current_score == this->optimalTripScore && t != this->myTree)
                this->ambiguity += 1;
            else if (current_score > this->optimalTripScore){
                this->optimalTripScore = current_score;
                this->optimalRoot = current->data;
                this->ambiguity = 1;
            }
        }
        INTTYPE_REST v_parent_score = current_score - tripCount->tR[v]; 
        bool v_parent_active = parent_active | sister_active;
        this->downroot(current->data,v_parent_score,v_parent_active);
    }   
}

TripletRooting::TripletRooting(){
    this->myRef = this->myTree = NULL;
    this->hdt = NULL;
    this->tripCount = NULL;
    this->dummyHDTFactory = NULL;
    this->ambiguity = 0;
}

bool TripletRooting::initialize(RootedTree *ref, RootedTree *tree){
    this->myRef = ref;
    this->myTree = tree;
    this->hdt = NULL;
    unsigned int N = this->myTree->set_all_idx(0);
    tripCount = new TripletCounter(N);
    dummyHDTFactory = new HDTFactory(myTree->maxDegree+1);
    return true;
}

TripletRooting::~TripletRooting(){
    delete tripCount;
    delete dummyHDTFactory;
}

void TripletRooting::update_tI(unsigned int nodeIdx, bool count_unresolved){
        this->tripCount->tI[nodeIdx] = this->hdt->getResolvedTriplets(0) + count_unresolved*this->hdt->getUnresolvedTriplets(0);
}

void TripletRooting::update_tO(unsigned int nodeIdx, unsigned int color, bool count_unresolved){
        this->tripCount->tO[nodeIdx] = this->hdt->getResolvedTriplets(color) + count_unresolved*this->hdt->getUnresolvedTriplets(color);
}

void TripletRooting::update_tR(unsigned int nodeIdx){
    this->tripCount->tR[nodeIdx] = this->hdt->getResolvedTriplets_root();
}


/*
void TripletRooting::updateCounters(unsigned int nodeIdx, unsigned int color){
    if (color == 0) {
        // update tI
        this->tripCount->tI[nodeIdx] = this->hdt->getResolvedTriplets(0) + this->hdt->getUnresolvedTriplets(0);
    }
    else { 
        // update tO
        this->tripCount->tO[nodeIdx] = this->hdt->getResolvedTriplets(color) + this->hdt->getUnresolvedTriplets(color);
    }
    // update tR
    this->tripCount->tR[nodeIdx] = this->hdt->getResolvedTriplets_root();
}*/

void TripletRooting::compute_tA(RootedTree *v){
    INTTYPE_REST acc = this->tripCount->tI[v->idx];
    for(TemplatedLinkedList<RootedTree*> *current = v->children; current != NULL; current = current->next) {
        this->compute_tA(current->data);
        acc += this->tripCount->tA[current->data->idx];
    }
    this->tripCount->tA[v->idx] = acc;    
}

void TripletRooting::count(RootedTree *v) {
  if (v->isLeaf()) {    
    hdt->updateCounters();
    tripCount->tR[v->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);

    // This will make sure the entire subtree has color 0!
    v->colorSubtree(0);
    return;
  }

  // v is not a leaf!
  // Find largest subtree
  TemplatedLinkedList<RootedTree*> *largest = v->children;
  int largestN = largest->data->n;
  TemplatedLinkedList<RootedTree*> *beforeLargest = NULL;
  TemplatedLinkedList<RootedTree*> *prev = v->children;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    if (current->data->n > largestN) {
      largest = current;
      beforeLargest = prev;
      largestN = largest->data->n;
    }
    prev = current;
  }
  if (beforeLargest != NULL) {
    beforeLargest->next = largest->next;
    largest->next = v->children;
    v->children = largest;
  }
  
  // Color i'th subtree (i > 1) with color i
  int c = 2;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    current->data->colorSubtree(c);
    c++;
  }

  // Update counters in the HDT
  hdt->updateCounters();
  update_tI(v->idx);
  
  // compute triplets outside of each child of v
  int c = 1;
  for(TemplatedLinkedList<RootedTree*> *current = v->children; current != NULL; current = current->next) {
      update_tO(current->data->idx,c);
      c++;
  }      

  // Color to 0
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
    current->data->colorSubtree(0);
  }

  // recurse on 1st child
  RootedTree *firstChild = v->children->data;
  hdt->updateCounters();  
  tripCount->tR[firstChild->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);
    
  if (firstChild->isLeaf()) {    
    // Do "nothing" (except clean up and possibly color!)
    firstChild->colorSubtree(0);
  } else {
    count(firstChild);
  }
  
  // Color 1 and recurse
  c = 0;
  for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next) {
      current->data->colorSubtree(1);
      hdt->updateCounters();  
      tripCount->tR[current->data->idx] = this->hdt->getResolvedTriplets(2) + this->hdt->getUnresolvedTriplets(2);
      
      count(current->data);
    c++; // Weee :)
  }
}

void TripletRooting::countChildren(RootedTree *t) {
  if (t->isLeaf()) {
    t->n = 1;
    return;
  }
  
  int nSum = 0;
  for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next) {
    RootedTree *childI = i->data;
    countChildren(childI);
    nSum += childI->n;
  }
  t->n = nSum;
}
