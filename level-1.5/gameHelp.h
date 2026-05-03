#ifndef GAME_HELP_H
#define GAME_HELP_H

#include <chrono>
#include <iostream>
#include <string>

template <typename Function>
auto measureExecutionTime(const std::string& label, Function func, bool enabled)
    -> std::invoke_result_t<Function> {
    if (!enabled) {
        return func();
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[TIME] " << label << ": " << duration.count() << " ms" << std::endl;

    return result;
}

#endif 
