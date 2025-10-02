/* Αντωνίου Σωτήριος, 1067512 */
/* Κωνσταντίνος Χριστάκος, 1070903 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMORY_SIZE 512 // 512 KB
#define TIME_QUANTUM 3  // 3 ms

// Process structure
typedef struct {
    int pid;
    int arrival_time;
    int duration;
    int remaining_time;
    int memory_needed;
    bool in_memory;
} Process;

// Memory block structure
typedef struct {
    int start;
    int size;
    bool free;
    int pid; // Process ID occupying this block (-1 if free)
} MemoryBlock;

// Function prototypes
void initialize_memory(MemoryBlock memory[]);
int allocate_memory(MemoryBlock memory[], int pid, int size);
void deallocate_memory(MemoryBlock memory[], int pid);
void simulate_round_robin(Process processes[], int n, MemoryBlock memory[], int memory_size);

int main() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    Process processes[n];
    MemoryBlock memory[MEMORY_SIZE];

    // Initialize memory
    initialize_memory(memory);

    // Input process details
    for (int i = 0; i < n; i++) {
        printf("Enter details for process %d (arrival_time duration memory_needed): ", i + 1);
        processes[i].pid = i + 1;
        scanf("%d %d %d", &processes[i].arrival_time, &processes[i].duration, &processes[i].memory_needed);
        processes[i].remaining_time = processes[i].duration;
        processes[i].in_memory = false;
    }

    // Simulate scheduling and memory management
    simulate_round_robin(processes, n, memory, MEMORY_SIZE);

    return 0;
}

// Initialize memory
void initialize_memory(MemoryBlock memory[]) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i].start = i;
        memory[i].size = 1;
        memory[i].free = true;
        memory[i].pid = -1; // No process occupies this block
    }
}

// Allocate memory using first-fit
int allocate_memory(MemoryBlock memory[], int pid, int size) {
    int start = -1, count = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].free) {
            if (start == -1) start = i; // Mark potential start
            count++;
            if (count == size) {
                // Mark blocks as allocated
                for (int j = start; j < start + size; j++) {
                    memory[j].free = false;
                    memory[j].pid = pid;
                }
                return start;
            }
        } else {
            start = -1; // Reset if a free block is not contiguous
            count = 0;
        }
    }
    return -1; // Not enough memory
}

// Deallocate memory
void deallocate_memory(MemoryBlock memory[], int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].pid == pid) {
            memory[i].free = true;
            memory[i].pid = -1;
        }
    }
}

void print_compact_memory_state(MemoryBlock memory[], int memory_size) {
    int start = 0; // Starting index of a region
    int current_pid = memory[0].pid; // Current PID (-1 if free)

    printf("  Memory:\n");
    for (int i = 1; i <= memory_size; i++) {
        // Check if the region ends (different PID or end of memory)
        if (i == memory_size || memory[i].pid != current_pid) {
            // Print the current region
            if (current_pid == -1) {
                printf("    %d-%d: Free\n", start, i - 1);
            } else {
                printf("    %d-%d: Process %d\n", start, i - 1, current_pid);
            }

            // Start a new region
            if (i < memory_size) {
                start = i;
                current_pid = memory[i].pid;
            }
        }
    }
}

void simulate_round_robin(Process processes[], int n, MemoryBlock memory[], int memory_size) {
    int time = 0, completed = 0;

    while (completed < n) {
        bool progress = false;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0) {
                if (!processes[i].in_memory) {
                    int start = allocate_memory(memory, processes[i].pid, processes[i].memory_needed);
                    if (start != -1) {
                        processes[i].in_memory = true;
                        printf("Time %d: Process %d loaded into memory\n", time, processes[i].pid);
                    } else {
                        printf("Time %d: Not enough memory for process %d\n", time, processes[i].pid);
                        continue;
                    }
                }

                // Execute process for the time quantum
                int exec_time = (processes[i].remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : processes[i].remaining_time;
                processes[i].remaining_time -= exec_time;
                time += exec_time; // Advance time
                progress = true;

                printf("Time %d:\n", time);
                printf("  CPU: Executing Process %d for %d ms, remaining time: %d\n",
                       processes[i].pid, exec_time, processes[i].remaining_time);
                print_compact_memory_state(memory, memory_size);

                if (processes[i].remaining_time == 0) {
                    completed++;
                    printf("  Process %d completed\n", processes[i].pid);
                    deallocate_memory(memory, processes[i].pid);
                    processes[i].in_memory = false;
                }
            }
        }

        if (!progress) {
            time++; // Increment time if no process made progress
        }
    }
}
