/****************************************************************************
  FileName     [ proveCmd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ Define basic prove package commands ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iomanip>
#include <cstring>
#include "bddMgrV.h"
#include "proveCmd.h"
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3Msg.h"

using namespace std;

bool
initProveCmd() {
  return (
      v3CmdMgr->regCmd("PINITialstate", 5, new PInitialStateCmd) &&
      v3CmdMgr->regCmd("PTRansrelation", 3, new PTransRelationCmd) &&
      v3CmdMgr->regCmd("PIMAGe", 5, new PImageCmd) &&
      v3CmdMgr->regCmd("PCHECKProperty", 7, new PCheckPropertyCmd)
      );
}

extern BddMgrV*            bddMgrV;
extern BddNodeV            getBddNodeV(const string& bddName);

static bool valid()
{
  V3NtkHandler* const handler = v3Handler.getCurHandler();

  if (!handler)
    Msg(MSG_WAR) << "Design does not exist !!!" << endl;
  else if (handler->getNtk()->getModuleSize())
    Msg(MSG_WAR) << "Design has not been flattened !!!" << endl;
  else if (dynamic_cast<const V3BvNtk*>(handler->getNtk()))
    Msg(MSG_ERR) << "Current Network is NOT an AIG Ntk (try \"blast ntk\" first)!!" << endl;
  else if (!handler->getNtk()->isBddBuilt())
    Msg(MSG_WAR) << "BDD For Current Design Has Not Been Constructed !!!" << endl;
  else return true;
  return false;
}

static bool isValidVarName(const string& str) {
  size_t n = str.size();
  if (n == 0) return false;
  if (!isalpha(str[0]) && str[0] != '_')
    return false;
  for (size_t i = 1; i < n; ++i)
    if (!isalnum(str[i]) && str[i] != '_')
      return false;
  return true;
}

//----------------------------------------------------------------------
//    PINITialstate [(string varName)]
//----------------------------------------------------------------------
V3CmdExecStatus
PInitialStateCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;

  vector<string> options;
  V3CmdExec::lexOptions(option, options);
  if(options.size() > 1) {
    return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
  }
  string token = "";
  if(options.size()) {
    if(!isValidVarName(options[0]))
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    else
      token = options[0];
  }

  bddMgrV->buildPInitialState(); 
  if( !token.empty() && !bddMgrV->addBddNodeV(token, bddMgrV->getPInitState()())) {
    Msg(MSG_ERR) << "\"" << token << "\" has Already been Associated With Another BddNode!!" << endl;
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
  }
  return CMD_EXEC_DONE;
}

void
PInitialStateCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: PINITialstate [(string varName)]" << endl;
}

void
PInitialStateCmd::help() const
{
  cout << setw(20) << left << "PINITialstate: "
    << "Set initial state BDD" << endl;
}

//----------------------------------------------------------------------
//    PTRansrelation [(string triName)] [(string trName)]
//----------------------------------------------------------------------
V3CmdExecStatus
PTransRelationCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;

  size_t op = 0;
  vector<string> options;
  V3CmdExec::lexOptions(option, options);
  if( options.size() > 2 )
    return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

  string triName, trName;
  if( op < options.size()) {
    triName = options[op++];
    if(!isValidVarName(triName))
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, triName);
  }
  if( op < options.size() ) {
    trName = options[op++];
    if(!isValidVarName(trName))
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, trName);
  }
  bddMgrV->buildPTransRelation();
  if( !triName.empty() && !bddMgrV->addBddNodeV(triName, bddMgrV->getPTri()())) {
    Msg(MSG_ERR) << "\"" << triName << "\" has Already been Associated With Another BddNode!!" << endl;
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, triName);
  }
  if( !trName.empty() && !bddMgrV->addBddNodeV(trName, bddMgrV->getPTr()())) {
    Msg(MSG_ERR) << "\"" << trName << "\" has Already been Associated With Another BddNode!!" << endl;
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, trName);
  }
  return CMD_EXEC_DONE;
}

void
PTransRelationCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: PTRansrelation [(string triName)] [(stirng trName)]" << endl;
}

void
PTransRelationCmd::help() const
{
  cout << setw(20) << left << "PTRansrelation: "
    << "build the transition relationship in BDDs" << endl;
}

//----------------------------------------------------------------------
//    PIMAGe [-Next <(int numTimeframes)>] [(string varName)]
//----------------------------------------------------------------------
V3CmdExecStatus
PImageCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;
  if (bddMgrV->getPInitState()() == 0) {
    Msg(MSG_WAR) << "BDD of Initial State is Not Yet Constructed !!!" << endl;
    return CMD_EXEC_ERROR;
  } else if (bddMgrV->getPTr()() == 0) {
    Msg(MSG_WAR) << "BDD of Transition Relation is Not Yet Constructed !!!" << endl;
    return CMD_EXEC_ERROR;
  }

  int level = 1;
  string name;
  vector<string> options;
  V3CmdExec::lexOptions(option, options);

  for (size_t i = 0, n = options.size(); i < n; ++i)
    if (v3StrNCmp("-Next", options[i], 2) == 0)
      if(++i < n) {
        if (!v3Str2Int(options[i], level) || level <= 0)
          return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else return V3CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
    else if( name.empty() ) {
      name = options[i];
      if(!isValidVarName(name))
        return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, name);
    } else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
  bddMgrV->buildPImage( level );
  if( !name.empty() )
    bddMgrV->forceAddBddNodeV(name, bddMgrV->getPReachState()());
  return CMD_EXEC_DONE;
}

void
PImageCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: PIMAGe [-Next <(int numTimeframes)>] [(string varName)]"
    << endl;
}

void
PImageCmd::help() const
{
  cout << setw(20) << left << "PIMAGe: "
    << "build the next state images in BDDs" << endl;
}

//----------------------------------------------------------------------
//    PCHECKProperty < -Netid <netId> | -Output <outputIndex> > >
//----------------------------------------------------------------------
V3CmdExecStatus
PCheckPropertyCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;
  if (bddMgrV->getPReachState()() == 0) {
    Msg(MSG_WAR) << "BDD of Reached State is Not Yet Constructed !!!" << endl;
    return CMD_EXEC_ERROR;
  }

  vector<string> options;
  V3CmdExec::lexOptions(option, options);

  if( options.size() < 2 )
    return V3CmdExec::errorOption(CMD_OPT_MISSING, "");
  if( options.size() > 2)
    return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

  V3NtkHandler* const handler = v3Handler.getCurHandler();
  V3Ntk* const ntk = handler->getNtk();
  bool isNet = false;

  if (v3StrNCmp("-Netid", options[0], 2) == 0) isNet = true;
  else if (v3StrNCmp("-Output", options[0], 2) == 0) isNet = false;
  else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

  int num = 0;
  V3NetId netId;
  if(!v3Str2Int(options[1], num) || (num < 0))
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
  if(isNet) {
    if((unsigned)num >= ntk->getNetSize()) {
      Msg(MSG_ERR) << "Net with Id " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = V3NetId::makeNetId(num);
  } else {
    if((unsigned)num >= ntk->getOutputSize()) {
      Msg(MSG_ERR) << "Output with Index " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = ntk->getOutput(num);
  }

  BddNodeV monitor = bddMgrV->getBddNodeV( netId.id );
  if(netId.cp) monitor = ~monitor;
  assert(monitor());
  bddMgrV->runPCheckProperty( handler->getNetNameOrFormedWithId(netId),
                              monitor );

  return CMD_EXEC_DONE;
}

void
PCheckPropertyCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: PCHECKProperty < -Netid <netId> | -Output <outputIndex> >" << endl;
}

void
PCheckPropertyCmd::help() const
{
  cout << setw(20) << left << "PCHECKProperty:"
    << "check the monitor by BDDs" << endl;
}

