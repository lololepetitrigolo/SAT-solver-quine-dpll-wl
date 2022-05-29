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
#include "Type/liste.h"
#include "satSolverFunction.h"
#include "analyse.h"
#include "logiquewl.h"

//renvoie true si une variable apparait sous sa forme positive et négative dans une même clause
//utile pour réduire la clause dès le depart
bool trivial_clause(clause c){
  for(int i = 0; i < c->len; i++) {
    for(int j = i; j < c->len; j++) {
      if(c->l[i] == -c->l[j]) return true;
    }
  }
  return false;
}

//renvoie une fnc sans doublon dans les clauses
void suprr_doubl(int*l,int*size_nc,clause c){
  for(int i = 0; i < c->len; i++) {
    bool b = true;
    for(int j = i+1; j < c->len; j++) {
      if(c->l[i] == c->l[j]) b=false;
    }
    if(b){
      l[*size_nc] = c->l[i];
      (*size_nc)++;
    }
  }
}

//suprime les doublons dans les clauses et les clauses forcément satisfaite
fnc pre_traitement(fnc f){
  clause*c = (clause*)malloc(f->len*sizeof(clause));
  int size_nf = 0;
  for(int i = 0; i < f->len; i++){
    litteral*l = (litteral*)malloc((f->c[i]->len)*sizeof(litteral));
    assert(l != NULL);
    int size_nc = 0;
    bool is_trivial = trivial_clause(f->c[i]);//clause avec x et non x
    suprr_doubl(l,&size_nc,f->c[i]);//suprimme les doublons des clauses
    if(!is_trivial){
      c[size_nf] = create_clause(l,size_nc);
      size_nf++;
    }else free(l);
  }
  fnc nf = create_fnc(c,size_nf,f->nb_lit);
  free_fnc(f);
  return nf;
}

//supprime les clauses unitaires
fnc unit_propagation(int*M,int*ind,bool*unsat,fnc f){
  litteral x;
  while((x = unit_clause(f)) != 0){
    fnc nf = suppr(x,f);
    free_fnc(f);
    M[*ind] = x;//on stocke les choix fait pour le modèle
    (*ind)++;
    f = nf;
    //on regarde que l'on ait pas une fnc vide ou de clause vide
    if(f->len == 0) return f;
    if(mem_vide(f)){*unsat=true; return f;}
  }
  return f;
}

// renvoie true si les variables affectées dans notre model satisfont la clause
bool is_sat_clause(clause c,int*M,int len){
  for(int i = 0; i < c->len; i++) {
    if(mem(c->l[i],M,len)) return true;
  }
  return false;
}


//renvoie la taille de la clause en prenant en compte les affectation
int size_clause(int*M,int ind_M,clause c){
  int cmp = 0;
  for(int i = 0; i < c->len; i++){
    if(!mem_abs(c->l[i],M,ind_M)) cmp++; //on compte les variables non affectée
  }
  return cmp;
}

//revoie la taille de la plus grande clause en prenant en compte les affectations
int max_size_clause_wl(fnc f,int*M,int ind_M){
  int max_size = 0;
  for(int i = 0; i < f->len; i++) {
    if(max_size < size_clause(M,ind_M,f->c[i]) && !is_sat_clause(f->c[i],M,ind_M)) max_size=size_clause(M,ind_M,f->c[i]);
  }
  return max_size;
}

//revoie la taille de la plus petite clause en prenant en compte les affectations
int min_size_clause_wl(fnc f,int*M,int ind_M){
  int min_size = 2147483647; //max_size_clause_wl(f,M,ind_M);
  for(int i = 0; i < f->len; i++) {
    if(min_size > size_clause(M,ind_M,f->c[i]) && !is_sat_clause(f->c[i],M,ind_M)){
      min_size=size_clause(M,ind_M,f->c[i]);
    }
  }
  return min_size;
}
//------------------------- Heuristique WL -------------------------------------
//heuristique -max renvoie le littéral qui apparait le plus en prenant en compte les affectations
int max_apparition_wl(fnc f,int*M,int ind_M){
  int max_lit = find_max_lit(f);
  int*apparition = (int*)malloc((2*max_lit+1)*sizeof(int));
  for(size_t i = 0; i < 2*max_lit+1; i++) apparition[i]=0;
  //on compte les apparitions
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++) {
      if(!mem_abs(f->c[i]->l[j],M,ind_M)) apparition[max_lit + f->c[i]->l[j]]++;
    }
  }
  //on cherche le maximum dans la liste
  int max = 0,res = 0;
  for(int i = 0; i < 2*max_lit+1; i++){
    if(max < apparition[i]){
      max = apparition[i];
      res = i -max_lit;
    }
  }
  free(apparition);
  return res;
}

//renvoie un littéral aléatoire d'une fnc en prenant en compte les affectations
int rand_wl(fnc f,int*M,int ind_M){
  srand(time(NULL));
  int max_lit = find_max_lit(f), ind = 0;
  int*apparition = (int*)malloc((2*max_lit+1)*sizeof(int));
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++) {
      if(!mem_abs(f->c[i]->l[j],M,ind_M) && !mem(f->c[i]->l[j],apparition,ind)){
        apparition[ind] = f->c[i]->l[j];
        ind++;
      }
    }
  }
  //on tire un littéral au hasard dans la liste
  int i = rand() % ind;
  litteral res = apparition[i];
  free(apparition);
  return res;
}

//renvoie un littéral apparaisant dans une clause de taille minimale en prenant en compte les affectations
litteral min_clause_wl(fnc f,int*M,int ind_M){
  //on cherche la taille minimale des clauses
  int min_size = min_size_clause_wl(f,M,ind_M);
  for(int i = 0; i < f->len; i++) {
    if(size_clause(M,ind_M,f->c[i]) == min_size){
      for(int j = 0; j < f->c[i]->len; j++) {
        if(!mem_abs(f->c[i]->l[j],M,ind_M)) return f->c[i]->l[j];
      }
    }
  }
  return 0;
}

//implémente l'heuristic MOMS
litteral moms_wl(fnc f,int*M,int ind_M){
  //on cherche la taille minimale des clauses
  int min_size = min_size_clause_wl(f,M,ind_M);
  //on determine la plus grande varriable
  litteral max_lit = find_max_lit(f);
  int*lit_list = (int*)malloc((max_lit*2 + 1)*sizeof(int));
  for(int i = 0; i < max_lit*2 + 1; i++) lit_list[i] = 0;
  //on compte le nombre de fois que les littéraux apparaisent dans des clauses de taille minimale
  for(int i = 0; i < f->len; i++){
    if(!is_sat_clause(f->c[i],M,ind_M) && size_clause(M,ind_M,f->c[i]) == min_size){
      for(int j = 0; j < f->c[i]->len; j++){
        if(!mem_abs(f->c[i]->l[j],M,ind_M)) lit_list[f->c[i]->l[j] + max_lit]++;
      }
    }
  }
  //on cherche le maximum dans la liste qui n'est pas dans notre model
  litteral res = 0;
  int max = 0;
  for(int i = 0; i < 2*max_lit+1; i++) {
    if(lit_list[i] > max && !mem_abs(i - max_lit,M,ind_M)){
      max = lit_list[i];
      res = i - max_lit;
    }
  }
  free(lit_list);
  return res;
}

//équivalent de first(ie renvoie le premier littéral non affecté)
litteral choose_lit(fnc f,int*M,int ind_M){
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++) {
      if(!mem_abs(f->c[i]->l[j],M,ind_M)) return f->c[i]->l[j];
    }
  }
  return 0;
}

//-------------------------two watched literals (TWL)--------------------------
void init_twl(liste*wl,liste*clause_wl,int max_lit,fnc f){
  for(int i = 0; i < 2*max_lit+1; i++) wl[i]=NULL;
  for(int i = 0; i < f->len; i++) clause_wl[i]=NULL;
  //comme on a supprimé les clauses unitaires toutes les clauses sont de taille 2 minimum
  for(int i = 0; i < f->len; i++){
    //on ajout la clause i dans wl à l'indice adéquat
    wl[max_lit + f->c[i]->l[0]] = add(i,wl[max_lit + f->c[i]->l[0]]);
    wl[max_lit + f->c[i]->l[1]] = add(i,wl[max_lit + f->c[i]->l[1]]);
    //on ajout à la clause i les littéraux concernés
    clause_wl[i]=add(f->c[i]->l[0],clause_wl[i]);
    clause_wl[i]=add(f->c[i]->l[1],clause_wl[i]);
  }
}


bool twl_propagation(int*M,int*ind_M,liste*wl,int max_lit,liste*clause_wl,litteral lit,fnc f){
  liste prop = create_liste(lit);
  while(prop != NULL){
    litteral l = prop->val;
    prop = prop->next;
    liste parcoureur = wl[max_lit - l];
    while(parcoureur != NULL){ //on parcourt les clauses concernées
      int clause = parcoureur->val;
      parcoureur = parcoureur->next;
      if(!is_sat_clause(f->c[clause],M,*ind_M)){//on cherche un nouveau wl si la clause n'est pas satisfaite
        litteral x,new_wl = 0;
        if(clause_wl[clause]->val == -l) x = clause_wl[clause]->next->val;
        else x = clause_wl[clause]->val;

        //on cherche un nouveau wl
        for (size_t i = 0; i < f->c[clause]->len; i++) {
          if(!mem_abs(f->c[clause]->l[i],M,*ind_M) && f->c[clause]->l[i] != x) new_wl = f->c[clause]->l[i];
        }
        //si on a pas trouvé et que le deuxieme wl est faux il y a conflit
        if(mem(-x,M,*ind_M) && new_wl == 0) return false;

        if(new_wl != 0){//on a trouvé notre nouveau wl
          wl[max_lit - l] = del(clause,wl[max_lit - l]);
          clause_wl[clause] = del(-l,clause_wl[clause]);
          clause_wl[clause] = add(new_wl,clause_wl[clause]);
          wl[max_lit + new_wl] = add(clause,wl[max_lit + new_wl]);
        }else{//clause unitaire on propage
          M[*ind_M] = x;
          (*ind_M) = (*ind_M) + 1;
          prop = add(x,prop);
        }
      }
    }
  }
  return true;
}
//------------------------------------------------------------------------------

/*fonction basé sur l'algorithme de quine et l'heuristique watched literals
  renovoie true si la fnc est satisifiable et un modele partiel valide (M)*/
bool quine_wl (int*M,int*ind_M,liste*wl,int max_lit,liste*clause_wl,litteral (*heuristic) (fnc,int*,int),fnc f){
  if(*ind_M == f->nb_lit || est_model(M,*ind_M,f)) return true;
  //on choisit le littéral
  litteral x = (*heuristic)(f,M,*ind_M);

  int back = *ind_M;//on sauvegarde notre position
  M[*ind_M] = x;
  (*ind_M)++;
  //on propage avec x
  if(twl_propagation(M,ind_M,wl,max_lit,clause_wl,x,f) && quine_wl(M,ind_M,wl,max_lit,clause_wl,heuristic,f)) return true;

  (*ind_M) = back;//on retourne à notre position
  M[back] = -x;
  (*ind_M)++;
  //si en propageant avec x on a aboutit à un coflit on propage non x
  return twl_propagation(M,ind_M,wl,max_lit,clause_wl,-x,f) && quine_wl(M,ind_M,wl,max_lit,clause_wl,heuristic,f);
}

/*fonction qui créer les structures necessaire pour quine_wl,
  applique les simplifications possible sur la fnc de base
  et libère la mémoire une fois que quine_wl a renvoyé le résultat*/
bool quine_wl_init(int*M,int*len,litteral (*heuristic) (fnc,int*,int),fnc f){
  if(f->len == 0){free_fnc(f);return true;}
  if(mem_vide(f)){free_fnc(f);return false;}
  bool unsat = false;
  /*il faut supprimer les doublons dans les clauses pour le bon fonctionnement de twl_propagation
  et les clauses deja satisfaite*/
  f = pre_traitement(f);
  //on supprime les clauses unitaires si il y en a initialement
  f = unit_propagation(M,len,&unsat,f);

  if(f->len == 0){free_fnc(f);return true;}
  else if(unsat){free_fnc(f);return false;}

  //on initialise les tableau de liste qui vont nous servir pour watched litterals
  int max_lit = find_max_lit(f);
  liste*wl = (liste*)malloc((2*max_lit+1)*sizeof(liste));
  liste*clause_wl = (liste*)malloc(f->len*sizeof(liste));
  assert(wl != NULL && clause_wl != NULL);
  init_twl(wl,clause_wl,max_lit,f);

  //on appel la fonction pour savoir si la fnc est satisfiable
  bool res =  quine_wl(M,len,wl,max_lit,clause_wl,heuristic,f);

  //on libere la memoire prise par watched litterals
  for(int i = 0; i < 2*max_lit+1; i++) free_liste(wl[i]);
  free(wl);
  for(int i = 0; i < f->len; i++) free_liste(clause_wl[i]);
  free_fnc(f);
  free(clause_wl);

  return res;
}

void exe_wl(char*fichier,char*heur){
  //on convertit le fichier en fnc
  fnc f = file_to_fnc(fichier);
  fnc nf = file_to_fnc(fichier);

  int*M = (int*)malloc(f->nb_lit*sizeof(int));
  assert(M!=NULL);
  int len = 0;
  //on determine l'heuristique à utiliser
  litteral (*heuristic) (fnc,int*,int) = NULL;
  if(strcmp(heur,"first")==0) heuristic = &choose_lit;
  else if(strcmp(heur,"rand")==0) heuristic = &rand_wl;
  else if(strcmp(heur,"max")==0) heuristic = &max_apparition_wl;
  else if(strcmp(heur,"min")==0) heuristic = &min_clause_wl;
  else heuristic = &moms_wl;

  bool res = quine_wl_init(M,&len,heuristic,f);
  if(res){
    printf("s SATISFIABLE\n");
    int len_mod;
    int*mod;
    // on supprime les zero et on remplit le modele avce les litteraux qui ne serait pas affecté
    fill_model(M,len,nf,&mod,&len_mod);
    for(int i = 0; i < len_mod; i++) {
      printf("v %d\n",mod[i]);
    }
    assert(est_model(mod,len_mod,nf));
    free(mod);
  }else printf("s UNSATISFIABLE\n");
  free(M);
  free_fnc(nf);
}
