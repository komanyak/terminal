#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_PROCESSES 10
#define MAX_COMMAND_LENGTH 100

// Структура для хранения информации о процессе
typedef struct {
    pid_t pid;
    char command[MAX_COMMAND_LENGTH];
} Process;

Process processes[MAX_PROCESSES];
int num_processes = 0;

// Обработчик сигнала SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    printf("\nReceived SIGINT. Terminating all processes.\n");
    // Завершаем все процессы
    for (int i = 0; i < num_processes; ++i) {
        kill(processes[i].pid, SIGKILL);
    }
    exit(0);
}

int main() {
    // Устанавливаем обработчик сигнала SIGINT
    signal(SIGINT, sigint_handler);

    char input[MAX_COMMAND_LENGTH];

    while (1) {
        printf("Terminal> ");
        fflush(stdout);
        fgets(input, sizeof(input), stdin);
        
        // Удаление символа новой строки
        input[strcspn(input, "\n")] = 0;

        // Разбиваем входную строку на токены
        char *token = strtok(input, " ");

        while (token != NULL) {
            if (strcmp(token, "ls") == 0) {
                printf("Running 'ls' command...\n");
                pid_t pid = fork();
                if (pid == 0) {
                    execlp("ls", "ls", (char *)NULL);
                    exit(0);
                } else {
                    // Сохраняем информацию о процессе
                    processes[num_processes].pid = pid;
                    strcpy(processes[num_processes].command, "ls");
                    num_processes++;
                }
            } else if (strcmp(token, "cat") == 0) {
                printf("Running 'cat' command...\n");
                pid_t pid = fork();
                if (pid == 0) {
                    execlp("cat", "cat", "example.txt", (char *)NULL);
                    exit(0);
                } else {
                    processes[num_processes].pid = pid;
                    strcpy(processes[num_processes].command, "cat");
                    num_processes++;
                }
            } else if (strcmp(token, "nice") == 0) {
                printf("Running 'nice' command...\n");
                pid_t pid = fork();
                if (pid == 0) {
                    execlp("nice", "nice", "-n", "10", "ls", (char *)NULL);
                    exit(0);
                } else {
                    processes[num_processes].pid = pid;
                    strcpy(processes[num_processes].command, "nice");
                    num_processes++;
                }
            } else if (strcmp(token, "killall") == 0) {
                printf("Running 'killall' command...\n");
                // Завершаем все процессы
                for (int i = 0; i < num_processes; ++i) {
                    kill(processes[i].pid, SIGKILL);
                }
                num_processes = 0;
            } else if (strcmp(token, "exit") == 0) {
                printf("Exiting terminal...\n");
                // Завершаем все процессы
                for (int i = 0; i < num_processes; ++i) {
                    kill(processes[i].pid, SIGKILL);
                }
                exit(0);
            } else {
                // Если не найдено ключевое слово, пытаемся запустить программу
                printf("Running '%s'...\n", token);
                pid_t pid = fork();
                if (pid == 0) {
                    execlp(token, token, (char *)NULL);
                    exit(0);
                } else {
                    // Сохраняем информацию о процессе
                    processes[num_processes].pid = pid;
                    strcpy(processes[num_processes].command, token);
                    num_processes++;
                }
            }
            token = strtok(NULL, " ");
        }
    }

    return 0;
}
