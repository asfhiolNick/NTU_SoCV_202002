/****************************************************************************
  FileName     [ v3NtkBdd.cpp ]
  PackageName  [ v3/src/ntk ]
  Synopsis     [ V3 Network to BDDs. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_NTK_C
#define V3_NTK_C

#include "v3NtkHandler.h" // MODIFICATION FOR SoCV BDD
#include "v3Ntk.h"
#include "v3Msg.h"
#include "v3StrUtil.h"
#include "bddNodeV.h" // MODIFICATION FOR SoCV BDD
#include "bddMgrV.h" // MODIFICATION FOR SoCV BDD

extern BddMgrV* bddMgrV; // MODIFICATION FOR SoCV BDD

const bool V3Ntk::setBddOrder(V3NtkHandler* const handler, const bool& file) const {
  unsigned supportSize = getInputSize() + getInoutSize() + 2*getLatchSize();
  if(supportSize >= bddMgrV->getNumSupports()) {
    Msg(MSG_ERR) << "BDD Support Size is Smaller Than Current Design Required !!" << endl;
    return false;
  }
  // build support
  unsigned supportId = 1;
  for(unsigned i = 0, n = getInputSize(); i < n; ++i) {
    const V3NetId& nId = (file)? getInput(i) : getInput(n-i-1);
    bddMgrV->addBddNodeV(nId.id, bddMgrV->getSupport(supportId)());
    bddMgrV->addBddNodeV(handler->getNetNameOrFormedWithId(nId),
        bddMgrV->getSupport(supportId)());
    ++supportId;
  }
  for(unsigned i = 0, n = getInoutSize(); i < n; ++i) {
    const V3NetId& nId = (file)? getInout(i) : getInout(n-i-1);
    bddMgrV->addBddNodeV(nId.id, bddMgrV->getSupport(supportId)());
    bddMgrV->addBddNodeV(handler->getNetNameOrFormedWithId(nId),
        bddMgrV->getSupport(supportId)());
    ++supportId;
  }
  for(unsigned i = 0, n = getLatchSize(); i < n; ++i) {
    const V3NetId& nId = (file)? getLatch(i) : getLatch(n-i-1);
    bddMgrV->addBddNodeV(nId.id, bddMgrV->getSupport(supportId)());
    bddMgrV->addBddNodeV(handler->getNetNameOrFormedWithId(nId),
        bddMgrV->getSupport(supportId)());
    ++supportId;
  }
  // Next State
  for(unsigned i = 0, n = getLatchSize(); i < n; ++i) {
    const V3NetId& nId = (file)? getLatch(i) : getLatch(n-i-1);
    bddMgrV->addBddNodeV(handler->getNetNameOrFormedWithId(nId)+"_ns",
        bddMgrV->getSupport(supportId)());
    ++supportId;
  }

  // Constants
  for (uint32_t i = 0; i < getConstSize(); ++i) {
    assert(getGateType(getConst(i)) == AIG_FALSE);
    bddMgrV->addBddNodeV(getConst(i).id, BddNodeV::_zero());
  }

  return true;
}

void V3Ntk::buildNtkBdd() {
  // TODO: build BDD for ntk here
  // Perform DFS traversal from DFF inputs, inout, and output gates.
  // Collect ordered nets to a V3NetVec
  // Construct BDDs in the DFS order
  _isBddBuilt = true;
}

void V3Ntk::buildBdd(const V3NetId& netId) {
  V3NetVec orderedNets;

  orderedNets.clear();
  orderedNets.reserve(getNetSize());

  newMiscData();
  dfsOrder(netId, orderedNets);
  assert (orderedNets.size() <= getNetSize());

  // TODO: build BDD for the specified net here
}

// put fanins of a net (id) into a vector (nets) in topological order
void V3Ntk::dfsOrder(const V3NetId& id, V3NetVec& nets) {
  if (isLatestMiscData(id)) return;
  // Set Latest Misc Data
  setLatestMiscData(id);
  // Traverse Fanin Logics
  const V3GateType type = getGateType(id);
  if(type == AIG_NODE) {
    dfsOrder(getInputNetId(id, 0), nets);
    dfsOrder(getInputNetId(id, 1), nets);
  }
  nets.push_back(id);  // Record Order
}

#endif
