#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_ARGS 10
#define MAX_COMMAND_LENGTH 100

int num_processes = 0;     // Хранит текущее количество запущенных процессов
pid_t processes[MAX_ARGS]; // Массив для хранения PID запущенных процессов

// Обработчик сигнала CTRL+C
void handle_signal(int signum)
{
    for (int i = 0; i < num_processes; ++i)
    {
        kill(processes[i], SIGKILL); 
        printf("Process %d killed\n", processes[i]);
    }
    exit(0); 
}

// Функция для запуска нового процесса
void launch_process(char *args[])
{
    pid_t pid = fork(); // Создаем новый процесс
    if (pid == 0)
    {                                        // В дочернем процессе

        execvp(args[0], args);               
        perror("execvp");                    
        exit(1);                             
    }
    else if (pid > 0)
    {                                                             // В родительском процессе
        processes[num_processes++] = pid;                         // Сохраняем PID нового процесса
        printf("Parent PID: %d, Child PID: %d\n\n", getpid(), pid); 
    }
    else
    {                   // В случае ошибки при создании нового процесса
        perror("fork"); // Выводим сообщение об ошибке
    }
}

int main()
{
    char input[MAX_COMMAND_LENGTH]; // Буфер для ввода команды
    char *args[MAX_ARGS];           // Массив аргументов команды

    signal(SIGINT, handle_signal); // Устанавливаем обработчик сигнала CTRL+C

    while (1)
    {                                       // Бесконечный цикл для ввода команд
        printf("\nCustom Terminal> ");       // Выводим приглашение к вводу
        fgets(input, sizeof(input), stdin); // Получаем ввод пользователя
        input[strcspn(input, "\n")] = '\0'; // Удаляем символ новой строки из ввода

        char *token = strtok(input, " "); // Разбиваем ввод на слова
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) // Пока не достигнут конец строки или максимальное количество аргументов
        {                              
            args[i++] = token;         // Сохраняем текущий токен в массив аргументов
            token = strtok(NULL, " "); // Получаем следующий токен
        }
        args[i] = NULL; // Устанавливаем последний аргумент в NULL, необходимо для execvp

        if (args[0] != NULL) 
        { 
            if (strcmp(args[0], "browser") == 0) // Если введена команда "browser"
            {   
                int saved_stderr = dup(STDERR_FILENO);
                freopen("/dev/null", "w", stderr);
                char* browser[] = {"/usr/bin/yandex-browser", NULL};
                launch_process(browser); 
                dup2(saved_stderr, STDERR_FILENO);
            }
            else
            {
                launch_process(args); // Если введена другая команда, запускаем ее процесс
            }
        }
        wait(NULL); // Ожидаем завершения дочернего процесса
    }

    return 0;
}
