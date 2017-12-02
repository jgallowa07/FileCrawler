# fileCrawler
This is a general file crawler that will search through 
the CWD or a given directory and all of the sub directories
for file matching bash 'pattern' * ? and [] are acceptable 
pattern symbols.

The core source code for the file crawler 
is located in the file_crawler.c 

a Makefile is included to create the executable

the rest of the files are ADT's borrowed 
from JOE SVENTECK's public git hub

usage is 

./file_crawler 'pattern'
