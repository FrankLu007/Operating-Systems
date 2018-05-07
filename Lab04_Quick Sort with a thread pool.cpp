#include <fstream>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <iomanip>
#include <string>
using namespace std;
int N, num[200005], sample[200005], l[15], r[15], cnt, job;
pthread_t thread[8];
sem_t sem[15], mutex, finish, work, mutex_cnt;
void* q_sort(void* args)
{
	int d;
	sem_wait(&work);
	while(true)
	{
		sem_wait(&mutex);
		d = job++;
		if(job == 15) job = 0;
		sem_post(&mutex);
		sem_wait(&sem[d]);
	    if(d > 6)
	    {
	        for(int i = l[d] ; i <= r[d] ; i++)
	            for(int j = i+1 ; j <= r[d] ; j++)
	                if(sample[i] > sample[j]) swap(sample[i], sample[j]);
	        sem_wait(&mutex_cnt);
	        cnt++;
	        if(cnt == 8) sem_post(&finish);
	        sem_post(&mutex_cnt);
	        continue;
	    }
	    int i = l[d], j = r[d]+1, s = sample[i];
	    while(true)
	    {
	        while(i < r[d] && sample[++i] <= s);
	        while(j > l[d] && sample[--j] > s);
	        if(i >= j) break;
	        swap(sample[i], sample[j]);
	    }
	    swap(sample[l[d]], sample[j]);
	    l[(d << 1) + 1] = l[d];
	    r[(d << 1) + 1] = j-1;
	    l[(d << 1) + 2] = j+1;
	    r[(d << 1) + 2] = r[d];
	    sem_post(&sem[(d << 1) + 1]);
	    sem_post(&sem[(d << 1) + 2]);
	}
	return NULL;
}
int main()
{
	ifstream fin;
	ofstream fout;
	string filename;
	struct timeval start, End;
	int sec, usec;

	cout << "Input file:";
	cin >> filename;
	fin.open(filename.c_str());
	fin >> N;
	for(int i = 0 ; i < N ; i++) fin >> num[i];
	fin.close();

        for(int i = 0 ; i < 15 ; i++) sem_init(&sem[i], 0, 0);
	sem_init(&mutex, 0, 1);
	sem_init(&mutex_cnt, 0, 1);
	sem_init(&work, 0, 0);
        sem_init(&finish, 0, 0);

    	l[0] = 0;
    	r[0] = N-1;
    	for(int j = 0 ; j < 8 ; j++) pthread_create(&thread[j], NULL, q_sort, NULL);

    	job = 0;
	for(int i = 0 ; i < 8 ; i++)
	{
		for(int j = 0 ; j < N ; j++) sample[j] = num[j];
        	cnt = 0;
		sem_post(&work);

		gettimeofday(&start, 0);
		sem_post(&sem[0]);
		sem_wait(&finish);
		gettimeofday(&End, 0);

		sec = End.tv_sec - start.tv_sec;
    		usec = End.tv_usec - start.tv_usec;
    		cout << "Elapsed time of " << i+1 << " thread(s): " << fixed << setprecision(3) << sec + (usec / 1000000.0) << " sec\n";

		filename = "output_" + to_string(i+1) + ".txt";
		fout.open(filename.c_str());
		for(int j = 0 ; j < N ; j++) fout << sample[j] << " ";
		fout.close();
	}
	return 0;
}
