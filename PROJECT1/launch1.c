#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h> // Required for O_CREAT and O_EXCL

#define PASSENGERS 20 // Συνολικός αριθμός επιβατών
#define SEATS 5       // Θέσεις ανά λέμβο
#define BOATS 2       // Συνολικός αριθμός λέμβων

sem_t* seat_sems[BOATS]; // Σημαφόροι για τις θέσεις στις λέμβους

void* boarding(void* args) {
    int id = *(int*)args;
    int boarded = 0; // Σημειώνει αν ο επιβάτης επιβιβάστηκε

    while (!boarded) {
        for (int i = 0; i < BOATS; i++) {
            if (sem_trywait(seat_sems[i]) == 0) { // Προσπαθεί να μπει στη λέμβο i
                printf("Επιβάτης %d: Επιβιβάστηκα στη λέμβο %d.\n", id, i + 1);
                sleep(2); // Προσομοίωση χρόνου διαδρομής
                printf("Επιβάτης %d: Έφτασα στον προορισμό και αποβιβάστηκα από τη λέμβο %d.\n", id, i + 1);
                sleep(2);
                sem_post(seat_sems[i]); // Απελευθέρωση θέσης στη λέμβο
                boarded = 1;
                break;
            }
        }
        if (!boarded) {
            printf("Επιβάτης %d: Περιμένω θέση σε κάποια λέμβο...\n", id);
            sleep(5); // Προσομοίωση χρόνου αναμονής πριν ξαναπροσπαθήσει
        }
    }

    free(args);
    return 0;
}

int main(int argc, char *argv[]) {
    pthread_t threads[PASSENGERS];

    // Αρχικοποίηση σημαφόρων για κάθε λέμβο
    for (int i = 0; i < BOATS; i++) {
        char sem_name[20];
        snprintf(sem_name, 20, "/seat_semaphore_%d", i);
        seat_sems[i] = sem_open(sem_name, O_CREAT, 0644, SEATS);
        if (seat_sems[i] == SEM_FAILED) {
            perror("Failed to open semaphore");
            exit(EXIT_FAILURE);
        }
    }

    // Δημιουργία threads
    for (int i = 0; i < PASSENGERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&threads[i], NULL, &boarding, id) != 0) {
            perror("Failed to create thread");
        }
    }

    // Αναμονή για όλα τα threads
    for (int i = 0; i < PASSENGERS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    // Καταστροφή των σημαφόρων
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

    printf("Όλοι οι επιβάτες διασώθηκαν!\n");
    return 0;
}
