#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "Type/types.h"
#include "Type/fnc.h"
#include "Type/clause.h"
#include "Type/litteral.h"
#include "analyse.h"
#include "logique.h"
#include "satSolverFunction.h"

//remplit le premier tableau avec le deuxieme
void concat(int*l1,int*ind,int*l2,int len2) {
  for(int i = *ind; i < *ind+len2; i++) {
    l1[i]=l2[i - *ind];
  }
  *ind = *ind + len2;
}

//renvoie la taille de la plus petite clause d'une fnc
int min_size_clause(fnc f){
  int min_size = f->c[0]->len;
  for(int i = 1; i < f->len; i++) {
    if(min_size > f->c[i]->len) min_size=f->c[i]->len;
  }
  return min_size;
}

//renvoie la taille de la plus grande clause d'une fnc
int max_size_clause(fnc f){
  int max_size = f->c[0]->len;
  for(int i = 1; i < f->len; i++) {
    if(max_size < f->c[i]->len) max_size=f->c[i]->len;
  }
  return max_size;
}

//------------------ choix du litteral -----------------------------------------
//renvoie le premier littéral d'une fnc
litteral first(fnc f){
  assert(f!=NULL && f->len != 0);
  return f->c[0]->l[0];
}

//renvoie le littéral le plus présent dans une fnc
litteral max_apparition(fnc f){
  int ind=0,max=0;
  int*watch = (int*)malloc(2*(f->nb_lit)*sizeof(int));
  litteral res;
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      litteral x = f->c[i]->l[j];
      if(!mem(x,watch,ind)){//on regarde si on a déjà compter le nombre de litteraux x
        watch[ind]=x;
        ind++;
        int cmp = 1;
        for(int k = 0; k < f->len; k++){// on compte le nombre de fois qu'apparait x dans f
          for(int l = 0; l < f->c[k]->len; l++) {
            if(f->c[k]->l[l] == x) cmp++;
          }
        }
        if(cmp > max){//si c'est le maximum on le garde en mémoire
          res = x;
          max = cmp;
        }
      }
    }
  }
  free(watch);
  return res;
}

//renvoie un littéral apparaisant dans une clause de longeur minimale
litteral min_clause(fnc f){
  assert(f!=NULL && f->len != 0);
  int min = f->c[0]->len;
  litteral x = f->c[0]->l[0];
  for(int i = 1; i < f->len; i++){
    if(min > f->c[i]->len && f->c[i]->len != 0){
      min = f->c[i]->len;
      x=f->c[i]->l[0];
    }
  }
  return x;
}

//renvoie un littéral aléatoire d'une fnc
litteral random_lit_uni(fnc f){
  srand(time(NULL));
  int ind=0;
  int*watch = (int*)malloc((2*f->nb_lit)*sizeof(int));
  //on créer une liste des littéraux pour ne les compter qu'une seule fois
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      if(!mem(f->c[i]->l[j],watch,ind)){//on regarde si on a déjà ajouté x à watch
        watch[ind]=f->c[i]->l[j];
        ind++;
      }
    }
  }
  //on tire un littéral au hasard dans la liste
  int i = rand() % ind;
  litteral res = watch[i];
  free(watch);
  return res;
}

//implémente l'heuristic MOMS
litteral moms(fnc f){
  //on cherche la taille minimale des clauses
  int min_size = min_size_clause(f);
  //on determine la plus grande varriable

  litteral max_lit = find_max_lit(f);
  int*lit_list = (int*)malloc((max_lit*2 + 1)*sizeof(int));
  for(int i = 0; i < max_lit*2 + 1; i++) lit_list[i] = 0;
  //on compte le nombre de fois que les littéraux apparaisent dans des clauses de taille minimale
  for(int i = 0; i < f->len; i++) {
    if(f->c[i]->len == min_size){
      for(int j = 0; j < f->c[i]->len; j++) lit_list[f->c[i]->l[j] + max_lit] = lit_list[f->c[i]->l[j] + max_lit]+1;
    }
  }
  //on cherche le maximum dans la liste
  litteral res = -max_lit;
  int max = lit_list[0];
  for(int i = 1; i < max_lit*2+1; i++) {
    if(lit_list[i] > max){
      max = lit_list[i];
      res = i - max_lit;
    }
  }
  free(lit_list);
  return res;
}

//renvoie le nombre de clauses de taille k contenant le littéral x. (sert pour bohm)
int h(fnc f,litteral x,int k){
  int res = 0;
  for(int i = 0; i < f->len; i++) {
    if(f->c[i]->len == k){
      for(int j = 0; j < f->c[i]->len; j++) {
        if(f->c[i]->l[j] == x) res++;
      }
    }
  }
  return res;
}

//calcule la formule de BOHM
int sbohm(fnc f,litteral x,int k){
  int h_plus = h(f,x,k);
  int h_moins = h(f,-x,k);
  int alpha = 1,beta = 2;
  return alpha*max(h_plus,h_moins)+beta*min(h_plus,h_moins);
}

//implémente l'heuristic BOHM
litteral bohm(fnc f){
  litteral res = abs_lit(f->c[0]->l[0]);
  int max = sbohm(f,abs_lit(f->c[0]->l[0]),1);
  int*lit_list = (int*)malloc(f->nb_lit*sizeof(int));
  int*watch = (int*)malloc(f->nb_lit*sizeof(int));
  int ind_lit_list = 0,ind_watch = 0,k=min_size_clause(f);

  for(int i = 0; i < f->len; i++) {
    for(int j = 0; j < f->c[i]->len; j++) {
      if(!mem(abs_lit(f->c[i]->l[j]),watch,ind_watch) && sbohm(f,abs_lit(f->c[i]->l[j]),k) > max){
        watch[ind_watch] = abs_lit(f->c[i]->l[j]);
        ind_watch ++;
        max = sbohm(f,abs_lit(f->c[i]->l[j]),k);
        res = abs_lit(f->c[i]->l[j]);
        lit_list[0] = abs_lit(f->c[i]->l[j]);
        ind_lit_list = 1;
      }else if(!mem(abs_lit(f->c[i]->l[j]),watch,ind_watch) && sbohm(f,abs_lit(f->c[i]->l[j]),k) == max){
        watch[ind_watch] = abs_lit(f->c[i]->l[j]);
        ind_watch ++;
        lit_list[ind_lit_list] = abs_lit(f->c[i]->l[j]);
        ind_lit_list++;
      }
    }
  }
  k++;
  free(watch);

  while (ind_lit_list != 1  && k <= max_size_clause(f)) {
    int*watch = (int*)malloc(f->nb_lit*sizeof(int));
    for(int i = 0; i < f->len; i++) {
      for(int j = 0; j < f->c[i]->len; j++) {
        if(!mem(abs_lit(f->c[i]->l[j]),watch,ind_watch) && mem(abs_lit(f->c[i]->l[j]),lit_list,ind_lit_list) && sbohm(f,abs_lit(f->c[i]->l[j]),k) > max){
          watch[ind_watch] = abs_lit(f->c[i]->l[j]);
          ind_watch ++;
          max = sbohm(f,abs_lit(f->c[i]->l[j]),k);
          res = abs_lit(f->c[i]->l[j]);
          lit_list[0] = abs_lit(f->c[i]->l[j]);
          ind_lit_list = 1;
        }else if(!mem(abs_lit(f->c[i]->l[j]),watch,ind_watch) && mem(abs_lit(f->c[i]->l[j]),lit_list,ind_lit_list) && sbohm(f,abs_lit(f->c[i]->l[j]),k) == max){
          watch[ind_watch] = abs_lit(f->c[i]->l[j]);
          ind_watch ++;
          lit_list[ind_lit_list] = abs_lit(f->c[i]->l[j]);
          ind_lit_list++;
        }
      }
    }
    k++;
    free(watch);
  }

  //on chosit la polaritée
  int somme_h_moins = 0,somme_h_plus = 0;
  for(int i = 0; i <= k; i++) somme_h_plus = somme_h_plus + h(f,res,i);
  for(int i = 0; i <= k; i++) somme_h_moins = somme_h_moins + h(f,-res,i);
  free(lit_list);
  if(somme_h_moins>somme_h_plus) res = -res;
  return res;
}
//------------------------------------------------------------------------------

//fonction qui renvoie un littéral pur s'il existe sinon renvoie 0
litteral pure_lit(fnc f){
  int ind=0;
  int*watch = (int*)malloc((f->nb_lit)*sizeof(int));
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      litteral x = f->c[i]->l[j];
      if(!mem(abs_lit(x),watch,ind)){//on regarde si on a deja testé avec le litteral x
        watch[ind]=abs_lit(x);
        ind++;
        //on regarde si la negation du litteral apparait dans la fnc
        for(int k = i; k < f->len; k++){
          for(int l = 0; l < f->c[k]->len; l++) {
            if(f->c[k]->l[l] == -x){x=0;break;}
          }
          if(x==0) break;
        }
        if(x!=0){//on a trouvé notre littéral pur
          free(watch);
          return x;
        }
      }
    }
  }
  free(watch);
  return 0;
}

//implémentation de l'algorithme de quine
bool quine (fnc f,litteral (*heuristic) (fnc),int**l,int*ind){
 //la liste et l'indice en parramètre permet de stocker une éventuelle solution
 if(f->len == 0){//on regarde si on a affaire à une fnc vide
   free_fnc(f);
   return true;
 }
 if(mem_vide(f)){//on regarde si la fnc contient une clause vide
   free_fnc(f);
   return false;
 }
 litteral x = (*heuristic)(f); //on choisit le littéral
 if(quine(suppr(x,f),heuristic,l,ind)){//on teste avec x
   free_fnc(f);
   //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
   (*l)[*ind] = x;
   (*ind) = (*ind) + 1;
   return true;
 }
 bool res = quine(suppr(-x,f),heuristic,l,ind); //si avec x la fnc n'est pas satisfiable on teste avec non x
 if(res){
   //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
   (*l)[*ind] = -x;
   (*ind) = (*ind) + 1;
 }
 free_fnc(f);
 return res;
}

//implémentation de dpll
bool dpll(fnc f,litteral (*heuristic) (fnc),int**l,int*ind){
  if(f->len == 0){
    free_fnc(f);
    return true;
  }
  if(mem_vide(f)){
    free_fnc(f);
    return false;
  }
  int*mem = (int*)malloc((f->nb_lit)*sizeof(int));
  int i = 0;
  assert(mem != NULL);
  litteral x;
  while((x = unit_clause(f)) != 0){//on supprime les clauses unitaires
    fnc nf = suppr(x,f);
    free_fnc(f);
    mem[i] = x;//stocke les choix fais pour le modèle
    i++;
    f = nf;
    //on regarde que l'on ait pas une fnc vide ou de clause vide
    if(f->len == 0){concat(*l,ind,mem,i);free_fnc(f);free(mem);return true;}
    if(mem_vide(f)){free(mem);free_fnc(f);return false;}
  }
  while((x = pure_lit(f)) != 0){//on suppprime les littéraux purs
    fnc nf = suppr(x,f);
    mem[i] = x;//stocke les choix fais pour le modèle
    i++;
    free_fnc(f);
    f = nf;
    //on regarde que l'on ait pas une fnc vide ou de clause vide
    if(f->len == 0){concat(*l,ind,mem,i);free_fnc(f);free(mem);return true;}
    if(mem_vide(f)){free(mem);free_fnc(f);return false;}
  }

  x = (*heuristic)(f);//on choisit un littéral
  if(dpll(suppr(x,f),heuristic,l,ind)){//on teste avec x
    //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
    (*l)[*ind] = x;
    (*ind) = (*ind) + 1;
    concat(*l,ind,mem,i);
    free(mem);
    free_fnc(f);
    return true;
  }
  bool res = dpll(suppr(-x,f),heuristic,l,ind); //si avec x la fnc n'est pas satisfiable on teste avec non x
  if(res){
    //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
    (*l)[*ind] = -x;
    (*ind) = (*ind) + 1;
    concat(*l,ind,mem,i);
  }
  free(mem);
  free_fnc(f);
  return res;
}

/*fonction qui combine l'algorithme à utiliser et l'heuristique
  qui affiche si la fnc provenant de fichier est satisfiable ou non
  et si c'est le cas affiche un modèle de la fnc*/
void exe(char*fichier,char*algo,char*heur) {
  //on convertit le fichier en fnc
  fnc f = file_to_fnc(fichier);
  fnc nf = file_to_fnc(fichier);

  int*l = (int*)malloc((f->nb_lit)*sizeof(int));
  assert(l!=NULL);
  int ind = 0;
  bool res;
  litteral (*heuristic) (fnc) = NULL;
  //sélection de l'heuristique
  if(strcmp(heur,"first")==0) heuristic = &first;
  else if(strcmp(heur,"rand")==0) heuristic = &random_lit_uni;
  else if(strcmp(heur,"min")==0) heuristic = &min_clause;
  else if(strcmp(heur,"max")==0) heuristic = &max_apparition;
  else if(strcmp(heur,"bohm")==0) heuristic = &bohm;
  else heuristic = &moms;
  //sélection de l'algo
  if(strcmp(algo,"quine")==0) res = quine(f,heuristic,&l,&ind);
  else res = dpll(f,heuristic,&l,&ind);
  //on exploite le résultat
  if(res){
    printf("s SATISFIABLE\n");
    int len_mod;
    int*mod;
    fill_model(l,ind,nf,&mod,&len_mod);
    /*for(int i = 0; i < len_mod; i++) {
      printf("v %d\n",mod[i]);
    }*/
    assert(est_model(mod,len_mod,nf));
    free(mod);
  }else printf("s UNSATISFIABLE\n");
  free(l);
  free_fnc(nf);
}
