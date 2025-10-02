#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 80

#define PROC_NEW    0
#define PROC_STOPPED 1
#define PROC_RUNNING 2
#define PROC_EXITED 3

typedef struct proc_desc {
    struct proc_desc *next;
    char name[80];
    int pid;
    int status;
    double t_submission, t_start, t_end;
} proc_t;

struct single_queue {
    proc_t  *first;
    proc_t  *last;
    long members;
};

struct single_queue global_q;

#define proc_queue_empty(q) ((q)->first == NULL)

void proc_queue_init(register struct single_queue *q) {
    q->first = q->last = NULL;
    q->members = 0;
}

void proc_to_rq_end(register proc_t *proc) {
    if (proc_queue_empty(&global_q))
        global_q.first = global_q.last = proc;
    else {
        global_q.last->next = proc;
        global_q.last = proc;
        proc->next = NULL;
    }
}

proc_t *proc_rq_dequeue() {
    register proc_t *proc;
    proc = global_q.first;
    if (proc == NULL)
        return NULL;

    global_q.first = proc->next;
    proc->next = NULL;

    return proc;
}

double proc_gettime() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double)(tv.tv_sec + tv.tv_usec / 1000000.0);
}

int num_processors = 1;  // Number of processors (or threads) to run simultaneously
double global_t;

pthread_t *processors;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *process_executor(void *arg) {
    proc_t *proc;
    int pid;
    int status;

    while (1) {
        pthread_mutex_lock(&queue_mutex);  // Locking the global queue before accessing
        proc = proc_rq_dequeue();  // Dequeue the first process from the global queue
        pthread_mutex_unlock(&queue_mutex);  // Unlock the queue after access

        if (proc == NULL) {
            break;  // Exit loop if no processes are left to execute
        }

        // Execute the process if it is new
        if (proc->status == PROC_NEW) {
            proc->t_start = proc_gettime();
            pid = fork();
            if (pid == -1) {
                perror("Fork failed");
                exit(1);
            }

            if (pid == 0) {
                // Child process executes the command
                printf("Executing %s\n", proc->name);
                execl(proc->name, proc->name, NULL);
            } else {
                // Parent process waits for the child process to finish
                proc->pid = pid;
                proc->status = PROC_RUNNING;
                waitpid(proc->pid, &status, 0);  // Wait for child process to finish
                proc->status = PROC_EXITED;
                proc->t_end = proc_gettime();

                // Output information about process execution
                printf("PID %d - CMD: %s\n", proc->pid, proc->name);
                printf("\tElapsed time = %.2lf secs\n", proc->t_end - proc->t_submission);
                printf("\tExecution time = %.2lf secs\n", proc->t_end - proc->t_start);
                printf("\tWorkload time = %.2lf secs\n", proc->t_end - global_t);
            }

            // Free the process memory after it finishes execution
            free(proc);
        }
    }
    return NULL;
}

void fcfs() {
    processors = malloc(sizeof(pthread_t) * num_processors);

    // Create threads (processors) for each processor
    for (int i = 0; i < num_processors; i++) {
        if (pthread_create(&processors[i], NULL, process_executor, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    // Wait for all threads (processors) to finish
    for (int i = 0; i < num_processors; i++) {
        pthread_join(processors[i], NULL);
    }

    free(processors);
}

int main(int argc, char **argv) {
    FILE *input;
    char exec[80];
    int c;
    proc_t *proc;

    if (argc == 1) {
        fprintf(stderr, "Invalid usage\n");
        exit(1);
    } else if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            perror("Invalid input file name");
            exit(1);
        }
    } else if (argc > 2) {
        if (!strcmp(argv[1], "FCFS")) {
            num_processors = atoi(argv[2]);  // Set number of processors from argument
            input = fopen(argv[3], "r");
            if (input == NULL) {
                perror("Invalid input file name");
                exit(1);
            }
        } else {
            fprintf(stderr, "Invalid usage\n");
            exit(1);
        }
    }

    // Read input file and create processes
    while ((c = fscanf(input, "%s", exec)) != EOF) {
        proc = malloc(sizeof(proc_t));
        proc->next = NULL;

        // Check if there is a number (either 1 or some other) following the process name
        int num;
        if (fscanf(input, "%d", &num) == 1) {
            // If a number is found (in this case `1`), simply ignore it and continue
            // No need to store the number, just move on to the next process name
        } else {
            // If no number is found, move the file pointer back one character (i.e., to the space)
            fseek(input, -1, SEEK_CUR);
        }

        // Copy the process name into the struct
        strcpy(proc->name, exec);
        proc->pid = -1;
        proc->status = PROC_NEW;
        proc->t_submission = proc_gettime();

        proc_to_rq_end(proc);  // Add process to global queue
    }

    global_t = proc_gettime();
    fcfs();

    printf("WORKLOAD TIME: %.2lf secs\n", proc_gettime() - global_t);
    printf("Scheduler exits\n");

    return 0;
}
