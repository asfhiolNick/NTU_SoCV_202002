/****************************************************************************
  FileName     [ proveCmd.h ]
  PackageName  [ prove ]
  Synopsis     [ Define basic prove package commands ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef PROVE_CMD_H_
#define PROVE_CMD_H_

#include "v3CmdMgr.h"

// ============================================================================
// Classes for Prove package commands 
// ============================================================================
V3_COMMAND(PInitialStateCmd,  CMD_TYPE_BDD);
V3_COMMAND(PTransRelationCmd, CMD_TYPE_BDD);
V3_COMMAND(PImageCmd,         CMD_TYPE_BDD);
V3_COMMAND(PCheckPropertyCmd, CMD_TYPE_BDD);

#endif

