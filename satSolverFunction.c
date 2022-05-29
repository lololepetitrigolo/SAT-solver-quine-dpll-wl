#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Type/types.h"
#include "Type/fnc.h"
#include "Type/clause.h"
#include "Type/litteral.h"

//fonction vérifiant si la clause vide apparait dans fnc f
bool mem_vide (fnc f){
  for(int i = 0; i < f->len; i++) {
    if(f->c[i]->len == 0) return true;
  }
  return false;
}

//renvoie le plsu grand litteral présent dans une fnc
litteral find_max_lit(fnc f){
  litteral max = f->c[0]->l[0];
  for(int i = 0; i < f->len; i++) {
      for(int j = 0; j < f->c[i]->len; j++){
        if(max < abs_lit(f->c[i]->l[j])) max = abs_lit(f->c[i]->l[j]);
      }
  }
  return max;
}

//fonction qui détermine si le littéral x est pr&sent dans une liste de littéraux
bool mem(litteral x,int*l,int len){
  for(int i = 0; i < len; i++){
    if(l[i]==x) return true;
  }
  return false;
}

//fonction qui determine le nombre de clause contenant le litteral x
int nb_clause_with_x(fnc f,litteral x){
  int res = 0;
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      if(f->c[i]->l[j] == x){
        res++;
        break;
      }
    }
  }
  return res;
}

//fonction qui cherche une clause unitaire et renvoie le litteral de cette clause (renvoie 0 si il n'y en a pas)
litteral unit_clause(fnc f){
  for(int i = 0; i < f->len; i++){
    if(f->c[i]->len == 1) return f->c[i]->l[0];
  }
  return 0;
}

//supprime les clauses avec x et retire non x des clauses
fnc suppr(litteral x, fnc f){
  clause*c = (clause*)malloc((f->len - nb_clause_with_x(f,x))*sizeof(clause));
  int size_nf = 0;
  for(int i = 0; i < f->len; i++){
    litteral*l = (litteral*)malloc((f->c[i]->len)*sizeof(litteral));
    assert(l != NULL);
    int size_nc = 0;
    for (int j = 0; j < f->c[i]->len; j++){
      if(f->c[i]->l[j] == x){// le littéral x apparait dans la clause on ne la prend pas en compte
        size_nc = -1;
        free(l);
        break;
      }else if(abs_lit(f->c[i]->l[j]) != abs_lit(x)){//le litteral est ni x ni non x
        l[size_nc] = f->c[i]->l[j]; //on ajoute le litteral à la clause
        size_nc++;
      }
    }
    if(size_nc == 0){//on a une clause vide
      l[0] = 0;
      c[size_nf] = create_clause(l,0);
      size_nf++;
    }else if(size_nc != -1){//on créer et ajoute notre clause a notre fnc finale
      c[size_nf] = create_clause(l,size_nc);
      size_nf++;
    }
  }
  fnc nf = create_fnc(c,size_nf,f->nb_lit - 1);
  return nf;
}

//renvoie le maximum de deux entier
int max(int a,int b){
  if(a<=b)return b;
  return a;
}

//renvoie le minimum de deux entier
int min(int a,int b){
  if(a<=b)return a;
  return b;
}

// pour faire les tests renvoie true si les variables affecté dans notre model satisfont la clause
bool is_sat(clause c,int*M,int len){
  for(int i = 0; i < c->len; i++) {
    if(mem(c->l[i],M,len)) return true;
  }
  return false;
}

//pour faire les tests et verifié que on a bien un model
bool est_model(int*M,int ind_M,fnc f){
  for(int i = 0; i < f->len; i++) {
    if(!is_sat(f->c[i],M,ind_M)) return false;
  }
  return true;
}

//fonction qui regarde si un littéral est présent en valeur absolut dans la liste
//très utile pour savoir si on a affecté une valeur à une variable dans notre modele
bool mem_abs(litteral x,int*l,int len){
  for(int i = 0; i < len; i++){
    if(abs_lit(l[i])==abs_lit(x)) return true;
  }
  return false;
}

void fill_model(int*M,int ind_M,fnc f,int**res,int*len_res){
  int len = 0;
  int*mod = (int*)malloc(f->nb_lit*sizeof(int));
  //on commence par supprimer les zero
  for(int i = 0; i < ind_M; i++){
    if(M[i] != 0){
      mod[len]=M[i];
      len++;
    }
  }
  if(len!=f->nb_lit){//si il nous manque des littéraux on les cherche
    for(int i = 0; i < f->len; i++) {
      for(int j = 0; j < f->c[i]->len; j++) {
        if(!mem_abs(f->c[i]->l[j],mod,len)){
          mod[len]=abs_lit(f->c[i]->l[j]);//on decide d'affecter les variables à vrai (on pourrait choisir faux)
          len++;
          if(len!=f->nb_lit) break; //on évite de parcourir toute la fnc si on les a tous
        }
        if(len!=f->nb_lit) break; //on évite de parcourir toute la fnc si on les a tous
      }
    }
  }
  *res = mod;
  *len_res = len;
}
