#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
using namespace std;
int main()
{
    unsigned int SIZE;
    int sec, usec;
    struct timeval start, End;
    cout << "Input the matrix dimension: ";
    cin >> SIZE;
    int A_id = shmget(0, (SIZE * SIZE) << 2, IPC_CREAT | 0666),
        B_id = shmget(0, (SIZE * SIZE) << 2, IPC_CREAT | 0666),
        C_id = shmget(0, (SIZE * SIZE) << 2, IPC_CREAT | 0666),
        *A_addr = (int*)shmat(A_id, NULL, 0),
        *B_addr = (int*)shmat(B_id, NULL, 0),
        *C_addr = (int*)shmat(C_id, NULL, 0);
    for(int i = 0 ; i < 16 ; i++)
    {
        int tmp, NO, limit_up, limit_down, check_sum = 0;
        for(int m = 0 ; m < SIZE ; m++)
            for(int n = 0 ; n < SIZE ; n++)
            {
                tmp = m * SIZE + n;
                *(A_addr + tmp) = *(B_addr + tmp) = tmp;
                *(C_addr + tmp) = 0;
            }
        gettimeofday(&start, 0);
        for(NO = 0 ; NO < i ; NO++) if(!fork()) break;
        limit_down = (i) ? (SIZE/(i+1))*NO : 0;
        limit_up = (NO == i) ? SIZE : limit_down + SIZE/(i+1);
        for(int m = limit_down ; m < limit_up ; m++)
            for(int n = 0 ; n < SIZE ; n++)
                for(int k = 0 ; k < SIZE ; k++)
                    *(C_addr + m * SIZE + n) += (*(A_addr + m * SIZE + k)) * (*(B_addr + k * SIZE + n));
        if(NO != i) return 0;
        while(wait(NULL) != -1);
        for(int m = 0 ; m < SIZE ; m++) for(int n = 0 ; n < SIZE ; n++) check_sum += *(C_addr + m * SIZE + n);
        gettimeofday(&End, 0);
        sec = End.tv_sec - start.tv_sec;
        usec = End.tv_usec - start.tv_usec;
        cout << "Multiplying matrices using " << i+1 << " process\n";
        cout << "Elapsed time: " << sec + (usec / 1000000.0) << " sec, Checksum: " << check_sum << "\n";
    }
    return 0;
}
