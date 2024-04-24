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
        kill(processes[i], SIGKILL); // Завершаем все запущенные процессы
    }
    exit(0); // Выходим из программы
}

// Функция для запуска нового процесса
void launch_process(char *args[])
{
    pid_t pid = fork(); // Создаем новый процесс
    if (pid == 0)
    {                                        // В дочернем процессе
        printf("Child PID: %d\n", getpid()); // Выводим PID дочернего процесса
        execvp(args[0], args);               // Заменяем текущий процесс на указанный исполняемый файл
        perror("execvp");                    // В случае ошибки выводим сообщение
        exit(1);                             // Завершаем дочерний процесс
    }
    else if (pid > 0)
    {                                                             // В родительском процессе
        processes[num_processes++] = pid;                         // Сохраняем PID нового процесса
        printf("Parent PID: %d, Child PID: %d\n", getpid(), pid); // Выводим PID родительского и дочернего процессов
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
        printf("Custom Terminal $ ");       // Выводим приглашение к вводу
        fgets(input, sizeof(input), stdin); // Получаем ввод пользователя
        input[strcspn(input, "\n")] = '\0'; // Удаляем символ новой строки из ввода

        char *token = strtok(input, " "); // Разбиваем ввод на токены (слова)
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1)
        {                              // Пока не достигнут конец строки или максимальное количество аргументов
            args[i++] = token;         // Сохраняем текущий токен в массив аргументов
            token = strtok(NULL, " "); // Получаем следующий токен
        }
        args[i] = NULL; // Устанавливаем последний аргумент в NULL, необходимо для execvp

        if (args[0] != NULL)
        { // Если введена какая-то команда
            if (strcmp(args[0], "exit") == 0)
            {                          // Если команда - "exit"
                handle_signal(SIGINT); // Вызываем обработчик сигнала CTRL+C для завершения программы
            }
            else if (strcmp(args[0], "ls") == 0 || strcmp(args[0], "cat") == 0 ||
                     strcmp(args[0], "nice") == 0 || strcmp(args[0], "killall") == 0 ||
                     strcmp(args[0], "browser") == 0)
            {
                launch_process(args); // Запускаем процесс для введенной команды
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
