#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Type/types.h"
#include "Type/fnc.h"
#include "Type/clause.h"
#include "analyse.h"

//fonction qui determine les caractéristiques du fichier (nombre de ligne et taille maximal d'une ligne)
void nb_lines(FILE*file,int*nb,int*max){
  char c; int m=0;
  while((c=fgetc(file)) != EOF){
    m++;
    if(c=='\n'){
      (*nb)++;
      if(m > *max) *max=m;
      m = 0;
    }
  }
}

//fonction qui decompose un fichier en liste des lignes du fichier
char** get_lines(FILE*file,int n,int m,int*reslen){
  int i = 0;
  char**lines = (char**)malloc(n*sizeof(char*));
  char*c = (char*)malloc((m+1)*sizeof(char));
  assert(lines != NULL && c != NULL);
  //on lit le fichier ligne par ligne
  while(fscanf(file, "%[^\n] ", c) != EOF){
    if(strlen(c)!=0 && c[0]!='c'){ //on ne prend pas en compte les lignes vide et les commentaire
      lines[i] = (char*)malloc((strlen(c)+1)*sizeof(char));
      assert(lines[i] != NULL);
      strcpy(lines[i],c);
      i++;
    }
  }
  free(c);
  *reslen = i;
  return lines;
}

//fonction qui renvoie tous les nombres present dans la chaine de caractère(sous forme de liste)
int* get_all_int (char*s,int*reslen){
  int ind=0,len = strlen(s);
  bool neg = false,num = false;
  int*list_nb = (int*)malloc(len*sizeof(int));
  assert(list_nb!=NULL);
  for(int i = 0; i < len; i++) list_nb[i]=0;
  //on parcourt toute notre chaine de caractère pour en extraire les nombres
  for(int i = 0; i < len-1; i++){
    if(s[i]=='-' && s[i+1]>='0' && s[i+1]<='9') neg = true;
    else if(s[i]>='0' && s[i]<='9'){
      list_nb[ind] = list_nb[ind]*10 + (s[i] - '0');
      num = true;
    }
    if(num && (s[i+1]<'0' || s[i+1]>'9')){
      //on est arrivé à la fin du nombre
      if(neg) list_nb[ind] = list_nb[ind] * -1;
      neg = false;
      num = false;
      ind++;
    }
  }
  //on etudie le cas particulier du dernier caractère,ind
  if(strlen(s)>1 && s[len-1]>='0' && s[len-1]<='9'){
    list_nb[ind] = list_nb[ind]*10 + s[len-1] - '0';
    ind++;
  }
  //on réalloue la memoire pour gagner de la place
  int*res = (int*)malloc(ind*sizeof(int));
  assert(res!=NULL);
  for(int i = 0; i < ind; i++){
    res[i]=list_nb[i];
  }
  free(list_nb);
  *reslen = ind;
  return res;
}

//fonction qui convertit une liste de chaine de caractère en fnc
fnc str_to_fnc(char**lines,int n){
  int ind=0,len = 0;
  //on cherche la ligne p cnf XX XX
  while(lines[ind][0] == 'c'|| (lines[ind][0] != 'p' && lines[ind][1] != ' ' && lines[ind][2] != 'c' && lines[ind][3] != 'n' && lines[ind][4] != 'f' && ind < n)) ind++;
  int*all_int = get_all_int(lines[ind],&len);
  if(ind==n || len!=2){
    printf("Le fichier ne comporte pas de ligne 'p cnf XX XX' valide\nConseil : essayez de supprimer les espaces devant le p");
    assert(len >= 2 && ind!=n);
  }
  int nb_clause = all_int[1];
  assert(nb_clause >= 0);
  clause*c = (clause*)malloc(nb_clause*sizeof(clause));
  //on convertit toutes les lignes qui suivent la ligne p cnf en clauses
  for(int i = ind+1; i < nb_clause+ind+1; i++){
    int lenl = 0;
    int*list_lit = get_all_int(lines[i],&lenl);
    if(lenl <= 1) c[i-(ind+1)] = create_clause(list_lit,0);
    else c[i-(ind+1)] = create_clause(list_lit,lenl-1);
  }
  //on creer la fnc
  fnc f = create_fnc(c,nb_clause,all_int[0]);
  free(all_int);
  return f;
}

//fonction qui convertit un fichier cnf au format DIMACS en fnc
fnc file_to_fnc(char*file){
  int len=0,max=0,clen=0;
  //on a besoin d'ouvrir deux fois le fichier
  //(une fois pour determiner les caractéristiques et l'autre fois pour en exraire les lignes)
  FILE*f1 = fopen(file,"r");
  FILE*f2 = fopen(file,"r");
  if(f1==NULL || f2==NULL){
    printf("Fichier non trouvé\n");
    assert(f1!=NULL && f2 != NULL);
  }
  //on combine les fonctions précédentes pour convertir le fichier en fnc
  nb_lines(f1,&len,&max);
  fclose(f1);
  char**c = get_lines(f2,len,max,&clen);
  fclose(f2);
  fnc f = str_to_fnc(c,clen);
  for(int i = 0; i < clen; i++) free(c[i]);
  free(c);
  return f;
}
