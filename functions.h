//functions used by the commands
#ifndef _FUNCTIONS__
#define _FUNCTIONS__
#include "hash.h"
#include "bloomfilter.h"
#include "skiplist.h"
#include "hashtable.h"
#include "records.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define hashvalue 137//hashvalue
//return 0 if its succesful else 1
int vaccineStatusBloom(hashtable* viruses,char* virusName,char* citizenID);

int vaccineStatusA(hashtable* viruses,char* virusName,int citizenID);//vaccineStatus for specific virus

int vaccineStatusB(hashtable* viruses,int citizenID);//vaccineStatus for all virus


int populationStatusA(hashtable* countries ,hashtable* viruses,char* country,char* virusName,time_t* a,time_t* b);//populationStatus command for the country given dates are NULL it doesnt check for vaccination date

typedef struct 
{
char* country;
int positive_count;
int negative_count;
}ps;//struct used for populationStatusB function no worry for the country pointer because its going to be freed later by the countries hashtable

int populationStatusB(hashtable* countries ,hashtable* viruses,char* virusName,time_t* a,time_t* b);//populationStatus command for all countries

int popStatusByAgeA(hashtable* countries ,hashtable* viruses,char* country,char* virusName,time_t* a,time_t* b);//popStatusByAge command for the country given dates are NULL it doesnt check for vaccination date

typedef struct 
{
char* country;
int positive_count[4];//positive_count[0] for age group 0-20,positive_count[1] for age group 20-40 etc
int negative_count[4];//negative_count[0] for age group 0-20,negative_count[1] for age group 20-40 etc
}psa;//struct used for popStatusByAgeB function no worry for the country pointer because its going to be freed later by the countries hashtable

int popStatusByAgeB(hashtable* countries ,hashtable* viruses,char* virusName,time_t* a,time_t* b);//popStatusByAge command for all countries

int list_nonVaccinated_Persons(hashtable*viruses,char* virusName);
#endif