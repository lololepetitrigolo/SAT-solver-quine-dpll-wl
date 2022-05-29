#ifndef _satSolverFunction_h
#define _satSolverFunction_h
extern bool mem_vide (fnc f);
extern litteral find_max_lit(fnc f);
extern bool mem(litteral x,int*l,int len);
extern litteral unit_clause(fnc f);
extern int nb_clause_with_x(fnc f,litteral x);
extern fnc suppr(litteral x, fnc f);
extern int min(int a,int b);
extern int max(int a,int b);
extern bool est_model(int*M,int ind_M,fnc f);
extern void fill_model(int*M,int ind_M,fnc f,int**res,int*len_res);
extern bool mem_abs(litteral x,int*l,int len);
#endif
