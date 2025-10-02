/* Αντωνίου Σωτήριος, 1067512 */
/* Κωνσταντίνος Χριστάκος, 1070903 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h> // Required for O_CREAT and O_EXCL
#include "ipc_utils.h" // Include the common header

const int BOATS = 2;
const int PASSENGERS = 20;
const int SEATS = 5;

sem_t* seat_sems[BOATS]; // Semaphore array for boat seat availability

int main(int argc, char *argv[]) {
    pthread_t threads[PASSENGERS];

    // Initialize semaphores for each boat
    for (int i = 0; i < BOATS; i++) {
        char sem_name[20];
        snprintf(sem_name, 20, "/seat_semaphore_%d", i);
        seat_sems[i] = sem_open(sem_name, O_CREAT, 0644, SEATS);
        if (seat_sems[i] == SEM_FAILED) {
            perror("Failed to open semaphore");
            exit(EXIT_FAILURE);
        }
    }

    // Create passenger threads
    for (int i = 0; i < PASSENGERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&threads[i], NULL, &boarding, id) != 0) {
            perror("Failed to create thread");
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < PASSENGERS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    // Clean up semaphores
    for (int i = 0; i < BOATS; i++) {
        if (sem_close(seat_sems[i]) != 0) {
            perror("Failed to close semaphore");
        }
        char sem_name[20];
        snprintf(sem_name, 20, "/seat_semaphore_%d", i);
        if (sem_unlink(sem_name) != 0) {
            perror("Failed to unlink semaphore");
        }
    }

    printf("All passengers have been saved!\n");
    return 0;
}
