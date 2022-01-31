all:		vaccineMonitor

records.o: 	records.c
		gcc -c records.c

hash.o: 	hash.c
		gcc -c hash.c

skiplist.o: 	skiplist.c
		gcc -c skiplist.c

bloomfilter.o: 	bloomfilter.c
		gcc -c bloomfilter.c

hashtable.o: 	hashtable.c
		gcc -c hashtable.c

functions.o: 	functions.c
		gcc -c functions.c
main.o: 	main.c
		gcc -c main.c
vaccineMonitor: 	hash.o bloomfilter.o skiplist.o records.o hashtable.o functions.o main.o
		gcc hash.o bloomfilter.o skiplist.o records.o hashtable.o functions.o main.o -o vaccineMonitor

clean:
	rm *o vaccineMonitor