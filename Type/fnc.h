#ifndef _fnc_h
#define _fnc_h
extern void free_fnc(fnc f);
extern fnc create_fnc(clause*c,int len,int nb_lit);
extern void print_fnc(fnc f);
#endif
