#ifndef _SLIST__
#define _SLIST__
#include "records.h"
typedef struct snode snode;

typedef struct snode
	{
	int key;//citizen id
	time_t* date;//pointer to a date of vaccination if there is one
	record* rec;//pointer to the record
	snode* above;
	snode* below;
	snode* prev;
	snode* next;
	}snode;

typedef struct
	{
	int height;//current height of skiplisy
	int size;//number of snodes excluding headers
	snode* header;//upper left header
	}skiplist;

int skiplist_init(skiplist* slst);//initializes skip list
int coinflip();//returns 1 for heads 0 for tails
snode* skiplist_search(skiplist* slst,int key);//returns max id snode thats less or equal than ours
int skiplist_check(skiplist* slst,int key);//returns 0 if it finds it else 1 
int skiplist_insert(skiplist* slst,int key,record* rec,time_t* date);//inser new person if he is  not vaccinated  date should be NULL returns 0 if it already exist else 1
int skiplist_delete(skiplist* slst,int key);//deletes snode with the key in all levels returns 1 if its succesful else 0
void skiplist_free(skiplist* slst);//frees skiplist memory
//exrtras
int skiplist_check_vaccineStatus(skiplist* slst,int key);//special function for vaccineStatus function basicly its the same with with skiplist_check but  prints different messages
#endif