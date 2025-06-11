#include "config.h"
#include <fstream>
#include <sstream>

bool Config::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string key;
    while (file >> key) {
        if (key == "num_cpu") file >> num_cpu;
        else if (key == "scheduler_type") file >> scheduler_type;
        else if (key == "quantum_cycles") file >> quantum_cycles;
        else if (key == "batch_freq") file >> batch_freq;
        else if (key == "min_ins") file >> min_ins;
        else if (key == "max_ins") file >> max_ins;
        else if (key == "delay_per_exec") file >> delay_per_exec;
    }

    return true;
}
