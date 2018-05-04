#include <fstream>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <iomanip>
using namespace std;
int N, num[200005], sample[200005], l[15], r[15], index[15], cnt;
pthread_t thread[15];
sem_t sem[15], mutex, finish;
ofstream fout;
bool MT;
void* q_sort(void* dd)
{
    int d = *(int*)dd;
    if(MT) sem_wait(&sem[d]);
    if(d > 6)
    {
        for(int i = l[d] ; i <= r[d] ; i++)
            for(int j = i+1 ; j <= r[d] ; j++)
                if(sample[i] > sample[j]) swap(sample[i], sample[j]);
        if(MT)
        {
            sem_wait(&mutex);
            cnt++;
            if(cnt == 8) sem_post(&finish);
            sem_post(&mutex);
        }
        return NULL;
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
    if(MT)
    {
        sem_post(&sem[(d << 1) + 1]);
        sem_post(&sem[(d << 1) + 2]);
    }
    else
    {
        q_sort(&index[(d << 1)+1]);
        q_sort(&index[(d << 1)+2]);
    }
    return NULL;
}
int main()
{
    int sec, usec;
    struct timeval start, End;
    char filename[100];
    ifstream fin;
    cin >> filename;
    fin.open(filename);
    fin >> N;
    for(int i = 0 ; i < N ; i++) fin >> num[i];
    fin.close();
    for(int i = 0 ; i < N ; i++) sample[i] = num[i];
    fout.open("output1.txt");
    for(int i = 0 ; i < 15 ; i++) sem_init(&sem[i], 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&finish, 0, 0);
    MT = 1;
    cnt = l[0] = 0;
    r[0] = N-1;
    gettimeofday(&start, 0);
    for(int i = 0 ; i < 15 ; i++) pthread_create(&thread[i], 0, q_sort, &(index[i] = i));
    sem_post(&sem[0]);
    sem_wait(&finish);
    gettimeofday(&End, 0);
    fout << sample[0];
    for(int i = 1 ; i < N ; i++) fout << " " << sample[i];
    fout << "\n";
    fout.close();
    sec = End.tv_sec - start.tv_sec;
    usec = End.tv_usec - start.tv_usec;
    cout << "MT Elapsed time: " << fixed << setprecision(2) << sec + (usec / 1000000.0) << "\n";
    for(int i = 0 ; i < N ; i++) sample[i] = num[i];
    fout.open("output2.txt");
    MT = 0;
    gettimeofday(&start, 0);
    q_sort(&index[0]);
    gettimeofday(&End, 0);
    fout << sample[0];
    for(int i = 1 ; i < N ; i++) fout << " " << sample[i];
    fout << "\n";
    fout.close();
    sec = End.tv_sec - start.tv_sec;
    usec = End.tv_usec - start.tv_usec;
    cout << "ST Elapsed time: " << sec + (usec / 1000000.0) << "\n";
    return 0;
}
