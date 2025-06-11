#include "console.h"
#include <iostream>

Console::Console() : name("unnamed"), currentLine(0) {}

Console::Console(const std::string& name) : name(name), currentLine(0) {}

void Console::print_info() const {
    std::cout << "Session: " << name << " | Current Line: " << currentLine << std::endl;
}

const std::string& Console::get_name() const {
    return name;
}

void Console::add_process(int pid) {
    process_ids.push_back(pid);
}

const std::vector<int>& Console::get_processes() const {
    return process_ids;
}
