#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;
int main()
{
    char input[100], *argv[10], *depart, write[100], read[100];
    int nargv, fd[2];
    bool file = 0, pp = 0;
    pid_t pid;
    pipe(fd);
    while(1)
    {
        pid = fork();
        if(pid < 0){cout << "fork error!\n"; return 1;}
        else if(pid == 0)
        {
            file = 0;
            cout << ">";
            cin.getline(input, 100);
            argv[0] = strtok(input, " \n");
            for(nargv = 1 ; depart = strtok(NULL, " \n") ; nargv++)
            {
                argv[nargv] = depart;
                if(!strcmp(argv[nargv], ">")) {file = 1; break;}
                if(!strcmp(argv[nargv], "|")) {pp = 1; break;}
            }
            if(!strcmp(argv[nargv-1], "&"))
            {
                pid = fork();
                if(pid < 0) {cout << "fork error!\n"; return 1;}
                else if(pid == 0)
                {
                    argv[nargv-1] = NULL;
                    execvp(argv[0], argv);
                }
                else return 0;
            }
            argv[nargv++] = NULL;
            if(pp)
            {
                int i;
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                pid_t tmp = fork();
                if(tmp != 0)execvp(argv[0], argv);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                for(i = 0 ; depart = strtok(NULL, " \n") ; i++) argv[i] = depart;
                if(i < nargv) for(i; i < nargv ; i++) argv[i] = NULL;
                argv[i++] = NULL;
                dup2(STDOUT_FILENO, fd[1]);
                execvp(argv[0], argv);
                return 0;
            }

            if(file)
            {
                int fd = open(strtok(NULL, " \n"), O_WRONLY | O_CREAT, 0777 );
                if( fd == -1 ) {
                    cout << "file error!\n";
                    return 1;
                }
                fclose(stdout);
                dup2( fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(argv[0], argv);
        }
        else
        {
            wait(NULL);
        }
    }
    return 0;
}
