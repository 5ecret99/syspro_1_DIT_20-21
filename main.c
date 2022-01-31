#include "hash.h"
#include "bloomfilter.h"
#include "skiplist.h"
#include "hashtable.h"
#include "records.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include <time.h>
//insertCitizenRecord function is used to insert records from inputfile also for the "insertCitizenRecord" and "vaccinateNow" commands
void insertCitizenRecord(int bloomSize,hashtable* viruses,hashtable* records,hashtable* countries,char* citizenID,char* firstName,char* lastName,char* country,char* age,char* virus_name,char* yes_no,char* date,int vaccinateNow)
	{
	if(viruses==NULL||records==NULL||countries==NULL||citizenID==NULL||firstName==NULL||lastName==NULL||country==NULL||age==NULL||virus_name==NULL||yes_no==NULL){printf("ERROR something is missing from this record cant insert");return;}
	int id=atoi(citizenID);
	int aa=atoi(age);
	int yn=-1;
	time_t dd=1;
	if(vaccinateNow==1)//vaccinateNow command
		{
		yn=1;
		dd=currdate();
		}
	else if(strcmp(yes_no,"YES")==0)
		{
		yn=1;
		if(strcmp(date,"")==0)
			{
			printf("ERROR IN RECORD  %s %s %s %s %s %s %s %s \n",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);//theres isnt a  date next to a YES
			return;
			}
		dd=dateconverter(date);
		}
	else
		{
		yn=0;
		if(strcmp(date,"")!=0)
			{
			printf("ERROR IN RECORD  %s %s %s %s %s %s %s %s \n",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);//theres a date next to a NO
			return;printf("error return command skipped\n");
			}
		}
	if (dd==-1)
		{
		printf("ERROR IN RECORD  %s %s %s %s %s %s %s %s \n",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);//date is wrong
		return;
		}
	unsigned long country_hash=hash_i(country,hashvalue);//hashvalue is a universal integer i use for hashing located in functions.h
	char* country_data;
	bucket* cbucket=hashtable_insert(countries,country_hash);
	if(cbucket->data==NULL)
		{
		country_data=(char*) malloc(((int)strlen(country))*sizeof(char));//printf("%s with len %ld \n",country,strlen(country));
		strcpy(country_data,country);
		cbucket->data=country_data;
		}
	else
		{
		country_data=(char*) cbucket->data;
		if (strcmp(country_data,country)!=0)
			{
			printf("ERROR IN RECORD  %s %s %s %s %s %s %s %s \n",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);
			return;printf("error return command skipped\n");
			}
		}

	record* rec;
	bucket* rbucket=hashtable_insert(records,id);
	if(rbucket->data==NULL)
		{
		rec=(record*)malloc(sizeof(record));
		rec->citizenID=id;
		rec->age=aa;
		rec->country=country_data;
		rec->firstName=(char*)malloc(strlen(firstName)*sizeof(char));strcpy(rec->firstName,firstName);
		rec->lastName=(char*)malloc(strlen(lastName)*sizeof(char));strcpy(rec->lastName,lastName);
		//printf("record :%d %s %s %s %d inserted succesful\n",id,rec->firstName,rec->lastName,rec->country,aa);
		rbucket->data=rec;
		}
	else
		{
		rec=(record*) rbucket->data;
		if(rec->age!=aa ||strcmp(rec->country,country_data)||strcmp(rec->firstName,firstName)!=0||strcmp(rec->lastName,lastName)!=0)
			{
			printf("ERROR IN RECORD  %s %s %s %s %s %s %s %s \n",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);
			return;printf("error return command skipped\n");
			}
		}

	bucket* vbucket=hashtable_insert(viruses,hash_i(virus_name,hashvalue));
	virus* vv_data;

	if(vbucket->data==NULL)
		{
		vv_data=(virus*) malloc(sizeof(virus));
		vv_data->virus_name=(char*)malloc(strlen(virus_name)*sizeof(char));strcpy(vv_data->virus_name,virus_name);
		vv_data->not_vaccinated_bloom=(bloom*) malloc(sizeof(bloom));bloom_init(vv_data->not_vaccinated_bloom,bloomSize);
		vv_data->vaccinated_persons=(skiplist*) malloc(sizeof(skiplist));skiplist_init(vv_data->vaccinated_persons);
		vv_data->not_vaccinated_persons=(skiplist*) malloc(sizeof(skiplist));skiplist_init(vv_data->not_vaccinated_persons);
		vbucket->data=vv_data;
		}
	else
		vv_data=(virus*) vbucket->data;

	if(yn==0)
		{
		if(skiplist_check(vv_data->vaccinated_persons,id))
			{
			skiplist_insert(vv_data->not_vaccinated_persons,id,rec,NULL);
			}
		}
	else if(yn==1)
		{
		if(vaccinateNow==1)//vaccinateNow command
			{
			skiplist_delete(vv_data->not_vaccinated_persons,id);
			time_t* day=(time_t*) malloc(sizeof(time_t));*day=dd;
			if(skiplist_insert(vv_data->vaccinated_persons,id,rec,day))//if he is not vaccinated already
				bloom_add(vv_data->not_vaccinated_bloom,citizenID);
			else//we dont need the date because citizen already vaccinated
				free(day);
			}
		else if(skiplist_check(vv_data->not_vaccinated_persons,id))//if he is not in the not_vaccinated_persons skiplist 
			{
			time_t* day=(time_t*) malloc(sizeof(time_t));*day=dd;
			if(skiplist_insert(vv_data->vaccinated_persons,id,rec,day))
				bloom_add(vv_data->not_vaccinated_bloom,citizenID);
			else
				free(day);
			}
		}
	}

//./vaccineMonitor -c citizenRecordsFile –b bloomSize
int main(int argc, char const *argv[])
	{
	if(argc!=3){printf("u should run ./vaccineMonitor [inputfile.txt] [bloomSize]");return 1;}
	srand ((unsigned int)time(NULL));
	char citizenID[5];
	char firstName[30];
	char lastName[30];
	char country[30];
	char age[5];
	char virus_name[30];
	char yes_no[5];
	char date[15];
	int bloomSize=atoi(argv[2]);
	int size=100;
	if(bloomSize/10>100)
		size=bloomSize/10;
	//3 hashtables 1 for viruses including bloomfilter and 2 skiplist one for the citizen records and one for the countries
	hashtable* viruses=(hashtable*)malloc(sizeof(hashtable));viruses=hashtable_init(viruses,size,0);
	hashtable* records=(hashtable*)malloc(sizeof(hashtable));records=hashtable_init(records,size,1);
	hashtable* countries=(hashtable*)malloc(sizeof(hashtable));countries=hashtable_init(countries,size,2);
	const char* filename=argv[1];
	FILE* citizenRecordsFile=fopen(filename,"r");
	if (citizenRecordsFile==NULL){printf("error with reading file %s\n",filename);return 1;}
	char* line=NULL;
	size_t len=0;
	clock_t begin=clock();
	int linecount=0;
	while(getline(&line,&len,citizenRecordsFile)!=EOF)
		{
		linecount++;
		strcpy(date,"");strcpy(country,"");strcpy(virus_name,"");
		sscanf(line,"%s %s %s %s %s %s %s %s",citizenID,firstName,lastName,country,age,virus_name,yes_no,date);//breaking the line into words
		insertCitizenRecord(bloomSize,viruses,records,countries,citizenID,firstName,lastName,country,age,virus_name,yes_no,date,0);
		}
	clock_t end=clock();
	fclose(citizenRecordsFile);
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("\nfile with %d lines read in  %f s\n",linecount,time_spent);
	printf("==========================================================================\n");
	printf("commands:\n/Exit\n/vaccineStatusBloom citizenID virus_name\n/vaccineStatus citizenID [virus_name]\n/populationStatus [country] virus_name [dateA dateB]\n/popStatusByAge [country] virus_name [dateA dateB]\n");
	printf("/insertCitizenRecord citizenID firstName lastName country age virus_name yes_no [date]\n/vaccinateNow citizenID firstName lastName country age virus_name\n/list-nonVaccinated-Persons virus_name\n");
	char a[10][50];
	int word_count=0;
	char* words;
	while(1)
		{
		printf("==========================================================================\n");
		printf("write command:\n/");
		word_count=0;
		getline(&line,&len,stdin);

		words= strtok (line," \n\0");
		while(words!=NULL)
			{
			if(word_count<10)//the biggest command has 9 words
				strcpy(a[word_count],words);
			words= strtok (NULL," \n\0");
			word_count++;
			}
		if (strcmp(a[0],"Exit")==0)
			break;
		else if(strcmp(a[0],"vaccineStatusBloom")==0)
			{
			if (word_count!=3){printf("given more or less words than needed\n");continue;}
			if(vaccineStatusBloom(viruses,a[2],a[1])){printf("error with vaccineStatusBloom\n");}
			}
		else if(strcmp(a[0],"vaccineStatus")==0)
			{
			if(word_count==3)
				{
				if (vaccineStatusA(viruses,a[2],atoi(a[1]))){printf("error with vaccineStatusA\n");}
				}
			else if(word_count==2)
				{
				if (vaccineStatusB(viruses,atoi(a[1]))){printf("error with vaccineStatusB\n");}
				}
			else
				printf("given more or less words than needed\n");
			}

		else if(strcmp(a[0],"populationStatus")==0)
			{
			if(word_count==3)
				{
				if (populationStatusA(countries ,viruses,a[1],a[2],NULL,NULL)){printf("error with populationStatusA\n");}
				}
			else if(word_count==2)
				{
				if (populationStatusB(countries ,viruses,a[1],NULL,NULL)){printf("error with populationStatusB\n");}
				}
			else if (word_count==5)
				{
				time_t* dateA=(time_t*) malloc(sizeof(time_t));*dateA=dateconverter(a[3]);if(*dateA==-1){free(dateA);dateA=NULL;}
				time_t* dateB=(time_t*) malloc(sizeof(time_t));*dateB=dateconverter(a[4]);if(*dateB==-1){free(dateB);dateB=NULL;}
				if (populationStatusA(countries ,viruses,a[1],a[2],dateA,dateB)){printf("error with populationStatusA\n");}
				if(dateA!=NULL){free(dateA);}
				if(dateB!=NULL){free(dateB);}
				}
			else if (word_count==4)
				{
				time_t* dateA=(time_t*) malloc(sizeof(time_t));*dateA=dateconverter(a[2]);if(*dateA==-1){free(dateA);dateA=NULL;}
				time_t* dateB=(time_t*) malloc(sizeof(time_t));*dateB=dateconverter(a[3]);if(*dateB==-1){free(dateB);dateB=NULL;}
				if (populationStatusB(countries ,viruses,a[1],dateA,dateB)){printf("error with populationStatusB\n");}
				if(dateA!=NULL){free(dateA);}
				if(dateB!=NULL){free(dateB);}
				}
			else
				printf("given more or less words than needed\n");
			}

		else if(strcmp(a[0],"popStatusByAge")==0)
			{
			if(word_count==3)
				{
				if (popStatusByAgeA(countries ,viruses,a[1],a[2],NULL,NULL)){printf("error with popStatusByAgeA\n");}
				}
			else if(word_count==2)
				{
				if (popStatusByAgeB(countries ,viruses,a[1],NULL,NULL)){printf("error with popStatusByAgeB\n");}
				}
			else if (word_count==5)
				{
				time_t* dateA=(time_t*) malloc(sizeof(time_t));*dateA=dateconverter(a[3]);if(*dateA==-1){free(dateA);dateA=NULL;}
				time_t* dateB=(time_t*) malloc(sizeof(time_t));*dateB=dateconverter(a[4]);if(*dateB==-1){free(dateB);dateB=NULL;}
				if (popStatusByAgeA(countries ,viruses,a[1],a[2],dateA,dateB)){printf("error with popStatusByAgeA\n");}
				if(dateA!=NULL){free(dateA);}
				if(dateB!=NULL){free(dateB);}
				}
			else if (word_count==4)
				{
				time_t* dateA=(time_t*) malloc(sizeof(time_t));*dateA=dateconverter(a[2]);if(*dateA==-1){free(dateA);dateA=NULL;}
				time_t* dateB=(time_t*) malloc(sizeof(time_t));*dateB=dateconverter(a[3]);if(*dateB==-1){free(dateB);dateB=NULL;}
				if (popStatusByAgeB(countries ,viruses,a[1],dateA,dateB)){printf("error with popStatusByAgeB\n");}
				if(dateA!=NULL){free(dateA);}
				if(dateB!=NULL){free(dateB);}
				}
			else
				printf("given more or less words than needed\n");
			}

		else if(strcmp(a[0],"insertCitizenRecord")==0)
			{
			if(word_count==8)
				{
				insertCitizenRecord(bloomSize,viruses,records,countries,a[1],a[2],a[3],a[4],a[5],a[6],a[7],"",0);
				}
			else if(word_count==9)
				{
				insertCitizenRecord(bloomSize,viruses,records,countries,a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],0);
				}
			else
				printf("given more or less words than needed\n");
			}
		else if(strcmp(a[0],"vaccinateNow")==0)
			{
			if(word_count==7)
				{
				insertCitizenRecord(bloomSize,viruses,records,countries,a[1],a[2],a[3],a[4],a[5],a[6],"YES","",1);
				}
			else
				printf("given more or less words than needed\n");
			}

		else if(strcmp(a[0],"list-nonVaccinated-Persons")==0)
			{
			if (word_count==2)
				{
				if(list_nonVaccinated_Persons(viruses,a[1])){printf("error with list-nonVaccinated-Persons\n");}
				}
			else
				printf("given more or less words than needed\n");
			}
		else
			printf("didnt match any command try again :/\n");
		}
	printf("==========================================================================\n");
	printf("countries count : %d\n",countries->count);
	printf("person count: %d\n",records->count);
	printf("virus count : %d\n",viruses->count);
	//freeing
	if (line!=NULL)
		free(line);
	hashtable_free(countries);
	hashtable_free(records);
	hashtable_free(viruses);
	return 0;
	}