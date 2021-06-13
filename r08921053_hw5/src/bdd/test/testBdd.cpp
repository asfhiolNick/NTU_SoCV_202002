/****************************************************************************
  FileName     [ testBdd.cpp ]
  PackageName  [ ]
  Synopsis     [ Define main() ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-2013 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "bddNodeV.h"
#include "bddMgrV.h"

using namespace std;

/**************************************************************************/
/*                        Define Global BDD Manager                       */
/**************************************************************************/
BddMgrV bm;

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
   initBdd(5, 127, 61);

   /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNodeV a(bm.getSupport(1));
   BddNodeV b(bm.getSupport(2));
   BddNodeV c(bm.getSupport(3));
   BddNodeV d(bm.getSupport(4));
   BddNodeV e(bm.getSupport(5));

   BddNodeV f = ~(a & b);
   cout << f << endl;

   BddNodeV g = c | d;
   cout << g << endl;

   BddNodeV h = ~e;
   cout << h << endl;

   BddNodeV i = f ^ (c | d); // f ^ g;
   cout << i << endl;

   BddNodeV j = ~a | ~b;
   cout << j << endl;

   ofstream ofile("i.dot");
   i.drawBdd("i", ofile);
   system("dot -o i.png -Tpng i.dot");

   /*----------- END OF TEST CODE ------------*/
}


/**************************************************************************/
/*                          Define Static Functions                       */
/**************************************************************************/
static void
initBdd(size_t nin, size_t h, size_t c)
{
   BddNodeV::_debugBddAddr = true;
   BddNodeV::_debugRefCount = true;

//   bm.reset();
   bm.init(nin, h, c);
}

