#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    int num_cpu;
    std::string scheduler_type;
    int quantum_cycles;
    int batch_freq;
    int min_ins;
    int max_ins;
    int delay_per_exec;

    bool loadConfig(const std::string& filename);
};

#endif
