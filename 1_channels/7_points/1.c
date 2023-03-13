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
    

////////////////////////
    int     fd, result;
        size_t  size;
        // char msg[] = "Hello, reader!";
        // int msg_size = sizeof(msg);
        char    name[] = "fifo.txt";   // имя именованного канала
        char name2[] = "fifo2.txt";
        (void) umask(0);
        mknod(name, S_IFIFO | 0666, 0);  // создаем канал без проверки, так как это позволяет иcпользовать уже созданный FIFO
        mknod(name2, S_IFIFO | 0666, 0);
 
    char msg[] = "Hello, reader!";
    int msg_size = sizeof(msg);

    int buf_size = MAX_SIZE;
    char str_buf[buf_size];
////////////////////////

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
        // printf("%d//", strlen(got));  // FOR DEBUG
        for (int i = 0; i < input_file_size + 1; ++i) {
            got[i] = '\0';
        }
        read(input_file, got, input_file_size);  // system call read 
        printf("LALALALA\n");
        printf("|||\n%s|||%lu\n", got, strlen(got));  // FOR DEBUG
        printf("GOT: %s\n", got);
        write(pipes[1], got, strlen(got) + 1);
        close(input_file);

        if((fd = open(name, O_WRONLY | O_CREAT)) < 0){
            printf("Can\'t open FIFO for writting\n");
            exit(-1);
        }
        // printf("HEREEEEEEEEEE\n");
        size = write(fd, got, strlen(got) + 1);  // запись в канал строки msg
        if(size != strlen(got) + 1){
            printf("AAAAAAAAAA\n");
            exit(-1);
        }
        if(close(fd) < 0){  // закрытие именованного канала
            printf("Writer: Can\'t close FIFO\n"); 
            exit(-1);
        }
        sem_wait(&sem);

// COME HERE
        sleep(1);
        //////////// вывод данных в заданный файл ////////////
            int output_file = open(output_file_name, O_WRONLY | O_CREAT | O_RDONLY, 0644);
            char num[10];
            // printf("LALALLA");
                // read(pipes[0], num, 10);

            if((fd = open(name, O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }
            size = read(fd, num, 10);
            if (size < 0) {
                printf("Can\'t read string from FIFO\n");
                exit(-1);
            }
            printf("Reader exit, str_buf: %s len:%lu\n", str_buf, strlen(str_buf));
            if(close(fd) < 0){
                printf("Reader: Can\'t close FIFO\n"); exit(-1);
            }
            // sleep(1);
            printf("NUM(=answer): %s\n", num);
            // printf("BUFBUFBUF: %s||", number);
            int bytes_written = write(output_file, num, strlen(num));
            close(output_file);
            printf("Конец программы\n");
            close(pipes[0]);
            close(pipes[1]);
    } else {  // parent process
        // wait(NULL);
        // sleep(3);  // за 3 секунды точно произойдет чтение данных из файла
        pid2 = fork();
        
        if (pid2 == 0) {  // child process (#2)
            printf("child process #2\n");
//////////// обрабатываем данные и передаем результат третьему процессу ////////////

            if((fd = open(name, O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }
            size = read(fd, str_buf, buf_size);
            if (size < 0) {
                printf("Can\'t read string from FIFO\n");
                exit(-1);
            }
            printf("Reader exit, str_buf: %s len:%lu\n", str_buf, strlen(str_buf));
            while (strlen(str_buf) == 0) {  // чтобы избежать состояния гонки
                size = read(fd, str_buf, buf_size);
                if (size < 0) {
                    printf("Can\'t read string from FIFO\n");
                    exit(-1);
                }
            }
            if(close(fd) < 0){
                printf("Reader: Can\'t close FIFO\n"); exit(-1);
            }

            // read(pipes[0], buf, MAX_SIZE);
            char *str = buf;
            char *substring = argv[3];
            int index = indexOfSubstringCustom(str_buf, substring);
            printf("str:|%s|, s:|%s|\n", str_buf, substring);
            printf("First found index: %d\n", index);
            // printf("%s\n", buf);  // for DEBUG

            char number[10];
            sprintf(number, "%d", index);
            // printf("WOW: %s", number);
            // write(pipes[1], number, strlen(number) + 1);

            if((fd = open(name, O_WRONLY | O_CREAT)) < 0){
                printf("Can\'t open FIFO for writting\n");
                exit(-1);
            }
            // printf("HEREEEEEEEEEE\n");
            size = write(fd, number, strlen(number) + 1);  // запись в канал строки msg
            if(size != strlen(msg) + 1){
                exit(-1);
            }
            if(close(fd) < 0){  // закрытие именованного канала
                printf("Writer: Can\'t close FIFO\n"); 
                exit(-1);
            }
            sem_post(&sem);
        } else {  // parent process
            // some trash
        }
    }
    return 0;
}