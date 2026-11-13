#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h> 

static void(char* buffer, size_t size){

	FILE* file = fopen("/proc/cpuinfo", "r");

	if(!file){
		
		fprintf(stderr, "Not allowed to open this file");
		warn("error");
	}

	char buf[256];
	
	while(file != EOF){
		long c = fgets("


