// an arbitrary assumption on the max length of a dir's path
// this system will probably change later
#define MAX_DIR_LEN 1024

// cwd buffer, given an arbitrary but generous size - good enough for now
extern char cwdenv[MAX_DIR_LEN + 4];
extern char* cwd;

// buffers and pointers for the shell environment variable
extern char shellenv[MAX_DIR_LEN + 6];
extern char* shell; // points to the string contains msh's path

extern char parentenv[MAX_DIR_LEN + 7]; // we are also responsible for ensuring child processes know their parent