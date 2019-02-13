#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "queue.h"

#define SEM_PATH "tmp/sem"
#define BUFFER_PATH "tmp/queue"

#define NSEMS 4
#define SEM_FLAGS 0
#define P_A_LETTERS 1
#define P_B_LETTERS 3
#define C_A_LETTERS 1
#define C_B_LETTERS 2

#define DEV 0

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

typedef enum
{
    EMPTY = 0,
    MUTEX,
    S_A,
    S_B
} Semaphore;

typedef enum
{
    A = 'A',
    B,
} Name;

int sem_id;
int queue_id;

key_t create_key()
{
    int status = 0;
    if ((status = ftok(SEM_PATH, 'S')) == -1)
    {
        fprintf(stderr, "error with code %d occured: failed to create key\n", errno);
        exit(errno);
    }
    return status;
}

int create_semaphores(key_t key)
{
    int status = 0;
    if ((status = semget(key, NSEMS, IPC_CREAT | S_IRUSR | S_IWUSR | 0777)) == -1)
    {
        fprintf(stderr, "error with code %d occured: failed to create semaphores\n", errno);
        exit(errno);
    }
    return status;
}

int set_sem_val(int sem_num, int val)
{
    union semun arg;
    // arg.buf = &semid_ds;
    arg.val = val;
    int status = 0;
    if ((status = semctl(sem_id, sem_num, SETVAL, arg)) == -1)
    {
        fprintf(stderr, "error with code %d occured: failed to set value to semaphore %d\n", errno, sem_num);
        exit(errno);
    }
    return status;
}

int change(int sem_num, int val, int sem_flags)
{
    struct sembuf sop;
    sop.sem_num = sem_num;
    sop.sem_op = val;
    sop.sem_flg = sem_flags;
    if (semop(sem_id, &sop, 1) == -1)
    {
        fprintf(stderr, "error with code %d occured: failed to change semaphore %d value by %d\n",
                errno,
                sem_num,
                val);
        exit(errno);
    }
    return 1;
}

int down(int sem_num, int val)
{
    return change(sem_num, -val, SEM_FLAGS);
}

int up(int sem_num, int val)
{
    return change(sem_num, val, SEM_FLAGS);
}

int create_shared_queue(key_t key)
{
    int status = 0;
    if ((status = shmget(key, sizeof(Queue), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)
    {
        fprintf(stderr, "error with code %d occured: failed to create queue\n", errno);
        exit(errno);
    }
    return status;
}

Queue *get_queue_address()
{
    Queue *queueAddr = (Queue *)shmat(queue_id, (void *)0, 0);
    if (queueAddr == (void *)-1)
    {
        fprintf(stderr, "error with code %d occured: failed to get queue addresss\n", errno);
        exit(errno);
    }
    return queueAddr;
}

void print_sem_values()
{
    short values[NSEMS];
    union semun arg;
    arg.array = values;
    int status = 0;
    if ((semctl(sem_id, 0, GETALL, arg)) == -1)
    {
        exit(errno);
    }

    printf("SEM VALUES: ");
    for (int i = 0; i < NSEMS; i++)
    {
        printf("%d ", values[i]);
    }
    printf("\n");
}

int get_sem_val(int sem_num)
{
    short values[NSEMS];
    union semun arg;
    arg.val = 0;
    int status = 0;
    if ((semctl(sem_id, 0, GETVAL, arg)) == -1)
    {
        exit(errno);
    }
    return arg.val;
}
void producer(Name producer_name, int letters_num)
{
    Queue *queue = get_queue_address();

    if (queue == (void *)-1)
    {
        fprintf(stderr, "error with code %d occured: producer %c failed to access queue \n", errno, producer_name);
        exit(errno);
    };
    while (1)
    {
#if DEV == 1
        // printf("Producer %c entered loop\n", producer_name);
        // print_sem_values();
        sleep(1);
#endif

        down(EMPTY, letters_num);
        down(MUTEX, 1);
        queue->prod['A' - producer_name] = queue->prod['A' - producer_name] + 1;
        printf("Producer %c entered CS\n", producer_name);
        print(queue);
        for (int i = 0; i < letters_num; i++)
        {
            char c = rand() % 26 + 97;
            push(queue, c);
            printf("Producer %c pushed %c to the queue\n", producer_name, c);
        }
        print(queue);

        // check minimal number of elements in queue
        // for consumer A
        if (queue->count >= C_A_LETTERS + 3 && !get_sem_val(S_A))
        {
            change(S_A, 1, IPC_NOWAIT);
        }
        else
        {
            // change(S_A, -1, IPC_NOWAIT);
        }

        // and check for consumer B
        if (queue->count >= C_B_LETTERS + 3 && !get_sem_val(S_B))
        {
            change(S_B, 1, IPC_NOWAIT);
        }
        else
        {
            // change(S_B, -1, IPC_NOWAIT);
        }

        up(MUTEX, 1);
    }
}

void consumer(Name consumer_name, Semaphore consumer_sem, int letters_num)
{
    Queue *queue = get_queue_address();
    while (1)
    {
#if DEV == 1
        // printf("Consumer %c entered loop\n", consumer_name);
        // print_sem_values();
        sleep(1);
#endif

        down(consumer_sem, 1);
        down(MUTEX, 1);
        queue->con['A' - consumer_name] = queue->con['A' - consumer_name] + 1;
        printf("Consumer %c entered CS\n", consumer_name);
        print(queue);
        for (int i = 0; i < letters_num; i++)
        {
            printf("Consumer %c popped %c from the queue\n", consumer_name, pop(queue));
        }
        print(queue);
        // check minimal number of elements in queue
        // for consumer A
        if (queue->count < S_A + 3 && get_sem_val(S_A))
        {
            change(S_A, -1, IPC_NOWAIT);
        }
        else
        {
            // change(S_A, 1, IPC_NOWAIT);
        }

        // and check for consumer B
        if (queue->count < S_B + 3 && get_sem_val(S_B))
        {
            change(S_B, -1, IPC_NOWAIT);
        }
        else
        {
            // change(S_B, 1, IPC_NOWAIT);
        }

        up(MUTEX, 1);
        up(EMPTY, letters_num);
    }
}

int main(int argv, char *argc[])
{
    srand(time(NULL));
    key_t key = create_key();
    sem_id = create_semaphores(key);
    queue_id = create_shared_queue(key);
    Queue *queue = get_queue_address();
    init(queue);

    set_sem_val(EMPTY, BUFFER_SIZE);
    set_sem_val(MUTEX, 0);
    set_sem_val(S_A, 0);
    set_sem_val(S_B, 0);

    printf("sem_id: %d, queue_id: %d\n", sem_id, queue_id);

    int pids[4];
    int pid;

    printf("parent process with pid %d\n", getpid());

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "error with code %d occured: failed to create producer %c\n", errno, A);
        break;
    case 0:
        printf("Producer A: process with pid %d\n", getpid());
        producer(A, P_A_LETTERS);
        return 0;
    default:
        pids[0] = pid;
        break;
    }

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "error with code %d occured: failed to create producer %c\n", errno, B);
        break;
    case 0:
        printf("Producer B: process with pid %d\n", getpid());
        producer(B, P_B_LETTERS);
        return 0;
    default:
        pids[1] = pid;
        break;
    }

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "error with code %d occured: failed to create producer %c\n", errno, A);
        break;
    case 0:
        printf("Consumer A: process with pid %d\n", getpid());
        consumer(A, S_A, C_A_LETTERS);
        return 0;
    default:
        pids[2] = pid;
        break;
    }

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "error with code %d occured: failed to create producer %c\n", errno, B);
        break;
    case 0:
        printf("Consumer B: process with pid %d\n", getpid());
        consumer(B, S_B, C_B_LETTERS);
        return 0;
    default:
        pids[3] = pid;
        break;
    }

    up(MUTEX, 1);
    getchar();
    for (int i = 0; i < 4; i++)
    {
        printf("Killing %d\n", pids[i]);
        kill(pids[i], SIGKILL);
    }

    printf("%d %d %d %d", queue->prod[0], queue->prod[1], queue->con[0], queue->con[1]);

    shmdt(queue);
    shmctl(queue_id, IPC_RMID, 0);
    semctl(sem_id, 0, IPC_RMID);
    return 0;
}