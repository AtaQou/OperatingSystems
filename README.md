🖥️ Operating Systems Projects 2024-2025

👥 Authors

Sotirios Antoniou (1067512)

Konstantinos Christakos (1070903)

📁 Project Structure
PROJECT1/
├── erwthma1/      → Shell scripting exercise (Passenger management)
├── erwthma2/      → Process synchronization using semaphores
├── erwthma3/      → Process scheduling & memory management simulation
└── launch1.c      → Launcher helper file for testing

project2/
└── scheduler_v0/  → Multi-processor scheduler (FCFS, RR, RR-AFF)
├── scheduler_v1.c  → Phase 1: Multiple processors, all policies
├── scheduler_v2.c  → Phase 2: FCFS with multi-processor requests
├── Makefile
└── input/          → Workload files (homogeneous.txt, reverse.txt, etc.)

🧩 Project 1 – Shell Scripting

File: erwthma1/processes_ipc.sh

Simulates a ship evacuation drill by managing and analyzing passenger data.

<details> <summary>💡 Features (click to expand)</summary>
# Insert Data
./processes_ipc.sh insert passenger_data.csv

# Search Passenger
./processes_ipc.sh search "Christakos"

# Update Passenger
./processes_ipc.sh update 123 fullname:"Sotiris Antoniou"

# Display File (paginated)
./processes_ipc.sh display

# Generate Reports
./processes_ipc.sh reports

</details>
⚙️ Project 2 – Process Synchronization & Semaphores

Files: launch.c, passenger.c, ipc_utils.h, Makefile

Simulates passengers boarding limited-capacity lifeboats using threads and semaphores.

<details> <summary>💡 Concept (click to expand)</summary>

Each passenger → thread

Each lifeboat → semaphore controlling seat availability

Boats depart when full, then become available again

</details> <details> <summary>💻 Build & Run</summary>
cd PROJECT1/erwthma2
make
./launch


Modify passenger count, boat count, or seats in launch.c

</details>
🧮 Project 3 – CPU Scheduling & Memory Management Simulation

File: erwthma3/erwthma3.c

<details> <summary>💡 Details (click to expand)</summary>

Memory: 512 KB (1 KB blocks)

Processes: pid, arrival_time, duration, remaining_time, memory_needed, in_memory

Scheduling: Round Robin (quantum = 3 ms)

Memory Allocation: First Fit

</details> <details> <summary>💻 Build & Run</summary>
gcc erwthma3.c -o scheduler
./scheduler

</details>
🧠 Project 2 (Second Assignment) – Multiprocessor Scheduler

Directory: project2/scheduler_v0/scheduler/

Simulates a Unix scheduler with multiple processors and multiple policies:

FCFS – First Come First Served

RR – Round Robin

RR-AFF – Round Robin with processor affinity

<details> <summary>💻 Build</summary>
cd project2/scheduler_v0/scheduler
make

</details>
🧭 How to Run the Scheduler
<details> <summary>Phase 1 – v1 Scheduler (Multiple Policies)</summary>
# FCFS (1 processor)
./scheduler FCFS input/homogeneous.txt

# RR (quantum = 1000 ms)
./scheduler RR 1000 input/homogeneous.txt

# RR-AFF (quantum = 500 ms)
./scheduler RR-AFF 500 input/homogeneous.txt

# Reverse workload
./scheduler FCFS input/reverse.txt
./scheduler RR 1000 input/reverse.txt
./scheduler RR-AFF 500 input/reverse.txt


Number of processors is defined in scheduler_v1.c as NUM_PROCESSORS.

</details> <details> <summary>Phase 2 – v2 Scheduler (FCFS with Multi-Processor Requests)</summary>
# FCFS, 4 processors, homogeneous workload
./scheduler FCFS 4 input/homogeneous.txt

# FCFS, 2 processors, homogeneous workload
./scheduler FCFS 2 input/homogeneous.txt

# FCFS, 4 processors, reverse workload
./scheduler FCFS 4 input/reverse.txt

# FCFS, 2 processors, reverse workload
./scheduler FCFS 2 input/reverse.txt


Processes requesting more than available processors are skipped.

</details>
🧰 Full Commands Table
Version	Policy	Quantum	Processors	Input File	Command
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

All scripts/programs include comments explaining their logic.

Data files (e.g., passenger_data.csv) must be in the same directory as the scripts.

Output and reports are auto-generated in their respective directories.

Tested under Linux (Ubuntu 22.04).