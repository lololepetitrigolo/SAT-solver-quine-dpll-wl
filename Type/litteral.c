#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "litteral.h"

//renvoie la variable associé à un littéral
int abs_lit (litteral l){
  if(l<0) return -l;
  return l;
}
