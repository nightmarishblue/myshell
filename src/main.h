// an arbitrary assumption on the max length of a dir's path
// this system will probably change later
#define MAX_DIR_LEN 1024

// cwd buffer, given an arbitrary but generous size - good enough for now
extern char cwdenv[MAX_DIR_LEN + 4];
extern char* cwd;

// buffers and pointers for the shell environment variable
extern char shellenv[MAX_DIR_LEN + 6];
extern char* shell; // points to the string contains msh's path

#define MAN_NAME "readme"
#define DEFAULT_PAGER "more"

// msh doesn't have much in the way of options - until now
extern int longpath; // tracks whether or not the shell should display the full path in its prompt