#include <bdd.h>
#include <iostream>

int main(void) {
  bdd u,v,w,x,y,z;

  bdd_init(1000,100);
  bdd_setvarnum(5);

  x = bdd_ithvar(0);
  y = bdd_ithvar(1);
  w = bdd_ithvar(2);
  v = bdd_ithvar(3);
  u = bdd_ithvar(4);

  z = ((u & v) & w) & (x & y) >> (v & x);

  std::cout << z << std::endl;
  // bdd_printall();
  bdd_printdot(z);
  bdd_done();
}
