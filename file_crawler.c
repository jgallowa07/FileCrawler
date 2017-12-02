/*
AUTHORSHIP STATEMENT

JARED GALLOWAY
951 344 934
jgallowa
Extra-Credit Project CIS 415
Joe Sventek, Operating Systems 


this is is ALL my own work EXCEPT:

	(thank you)
	I got my create_threads() and join_threads() functions from Rocoa Casita's lab 7 
	I got the ADT's TSLinkedlist and TSOrderedSet from Joe Sventek's Public GitHub. 


*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tslinkedlist.h"
#include "tsorderedset.h"
#include "re.h"
#include <dirent.h>
#include <pthread.h>

#define UNUSED __attribute__ ((unused))	

//GLOBALS
const TSLinkedList *myWorkQueue;
const TSOrderedSet *myProcessed;
const TSIterator *processedIterator;
pthread_t *pthreads = NULL;
RegExp *rexobj;
pthread_mutex_t theLock;
int beingProcessed;

//THREAD FUNCTION FOR WORKERS
void *WorkerThread(UNUSED void *PARAM){

	DIR *dir;
	struct dirent *entry;
	
	while(!((myWorkQueue->size(myWorkQueue))==0) & (beingProcessed==0)){
		
		void *name;
		myWorkQueue->removeFirst(myWorkQueue,&name);

		if(!(dir = opendir((char *)name))){
			printf("Problem with dir");
			continue;
		}

		pthread_mutex_lock(&theLock);	
		beingProcessed++;
		pthread_mutex_unlock(&theLock);	
	
		while((entry = readdir(dir))!=NULL){
			if(entry->d_type == DT_DIR){
				char path[1024];
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
					continue;
				}
			    	snprintf(path, sizeof(path), "%s/%s", (char *)name, entry->d_name);
				char *p = (char *)malloc(strlen(path)+1);
				strcpy(p,path);
				myWorkQueue->addLast(myWorkQueue,(void *)p);
			}else if(re_match(rexobj,entry->d_name)){
				char path[1024];
				snprintf(path, sizeof(path), "%s/%s", (char *)name, entry->d_name);
				char *p = (char *)malloc(strlen(path)+1);
				strcpy(p,path);
				myProcessed->add(myProcessed,(void *)p);											
			}
		}
		free(name);
		
		pthread_mutex_lock(&theLock);	
		beingProcessed--;
		pthread_mutex_unlock(&theLock);	
	}
	return NULL;	
}

//WRAPPER FUNCTION TO AVOID TYPE WARNINGS
int myCompare(void *str1,void *str2){
	return strcmp((const char *)str1,(const char *)str2);
}

//BASH TO REGULAR EXPRESSION
char *bash_to_re(char *re){

	int index = 0;
	char ans[1000];
	ans[index++] = '^';
	int sizeofre = strlen(re);
	int i;
	for(i = 0; i < sizeofre; i++){
		
		if(re[i] == '*'){
			ans[index++] = '.';		
			ans[index++] = '*';		
		}else if(re[i] == '.'){
			ans[index++] = '\\';		
			ans[index++] = '.';		
		}else if(re[i] == '?'){
			ans[index++] = '.';
		}else{
			ans[index++] = re[i];
		}
	}
	ans[index++] = '$';
	char *ret = (char *)malloc(strlen(ans)+1);
	strcpy(ret,ans);
	return ret;

}

//CREATE ALL THREADS
void create_threads(int thread_count)
{
	int i = 0;
	for (i = 0; i < thread_count; i++)
	{
		pthread_create(&(pthreads[i]),NULL,WorkerThread,NULL);
	}
}

//JOIN ALL THREADS
void join_threads(int thread_count)
{
	void *returnValue = NULL;
	int i = 0;
	for (i = 0; i < thread_count; i++)
	{
		pthread_join(pthreads[i],&returnValue);
	}
}

int main(int argc, char **argv){

	if(argc < 2){
		printf("givearg\n");
		return 1;	
	}

	int numThreads;
	char *p;
	if((p = getenv("CRAWLER_THREADS"))!=NULL){
		numThreads = atoi(p);		
	}else{
		numThreads = 2;
	}

	rexobj = re_create();
	char *ans = bash_to_re(argv[1]);
	if(!(re_compile(rexobj,ans))){
		printf("problem with re_compile\n");
		return 1;	
	}	
	
	beingProcessed = 0;
	pthread_mutex_init(&theLock,NULL);
	myWorkQueue = TSLinkedList_create();
	myProcessed = TSOrderedSet_create(myCompare);
	processedIterator = myProcessed->itCreate(myProcessed);
	
	if(argc == 2){
		char *p = (char *)malloc(1);
		strcpy(p,".");
		myWorkQueue->addLast(myWorkQueue,(void *)p);	
	}else{
		int i;
		for (i = 2; i < argc; i++){
			char *p = (char *)malloc(strlen(argv[i])+1);
			strcpy(p,argv[i]);
			myWorkQueue->addLast(myWorkQueue,(void *)p);
		}
	}
	
	pthreads = (pthread_t *)malloc(sizeof(pthread_t)*numThreads);
	create_threads(numThreads);
	
	join_threads(numThreads);	
	free(pthreads);	
	 
	void *element[1000];
	while(myProcessed->pollFirst(myProcessed,element)){
		printf("%s\n",(char *)*element);	
		free(*element);
	}

	return 1;

}
