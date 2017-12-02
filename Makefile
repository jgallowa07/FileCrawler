CFLAGS=-W -Wall
OBJECTS=file_crawler.o tslinkedlist.o tsiterator.o linkedlist.o iterator.o re.o tsorderedset.o orderedset.o

myProgram: $(OBJECTS)
	gcc -o file_crawler $(OBJECTS) -lpthread

file_crawler.o: file_crawler.c tslinkedlist.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h
tsiterator.o: tsiterator.c tsiterator.h
linkedlist.o: linkedlist.c linkedlist.h
iterator.o: iterator.c iterator.h
re.o: re.c re.h
tsorderedset.o: tsorderedset.c tsorderedset.h
orderedset.o: orderedset.c orderedset.h 
