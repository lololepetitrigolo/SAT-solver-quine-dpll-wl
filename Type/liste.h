#ifndef _liste_h
#define _liste_h
extern liste create_liste(int x);
extern liste add(int x,liste l);
extern liste del(int x, liste l);
extern void free_liste(liste l);
extern void print_liste(liste l);
#endif
