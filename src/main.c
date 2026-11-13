#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

static void get_cpu_model(char* buffer, size_t size){
	FILE* f = fopen("/proc/cpuinfo", "r");
	if(!f){
		perror("sysprobe : /proc/cpuinfo");
		snprintf(buffer, size, "unknown");
		return;
	}


	char ligne[256];
	while(fgets(ligne, sizeof(ligne), f)){
		if(strncmp(ligne, "model name", 10) == 0){ // we compare if the ligne start with "model name" (only the 10 first characters).
			char* double_points = strchr(ligne, ':');
			if(double_points){
				char* valeur = double_points + 2;
				valeur[strcspn(valeur, "\n")] = '\0';
				snprintf(buffer, size, "%s", valeur);
				fclose(f);
				return;
			}
		}
	}

	snprintf(buffer, size, "unknown");
	fclose(f);
}





int main(void){

	long cores = sysconf(_SC_NPROCESSORS_ONLN); //we're getting how much cpu cores are actually free;
	
	// total ram, numbers of memory pages * page's size	
	long pages = sysconf(_SC_PHYS_PAGES); 
	long page_size = sysconf(_SC_PAGE_SIZE);
	double ram_gb = (double)pages * page_size / (1024.0 * 1024.0 * 1024.0); // we convert in gigabytes, bytes -> gigabytes, taht's why we divide by 1024**3
	
	char cpu_model[256];
	get_cpu_model(cpu_model, sizeof(cpu_model));

	printf("[+] sysprobe\n");
	printf("[+] CPU model : %s\n", cpu_model);
	printf("[+] CPU cores  :  %ld\n", cores);
	printf("[+] total RAM	: %.2f GB\n", ram_gb);

	return 0;
}



