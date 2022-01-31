# syspro_1_DIT_20-21

/////comands for main program
compiling: make
run: ./vaccineMonitor [input file] [bloom filter size in bytes]
clean: make clean
*tested on universities linux machines and my personal pc running Linux Mint 20 Cinnamon*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hash.h/hash.c contains hashing functions given by the instructor i only use has_i function

records.h/records.c contains record struct and some useful date functions.

bloomfilter.h/bloomfilter.c contains the bloomfilter and its functions for the bloomfilter(buff) i use a char* with dynamic size and i do bitwise operations to change idividual bits.

skiplist.h/skiplist.c contains skiplist struct and functions skiplist height is decided by a coinflip for each level upwards if it reaches higher height than the current it stops with coinflip and just adds a header.

hashtable.h/hashtable.c contains hashtable struct and functions .My hashtable can handle any data type that doesnt contain a pointer than needs to be freed by using void*.If u set the type to 0 its suitable for the virus struct( 2 skiplist and 1 bloomfilter hashtable.h) to 1 its suitable for the record struct( records.h).

functions.h/functions.c  contains functions for program comands except insertCitizenRecord,vaccinateNow.I have idividual functions for specific and all viruses in vaccineStatus,for specific and all countries in populationStatus,for specific and all countries in popStatusByAge.Also for populationStatusB,popStatusByAgeB(for all countries) i created a structs ps,psa acordingly and puting them in a hashtable(my versitale hashtable with void* ;) ) to keep track of each country's stats.

main.c contains the insertCitizenRecord function that i use to insert records from the inputfile.txt also for insertCitizenRecord vaccinateNow(if vaccinateNow integer is set to 1 it runs vaccinateNow command ) commands.Because my hashtable has a specific number of buckets i set it to bloomfiltersize/10 with a minumum value of 100.I decided to devide bloomfiltersize by 10 because instructor said that our programs would be tested with 20000-30000 records and that the bloomfiltersize would be in the 100000 so it kinda makes sense.I use a country hashtable,virus(virus struct) hashtable and a record(record struct) hashtable.The record struct only contains a pointer to the country and not a new allocated block of memory.
