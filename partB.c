#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int strlen_custom(const char str[]) // my strlen function
{
    int len = 0;

    while (str[len] != '\0')
    {
        len++;
    }

    return len;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("error");
        return -1;
    }
    int z = 3, s = 0, s_num, r, len, i = 0, fd1, fd2, fd3, fd4, fd5, fd_input, flag1 = 0, exitStatus;
    int bytes;
    char config[50];
    char d[150];  // will contain the path to the all students folder
    char in[50];  // will contain the path to the input file
    char out[50]; // will contain the path to the expected output file
    char name[20];
    char buff[100];
    char strarg1[1000];
    char strarg2[1000];
    char path[300];
    char buffer[100];
    int bytesRead;

    fd4 = open("results.csv", O_CREAT | O_WRONLY, 0666); // create a file for all the grades for each student
    if (fd4 == -1)
    {
        perror("error result file ");
        return -1;
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1)
    {
        perror("error! open(1)");
        return -1;
    }

    int l = lseek(fd1, 0, SEEK_SET);
    while ((bytes = read(fd1, &config[i], 1)) > 0) // reads from the config fille the all paths
    {
        if (config[i] == '\n' && flag1 == 0)
        {
            config[i] = '\0';
            sprintf(d, "%s", config); // copy the first line into 'd'
            config[0] = '\0';
            i = 0;
            flag1 = 1;
        }
        else if (config[i] == '\n' && flag1 == 1)
        {
            config[i] = '\0';
            sprintf(in, "%s", config); // copy the second line into 'in'
            config[0] = '\0';
            i = 0;
            flag1 = 2;
        }
        else if (config[i] == '\n' && flag1 == 2)
        {
            config[i] = '\0';
            sprintf(out, "%s", config); // copy the third line into 'out'
            config[0] = '\0';
            i = 0;
            flag1 = 0;
            break;
        }
        else
        {
            i++;
        }
    }
    close(fd1);
    if (bytes == -1) // checking if there was an error in write function while loop
    {
        perror("error read config");
        exit(1);
    }
 
    fd_input = open(in, O_RDONLY); // open the input file so we could take the 2 inputs(2 numbers) to send the students plans
    if (fd_input == -1)
    {
        perror("error open (2)");
        return -1;
    }

    while ((bytes = read(fd_input, &config[i], 1)) > 0)
    {
        if (config[i] == '\n' && flag1 == 0)
        {
            config[i] = '\0';
            sprintf(strarg1, "%s", config); // copy the first line into 'strarg1'
            config[0] = '\0';
            i = 0;
            flag1 = 1;
        }
        else if (config[i] == '\n' && flag1 == 1)
        {
            config[i] = '\0';
            sprintf(strarg2, "%s", config); // copy the second line into 'strarg2'
            config[0] = '\0';
            i = 0;
            flag1 = 0;
            break;
        }
        else
        {
            i++;
        }
    }
    close(fd_input);
    if (bytes == -1) // checking if there was an error in write function while loop
    {
        perror("error read input file from file");   
        exit(1);
    }

    pid_t pid = fork();
    int status;

    if (pid < 0)
    {
        printf("fork error\n");
        return -1;
    }
    else if (pid == 0)
    {
        fd5 = open("studenlist.txt", O_RDWR | O_CREAT, 0666); // the new file should contain the all students
        if (fd5 == -1)
        {
            perror("file error\n");
            return -1;
        }

        dup2(fd5, 1); // redirect stdout to the file
        close(fd5);

        char *args[] = {"ls", d, NULL}; // use the ls command to get all students names according the folders into a new file
        int x = execvp(args[0], args);
        if (x == -1)
        {
            perror("exec error\n");
            return -1;
        }
        exit(0);
    }
    else
    {
        wait(&status);
        fd5 = open("studenlist.txt", O_RDONLY); // should contain the all students in the file
        if (fd5 == -1)
        {
            perror("file error\n");
            return -1;
        }

        int num_rows = 0;
        while (bytes = read(fd5, &buff[i], 1) > 0) // reads from the file the number of rows into num_rows
        {
            if (buff[i] == '\n')
            {
                buff[i] = '\0';
                i = 0;
                num_rows++;
            }
            else
            {
                i++;
            }
        }
        if (bytes == -1) // checking if there was an error in write function while loop
        {
            perror("error read from studentlist file");
            exit(1); 
        }
        z = lseek(fd5, 0, SEEK_SET); // move the pointer to the begining of the file

        while (s < num_rows) // the number of rows = the number of students in studentlist.txt. s = 0. act for each student speratly
        {
            i = 0;
            while (read(fd5, &name[i], 1) == 1) // takes to first student name from the file
            {
                if (name[i] == '\n')
                {
                    name[i] = '\0';
                    break;
                }
                i++;
            }
            sprintf(path, "%s", d);
            sprintf(path + strlen_custom(path), "%s", name);
            sprintf(path + strlen_custom(path), "%s", "/main.exe"); // conatins a full link path with a student name and its c file

            if (i == 0)
            {
                break;
            }
            else if (i < 0)
            {
                perror("read");
                exit(1);
            }

            pid_t pid2 = fork(); // second child
            if (pid2 < 0)
            {
                printf("error fork \n");
                return -1;
            }
            else if (pid2 == 0)
            {
                fd2 = open("out.txt", O_RDWR | O_CREAT, 0666); // the file should contain the print result of the current student code
                if (fd2 == -1) // checking if there was an error in write function while loop
                {
                    perror("error read open (3)");
                    exit(1);
                }
                close(1);
                dup(fd2);
                close(fd2);
                char *args2[] = {path, strarg1, strarg2, NULL};
                int y = execvp(args2[0], args2);
                if (y == -1)
                {
                    printf("exec error 1 \n");
                    return -1;
                }
            }
            else
            {
                wait(&status);
            }

            pid_t pid3 = fork();
            int status2;
            if (pid3 < 0)
            {
                perror("error fork \n");
                return -1;
            }
            else if (pid3 == 0)
            {
                close(1);
                char *args3[] = {"./comp.out", "out.txt", out, NULL}; // send to part-A plan the output file for the student and the expected output
                z = execvp(args3[0], args3);
                if (z == -1)
                {
                    perror("exec error 2 \n");
                    return -1;
                }
                exit(0);
            }
            else
            {
                waitpid(pid3, &status2, 0);
                if (WIFEXITED(status2))
                {
                    exitStatus = WEXITSTATUS(status2);
                }

                if (exitStatus == 1)
                {
                    // if the output for the current student is '1', so write into "result.csv" the name with a grade
                    char str1[] = {",0\n"};
                    sprintf(name + strlen_custom(name), "%s", str1);
                    len = strlen_custom(name);
                    name[len] = '\0';
                    int w = write(fd4, &name, len);
                    if (w == -1)
                    {
                        perror("cannot write (3)");
                    }
                }

                else if (exitStatus == 2)
                {
                    // if the output for the current student is '2', so write into "result.csv" the name with a grade
                    char str2[] = {",100\n"};
                    sprintf(name + strlen_custom(name), "%s", str2);
                    len = strlen_custom(name);
                    name[len] = '\0';
                    int e = write(fd4, &name, len);
                    if (e == -1)
                    {
                        perror("cannot write (4)");
                    }
                }

                name[0] = '\0'; // clear the buffer fir the next name
                path[0] = '\0';
                s++;
            }
        }

        close(fd5);
        close(fd1);
        close(fd3);
        close(fd4);
    }
    return 0;
}
