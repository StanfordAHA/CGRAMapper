#include "context.hpp"
#include "passes.hpp"

#include "mapper.hpp"
#include "cgralib.hpp"
#include "stdlib.hpp"

using namespace CoreIR;

int main(int argc, char *argv[]){
  Context* c = newContext();
  
  getcgralib(c);
  getStdlib(c);

  if(argc!=3){
    cout << "usage: mapper premapped.json mapped.json" << endl;
    return 1;
  }

  bool err = false;
  cout << "Loading " << argv[1] << endl;
  Module* m = loadModule(c,argv[1],&err);
  if(err){c->die();}
  
  m->print();
  cout << "Trying to map" << endl;
  Module* mapped = mapper(c,m,&err);
  if(err){
    cout << "failed mapping!" << endl;
    c->die();
  }
  mapped->print();
  
  cout << "Typechecking" << endl;
  typecheck(c,mapped,&err);
  if(err){c->die();}

  cout << "Trying to save" << endl;
  saveModule(mapped,argv[2],&err);
  if (err) c->die();

  deleteContext(c);

  return 0;
}
