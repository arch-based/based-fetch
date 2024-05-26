#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

#define CYAN "\x1b[36m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

/**
 * @brief Executes a shell command and returns its output.
 * @param command The command to be executed.
 * @return The output of the command as a dynamically allocated string.
 */
char* execute_command(const char* command) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        fprintf(stderr, "Error executing command: %s\n", command);
        return NULL;
    }

    char* buffer = malloc(128);
    if (!fgets(buffer, 128, pipe)) {
        fprintf(stderr, "Error reading output from command: %s\n", command);
        pclose(pipe);
        free(buffer);
        return NULL;
    }

    pclose(pipe);
    buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character
    return buffer;
}

/**
 * @brief Prints a formatted output with a label and a value.
 * @param label The label for the output.
 * @param value The value to be printed.
 */
void print_output(const char* label, const char* value) {
    printf("%s%s%s%s %s%s%s\n", CYAN, BOLD, label, RESET, BOLD, value, RESET);
}

/**
 * @brief Determines the package manager in use and returns the command to get the number of installed packages.
 * @return The command to get the number of installed packages or "unknown" if no package manager is found.
 */
char* get_package_manager() {
    if (!system("command -v apt > /dev/null")) {
        return "apt-cache pkgnames | wc -l";
    } else if (!system("command -v dpkg > /dev/null")) {
        return "dpkg --list | wc -l";
    } else if (!system("command -v pacman > /dev/null")) {
        return "pacman -Qe | wc -l";
    } else if (!system("command -v zypper > /dev/null")) {
        return "zypper se -i | wc -l";
    } else if (!system("command -v dnf > /dev/null")) {
        return "dnf list installed | wc -l";
    } else {
        return "unknown";
    }
}

/**
 * @brief Retrieves the memory usage information.
 * @return The memory usage information as a dynamically allocated string.
 */
char* get_memory_usage() {
    return execute_command("free -m | awk 'NR==2{printf \"%sMiB / %sMiB (%.2f%%)\", $3,$2,$3*100/$2 }'");
}

int main() {
    // Retrieve system information
    char* user = USER;
    char* hostname = HOSTNAME;
    char* shell = SHELL;
    char* os = OS;
    char* kernel = execute_command("uname -sr");
    if (!kernel) return 1;
    char* uptime = execute_command("uptime -p | sed 's/up //'");
    if (!uptime) return 1;

    char* name = execute_command("cat /sys/devices/virtual/dmi/id/product_name");
    if (!name) return 1;
    char* version = execute_command("cat /sys/devices/virtual/dmi/id/product_version");
    if (!version) return 1;

    char host[256];
    sprintf(host, "%s %s", name, version);

    // Get the number of installed packages
    char* pkgs = execute_command(get_package_manager());
    if (!pkgs) return 1;

    // Get memory usage
    char* memory = get_memory_usage();
    if (!memory) return 1;

    // Print the formatted output
    printf("%s%s%s%s%s@%s%s%s%s\n", "                    ", CYAN, BOLD, user, RESET, CYAN, BOLD, hostname, RESET);
    printf("%s%s%s%s%s\n", "          ", CYAN, BOLD, "-----------------------------", RESET);
    print_output(" OS ~~~~~~~~~~>", os);
    print_output("󰌢 HOST: ~~~~~~~>", host);
    print_output("󰅐 UPTIME: ~~~~~>", uptime);
    print_output(" KERNEL: ~~~~~>", kernel);
    print_output(" SHELL: ~~~~~~>", shell);
    print_output(" PACKAGES: ~~~>", pkgs);
    print_output("󰍛 MEMORY: ~~~~~>", memory);    
 
    printf("\n");

    // Free dynamically allocated memory
    free(kernel);
    free(uptime);
    free(name);
    free(version);
    free(pkgs);
    free(memory);

    return 0;
}
