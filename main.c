#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logique.h"
#include "logiquewl.h"

//fonction qui renvoie la taille du plus grand paramètre donné en argument
int max_length_arg (const char**argv,int argc){
  int m = 0;
  for(int i = 0; i < argc; i++){
    if(strlen(argv[i])>m) m=strlen(argv[i]);
  }
  return m;
}

//fonction qui analyse les arguments donnés en paramètre
void extract_information(const char**argv,int argc,char**algo,char**heuristic,char**filename) {
  for(int i = 1; i < argc-1; i++) {
    if(strcmp(argv[i],"-algo")==0 && i < argc-2){
      strcpy(*algo,argv[i+1]);
    }else if(strcmp(argv[i],"-min")==0){
      strcpy(*heuristic,"min");
    }else if(strcmp(argv[i],"-max")==0){
      strcpy(*heuristic,"max");
    }else if(strcmp(argv[i],"-first")==0){
      strcpy(*heuristic,"first");
    }else if(strcmp(argv[i],"-rand")==0){
      strcpy(*heuristic,"rand");
    }else if(strcmp(argv[i],"-moms")==0){
      strcpy(*heuristic,"moms");
    }else if(strcmp(argv[i],"-bohm")==0){
      strcpy(*heuristic,"bohm");
    }
  }
  strcpy(*filename,argv[argc-1]);
}

int main(int argc, char const *argv[]) {
  int s = max_length_arg(argv,argc);
  char*algo = (char*)malloc((s+1)*sizeof(char));
  char*heuristic = (char*)malloc((s+1)*sizeof(char));
  char*filename = (char*)malloc((s+1)*sizeof(char));
  extract_information(argv,argc,&algo,&heuristic,&filename);
  //on determine quel algorithme et heuristique utiliser
  if(strcmp(algo,"wl") == 0) exe_wl(filename,heuristic);
  else exe(filename,algo,heuristic);//on affiche si la fnc est satisifiable(et son modele) ou si elle ne l'est pas
  free(algo);
  free(heuristic);
  free(filename);
  return 0;
}

//fonction main utilisé pour faire les tests
/*
int main(int argc, char const *argv[]) {
  int d = 1,fin =1001;
  char debut[1000] = "FichierTest/Sat/UF50.218.1000/uf50-0";
  time_t depart, arrivee;

  time(&depart);
  printf("Quine first\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);;
    exe(deb,"quine","first");
  }
  time(&arrivee);
  printf("temps d'execution : %f s \n", difftime(arrivee, depart));
  time(&depart);
  printf("Quine max\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"quine","max");
  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("Quine min\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"quine","min");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("Quine rand\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"quine","rand");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("Quine moms\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"quine","moms");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("Quine bohm\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"quine","bohm");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));

  time(&depart);
  printf("DPLL first\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","first");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("DPLL max\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","max");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("DPLL min\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","min");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("DPLL rand\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","rand");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("DPLL moms\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","moms");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("DPLL bohm\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe(deb,"dpll","bohm");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("WL first\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe_wl(deb,"first");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("WL max\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe_wl(deb,"max");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("WL min\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe_wl(deb,"min");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("WL rand\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe_wl(deb,"rand");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  time(&depart);
  printf("WL moms\n");
  for (int i = d; i < fin; i++) {
    char filename[200];
    char deb[100];
    strcpy(deb,debut);
    sprintf(filename,"%d.cnf", i);
    strcat(deb,filename);
    exe_wl(deb,"moms");

  }
  time(&arrivee);
  printf("temps d'execution : %fs \n", difftime(arrivee, depart));
  return 0;
}
*/
