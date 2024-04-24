#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>

// команда для терминала
char *ps_argv[] = {"ls", "-a", 0};
int iMassPID = 0;
pid_t MassPID[20];
char ignor[10] = "999";     // magic char

void onCtrlC(int snum) 
{

    printf("\nФункция убивания процесса\n");

    // Если детей не осталось
    if((iMassPID == 0) || (strcmp(ps_argv[0], "killme") == 0))
    {
        printf("Убит родительский процесс %d\n", getpid());
        kill(getpid(), SIGTERM);
        signal(SIGINT, SIG_DFL);
    }
    else if(snum != 5)
    {
        signal(SIGINT, SIG_IGN);
        // Печать детей
        printf("Дочерние процессы:\n");
        for(int i = 0; i < iMassPID; i++)
            printf("iMassPID:%d\t%d\n", i, MassPID[i]);
        
        // Процесс убийства и логгирование
        printf("Убит %d процесс\n", MassPID[iMassPID-1]);
        // Поместить дочерний процесс в состояние зомби
        sleep(1);
        kill(MassPID[iMassPID-1], SIGTERM);
        MassPID[iMassPID-1] = 0;
        iMassPID--;
    }
    else
    {
        signal(SIGINT, SIG_IGN);
        // Печать детей
        printf("Дочерние процессы:\n");
        for(int i = 0; i < iMassPID; i++)
            printf("iMassPID:%d\t%d\n", i, MassPID[i]);
        
        // Процесс убийства и логгирование
        printf("Убит %d процесс\n", MassPID[iMassPID-1]);
        // Поместить дочерний процесс в состояние зомби
        sleep(1);
        kill(MassPID[iMassPID-1], SIGTERM);
        MassPID[iMassPID-1] = 0;
        iMassPID--;
    }
    return;
    
}

void arg(char *ps_argv[])
{
    // Ввод команды
    char buff[256];
    printf("Команда: ");
    if (fgets(buff, sizeof buff, stdin) == NULL) 
    {
        printf("Input error.\n");
        exit(1);
    }

    // Расчленение на части
    int i = 0;              // индекс букв в получено массиве
    int j = 0;              // слова
    int k = 0;              // индекс буквы в слове
    char comd[10][100];

    while(i < strlen(buff))
    {
        if(buff[i] == ' ') 
        {
            i++;
            continue;
        }

        if(buff[0] == 10 || buff[0] == 13 || buff[0] == '\n')
        {
            ps_argv[0] = ignor;
            return;
        }

        while ((buff[i] != ' ') && (i < strlen(buff)))
        {
            if(buff[i] == '\n')
            {
                i++;
                continue;
            }
            comd[j][k] = buff[i];

            i++;
            k++;
        }
        comd[j][k] = '\0';
        ps_argv[j] = comd[j];
        j++;
        k = 0;
    }
    ps_argv[j] = 0;

}

int main()
{
    int pid;
    printf("Для выхода пропишите \"qiut\"\n");

    while(1)
    {
        // Сигнал на удаление
        signal(SIGINT, onCtrlC);

        // Ввод команды
        arg(ps_argv);

        // Выход
        if(strcmp(ps_argv[0], "quit") == 0)
            break;
        // Отладка
        if(strcmp(ps_argv[0], "process") == 0)
            printf("Родительский процесс: %d\n",  getpid());
        // Посмотреть детей
        else if(strcmp(ps_argv[0], "child") == 0)
        {
            if(iMassPID == 0)
                 printf("У вас детей нет\n");
            else
            {
                printf("Дочерние процессы:\n");
                for(int i = 0; i < iMassPID; i++)
                   printf("iMassPID:%d\t%d\n", i, MassPID[i]);
            }
        }
        else if((strcmp(ps_argv[0], "999") != 0) && (strcmp(ps_argv[0], "5") != 0))
        {
            pid = fork();
            switch(pid) 
            {
                case -1:/* Аварийное завершение */
                    perror("fork failed");
                    exit(1);
                case 0: /* Дочерний процесс */
                    execvp(ps_argv[0],  ps_argv);
                    exit(0);
                default: /*родительский процесс*/
                    sleep(1);
                    MassPID[iMassPID] = pid;
                    iMassPID++;
                    break;
            }
        }
        else if(strcmp(ps_argv[0], "5") == 0)
        {
            onCtrlC(5);
        }
    }

    exit(0);
}
