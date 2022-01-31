#include "functions.h"

int vaccineStatusBloom(hashtable* viruses,char* virusName,char* citizenID)
	{
	if(viruses==NULL|| virusName==NULL){return 1;}
	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));

	if(vbucket==NULL){return 1;}
	if(vbucket->data==NULL){return 1;}
	virus* vdata=(virus*) vbucket->data;
	bloom* bb=vdata->not_vaccinated_bloom;
	if (bb==NULL|| strcmp(virusName,(char*)vdata->virus_name)){return 1;}
	if(bloom_check(bb,citizenID))
		printf("NOT VACCINATED\n");
	else
		printf("MAYBE\n");
	return 0;
	}
	
int vaccineStatusA(hashtable* viruses,char* virusName,int citizenID)
	{
	if(viruses==NULL|| virusName==NULL){return 1;}
	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));
	if(vbucket==NULL){return 1;}
	if(vbucket->data==NULL){return 1;}
	virus* vdata=(virus*) vbucket->data;
	skiplist* vaccinated_persons=vdata->vaccinated_persons;
	if (vaccinated_persons==NULL|| strcmp(virusName,(char*)vdata->virus_name)){return 1;}
	skiplist_check_vaccineStatus(vaccinated_persons,citizenID);
	return 0;
	}
int vaccineStatusB(hashtable* viruses,int citizenID)
	{
	int count=viruses->count;
	int buckets=viruses->bucket_num;
	int i=-1;
	bucket* curr;
	virus* vdata;
	skiplist* vaccinated_persons;
	while(i<buckets && count>0)
		{
		i++;
		curr=viruses->buckets[i];
		while(curr!=NULL)
			{
			count--;
			vdata=curr->data;
			if(vdata!=NULL)
				{
				vaccinated_persons=vdata->vaccinated_persons;
				if(vaccinated_persons!=NULL)
					{
					printf("%s ",(char*) vdata->virus_name);
					skiplist_check_vaccineStatus(vaccinated_persons,citizenID);
					}
				else
					return 1;
				}
			else
				return 1;
			curr=curr->next;
			}
		}
	return 0;
	}

int populationStatusA(hashtable* countries ,hashtable* viruses,char* country,char* virusName,time_t* a,time_t* b)
	{
	if(country==NULL||countries==NULL||viruses==NULL||virusName==NULL){return 1;}
	int flag_date=0;
	if(a==NULL && b==NULL){flag_date=1;}
	else if(a==NULL||b==NULL){printf("one of the dates is NULL\n");return 1;}


	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));
	if(vbucket==NULL || vbucket->data==NULL){printf("cant find specific virus\n");return 1;}

	int positive_count=0;
	int negative_count=0;

	virus* vdata=(virus*)vbucket->data;

	skiplist* positive_skiplist=vdata->vaccinated_persons;
	snode* positive_node=positive_skiplist->header;while(positive_node->below!=NULL){positive_node=positive_node->below;}
	skiplist* negative_skiplist=vdata->not_vaccinated_persons;
	snode* negative_node=negative_skiplist->header;while(negative_node->below!=NULL){negative_node=negative_node->below;}

	int max=positive_skiplist->size;if(negative_skiplist->size>max){max=negative_skiplist->size;}

	for (int i = 0; i < max; ++i)
		{
		if(positive_node->next!=NULL)
			{
			positive_node=positive_node->next;
			if(strcmp(positive_node->rec->country,country)==0 && (flag_date || date_between(*a,*positive_node->date,*b)))
				positive_count++;
			}
		
		if (negative_node->next!=NULL)
			{
			negative_node=negative_node->next;
			if(strcmp(negative_node->rec->country,country)==0)
				negative_count++;
			}
		
		}
	//printf("positive_count %d negative_count %d\n",positive_count,negative_count);
	float percent=(float)100*positive_count/(positive_count+negative_count);
	printf("%s:\n%d %f%%\n",country,positive_count,percent);
	return 0;
	}

int populationStatusB(hashtable* countries ,hashtable* viruses,char* virusName,time_t* a,time_t* b)
	{

	if(countries==NULL||viruses==NULL||virusName==NULL){return 1;}
	int flag_date=0;
	if(a==NULL && b==NULL){flag_date=1;}
	else if(a==NULL||b==NULL){printf("one of the dates is NULL\n");return 1;}

	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));
	if(vbucket==NULL || vbucket->data==NULL){printf("cant find specific virus\n");return 1;}

	hashtable* ps_hashtable=(hashtable*)malloc(sizeof(hashtable));ps_hashtable=hashtable_init(ps_hashtable,countries->count,2);
	bucket* ps_bucket;
	ps* ps_data;

	virus* vdata=(virus*)vbucket->data;

	skiplist* positive_skiplist=vdata->vaccinated_persons;
	snode* positive_node=positive_skiplist->header;while(positive_node->below!=NULL){positive_node=positive_node->below;}
	skiplist* negative_skiplist=vdata->not_vaccinated_persons;
	snode* negative_node=negative_skiplist->header;while(negative_node->below!=NULL){negative_node=negative_node->below;}

	int max=positive_skiplist->size;if(negative_skiplist->size>max){max=negative_skiplist->size;}

	for (int i = 0; i < max; ++i)
		{
		if(positive_node->next!=NULL)
			{
			positive_node=positive_node->next;
			if(flag_date || date_between(*a,*positive_node->date,*b))
				{
				ps_bucket=hashtable_insert(ps_hashtable,hash_i(positive_node->rec->country,hashvalue));
				ps_data=(ps*) ps_bucket->data;
				if(ps_data==NULL)
					{
					ps_bucket->data=(ps*) malloc(sizeof(ps));
					ps_data=(ps*) ps_bucket->data;
					ps_data->country=positive_node->rec->country;
					ps_data->positive_count=1;
					ps_data->negative_count=0;
					}
				else
					ps_data->positive_count+=1;
				}
			}
		if (negative_node->next!=NULL)
				{
				negative_node=negative_node->next;
				ps_bucket=hashtable_insert(ps_hashtable,hash_i(negative_node->rec->country,hashvalue));
				ps_data=(ps*) ps_bucket->data;
				if(ps_data==NULL)
					{
					ps_bucket->data=(ps*) malloc(sizeof(ps));
					ps_data=(ps*) ps_bucket->data;
					ps_data->country=negative_node->rec->country;
					ps_data->positive_count=0;
					ps_data->negative_count=1;
					}
				else
					ps_data->negative_count+=1;
				}
		
		}
	int count=0;
	for (int i = 0; i < ps_hashtable->bucket_num; ++i)
		{
		ps_bucket=ps_hashtable->buckets[i];
		while(ps_bucket!=NULL)
			{
			count++;
			ps_data=(ps*) ps_bucket->data;
			if(ps_data!=NULL)
				{
				float percent=(float)100*ps_data->positive_count/(ps_data->positive_count+ps_data->negative_count);
				printf("%s:\n%d %f%%\n",ps_data->country,ps_data->positive_count,percent);
				}
			
			ps_bucket=ps_bucket->next;
			}
		if(ps_hashtable->count<=count)
			break;
		}
	hashtable_free(ps_hashtable);return 0;
	}

int popStatusByAgeA(hashtable* countries ,hashtable* viruses,char* country,char* virusName,time_t* a,time_t* b)
	{
	if(country==NULL||countries==NULL||viruses==NULL||virusName==NULL){return 1;}
	int flag_date=0;
	if(a==NULL && b==NULL){flag_date=1;}
	else if(a==NULL||b==NULL){printf("one of the dates is NULL\n");return 1;}

	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));
	if(vbucket==NULL || vbucket->data==NULL){printf("cant find specific virus\n");return 1;}

	int age=0;
	int positive_count[4];
	int negative_count[4];
	for (int i = 0; i < 4; ++i){positive_count[i]=0;negative_count[i]=0;}
	virus* vdata=(virus*)vbucket->data;

	skiplist* positive_skiplist=vdata->vaccinated_persons;
	snode* positive_node=positive_skiplist->header;while(positive_node->below!=NULL){positive_node=positive_node->below;}
	skiplist* negative_skiplist=vdata->not_vaccinated_persons;
	snode* negative_node=negative_skiplist->header;while(negative_node->below!=NULL){negative_node=negative_node->below;}

	int max=positive_skiplist->size;if(negative_skiplist->size>max){max=negative_skiplist->size;}

	for (int i = 0; i < max; ++i)
		{
		if(positive_node->next!=NULL)
			{
			positive_node=positive_node->next;
			if(strcmp(positive_node->rec->country,country)==0 && (flag_date || date_between(*a,*positive_node->date,*b)))
				{
				age=positive_node->rec->age;
				if(age<20)
					positive_count[0]++;
				else if(age<40)
					positive_count[1]++;
				else if(age<60)
					positive_count[2]++;
				else
					positive_count[3]++;
				}
			}
		
		if (negative_node->next!=NULL)
			{
			negative_node=negative_node->next;
			if(strcmp(negative_node->rec->country,country)==0)
				{
				age=negative_node->rec->age;
				if(age<20)
					negative_count[0]++;
				else if(age<40)
					negative_count[1]++;
				else if(age<60)
					negative_count[2]++;
				else
					negative_count[3]++;
				}
			}
		}
	printf("%s\n",country );
	printf("0-20  %d %f %%\n",positive_count[0],(float)100*positive_count[0]/(positive_count[0]+negative_count[0]));
	printf("20-40 %d %f %%\n",positive_count[1],(float)100*positive_count[1]/(positive_count[1]+negative_count[1]));
	printf("40-60 %d %f %%\n",positive_count[2],(float)100*positive_count[2]/(positive_count[2]+negative_count[2]));
	printf("60+   %d %f %%\n\n",positive_count[3],(float)100*positive_count[3]/(positive_count[3]+negative_count[3]));
	return 0;
	}

int popStatusByAgeB(hashtable* countries ,hashtable* viruses,char* virusName,time_t* a,time_t* b)
	{
	if(countries==NULL||viruses==NULL||virusName==NULL){return 1;}
	int flag_date=0;
	if(a==NULL && b==NULL){flag_date=1;}
	else if(a==NULL||b==NULL){printf("one of the dates is NULL\n");return 1;}
	
	bucket* vbucket=hashtable_check(viruses,hash_i(virusName,hashvalue));
	if(vbucket==NULL || vbucket->data==NULL){printf("cant find specific virus\n");return 1;}
	hashtable* ps_hashtable=(hashtable*)malloc(sizeof(hashtable));ps_hashtable=hashtable_init(ps_hashtable,countries->count,2);
	bucket* ps_bucket;
	psa* ps_data;

	virus* vdata=(virus*)vbucket->data;

	skiplist* positive_skiplist=vdata->vaccinated_persons;
	snode* positive_node=positive_skiplist->header;while(positive_node->below!=NULL){positive_node=positive_node->below;}
	skiplist* negative_skiplist=vdata->not_vaccinated_persons;
	snode* negative_node=negative_skiplist->header;while(negative_node->below!=NULL){negative_node=negative_node->below;}

	int max=positive_skiplist->size;if(negative_skiplist->size>max){max=negative_skiplist->size;}
	int age=0;
	for (int i = 0; i < max; ++i)
		{

		
		if(positive_node->next!=NULL)
			{
			positive_node=positive_node->next;
			if(flag_date || date_between(*a,*positive_node->date,*b))
				{
				
				ps_bucket=hashtable_insert(ps_hashtable,hash_i(positive_node->rec->country,hashvalue));
				ps_data=(psa*) ps_bucket->data;
				if(ps_data==NULL)
					{
					ps_bucket->data=(psa*) malloc(sizeof(psa));
					ps_data=(psa*) ps_bucket->data;
					ps_data->country=positive_node->rec->country;
					for (int j = 0; i < 4; ++i){ps_data->positive_count[j]=0;ps_data->negative_count[j]=0;}
					}
				age=positive_node->rec->age;
				if(age<20)
					ps_data->positive_count[0]++;
				else if(age<40)
					ps_data->positive_count[1]++;
				else if(age<60)
					ps_data->positive_count[2]++;
				else
					ps_data->positive_count[3]++;
				}
			}
		if (negative_node->next!=NULL)
				{
				negative_node=negative_node->next;
				ps_bucket=hashtable_insert(ps_hashtable,hash_i(negative_node->rec->country,hashvalue));
				ps_data=(psa*) ps_bucket->data;
				if(ps_data==NULL)
					{
					ps_bucket->data=(psa*) malloc(sizeof(psa));
					ps_data=(psa*) ps_bucket->data;
					ps_data->country=negative_node->rec->country;
					for (int j = 0; i < 4; ++i){ps_data->positive_count[j]=0;ps_data->negative_count[j]=0;}
					}
				age=negative_node->rec->age;
				if(age<20)
					ps_data->negative_count[0]++;
				else if(age<40)
					ps_data->negative_count[1]++;
				else if(age<60)
					ps_data->negative_count[2]++;
				else
					ps_data->negative_count[3]++;
				}
		
		}
	int count=0;
	for (int i = 0; i < ps_hashtable->bucket_num; ++i)
		{
		ps_bucket=ps_hashtable->buckets[i];
		while(ps_bucket!=NULL)
			{
			count++;
			ps_data=(psa*) ps_bucket->data;
			if(ps_data!=NULL)
				{
				//printf("%s	%d	%f \% \n",ps_data->country,ps_data->positive_count,(float)ps_data->positive_count/(ps_data->positive_count+ps_data->negative_count));
				printf("%s\n",ps_data->country );
				printf("0-20  %d %f%%\n",ps_data->positive_count[0],(float)100*ps_data->positive_count[0]/(ps_data->positive_count[0]+ps_data->negative_count[0]));
				printf("20-40 %d %f%%\n",ps_data->positive_count[1],(float)100*ps_data->positive_count[1]/(ps_data->positive_count[1]+ps_data->negative_count[1]));
				printf("40-60 %d %f%%\n",ps_data->positive_count[2],(float)100*ps_data->positive_count[2]/(ps_data->positive_count[2]+ps_data->negative_count[2]));
				printf("60+   %d %f%%\n\n",ps_data->positive_count[3],(float)100*ps_data->positive_count[3]/(ps_data->positive_count[3]+ps_data->negative_count[3]));
				}
			ps_bucket=ps_bucket->next;
			}
			if(ps_hashtable->count<=count)
				break;
		}
	hashtable_free(ps_hashtable);return 0;
	}
int list_nonVaccinated_Persons(hashtable*viruses,char* virusName)
	{
	unsigned long hash=hash_i(virusName,hashvalue);
	bucket* vbucket=hashtable_check(viruses,hash);if(vbucket==NULL || vbucket->key!=hash || vbucket->data==NULL){return 1;}
	virus* vdata=(virus*) vbucket->data;
	skiplist* not_vaccinated_persons=vdata->not_vaccinated_persons;if(not_vaccinated_persons==NULL){return 1;}
	snode* vnode=not_vaccinated_persons->header;while(vnode->below!=NULL){vnode=vnode->below;}
	while(vnode->next!=NULL)
		{
		vnode=vnode->next;
		printf("%d %s %s %s %d\n",vnode->rec->citizenID,vnode->rec->firstName,vnode->rec->lastName,vnode->rec->country,vnode->rec->age);
		}
	return 0;
	}