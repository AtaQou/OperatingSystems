/* Αντωνίου Σωτήριος, 1067512 */
/* Κωνσταντίνος Χριστάκος, 1070903 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "ipc_utils.h" // My header

void* boarding(void* args) {
    int id = *(int*)args;
    int boarded = 0; // Tracks whether the passenger has boarded

    while (!boarded) {
        for (int i = 0; i < BOATS; i++) {
            if (sem_trywait(seat_sems[i]) == 0) { // Try to board boat i
                printf("Passenger %d: Boarded boat %d.\n", id, i + 1);
                sleep(2); // Simulate travel time
                printf("Passenger %d: Reached destination and disembarked from boat %d.\n", id, i + 1);
                sleep(2);
                sem_post(seat_sems[i]); // Release the seat
                boarded = 1;
                break;
            }
        }
        if (!boarded) {
            printf("Passenger %d: Waiting for a seat on a boat...\n", id);
            sleep(5); // Simulate waiting time before retrying
        }
    }

    free(args);
    return 0;
}
