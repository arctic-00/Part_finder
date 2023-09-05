#include "Arduino_Communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <spawn.h>
#include <sys/wait.h>

#define DATABASE_FILENAME   "Part_Finder.csv"
#define MAX_PATH_LEN        255
#define MAX_COMP_LEN        50
#define MAX_LINE_LEN        1000
#define MAX_BLOCK_LEN       120     // block is components in a single container, i.e. "component1       | componet2,"

extern char **environ;

///////////////////// Function Prototypes /////////////////////

FILE *get_database_fp(char *path);

char *get_database_path(char *argv[1]);

// Load database into array of strings
void load_components(FILE *fp, int num_rows, int num_col, 
                     char components[num_rows][num_col][2][MAX_COMP_LEN+1]);

// Find coordinates of component (i.e. which container it is in)
// and send them
pos find_pos(char *name, int fd, int num_rows, int num_col,
             char components[num_rows][num_col][2][MAX_COMP_LEN+1]);

// Gets name from stdin and replaces any spaces with '_'
void get_name(char *name);

// Opens database in given program (e.g. VS Codium)
void edit_database(char *database_path);

void print_database(int num_rows, int num_col, 
                    char components[num_rows][num_col][2][MAX_COMP_LEN+1]);

// Print components in alphabetical order
void print_sorted(int n, char components[n][MAX_COMP_LEN+1]);

// Defining comparator function for print_sorted
static int myCompare(const void* a, const void* b);

///////////////////////////////////////////////////////////////



int main(int argc, char *argv[]) {


    char *database_path = get_database_path(argv);
    FILE *fp = get_database_fp(database_path);

    char components[C_1_NUM_ROWS][C_1_NUM_COLS][2][MAX_COMP_LEN+1];
    load_components(fp, C_1_NUM_ROWS, C_1_NUM_COLS, components);
    fclose(fp);

    int fd = port_setup();

    printf("Type \"print\", \"print -a\", \"edit\"\n");
    printf("Or   \"p\",     \"pa\",       \"e\"    for database info.\n");
    printf("Type components (As a plural) to find:\n");

    while (1) {
        char name[MAX_COMP_LEN + 1];
        get_name(name);

        // Skip if string length == 0
        if (name[0] == 0)
            continue;

        if (strcmp(name, "edit") == 0 ||
            strcmp(name, "e")    == 0) {
            edit_database(database_path);
        } else if (strcmp(name, "print_-a") == 0 || 
                   strcmp(name, "pa") == 0) {
            print_sorted(C_1_NUM_ROWS * C_1_NUM_COLS * 2, &components[0][0][0]);
        } else if (strcmp(name, "print") == 0 ||
                   strcmp(name, "p") == 0) {
            print_database(C_1_NUM_ROWS, C_1_NUM_COLS, components);
        } else {
            find_pos(name, fd, C_1_NUM_ROWS, C_1_NUM_COLS, components);
        }
    }

    free(database_path);
}

FILE *get_database_fp(char *database_path) {
    FILE *fp = fopen(database_path, "r");
    if (fp == NULL) {
        printf("Path used: \"%s\"\n", database_path);
        perror("Error opening database file");
        sleep(TIME_BEFORE_EXIT);
        exit(EXIT_FAILURE);
    }

    return fp;
}

char *get_database_path(char *argv[1]) {
    char path[MAX_PATH_LEN];

    // Get the path to the command that was run
    // i.e. Get what was typed into the terminal with the file's name removed
    strcpy(path, argv[0]);
    int i;
    for (i = strlen(path) - 1; i >= 0; i--) {
        if (path[i] == '/') {
            path[i] = 0;
            break;
        }
    }

    // If the path given was just a command on PATH (No '/') check for a local .csv
    char *database_path;
    database_path = malloc((MAX_PATH_LEN + strlen(DATABASE_FILENAME) + 2) * sizeof(char ));
    if (i == -1) {
        char *cwd = malloc(strlen(DATABASE_FILENAME) + 1);
        getcwd(cwd, (strlen(DATABASE_FILENAME) + 1));
        sprintf(database_path, "%s/%s", cwd, DATABASE_FILENAME);
    } else {
        sprintf(database_path, "%s/%s", path, DATABASE_FILENAME);
    }

    return database_path;
}

// Load database into array of strings
void load_components(FILE *fp, int num_rows, int num_col, 
                     char components[num_rows][num_col][2][MAX_COMP_LEN+1]) {
    
    // Initialise array
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_col; j++) {
            components[i][j][0][0] = 0;
            components[i][j][1][0] = 0;
        }
    }

    // Load components
    for (int i = 0; i < num_rows; i++) {
        char line[MAX_LINE_LEN+1];
        fgets(line, MAX_LINE_LEN, fp);

        // Get components in the first container of the row
        char *block_pointer = strtok(line, " |,");
        strcpy(components[i][0][0], block_pointer);
        block_pointer = strtok(NULL, " |,");
        strcpy(components[i][0][1], block_pointer);
        
        // Get components in remaining containers in the row
        for (int j = 1; j < num_col; j++) {
            char *block_pointer = strtok(NULL, " |,");
            strcpy(components[i][j][0], block_pointer);
            block_pointer = strtok(NULL, " |,");
            strcpy(components[i][j][1], block_pointer);
        }
    }
}

// Find coordinates of component (i.e. which container it is in)
// and send them
pos find_pos(char *name, int fd, int num_rows, int num_col,
             char components[num_rows][num_col][2][MAX_COMP_LEN+1]) {
    pos component_pos;
    component_pos.row = -1;
    component_pos.col = -1;
   
    
    int i, j;
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_col; j++) {
            if (strcmp(components[i][j][0], name) == 0 ||
                strcmp(components[i][j][1], name) == 0) {

                component_pos.row = i + 1;
                component_pos.col = j + 1;

                write_pos(fd, CONTAINER_1_IDENTIFIER, component_pos);
            }
        }
    }

    // Add 's' to end of name and try again to check plural if it isn't already
    int str_len = strlen(name);
    if (name[str_len - 1] != 's' && str_len < MAX_COMP_LEN) {
        name[str_len++] = 's';
        name[str_len] = 0;
        component_pos = find_pos(name, fd, num_rows, num_col, components);
    }

    if (component_pos.row == -1)
        printf("Not found\n");

    return component_pos;
}

// Gets name from stdin and replaces any spaces with '_'
void get_name(char *name) {
    fgets(name, MAX_COMP_LEN, stdin);
    name[strlen(name) - 1] = 0;

    // Replaces any spaces in name with '_' before searching through database
    char *name_space_pos = strchr(name, ' ');
    if (name_space_pos != NULL)
        *name_space_pos = '_';

    // Make string lower case
    for (char *p = name; *p; ++p) *p = tolower(*p);
}

// Opens database in given program (e.g. VS Codium)
void edit_database(char *database_path) {
    pid_t pid;
    char *argv[] = {"codium", "-n", "-g", database_path, NULL};
    int status;
    status = posix_spawn(&pid, "/snap/bin/codium", NULL, NULL, argv, environ);
    if (status == 0) {
        do {
          if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
          }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        printf("posix_spawn: %s\n", strerror(status));
    }
}


void print_database(int num_rows, int num_col, 
                    char components[num_rows][num_col][2][MAX_COMP_LEN+1]) {
    
    for (int i = 0; i < 162; i++) printf("#");
    printf("\n");

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_col; j++) {
            printf("%-26s ", components[i][j][0]);
        }

        printf("\n");

        for (int j = 0; j < num_col; j++) {
            printf("%-26s ", components[i][j][1]);
        }

        printf("\n\n");

    }

    for (int i = 0; i < 162; i++) printf("#");
    printf("\n");
}

// Print components in alphabetical order
void print_sorted(int n, char components[n][MAX_COMP_LEN+1]) {
    // Print line
    for (int i = 0; i < 50; i++) printf("#");
    printf("\n");

    // Copy across component array to a douple pointer so that it can be rearranged and sorted
    char **arr_new = malloc(n * sizeof(char **));
    for (int i = 0; i < n; i++) {
        arr_new[i] = malloc((MAX_COMP_LEN + 1) * sizeof(char));
        strcpy(arr_new[i], components[i]);
    }

    qsort(arr_new, n, sizeof(char *), myCompare);

    // Format 1 (Split into two lines)
    // for (int i = 0; i < n; i += 2) {
    //     printf("%-40s %s\n", arr_new[i/2], arr_new[(n+i)/2]);
    // }

    // Format 2 (No duplicates, single line)
    printf("A:");
    printf("\t%s\n", arr_new[0]);
    for (int i = 1; i < n; i++) {

        if (strcmp(arr_new[i], arr_new[i - 1]) != 0) {
            if (arr_new[i][0] != arr_new[i-1][0]) {
                printf("%c:", (arr_new[i][0] & ~32 ));
            }

            printf("\t%s\n", arr_new[i]);
        }
    }

    for (int i = 0; i < 50; i++) printf("#");
    printf("\n");

    // free
    for (int i = 0; i < n; i++) {
        free(arr_new[i]);
    }
    free(arr_new);


}

// Defining comparator function as per the requirement
static int myCompare(const void* a, const void* b) {
 
    // setting up rules for comparison
    return strcmp(*(char**)a, *(char**)b);
}