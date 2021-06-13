/****************************************************************************
  FileName     [ v3Stack.h ]
  PackageName  [ v3/src/adt ]
  Synopsis     [ Abstract Data Type for Stack. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2012-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_ADT_STACK_H
#define V3_ADT_STACK_H

#include <stack>

template<class T>
struct V3Stack {
   typedef std::stack<T> Stack;
};

#endif

