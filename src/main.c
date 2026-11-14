#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <pthread.h>

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

static double now_sec(void){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

static unsigned long cpu_work(long iterations){
	unsigned long x = 12345;
	for(long i = 0; i < iterations; i++){
		x = x * 6364136223846793005UL + 1442695040888963407UL;
	}
	return x;
}

static void bench_cpu(void){
	const long iterations = 2000000000L;
	
	double debut = now_sec();
	unsigned long resultat = cpu_work(iterations);
	double fin = now_sec();
	
	volatile unsigned long sink = resultat;
	(void)sink;
	double duree = fin - debut;
	double millions_operations = (double)iterations / duree / 1e6;

	printf("CPU single-thread : %.0f Millions_operations / sec (%.2f s)\n", millions_operations, duree);
}

typedef struct {
	long iterations;
	unsigned long result;
} thread_arg_t;

static void* thread_func(void* arg){
	thread_arg_t* a = (thread_arg_t*)arg;
	a->result = cpu_work(a->iterations);
	return NULL;
}

static void bench_cpu_mul(int num_threads){
	const long iterations_per_threads = 2000000000L;
	if(num_threads > 64) num_threads = 64;
	pthread_t threads[64];
	thread_arg_t args[64];

	double debut = now_sec();
	for(int i = 0; i < num_threads; i++){
		args[i].iterations = iterations_per_threads;
		pthread_create(&threads[i], NULL, thread_func, &args[i]);
	}

	for(int i = 0; i < num_threads; i++){
		pthread_join(threads[i], NULL);
	}
	double fin = now_sec();
	volatile unsigned long sink = 0;
	for(int i = 0; i < num_threads; i++){
		sink+=args[i].result;
	}
	(void)sink;
	double duree = fin - debut;
	double total_ops = (double)iterations_per_threads * num_threads;
	double mops = total_ops / duree / 1e6;
	printf("CPU multi_thread : %.0f Millions_ops/s (%d threads, %.2f s)\n", mops, num_threads, duree);
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
	printf("\n");

	bench_cpu();
	bench_cpu_mul((int)cores);

	return 0;
}



