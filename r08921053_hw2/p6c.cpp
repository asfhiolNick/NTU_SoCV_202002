/****************************************************************************
  FileName     [ testBdd.cpp ]
  PackageName  [ ]
  Synopsis     [ Define main() ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "bddNode.h"
#include "bddMgr.h"

using namespace std;

/**************************************************************************/
/*                        Define Global BDD Manager                       */
/**************************************************************************/
BddMgr bm;

/**************************************************************************/
/*                    Define Static Function Prototypes                   */
/**************************************************************************/
static void initBdd(size_t nSupports, size_t hashSize, size_t cacheSize);


/**************************************************************************/
/*                             Define main()                              */
/**************************************************************************/
int
main()
{
   initBdd(6, 127, 61);

   /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNode a(bm.getSupport(1));
   BddNode b(bm.getSupport(2));
   BddNode c(bm.getSupport(3));
   BddNode d(bm.getSupport(4));
   BddNode e(bm.getSupport(5));
   BddNode f(bm.getSupport(6));

   BddNode g1 = b&d;
   BddNode g2 = (e&(~c));
   BddNode g3 = a&b;
   BddNode g4 = c&g1;
   BddNode g5 = g1|g2;
   BddNode g6 = g2|d;
   BddNode g7 = g6&g3;
   BddNode g8 = g4|g7;
   BddNode g9 = g5&g8&f;

   BddNode G1 = b&d;
   BddNode G2 = (e&(~c));
   BddNode G3 = a&b;
   BddNode G4 = c&G1;
   BddNode G5 = ~(G1|G2);
   BddNode G6 = G2|d;
   BddNode G7 = G6&G3;
   BddNode G8 = G4|G7;
   BddNode G9 = G5&G8&f;

   BddNode diff = g9^G9;
   cout << diff << endl;


   /*----------- END OF TEST CODE ------------*/
}


/**************************************************************************/
/*                          Define Static Functions                       */
/**************************************************************************/
static void
initBdd(size_t nin, size_t h, size_t c)
{
   BddNode::_debugBddAddr = true;
   BddNode::_debugRefCount = true;

//   bm.reset();
   bm.init(nin, h, c);
}

