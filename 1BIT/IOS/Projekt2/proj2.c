/**
*   Author: Tomas Zigo
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>

#define SEMAPHORE1_NAME "/xzigot00_semaphore_1"
#define SEMAPHORE2_NAME "/xzigot00_semaphore_2"
#define SEMAPHORE3_NAME "/xzigot00_semaphore_3"
#define SEMAPHORE4_NAME "/xzigot00_semaphore_4"
#define SEMAPHORE5_NAME "/xzigot00_semaphore_5"
#define LOCKED 0
#define UNLOCKED 1
#define OUTPUTFILE "proj2.out"

//semaphore names
sem_t *mutex = NULL;
sem_t *multiplex = NULL;
sem_t *bus = NULL;
sem_t *boarded = NULL;
sem_t *print = NULL;

//resources name
int *boarding_counter = NULL;
int *unboarding_counter = NULL;
int *shared_counter = NULL;
int *person = NULL;
int *enter = NULL;
int boarding_counter_id = 0;
int unboarding_counter_id = 0;
int shared_counter_id = 0;
int person_id = 0;
int enter_id = 0;

//file name
FILE *file = NULL;

//function test arguments
int test_args(int arg_num, char *arg[]);

//test if arg i number
int not_number(char *array);

//free all semaphores
void free_semaphores();

//free all resources
void free_resources();

//set reources
int set_resources();

//set semaphores
int set_semaphores();

//close file
void close_file();

//rider working process
void rider_work();



int main(int argc, char *argv[])
{
    int R,C,ART,ABT;
    srand((unsigned)time(NULL)); //randomize time
    //prevent buffering
    setbuf(stdout, NULL);   
    setbuf(stderr, NULL);
    if((test_args(argc, argv))==1)
        return 1;

    if(set_semaphores()!=0)
    {
        fprintf(stderr, "Error-Semaphores setting failed!\n");
        free_resources();
        free_semaphores();
        close_file();
        return 1;
    }

    if(set_resources()!=0)
    {
        fprintf(stderr, "Error-Resources setting failed!\n");
        free_resources();
        free_semaphores();
        close_file();
        return 1;
    }

    R=(int)strtol(argv[1],NULL,10);
    C=(int)strtol(argv[2],NULL,10);
    ART=(int)strtol(argv[3],NULL,10);
    ABT=(int)strtol(argv[4],NULL,10);
    //check if file can be opened
    if ((file = fopen(OUTPUTFILE, "w"))==NULL)
    {
        fprintf(stderr, "Error-cannot open file %s!\n", OUTPUTFILE);
        return 1;
    }
    //prevent file buffering 
    setbuf(file, NULL);

    int n = 0;
    pid_t riders_pid=0;

    //create bus process
    pid_t bus_pid = fork();
    //child process
    if (bus_pid == 0)
    {
        sem_wait(print);
        fprintf(file, "%d\t\t : BUS\t\t\t : start\n",(*shared_counter)++);
        sem_post(print);

        //until all riders are transported
        for(int i=0; i<R; i++)
        {
            sem_wait(print);
            fprintf(file, "%d\t\t : BUS\t\t\t : arrival\n",(*shared_counter)++);
            sem_post(print);
            sem_wait(mutex);

            if (*person < C)
            {
                n = *person;
            }
            else
            {
                n = C;
            }

            //loop all riders
            for(int i=0; i < n; i++)
            {   
                if (i==0)
                {
                    sem_wait(print);
                    fprintf(file, "%d\t\t : BUS\t\t\t : start boarding: %d\n",(*shared_counter)++,*enter);
                    sem_post(print);  
                }
                sem_post(bus); 
                sem_wait(boarded);
                (*enter)--; 
                if ((i+1)==n)
                {
                    sem_wait(print);
                    fprintf(file, "%d\t\t : BUS\t\t\t : end boarding: %d\n",(*shared_counter)++,*enter);
                    sem_post(print); 
                }

            }
            if ((*person-C) > 0)
            {
                *person = (*person - C);
            }
            else
            {
                *person = 0;
            }
            //when all riders are abroad
            sem_post(mutex);          
            sem_wait(print);
            fprintf(file, "%d\t\t : BUS\t\t\t : depart\n",(*shared_counter)++);
            sem_post(print);

            //set random bus ride time
            usleep((rand()%(ABT+1))*1000);

            sem_wait(print);
            fprintf(file, "%d\t\t : BUS\t\t\t : end\n",(*shared_counter)++);
            sem_post(print);

            //until somebody is aboard
            while (*unboarding_counter)
            {
                sem_post(multiplex);
                (*unboarding_counter)--;
            }

            //last rider
            if (*boarding_counter == R)
            {
                break;
            }
        }
        sem_wait(print);
        fprintf(file, "%d\t\t : BUS\t\t\t : finish\n",(*shared_counter)++);
        sem_post(print);

        //close and clean all resources...
        free_resources();
        free_semaphores();
        close_file();
        exit(0);
    }
    //error checking
    else if (bus_pid < 0)
    {
        fprintf(stderr, "Error-cannot create process bus\n");
        free_resources();
        free_semaphores();
        close_file();
        kill(riders_pid, SIGTERM);
        exit(1);
    }

    pid_t rider_pid;
    //rider generating process
    riders_pid = fork();
    if (riders_pid == 0)
    {
        for (int i = 0; i < R; i++)
        {
            //sleep rider for random time
            usleep((rand()%(ART+1))*1000);
            //process for every rider
            rider_pid = fork();
            if (rider_pid == 0)
            {
                //calling rider with rider_id
                rider_work(i+1);
                //clean resources..
                free_resources();
                free_semaphores();
                close_file();
                exit(0);
            }
            //error check
            else if (rider_pid < 0)
            {
                fprintf(stderr, "Error-cannot create process rider\n");
                free_resources();
                free_semaphores();
                close_file();
                //kill self-made process
                kill(0, SIGTERM);
                exit(1);
            }

        }
        //wait for all riders
        for (int i = 0; i < R; i++)
        {
            waitpid(rider_pid,NULL,0);
        }
        free_resources();
        free_semaphores();
        close_file();
        exit(0);
    }
    else if (riders_pid < 0)
    {
        fprintf(stderr, "Error-cannot create process riders\n");
        free_resources();
        free_semaphores();
        close_file();
        kill(bus_pid, SIGTERM);
        exit(1);
    }
    waitpid(bus_pid, NULL, 0);
    waitpid(riders_pid, NULL, 0);
    free_resources();
    free_semaphores();
    close_file();
    return 0;
}

int test_args(int arg_num, char *arg[])
{
    char *not_num=NULL;
    int err1,err2,err3,err4;
    if (arg_num != 5)
    {
        fprintf(stderr, "Error-wrong number of arguments!\n");
        return 1;
    }
    int arg_1=(int)strtol(arg[1],&not_num,10);
    err1=not_number(not_num);
    int arg_2=(int)strtol(arg[2],&not_num,10);
    err2=not_number(not_num);
    int arg_3=(int)strtol(arg[3],&not_num,10);
    err3=not_number(not_num);
    int arg_4=(int)strtol(arg[4],&not_num,10);
    err4=not_number(not_num);
    if (err1==1 || err2==1 || err3==1 || err4==1)
    {
        return 1;
    }
    else if ((arg_1 <= 0) || (arg_2 <= 0) || (arg_3 < 0)||(arg_3 > 1000) || ((arg_4 < 0)||(arg_4 > 1000)))
    {
        fprintf(stderr, "Error-wrong intervals in arguments!\n");
        return 1;
    }
    else
        return 0;
}

int not_number(char *array)
{
    if (*array != '\0')
    {
        fprintf(stderr, "Error-argument is not a number!\n");
        return 1;
    }
    return 0;
}

void free_semaphores()
{
    sem_close(mutex);
    sem_close(multiplex);
    sem_close(boarded);
    sem_close(bus);
    sem_close(print);
    sem_unlink(SEMAPHORE1_NAME);
    sem_unlink(SEMAPHORE2_NAME);
    sem_unlink(SEMAPHORE3_NAME);
    sem_unlink(SEMAPHORE4_NAME);
    sem_unlink(SEMAPHORE5_NAME);
}

void free_resources()
{
    shmdt(boarding_counter);
    shmdt(unboarding_counter);
    shmdt(shared_counter);
    shmdt(person);
    shmdt(enter);
    shmctl(boarding_counter_id, IPC_RMID, NULL);
    shmctl(unboarding_counter_id, IPC_RMID, NULL);
    shmctl(shared_counter_id, IPC_RMID, NULL);
    shmctl(person_id, IPC_RMID, NULL);
    shmctl(enter_id, IPC_RMID, NULL);
}

int set_resources()
{
    
    if ((boarding_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        return 2;
    }

    if ((boarding_counter = shmat(boarding_counter_id, NULL, 0)) == NULL)
    {
        return 2;
    }
    
    if ((unboarding_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        return 2;
    }

    if ((unboarding_counter = shmat(unboarding_counter_id, NULL, 0)) == NULL)
    {
        return 2;
    }

    if ((shared_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        return 2;
    }

    if ((shared_counter = shmat(shared_counter_id, NULL, 0)) == NULL)
    {
        return 2;
    }
    if ((person_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        return 2;
    }

    if ((person = shmat(person_id, NULL, 0)) == NULL)
    {
        return 2;
    }

    if ((enter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        return 2;
    }

    if ((enter = shmat(enter_id, NULL, 0)) == NULL)
    {
        return 2;
    }
    *shared_counter=1;
    return 0;
}

int set_semaphores()
{
    if ((mutex = sem_open(SEMAPHORE1_NAME, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED)
    {
        return 2;
    }

    if ((multiplex = sem_open(SEMAPHORE2_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        return 2;
    }

    if ((boarded = sem_open(SEMAPHORE3_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        return 2;
    }

    if ((bus = sem_open(SEMAPHORE4_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        return 2;
    }

    if ((print = sem_open(SEMAPHORE5_NAME, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED)
    {
        return 2;
    }

    return 0;
}

void close_file()
{
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
}

void rider_work(int rider_id)
{
    sem_wait(mutex);
    (*person)++; //number of riders waiting to enter station
    sem_wait(print);
    fprintf(file, "%d\t\t : RID %d\t\t : start\n",(*shared_counter)++,rider_id);
    sem_post(print);
    sem_post(mutex);

    sem_wait(print);
    (*enter)++; //number of riders on the station
    fprintf(file, "%d\t\t : RID %d\t\t : enter: %d\n",(*shared_counter)++,rider_id,*enter);
    sem_post(print);

    sem_wait(bus); //wait until until bus allow boarding
    sem_wait(print);
    fprintf(file, "%d\t\t : RID %d\t\t : boarding\n",(*shared_counter)++,rider_id);
    (*boarding_counter)++; 
    (*unboarding_counter)++;
    sem_post(print);
    sem_post(boarded);

    sem_wait(multiplex); //wait until bus print END
    sem_wait(print);
    fprintf(file, "%d\t\t : RID %d\t\t : finish\n",(*shared_counter)++,rider_id);
    sem_post(print);

}
