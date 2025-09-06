#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "FileSystem.hpp"
#include "FileAlreadyExistsException.hpp"
#include "FileMissingException.hpp"
#include "NoAvailableInodeException.hpp"
#include "NoFreeBlockAvailableException.hpp"
#include "UnformattedFilesystemException.hpp"

void printHelp() {
    std::cout << "Available commands:\n"
              << "  format                        - Format the filesystem\n"
              << "  create <filename>             - Create a new empty file\n"
              << "  write <filename> <data...>    - Write data to a file\n"
              << "  read <filename>               - Read and display file content\n"
              << "  delete <filename>             - Delete a file\n"
              << "  ls                            - List all files\n"
              << "  stats                         - Show block usage stats\n"
              << "  help                          - Show this help menu\n"
              << "  exit                          - Exit the program\n";
}

int main() {
    // Create a filesystem with 1000 blocks of 64 bytes each
    cse4733::FileSystem fs(1000 * 64, 64);

    std::cout << "In-Memory Filesystem Simulator\n";
    std::cout << "Type 'help' for a list of commands.\n";

    std::string line;
    while (true) {
        std::cout << "fs> ";
        if (!std::getline(std::cin, line)) break;  // EOF -> exit

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        try {
            if (cmd == "help") {
                printHelp();
            } else if (cmd == "exit") {
                break;
            } else if (cmd == "format") {
                fs.format();
                std::cout << "Filesystem formatted.\n";
            } else if (cmd == "create") {
                std::string filename;
                iss >> filename;
                if (filename.empty()) {
                    std::cout << "Usage: create <filename>\n";
                } else if (fs.createFile(filename)) {
                    std::cout << "Created file: " << filename << "\n";
                } else {
                    std::cout << "Failed to create file: " << filename << "\n";
                }
            } else if (cmd == "write") {
                std::string filename;
                iss >> filename;
                std::string data;
                std::getline(iss, data);
                if (filename.empty() || data.empty()) {
                    std::cout << "Usage: write <filename> <data>\n";
                } else {
                    // trim leading space from data
                    if (!data.empty() && data[0] == ' ') data.erase(0,1);
                    if (fs.writeFile(filename, data)) {
                        std::cout << "Wrote to " << filename << ": \"" << data << "\"\n";
                    } else {
                        std::cout << "Failed to write to " << filename << "\n";
                    }
                }
            } else if (cmd == "read") {
                std::string filename;
                iss >> filename;
                if (filename.empty()) {
                    std::cout << "Usage: read <filename>\n";
                } else {
                    std::string content = fs.readFile(filename);
                    if (!content.empty()) {
                        std::cout << filename << " content: \"" << content << "\"\n";
                    } else {
                        std::cout << filename << " is empty or does not exist.\n";
                    }
                }
            } else if (cmd == "delete") {
                std::string filename;
                iss >> filename;
                if (filename.empty()) {
                    std::cout << "Usage: delete <filename>\n";
                } else if (fs.deleteFile(filename)) {
                    std::cout << "Deleted file: " << filename << "\n";
                } else {
                    std::cout << "File not found: " << filename << "\n";
                }
            } else if (cmd == "ls") {
                std::vector<std::string> files = fs.listFiles();
                if (files.empty()) {
                    std::cout << "(no files)\n";
                } else {
                    for (const auto& f : files) {
                        std::cout << f << "\n";
                    }
                }
            } else if (cmd == "stats") {
                std::cout << "Free blocks: " << fs.getFreeBlockCount()
                          << " / " << fs.getTotalBlockCount() << "\n";
            } else if (!cmd.empty()) {
                std::cout << "Unknown command: " << cmd << "\n";
            }
        }
        catch (const cse4733::UnformattedFilesystemException&) {
            std::cout << "Error: filesystem not formatted. Run 'format' first.\n";
        }
        catch (const cse4733::FileAlreadyExistsException& e) {
            std::cout << e.what() << "\n";
        }
        catch (const cse4733::FileMissingException& e) {
            std::cout << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting filesystem.\n";
    return 0;
}