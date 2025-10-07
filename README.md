Operating Systems Projects 2024-2025

Authors:

Sotirios Antoniou (1067512)

Konstantinos Christakos (1070903)

📁 Project Structure
PROJECT1/
├── erwthma1/      → Shell scripting exercise (Passenger management)
├── erwthma2/      → Process synchronization using semaphores
├── erwthma3/      → Process scheduling & memory management simulation
└── launch1.c      → Launcher helper file for testing

project2/
└── scheduler_v0/  → Multi-processor scheduler implementation (FCFS, RR, RR-AFF)
├── scheduler_v1.c  → Phase 1: Multiple processors, all policies
├── scheduler_v2.c  → Phase 2: FCFS with multi-processor requests
├── Makefile
└── input/          → Workload files (homogeneous.txt, reverse.txt, etc.)

🧩 Project 1 – Shell Scripting

File: erwthma1/processes_ipc.sh

This Bash script simulates a ship evacuation drill by managing and analyzing passenger data.

Features:

Insert Data (insert_data): Add new passengers manually or from a CSV file

./processes_ipc.sh insert passenger_data.csv


Search Passenger (search_passenger): Search by first name, last name, or code

./processes_ipc.sh search "Christakos"


Update Passenger (update_passenger): Modify a single field or the whole record

./processes_ipc.sh update 123 fullname:"Sotiris Antoniou"


Display File (display_file): Paginated output

./processes_ipc.sh display


Generate Reports (generate_reports): Analytics files using awk, sed, grep

./processes_ipc.sh reports

⚙️ Project 2 – Process Synchronization & Semaphores

Files: erwthma2/launch.c, erwthma2/passenger.c, erwthma2/ipc_utils.h, erwthma2/Makefile

Description:

Simulates passengers boarding limited-capacity lifeboats during a ship evacuation using threads and semaphores.

Each passenger → thread

Each lifeboat → semaphore controlling seat availability

Boats depart when full and become available again

Build & Run:

cd PROJECT1/erwthma2
make
./launch


Parameters (passenger count, boat count, seats) can be modified in launch.c.

🧮 Project 3 – CPU Scheduling & Memory Management Simulation

File: erwthma3/erwthma3.c

Memory: 512 KB (1 KB blocks)

Processes: pid, arrival_time, duration, remaining_time, memory_needed, in_memory

Scheduling: Round Robin (quantum = 3 ms)

Memory Allocation: First Fit

Build & Run:

gcc erwthma3.c -o scheduler
./scheduler

🧠 Project 2 (Second Assignment) – Multiprocessor Scheduler

Directory: project2/scheduler_v0/scheduler/

Implements a Unix scheduler simulator for multiple processors and policies:

FCFS – First Come First Served

RR – Round Robin

RR-AFF – Round Robin with processor affinity

Each version refines or extends the scheduler to test new features.

Build:

cd project2/scheduler_v0/scheduler
make

🧭 How to Run the Scheduler (v1 & v2)
Phase 1 – v1 Scheduler (Multiple Policies, Multiple Processors)
# FCFS (1 processor)
./scheduler FCFS input/homogeneous.txt

# RR (quantum = 1000 ms)
./scheduler RR 1000 input/homogeneous.txt

# RR-AFF (quantum = 500 ms)
./scheduler RR-AFF 500 input/homogeneous.txt

# Example with reverse workload
./scheduler FCFS input/reverse.txt
./scheduler RR 1000 input/reverse.txt
./scheduler RR-AFF 500 input/reverse.txt


Note: Number of processors is defined in the source (scheduler_v1.c) as NUM_PROCESSORS.

Phase 2 – v2 Scheduler (FCFS with Multi-Processor Requests)
# FCFS, 4 processors, homogeneous workload
./scheduler FCFS 4 input/homogeneous.txt

# FCFS, 2 processors, homogeneous workload
./scheduler FCFS 2 input/homogeneous.txt

# FCFS, 4 processors, reverse workload
./scheduler FCFS 4 input/reverse.txt

# FCFS, 2 processors, reverse workload
./scheduler FCFS 2 input/reverse.txt


Each process may request multiple processors; processes requesting more than available are skipped.

🧰 Full Commands Table
Scheduler Version	Policy	Quantum	Processors	Input File	Command
v1	FCFS	–	1	homogeneous.txt	./scheduler FCFS input/homogeneous.txt
v1	RR	1000	1	homogeneous.txt	./scheduler RR 1000 input/homogeneous.txt
v1	RR-AFF	500	1	homogeneous.txt	./scheduler RR-AFF 500 input/homogeneous.txt
v1	FCFS	–	1	reverse.txt	./scheduler FCFS input/reverse.txt
v1	RR	1000	1	reverse.txt	./scheduler RR 1000 input/reverse.txt
v1	RR-AFF	500	1	reverse.txt	./scheduler RR-AFF 500 input/reverse.txt
v2	FCFS	–	4	homogeneous.txt	./scheduler FCFS 4 input/homogeneous.txt
v2	FCFS	–	2	homogeneous.txt	./scheduler FCFS 2 input/homogeneous.txt
v2	FCFS	–	4	reverse.txt	./scheduler FCFS 4 input/reverse.txt
v2	FCFS	–	2	reverse.txt	./scheduler FCFS 2 input/reverse.txt
🏁 Notes

All scripts and programs include comments explaining their logic.

Data files (e.g., passenger_data.csv) must be in the same directory as the scripts unless specified.

Output and reports are automatically created in the respective directories.

Tested under Linux (Ubuntu 22.04).