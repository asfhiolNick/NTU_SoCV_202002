#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <assert.h>
#include <algorithm>
#include "sat.h"
using namespace std;

class Gate
{
public:
   Gate(unsigned i = 0): _gid(i) {}
   ~Gate() {}

   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
   unsigned   _gid;  // for debugging purpose...
   Var        _var;
};

std::vector<Gate *> gates;

void
initCircuit(int n)
{
   // Init gates, 1~n
   for(int i=0; i<n; ++i){
       gates.push_back(new Gate(i+1));
   }
}

void
genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   for (size_t i = 0, n = gates.size(); i < n; ++i) {
      Var v = s.newVar();
      gates[i]->setVar(v);
   }
}


void reportResult(int N, const SatSolver& solver, bool result)
{
   //solver.printStats();
   //cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      cout << "Satisfiable assignment:" << endl;
      vector<int> ans;
      for (size_t i = 0, n = gates.size(); i < n; ++i){
         if( solver.getValue(gates[i]->getVar()) )
             ans.push_back(i);
      }
      sort(ans.begin(), ans.end());
      for(size_t i=0; i<ans.size(); ++i){
          cout << (ans[i])/N << "(" << (ans[i])%N << ")";
          if(i!=ans.size()-1)
              cout << ", "; 
          else
              cout << endl;
      }
   }
   else
       cout << "No satisfiable assignment can be found." << endl;
}

/***************main***************/
/***step0: Get N		***/
/***step1: Readin line 		***/
/***step2: Initial gate, solver	***/
/***step3: Generate model 	***/
/***step4: Solve SAT		***/
/**********************************/
int main(int argc, char **argv)
{
   if (argc != 2) {
      cerr << "Error: Missing input file!!" << endl;
      exit(-1);
   }
   vector<tuple<int, int, int>> restr;
   //readin file
   fstream file;
   char buffer[80];
   file.open(argv[1], ios::in);
   if(!file){
       cerr << "Error: File is unable to open" << endl;
       return 0;
   }
   file.getline(buffer, sizeof(buffer));
   //step0: Get N
   int N = stoi(buffer);
   assert(N>=1);

   //step1: Readin line      
   while(true){
       file.getline(buffer, sizeof(buffer));
       if(file.eof()) break;
       //step1-1: Get buffer.legnth()
       int end = 0;
       for( ; end<80 && buffer[end]!='\0'; ++end){}
       //step1-2: Get constraint type
       string type="", strI="", strJ="";
       int T=0, I, J, iter=0;
       for( ; iter<end && buffer[iter]!='('; ++iter){
           type += buffer[iter];
       }
       if(type=="Assign")		T = 1;
       else if(type=="AssignNot")	T = 2;
       else if(type=="LessThan")	T = 3;
       else if(type=="Adjacent")	T = 4;
       else if(type=="AdjacentNot")	T = 5;
       else
           cerr << "Error: Not belong to 5 types of constraints" << endl;
       //step1-3: Get mi
       if(iter<end) ++iter;
       for( ; iter<end && buffer[iter]!=','; ++iter){
           strI += buffer[iter];
       }
       I = stoi(strI);
       //step1-4: Get sj/mj
       if(iter<end) ++iter;
       if(iter<end) ++iter;
       for( ; iter<end && buffer[iter]!=')'; ++iter){
           strJ += buffer[iter];
       }           
       J = stoi(strJ);
       restr.push_back(make_tuple(T, I, J));
   };
   file.close();

   //step2: Initial gate, solver
   //Variable index
   // gate[x=1~N*N]	= a[(x-1)/N][(x-1)%N] 
   // a[I][J]		= gate[I*N+J+1]
   // gate[x=N+1~...]	= restricts(x-N)
   initCircuit(N*N);
   SatSolver solver;
   solver.initialize();

   //step3: Generate model 
   genProofModel(solver);
   
   //step4: Solve SAT
   bool result;
   solver.assumeRelease();

   //step4-1: each man sit on <=1 chair
   for(int i=0; i<N; ++i){
       for(int j=0; j<N-1; ++j){
           for(int m=j+1; m<N; ++m){
               Var newV = solver.newVar();
               solver.addAigCNF(newV, gates[i*N+j]->getVar(), false, gates[i*N+m]->getVar(), false);
               solver.assumeProperty(newV, false);
 
               newV = solver.newVar();
               solver.addAigCNF(newV, gates[j*N+i]->getVar(), false, gates[m*N+i]->getVar(), false);
               solver.assumeProperty(newV, false);
           }
       }
   }  
   //step4-2: each man sit on >=1 chair
   for(int i=0; i<N; ++i){
       //ai0' * ... * aiN-1' = 0
       Var preV = solver.newVar();
       solver.addAigCNF(preV, gates[i*N+0]->getVar(), true, gates[i*N+1]->getVar(), true);
       for(int j=2; j<N; ++j){
           Var newV = solver.newVar();
           solver.addAigCNF(newV, gates[i*N+j]->getVar(), true, preV, false);
           preV = newV;
       }
       solver.assumeProperty(preV, false);
       //a0i' * ... * aN-1i' = 0
       preV = solver.newVar();
       solver.addAigCNF(preV, gates[0*N+i]->getVar(), true, gates[1*N+i]->getVar(), true);
       for(int j=2; j<N; ++j){
           Var newV = solver.newVar();
           solver.addAigCNF(newV, gates[j*N+i]->getVar(), true, preV, false);
           preV = newV;
       }
       solver.assumeProperty(preV, false);
   }

    

  for(size_t i=0; i<restr.size(); ++i){
       int T= get<0>(restr[i]), I = get<1>(restr[i]), J = get<2>(restr[i]);
       //cout << "Type = " << get<0>(restr[i]) << "/ i = " << get<1>(restr[i]) << "/ j = " << get<2>(restr[i]) << endl;
       switch(T) {
           case 1:{
               solver.assumeProperty(gates[I*N+J]->getVar(), true);
               break;
           }
           case 2:{   
               solver.assumeProperty(gates[I*N+J]->getVar(), false);
               break;
           }
           case 3:{
               //k!=N-1
               for(int k=0; k<N-1; ++k){
                   Var preV = solver.newVar();
                   solver.addAigCNF(preV, gates[I*N+k]->getVar(), false, gates[J*N+k+1]->getVar(), true);
                   for(int m=k+2; m<N; ++m){
                       Var newV = solver.newVar();
                       solver.addAigCNF(newV, preV, false, gates[J*N+m]->getVar(), true);
                       preV = newV;
                   }
                   solver.assumeProperty(preV, false);
               }
               //k==N-1
	       solver.assumeProperty(gates[I*N+(N-1)]->getVar(), false);
               break;
           }
           case 4:{
               for(int k=0; k<N; ++k){
                   Var newV = solver.newVar();
                   if(k==0){
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+1]->getVar(), true);
                       solver.assumeProperty(newV, false);
                   }
                   else if(k==N-1){
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+N-2]->getVar(), true);
                       solver.assumeProperty(newV, false);
                   }
                   else{
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+k-1]->getVar(), true);
                       Var clause = solver.newVar();
                       solver.addAigCNF(clause, newV, false, gates[J*N+k+1]->getVar(), true);
                       solver.assumeProperty(clause, false);
                   }
               }
               break;
           }
           case 5:{
               for(int k=0; k<N; ++k){
                   Var newV = solver.newVar();
                   if(k==0){
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+1]->getVar(), false);
                       solver.assumeProperty(newV, false);
                   }
                   else if(k==N-1){
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+N-2]->getVar(), false);
                       solver.assumeProperty(newV, false);
                   }
                   else{
                       solver.addAigCNF(newV, gates[I*N+k]->getVar(), false, gates[J*N+k-1]->getVar(), false);
                       Var clause = solver.newVar();
                       solver.addAigCNF(clause, newV, false, gates[J*N+k+1]->getVar(), false);
                       solver.assumeProperty(clause, false);
                   }
               }
               break;
           }
      }
   }

   result = solver.assumpSolve();
   reportResult(N, solver, result);
   
}
