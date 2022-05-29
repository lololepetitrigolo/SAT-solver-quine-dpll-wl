#ifndef _clause_h
#define _clause_h
extern void free_clause(clause c);
extern clause create_clause(litteral*list,int size);
extern void print_clause(clause c);
#endif
