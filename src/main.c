#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

// Define ANSI escape codes for color and formatting
#define CYAN "\x1b[36m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

/**
 * @brief Executes a shell command and returns its output.
 *
 * This function takes a command as input, executes it using the popen() function,
 * and returns the output as a dynamically allocated string. If there is an error
 * executing the command or reading the output, the function will print an error
 * message to stderr and return NULL.
 *
 * @param command The command to be executed.
 * @return The output of the command as a dynamically allocated string, or NULL
 *         if there was an error.
 */
static char* execute_command(const char* command) {
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
    buffer[strcspn(buffer, "\n")] = 0;
    return buffer;
}

/**
 * @brief Prints a formatted output with a label and a value.
 *
 * This function takes a label and a value as input, and prints them in a
 * formatted way using the ANSI escape codes defined earlier.
 *
 * @param label The label for the output.
 * @param value The value to be printed.
 */
static void print_output(const char* label, const char* value) {
    printf("%s%s%s%s %s%s%s\n", CYAN, BOLD, label, RESET, BOLD, value, RESET);
}

/**
 * @brief Retrieves the memory usage information.
 *
 * This function uses the `execute_command()` function to run the `free -m` command
 * and extract the total and used memory information. It then formats the output
 * as a string in the format "used MiB / total MiB (percentage)".
 *
 * @return The memory usage information as a dynamically allocated string.
 */
static char* get_memory_usage() {
    return execute_command("free -m | awk 'NR==2{printf \"%sMiB / %sMiB (%.2f%%)\", $3,$2,$3*100/$2 }'");
}

/**
 * @brief Retrieves the system information.
 *
 * This function collects various system information, such as the user, hostname,
 * shell, OS, kernel version, uptime, and product name and version. It returns
 * these values as a dynamically allocated array of strings.
 *
 * @return An array of dynamically allocated strings containing the system information.
 */
static char** get_system_info() {
    char** info = malloc(sizeof(char*) * 7);

    info[0] = USER;
    info[1] = HOSTNAME;
    info[2] = OS;
    info[3] = execute_command("uname -sr");
    info[4] = SHELL;
    info[5] = execute_command("uptime -p | sed 's/up //'");
    info[6] = execute_command("cat /sys/devices/virtual/dmi/id/product_name");

    return info;
}

/**
 * @brief Retrieves the number of installed packages.
 *
 * This function checks for the presence of various package managers (apt, dpkg, pacman, zypper, dnf)
 * by using the `system()` function to check if the corresponding command is available. It then
 * returns the appropriate command to get the number of installed packages for the detected package
 * manager. If no package manager is found, it returns the string "unknown".
 *
 * @return The number of installed packages as a dynamically allocated string.
 */
static char* get_package_count() {
    if (!system("command -v apt > /dev/null")) {
        return execute_command("apt-cache pkgnames | wc -l");
    } else if (!system("command -v dpkg > /dev/null")) {
        return execute_command("dpkg --list | wc -l");
    } else if (!system("command -v pacman > /dev/null")) {
        return execute_command("pacman -Qe | wc -l");
    } else if (!system("command -v zypper > /dev/null")) {
        return execute_command("zypper se -i | wc -l");
    } else if (!system("command -v dnf > /dev/null")) {
        return execute_command("dnf list installed | wc -l");
    } else {
        return "unknown";
    }
}

int main() {
    // Retrieve system information
    char** info = get_system_info();

    // Get the product name and version
    char host[256];
    sprintf(host, "%s %s", info[6], execute_command("cat /sys/devices/virtual/dmi/id/product_version"));

    // Get the number of installed packages
    char* pkgs = get_package_count();

    // Get the memory usage information
    char* memory = get_memory_usage();

    // Print the formatted output
    
    // Print the formatted output
    printf("%s%s%s%s%s@%s%s%s%s\n", "                    ", CYAN, BOLD, info[0], RESET, CYAN, BOLD, info[1], RESET);
    printf("%s%s%s%s%s\n", "          ", CYAN, BOLD, "-----------------------------", RESET);
    print_output(" OS: ~~~~~~~~~~>", info[2]);
    print_output(" KERNEL: ~~~~~~>", info[3]);
    print_output("󰆍 SHELL: ~~~~~~~>", info[4]);
    print_output("󰅐 UPTIME: ~~~~~~>", info[5]);
    print_output("󰌢 PRODUCT: ~~~~~>", host);
    print_output("󰏖 PACKAGES: ~~~~>", pkgs);
    print_output("󰍛 MEMORY: ~~~~~~>", memory);

    printf("\n");

    // Free dynamically allocated memory
    for (int i = 4; i <= 6; i++) {
        free(info[i]);
    }
    free(info);
    free(pkgs);
    free(memory);

    return 0;
}
