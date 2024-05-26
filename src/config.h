#define USER getenv("USER")
#define HOSTNAME execute_command("cat /etc/hostname")
#define SHELL execute_command("basename $SHELL")
#define OS execute_command("hostnamectl | grep 'Operating System' | awk '{print $3,$4}'")
#define SHOW_MEMORY
