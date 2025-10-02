/* Αντωνίου Σωτήριος, 1067512 */
/* Κωνσταντίνος Χριστάκος, 1070903 */
#pragma once
#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <semaphore.h>

#define MAX_NAME_LEN 20

extern const int BOATS;
extern const int PASSENGERS;
extern const int SEATS;
extern sem_t* seat_sems[];  // Shared array for boat semaphores
// Function declaration for boarding
void* boarding(void* args);

#endif // IPC_UTILS_H
