#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>

class Console {
public:
    int currentLine;

    Console();                          // Default constructor
    Console(const std::string& name);   // Constructor with session name

    void print_info() const;

    const std::string& get_name() const;
    void add_process(int pid);
    const std::vector<int>& get_processes() const;

private:
    std::string name;
    std::vector<int> process_ids;       // Track processes linked to this console
};

#endif
