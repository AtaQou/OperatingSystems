/* Αντωνίου Σωτήριος, 1067512 */
/* Κωνσταντίνος Χριστάκος, 1070903 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 80
#define MAX_PROCESSORS 7 // Αριθμός επεξεργαστών

void fcfs(int num_processors);
void process_child(int processor_id, int read_fd, int write_fd);

#define PROC_NEW       0
#define PROC_STOPPED   1
#define PROC_RUNNING   2
#define PROC_EXITED    3

typedef struct proc_desc {
    struct proc_desc *next;
    char name[80];
    int pid;
    int status;
    double t_submission, t_start, t_end;
    int processors_required; // Number of processors required
} proc_t;

struct single_queue {
    proc_t *first;
    proc_t *last;
    long members;
};

struct single_queue global_q;

#define proc_queue_empty(q) ((q)->first == NULL)

void proc_queue_init(register struct single_queue *q) {
    q->first = q->last = NULL;
    q->members = 0;
}

void proc_to_rq (register proc_t *proc)
{
	if (proc_queue_empty (&global_q))
		global_q.last = proc;
	proc->next = global_q.first;
	global_q.first = proc;
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
    if (proc == NULL) return NULL;

    global_q.first = proc->next;
    proc->next = NULL;
    return proc;
}

double proc_gettime() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double)(tv.tv_sec + tv.tv_usec / 1000000.0);
}

#define FCFS    0

int policy = FCFS;
double global_t;
int available_processors; // Track available processors

void err_exit(char *msg) {
    printf("Error: %s\n", msg);
    exit(1);
}
void split_task(proc_t *proc);

void process_child(int processor_id, int read_fd, int write_fd) {
    proc_t proc;

    while (1) {
        // Notify parent that this processor is ready
        write(write_fd, "READY", 5);
        printf("[DEBUG] Processor %d sent READY signal.\n", processor_id);

        // Wait for a process
        if (read(read_fd, &proc, sizeof(proc_t)) <= 0) {
            printf("[DEBUG] Processor %d received no more processes. Exiting.\n", processor_id);
            break; // Exit if no process is available
        }

        printf("[DEBUG] Processor %d received process %s (requires %d processors).\n", 
               processor_id, proc.name, proc.processors_required);

        proc.t_start = proc_gettime();

        // Execute the process
        int pid = fork();
        if (pid == 0) {
            printf("[DEBUG] Processor %d starting execution of %s.\n", processor_id, proc.name);
            execl(proc.name, proc.name, NULL);
            perror("execl failed");
            exit(1);
        } else if (pid > 0) {
            proc.pid = pid; // Update the process's PID to the actual child PID
            waitpid(pid, NULL, 0);
            proc.t_end = proc_gettime();

            printf("\tProcessor %d completed process %s with pid %d.\n", processor_id, proc.name, proc.pid);
            printf("\tExecution time = %.2lf secs\n", proc.t_end - proc.t_start);
            printf("\tElapsed time = %.2lf secs\n", proc.t_end - proc.t_submission);
            printf("\tWorkload time = %.2lf secs\n", proc.t_end - global_t);

            // Notify parent that this processor has completed its task
            write(write_fd, "DONE", 4);
            printf("[DEBUG] Processor %d sent DONE signal.\n", processor_id);

            // Wait for acknowledgment from the parent
            char ack;
            if (read(read_fd, &ack, 1) > 0) {
                printf("[DEBUG] Processor %d received acknowledgment from parent.\n", processor_id);
            } else {
                printf("[DEBUG] Processor %d failed to receive acknowledgment.\n", processor_id);
            }
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    exit(0);
}

void fcfs(int num_processors) {
    int pipes[MAX_PROCESSORS][2];
    int feedback_pipes[MAX_PROCESSORS][2];
    pid_t processors[MAX_PROCESSORS];
    int processor_ready[MAX_PROCESSORS] = {0}; // Track readiness of each processor

    available_processors = num_processors;

    // Create pipes and fork processors
    for (int i = 0; i < num_processors; i++) {
        if (pipe(pipes[i]) < 0 || pipe(feedback_pipes[i]) < 0) {
            err_exit("Pipe creation failed!");
        }
        processors[i] = fork();
        if (processors[i] == 0) {
            close(pipes[i][1]); // Close write end in child
            close(feedback_pipes[i][0]); // Close read end in child
            process_child(i, pipes[i][0], feedback_pipes[i][1]);
        } else if (processors[i] > 0) {
            close(pipes[i][0]); // Close read end in parent
            close(feedback_pipes[i][1]); // Close write end in parent

            fcntl(feedback_pipes[i][0], F_SETFL, O_NONBLOCK); // Non-blocking feedback
        } else {
            err_exit("Failed to create processor!");
        }
    }

    proc_t *proc;
    int active_processors = num_processors;
    int proc_needed[MAX_PROCESSORS];

    while (active_processors > 0) {
        for (int i = 0; i < num_processors; i++) {
            char feedback[6] = {0};

            // Read feedback from the child process
            if (read(feedback_pipes[i][0], feedback, 5) > 0) {
                if (strcmp(feedback, "DONE") == 0) {
                    // Processor finished a task
                    available_processors += proc_needed[i];
                    printf("[DEBUG] Processor %d completed a task. Available processors: %d\n", 
                           i, available_processors);

                    // Send acknowledgment to the child
                    char ack = '1';
                    write(pipes[i][1], &ack, 1);
                    printf("[DEBUG] Parent sent acknowledgment to processor %d.\n", i);
                } else if (strcmp(feedback, "READY") == 0) {
                    // Processor is ready for a new task
                    processor_ready[i] = 1;
                    printf("[DEBUG] Processor %d is ready.\n", i);
                }
            }

            // Assign a new task if the processor is ready
            if (processor_ready[i]) {
                if (!proc_queue_empty(&global_q)) {
                    proc = proc_rq_dequeue();

                    if (proc->processors_required <= available_processors) {
                        if (proc->processors_required > 1) {
                            split_task(proc);
                            usleep(100);
                        }
                        else {
                            available_processors -= proc->processors_required;
                            proc_needed[i] = proc->processors_required;
                            printf("[DEBUG] Assigning process %s to processor %d with pid: %d. Available processors: %d\n",
                                proc->name, i, proc->pid, available_processors);
                            write(pipes[i][1], proc, sizeof(proc_t));
                            processor_ready[i] = 0; // Mark as busy
                        }
                    } else {
                        proc_to_rq_end(proc);
                    }
                } else {
                    // No more tasks, close processor pipe
                    close(pipes[i][1]);
                    active_processors--;
                    processor_ready[i] = 0;
                }
            }
        }
    }

    // Wait for all processors to terminate
    for (int i = 0; i < num_processors; i++) {
        waitpid(processors[i], NULL, 0);
    }

    printf("All processors finished execution.\n");
}

void split_task(proc_t *proc) {
    printf("[DEBUG] Splitting task: %s (PID: %d, Processors: %d)\n", proc->name, proc->pid, proc->processors_required);

    // Extract the workload from the task name (e.g., ../work/work7 -> 7)
    char *process_name = strrchr(proc->name, '/'); // Find the last '/' in the path
    if (process_name == NULL) {
        process_name = proc->name; // No path, use the full name
    } else {
        process_name++; // Move past the '/'
    }

    int total_workload = atoi(process_name + 4); // Skip "work" to get the number

    if (total_workload == 0) {
        printf("[DEBUG] Workload for task %s is 0. Skipping split.\n", proc->name);
        return;
    }

    int num_subtasks = proc->processors_required; // Number of subtasks to create
    int base_workload = total_workload / num_subtasks; // Base workload for each subtask
    int extra_workload = total_workload % num_subtasks; // Remainder to distribute unevenly

    for (int i = 0; i < num_subtasks; i++) {
        proc_t *subtask = malloc(sizeof(proc_t));
        if (!subtask) {
            printf("Error: Memory allocation for subtask failed!\n");
            exit(1);
        }

        // Assign workload to this subtask
        int workload = base_workload + (i < extra_workload ? 1 : 0);

        // Create new task name with adjusted workload and path prefix
        snprintf(subtask->name, sizeof(subtask->name), "../work/work%d", workload);

        // Assign unique PIDs
        subtask->pid = proc->pid + i + 1; // Ensure unique PIDs for subtasks
        subtask->processors_required = 1; // Each subtask requires 1 processor
        subtask->status = PROC_NEW;
        subtask->t_submission = proc_gettime();

        printf("[DEBUG] Created subtask: %s (Sub-PID: %d, Workload: %d)\n",
               subtask->name, subtask->pid, workload);

        // Requeue the subtask at the front of the queue
        proc_to_rq(subtask);
    }

    // Free the original process since it has been split into subtasks
    free(proc);
}

int main(int argc, char **argv) {
    FILE *input;
    char exec[80];
    proc_t *proc;

    if (argc < 3) {
        err_exit("Invalid usage. Provide policy, number of processors, and input file.");
    }

    int num_processors = atoi(argv[2]);
    if (num_processors <= 0 || num_processors > MAX_PROCESSORS) {
        err_exit("Invalid number of processors.");
    }

    input = fopen(argv[3], "r");
    if (input == NULL) err_exit("Invalid input file name");

    proc_queue_init(&global_q);

    /* Read input file */
   while (fscanf(input, "%s", exec) != EOF) {
    proc = malloc(sizeof(proc_t));
    proc->next = NULL;

    int processors_required = 1; // Default value
    if (fscanf(input, "%d", &processors_required) != 1) {
        // No number found, rewind for next read
        fseek(input, -1, SEEK_CUR);
    }

    proc->processors_required = processors_required; // Save processors required
    strcpy(proc->name, exec);
    proc->pid = -1;
    proc->status = PROC_NEW;
    proc->t_submission = proc_gettime();

    if (processors_required > num_processors) {
        printf("Error: Process %s requires %d processors, but only %d are available. Skipping.\n",
               proc->name, processors_required, num_processors);
        free(proc); // Clean up memory
    } else {
        proc_to_rq_end(proc); // Add to the queue
    }
}
    fclose(input);

    global_t = proc_gettime();
    if (policy == FCFS) {
        fcfs(num_processors);
    } else {
        err_exit("Only FCFS is implemented.");
    }

    printf("WORKLOAD TIME: %.2lf secs\n", proc_gettime() - global_t);
    printf("Scheduler exits.\n");
    return 0;
}
