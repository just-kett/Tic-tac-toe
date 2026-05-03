#include "config.h"
#include "gameLogger.h"

#include <iostream>
#include <string>

/**
 * Parse command line arguments for the program.
 */
RunConfig parseArgs(int argc, char* argv[]) {
    RunConfig config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-j" || arg == "--judge") {
            config.judge_mode = true;
            // auto turn off interactive
            config.interactive = false;

        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            config.input_file = argv[++i];

        } else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            config.log_file = argv[++i];
            if (config.log_file == "") {
                config.to_file = false;
            }

        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Tic-tac-toe Game (Version: " << VERSION << " by baluong.87)\n";
            std::cout << "Usage: \n"
                      << "\t" << "--judge, -j" << "\t" << "Enable judge mode (no UI, standard output only)\n"
                      << "\t" << "--input, -i" << "\t" << "Path to input file\n"
                      << "\t" << "--log, -l" << "\t" << "Path to log file (default: log.txt)\n";
            exit(0);
        }
    }

    return config;
}
