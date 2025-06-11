# CSOPESY MOCKUP-QUIZ---MO1---Process-Multiplexer-and-CLI

CSOPESY OS Emulator  
Process Scheduler & CLI Emulator

Developer:
- Raidon Manaois

Last Updated: June 11, 2025

------------------------------------------------------------

📂 How to Run the Program

Requirements:
- A C++ compiler (e.g., g++)
- Terminal or command prompt (Windows or Unix-based)

Steps to Compile and Run:

1. Open a terminal in the source code directory.
2. Compile the program:
   g++ -o csopesy main.cpp config.cpp console.cpp
3. Run the executable:
   ./csopesy        # On Unix-based systems
   csopesy.exe      # On Windows

------------------------------------------------------------

Entry Class File:

The main function is located in:
`main.cpp`

This file handles:
- Displaying the OS emulator interface
- Managing commands (e.g., screen -s, screen -ls, screen -r)
- Running the round-robin scheduler

------------------------------------------------------------

CLI Commands Summary:

- `screen -s <name>`       Start a new screen session with the given name
- `screen -ls`             List all active and finished processes
- `screen -r <name>`       Resume the screen session for a specific process and display `process-smi`
- `scheduler-test`         Starts scheduling with settings from `config.txt`
- `scheduler-stop`         Stops the scheduler
- `exit`                   Exit the OS emulator

------------------------------------------------------------

Notes for the Quiz:

✅ Processes to create:
- `proc-01`
- `proc-02`
- `proc-03`

✅ Use `screen -s` to create processes.

✅ Use `screen -ls` to verify processes.

✅ Use `screen -r <name>` followed by `process-smi` to display:
- Current instruction
- Total instructions
