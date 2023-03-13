// ФОРТОВ ЕГОР БПИ214
// Задание:
/*
Вариант 23. Разработать программу, которая ищет в ASCII-строке заданную подстроку 
и возвращает индекс первого символа первого вхождения подстроки в 
строке. Подстрока вводится как параметр.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/types.h>

// функция возвращает размер файла в байтах
off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0) {
        return st.st_size;
    }

    return -1; 
}

#define MAX_SIZE 5000

// ф-ия возвращает индекс первого символа первого вхождения
// строки s в строку str
// если такого вхождения не было, строка возвращает -1
int indexOfSubstringCustom(char str[], char s[]) {
    for (int i = 0; i < strlen(str); ++i) {
        for (int j = i, k = 0; j < strlen(str) && k < strlen(s); ++j, ++k) {
            if (str[j] != s[k]) {
                break;
            }
            if (k == strlen(s) - 1) {
                return i;
            }
        }
    }
    return -1;
}

int main(int argc, char** argv) {
    if (argc != 4) {  // argv[0] is the name of executed file
        printf("Command line must have 3 arguements - names of files input/output & substring\n");
        exit(1);
    }

    char *str = "a", *s = "a";  // for testing
    int input_file_size = 0;

    char *input_file_name = argv[1];
    char *output_file_name = argv[2];
    char buf[MAX_SIZE];  // наш буфер для работы с каналами
    int pipes[2];  
    // pipes[0] хранит дескриптор, через него пишем ИЗ канала
    // pipes[1] хранит открытый файловый дескриптор, через него пишем В канал
    pipe(pipes);

    sem_t sem;
    sem_init(&sem, 0, 1);

    // создадим 2 дочерних процесса
    pid_t pid1, pid2;
    pid1 = fork();
    if (pid1 == 0) {  // child process (#1)
        printf("child process #1\n");

//////////// читаем текстовые данные из файла ////////////
//////////// и через канал передаем их второму процессу ////////////
        int input_file = open(input_file_name, O_RDONLY);  // открываем файл на запись
        input_file_size = fsize(input_file_name);
        printf("FILE SIZE: %d\n", input_file_size);
        char got[input_file_size+1];  // буфер для чтения из файла
        for (int i = 0; i < input_file_size + 1; ++i) {
            got[i] = '\0';
        }
        read(input_file, got, input_file_size);  // system call read 
        write(pipes[1], got, strlen(got) + 1);
        close(input_file);
        printf("sem_wait(&mutex);\n");
        // sem_wait(&mutex);  // ждем, пока завершит работу второй дочерний процесс
        sem_wait(&sem);

        // HERE WE RETURN
        printf("BACK to child process #1\n");
//////////// вывод данных в заданный файл ////////////
        int output_file = open(output_file_name, O_WRONLY | O_CREAT | O_RDONLY, 0644);
        char num[10];
        // printf("LALALLA");
        sleep(1);  // чтобы все было на экране видно
        // sleep(1);
        read(pipes[0], num, 10);
        printf("NUM(=answer): %s\n", num);
        // printf("BUFBUFBUF: %s||", number);
        int bytes_written = write(output_file, num, strlen(num));
        close(output_file);
        close(pipes[0]);
        close(pipes[1]);
        // EXIT
    };
        // sleep(3);  // за 3 секунды точно произойдет чтение данных из файла
        pid2 = fork();
        
        if (pid2 == 0) {  // child process (#2)
            printf("child process #2\n");
//////////// обрабатываем данные и передаем результат третьему процессу ////////////
            read(pipes[0], buf, MAX_SIZE);
            // buf = "abcdefgh";
            char *str = buf;
            char *substring = argv[3];
            int index = indexOfSubstringCustom(str, substring);
            // printf("str:|%s|, s:|%s|\n", str, substring);
            // printf("First found index: %d\n", index);
            // printf("%s\n", buf);  // for DEBUG

            char number[10];
            sprintf(number, "%d", index);
            // printf("WOW: %s", number);
            write(pipes[1], number, strlen(number) + 1);
            printf("sem_post(&mutex);\n");
            sem_post(&sem);
            // sem_post(&mutex);  // возврат к 1-ому дочернему процессу
        } else {  // parent process
            // wait(NULL);
            // sleep(2);
            // sleep(3);  // за 3 сек функция точно отработает
        }
    return 0;
}
