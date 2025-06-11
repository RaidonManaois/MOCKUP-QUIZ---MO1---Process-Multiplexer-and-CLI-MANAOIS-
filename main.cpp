#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <atomic>
#include <cstdlib>
#include "config.h"

using namespace std;

struct Process {
    string name;
    time_t timestamp;
    int total_ins;
    int current_ins;
    int core;
    bool finished;
};

vector<Process> processes;
mutex cpu_mutex;
atomic<bool> scheduler_running(false);
atomic<bool> scheduler_should_stop(false);
Config config;
int finished_processes = 0;
int cores_used = 0;

// === Config File Reader ===
bool load_config(const string& filename, Config& config) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string key, value;
    while (file >> key >> value) {
        if (key == "num-cpu") config.num_cpu = stoi(value);
        else if (key == "scheduler") {
            if (value.front() == '"' && value.back() == '"')
                config.scheduler_type = value.substr(1, value.size() - 2);
            else
                config.scheduler_type = value;
        }
        else if (key == "quantum-cycles") config.quantum_cycles = stoi(value);
        else if (key == "batch-process-freq") config.batch_freq = stoi(value);
        else if (key == "min-ins") config.min_ins = stoi(value);
        else if (key == "max-ins") config.max_ins = stoi(value);
        else if (key == "delay-per-exec") config.delay_per_exec = stoi(value);
    }

    return true;
}

// === Process Creation ===
void create_process(const string& name) {
    Process p;
    p.name = name;
    p.timestamp = time(nullptr);
    p.total_ins = config.min_ins + rand() % (config.max_ins - config.min_ins + 1);
    p.current_ins = 0;
    p.core = -1;
    p.finished = false;
    lock_guard<mutex> lock(cpu_mutex);
    processes.push_back(p);
}

// === Screen Listing ===
void screen_ls() {
    lock_guard<mutex> lock(cpu_mutex);
    int running = 0;
    for (const auto& p : processes) {
        if (!p.finished) running++;
    }
    cores_used = min(config.num_cpu, running);

    cout << "\nCPU utilization: ";
    if (cores_used == config.num_cpu) cout << "100%\n";
    else if (cores_used == 0) cout << "0%\n";
    else cout << (cores_used * 100 / config.num_cpu) << "%\n";

    cout << "Cores used: " << cores_used << endl;
    cout << "Cores available: " << (config.num_cpu - cores_used) << endl;
    cout << "\n-------------------------------------------------------------------\n";

    cout << "Running processes:\n";
    for (const auto& p : processes) {
        if (!p.finished) {
            cout << "process " << p.name << " (" << ctime(&p.timestamp)
                << ") Core: " << (p.core == -1 ? 0 : p.core)
                << " " << p.current_ins << " / " << p.total_ins << endl;
        }
    }

    cout << "\nFinished processes:\n";
    for (const auto& p : processes) {
        if (p.finished) {
            cout << "process " << p.name << " (" << ctime(&p.timestamp)
                << ") Core: Finished " << p.total_ins << " / " << p.total_ins << endl;
        }
    }
    cout << "----------------------------------------------------\n\n";
}

// === Simple RR Scheduler ===
void scheduler() {
    scheduler_running = true;
    while (!scheduler_should_stop) {
        {
            lock_guard<mutex> lock(cpu_mutex);
            for (auto& p : processes) {
                if (p.finished) continue;
                int step = min(config.quantum_cycles, p.total_ins - p.current_ins);
                p.current_ins += step;
                if (p.current_ins >= p.total_ins) {
                    p.finished = true;
                    finished_processes++;
                }
            }
        }
        this_thread::sleep_for(chrono::milliseconds(config.delay_per_exec > 0 ? config.delay_per_exec : 100));
    }
    scheduler_running = false;
}

int main() {
    srand(time(nullptr));
    if (!load_config("config.txt", config)) {
        cerr << "Error loading config file.\n";
        return 1;
    }

    cout << R"(  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$$  /$$$$$$  /$$     /$$
 /$$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$_____/ /$$__  $$|  $$   /$$/
| $$  \__/| $$  \__/| $$  \ $$| $$  \ $$| $$      | $$  \__/ \  $$ /$$/
| $$      |  $$$$$$ | $$  | $$| $$$$$$$/| $$$$$   |  $$$$$$   \  $$$$/
| $$       \____  $$| $$  | $$| $$____/ | $$__/    \____  $$   \  $$/
| $$    $$ /$$  \ $$| $$  | $$| $$      | $$       /$$  \ $$    | $$
|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$      | $$$$$$$$|  $$$$$$/    | $$
 \______/  \______/  \______/ |__/      |________/ \______/     |__/
)" << endl;

    cout << "\nWelcome to the CSOPESY OS\n";
    cout << "Developer:\nRaidon Manaois\nLast Updated: 2025-06-11\n\n";

    string command;
    thread sched_thread;
    bool scheduler_test_ran = false;

    while (true) {
        cout << "Enter a command: ";
        getline(cin, command);

        if (command == "exit") break;
        else if (command == "scheduler -test") {
            if (!scheduler_running && !scheduler_test_ran) {
                cout << "scheduler-test command recognized. Doing something.\n";
                cout << "Scheduler Type   : " << config.scheduler_type << endl;
                cout << "Quantum Cycles   : " << config.quantum_cycles << endl;
                cout << "CPU Count        : " << config.num_cpu << endl;
                cout << "Batch Freq       : " << config.batch_freq << endl;
                cout << "Instruction Range: " << config.min_ins << " - " << config.max_ins << endl;
                cout << "Delay per Exec   : " << config.delay_per_exec << endl;

                for (int i = 0; i < config.num_cpu; ++i) {
                    create_process("p" + to_string(i));
                }

                scheduler_should_stop = false;
                sched_thread = thread(scheduler);
                scheduler_test_ran = true;

                cout << "Scheduler started with " << config.num_cpu << " CPUs.\n";
            }
        }
        else if (command == "scheduler -stop") {
            if (scheduler_running) {
                scheduler_should_stop = true;
                if (sched_thread.joinable()) sched_thread.join();
                cout << "Scheduler stopped.\n";

                // Mark all unfinished processes as finished
                lock_guard<mutex> lock(cpu_mutex);
                for (auto& p : processes) {
                    if (!p.finished) {
                        p.finished = true;
                        p.current_ins = p.total_ins;
                    }
                }
            }
            else {
                cout << "Scheduler is not running.\n";
            }
        }
        else if (command == "screen -ls" || command == "report-util") screen_ls();
        else if (command.rfind("screen -s ", 0) == 0) {
            string pname = command.substr(10);
            if (!pname.empty()) {
                create_process(pname);
                cout << "Process \"" << pname << "\" created.\n";
            }
            else {
                cout << "Usage: screen -s <name>\n";
            }
        }
        else if (command.rfind("screen -r ", 0) == 0) {
            string pname = command.substr(10);
            bool found = false;
            for (const auto& p : processes) {
                if (p.name == pname) {
                    cout << "Connected to " << pname << ". Type 'process -smi' to inspect.\n";
                    string subcmd;
                    getline(cin, subcmd);
                    if (subcmd == "process -smi") {
                        cout << pname << " :: " << p.current_ins << " / " << p.total_ins << endl;
                    }
                    else {
                        cout << "Unrecognized command in screen session.\n";
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Process " << pname << " not found.\n";
            }
        }
        else cout << "Unrecognized command.\n";
    }

    scheduler_should_stop = true;
    if (sched_thread.joinable()) sched_thread.join();

    return 0;
}
