#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

using namespace std;

// Execute a read-only system command and capture its output.
string runCommand(const string& command) {
    string result;
    char buffer[256];

    FILE* pipe = popen(command.c_str(), "r");

    if (pipe == nullptr) {
        return "Unable to retrieve information.";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);
    return result;
}

// Retrieve hardware information.
void showSystemInfo() {
    uint64_t memoryBytes = 0;
    size_t size = sizeof(memoryBytes);

    cout << "\n========== SYSTEM INFORMATION ==========\n";

    if (sysctlbyname("hw.memsize", &memoryBytes, &size,
                     nullptr, 0) == 0) {
        cout << "Physical Memory : "
             << fixed << setprecision(2)
             << (double)memoryBytes / (1024.0 * 1024.0 * 1024.0)
             << " GB\n";
    } else {
        cout << "Physical Memory : Unavailable\n";
    }

    cout << "Architecture    : Apple Silicon or compatible ARM64\n";
    cout << "Processor Cores : "
         << sysconf(_SC_NPROCESSORS_ONLN) << "\n";

    cout << "\nmacOS Version:\n";
    cout << runCommand("sw_vers -productVersion");
}

// Retrieve a CPU utilization sample from macOS.
void showCPUUsage() {
    cout << "\n========== CPU UTILIZATION ==========\n";

    string output = runCommand(
        "top -l 2 -n 0 | grep 'CPU usage' | tail -n 1"
    );

    if (output.empty()) {
        cout << "CPU utilization is currently unavailable.\n";
    } else {
        cout << output;
    }

    cout << "\nNote: CPU readings vary with workload.\n";
}

// Display processes with the highest CPU usage.
void showTopProcesses() {
    cout << "\n========== TOP PROCESSES ==========\n";

    cout << runCommand(
        "ps -A -o pid,comm,%cpu,%mem | "
        "sort -nr -k3 | head -n 11"
    );
}

// Display virtual memory statistics.
void showMemoryStats() {
    cout << "\n========== MEMORY STATISTICS ==========\n";

    cout << runCommand("vm_stat");

    cout << "\nNote: Memory pressure and swap activity "
            "provide additional context.\n";
}

// Main application menu.
int main() {
    int choice = 0;

    while (true) {
        cout << "\n";
        cout << "============================================\n";
        cout << "              OPTIMAC AI v0.1               \n";
        cout << "       macOS System Monitoring Tool          \n";
        cout << "============================================\n";
        cout << "1. System Information\n";
        cout << "2. CPU Utilization\n";
        cout << "3. Top Processes\n";
        cout << "4. Memory Statistics\n";
        cout << "5. Run Basic System Report\n";
        cout << "0. Exit\n";
        cout << "--------------------------------------------\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            if (cin.eof()) {
                break;
            }

            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                showSystemInfo();
                break;

            case 2:
                showCPUUsage();
                break;

            case 3:
                showTopProcesses();
                break;

            case 4:
                showMemoryStats();
                break;

            case 5:
                showSystemInfo();
                showCPUUsage();
                showTopProcesses();
                showMemoryStats();
                break;

            case 0:
                cout << "\nOptiMac AI shutting down safely.\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

