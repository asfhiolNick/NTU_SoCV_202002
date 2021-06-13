/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "v3NtkUtil.h"
#include "v3Msg.h"
#include "bddMgrV.h"
#include <algorithm>
#include <sstream>

//#define FrontierSimplification

void
BddMgrV::buildPInitialState()
{
  // TODO : remember to set _initState
  // Set Initial State to All Zero
  // Solve _initState := (~x0)(~x1)...(~xn) = 1 <-> Set x0,x1,...,xn = 0
  V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
  _initState = BddNodeV::_one;
  for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
    _initState &= ~getSupport(ntk->getLatch(i).id);    
  }
  if(_reachStates.empty())
    _reachStates.push_back(_initState);
}

void
BddMgrV::buildPTransRelation()
{
  // TODO : remember to set _tr, _tri  
  //step1: TR(Y,X,I) = ~(Y^delta(X,I))
  V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
  _tri = BddNodeV::_one;
  unsigned j = ntk->getLatchSize();
  for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
    BddNodeV delta = getBddNodeV(ntk->getInputNetId(ntk->getLatch(i),0).id);
    if(ntk->getInputNetId(ntk->getLatch(i),0).cp) delta = ~delta;  
    _tri &= ~(getSupport(j+ntk->getLatch(i).id) ^ delta);
  }

  //step2: TR(Y,X) = exist I TR(Y,X,I)
  _tr = _tri;
  for(unsigned i = 0, n = ntk->getInputSize(); i < n; ++i) {
    _tr  = _tr.exist(ntk->getInput(i).id);   
  }
}

void
BddMgrV::buildPImage( int level )
{
  // TODO : remember to add _reachStates and set _isFixed
  // Note:: _reachStates record the set of reachable states
  for(int i=0; i<level; ++i){
    if(!_isFixed) 
      BddMgrV::buildPImageStep();
  }   
  if(_isFixed)
    cout << "Fixed point is reached (time : " << _reachStates.size()-1 << ") " << endl;
}

void
BddMgrV::buildPImageStep()
{
  //Step1: S(Y,X) = TR(Y,X) ^ Rn(X)
  V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
  BddNodeV S = getPTr() & getPReachState();
#ifdef FrontierSimplification
  if(_reachStates.size()>=2)
    S = getPTr() & Restrict(getPReachState(), ~(_reachStates[_reachStates.size()-2]));  
#endif
  
  //step2: S(Y) = existX S(Y,X)
  for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
    S = S.exist(ntk->getLatch(i).id);
  }
 
  //step3: S(Y) -> S(X)
  bool isMoved;
  V3NetId id = ntk->getLatch(0);
  S = S.nodeMove(id.id+ntk->getLatchSize(), id.id, isMoved);

  //step4: Rn+1(X) = Rn(X)|S(X), set _isFixed
  BddNodeV oldR = getPReachState();
  if((oldR|S)==oldR){  
    _isFixed = true;
  }
  else
    _reachStates.push_back((oldR|S));
}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)
  V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
  BddNodeV target = monitor & getPReachState();
   
  if(target.countCube()!=0){
    //counter-example 
    cout << "Monitor \"" << name << "\" is violated." << endl;
    cout << "Counter Example:" << endl;
    int i = _reachStates.size()-1;
    while((monitor & _reachStates[i]).countCube()!=0)
      i--;
    i++; 
    BddNodeV S = target.getCube(), V;
    bool isMoved;
    vector<string> output;
    for(i; i>=0; --i){
      //Step1: Vn(I) = exist Y,X "TRI & Sn+1(X->Y)"
      S = S.nodeMove(ntk->getLatch(0).id, ntk->getLatch(0).id+ntk->getLatchSize(), isMoved);
      V = getPTri() & S;      
      for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
        V = V.exist(ntk->getLatch(i).id);
      }
      for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
        V = V.exist(ntk->getLatch(i).id+ntk->getLatchSize());
      }

      //Step2: Sn(X) = exist Y,I "TRI & Sn+1(X->Y) & Vn"
      S = (getPTri() & S & V);
      for(unsigned i = 0, n = ntk->getInputSize(); i < n; ++i) {
        S  = S.exist(ntk->getInput(i).id);   
      }
      for(unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
        S = S.exist(ntk->getLatch(i).id+ntk->getLatchSize());
      }

      //Step3: cout
      std::stringstream ss;
      ss << i << ": " << V.getCube().toString()[1];
      output.push_back(ss.str());
    }
    reverse(output.begin(), output.end());
    for(int j=0; j<output.size(); ++j)
      cout << output[j] << endl;

  }
  else if(_isFixed)
    cout << "Monitor \"" << name << "\" is safe." << endl;
  else
    cout << "Monitor \"" << name << "\" is safe up to time " << _reachStates.size()-1 << "." << endl;
}
