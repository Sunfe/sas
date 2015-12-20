/********************************************************
 * File Name:          
 * Description:       
 * Others:      
 * Author: lzc
 * Date: 2015-07-12
 **********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "sas_common.h"
#include "sas_derive.h"
#include "sas_import.h"
#include "sas_shell.h"

/***********************************************
  macro definition
 ***********************************************/

/***********************************************
  datatype definition
 ***********************************************/
typedef struct 
{
    char *name;                   /* User printable name of the function. */
    rl_icpfunc_t *func;           /* Function to call to do the job. */
    char *doc;                    /* Documentation for this function.  */
} COMMAND;


/***********************************************
  global variables
 ***********************************************/
COMMAND commands[] = 
{
    {"import",  sas_import,           "import <type>:1 index, 2 stock" },
    {"derive",  sas_derive,           "derive <type>:0 all" },
    {"quit",    sas_shell_quit,       "quit" },
    {"exit",    sas_shell_quit,       "quit" },
    {"help",    sas_com_help,         "Display this text" },
    {"?",       sas_com_help,         "Synonym for `help'" },
    { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};

/***********************************************
  static variables 
 ***********************************************/


/***********************************************
  extern variables
 ***********************************************/
extern char *xmalloc(size_t byte);
extern int execute_line (char *line );


/***********************************************
  extern functions 
 ***********************************************/


/***********************************************
  global function declarations
 ***********************************************/
void initialize_readline();
char *stripwhite ();
COMMAND *find_command ();


/***********************************************
  static function declarations
 ***********************************************/


/***********************************************
  global function definitions
 ***********************************************/
void shell_main (void )
{
    char *line, *s;
    initialize_readline (); 

    while(1)
    {
        line = readline ("sas>> ");

        if (!line)
            break;

        s = stripwhite (line);

        if (*s)
        {
            add_history (s);
            execute_line (s);
        }

        free (line);
    }

    exit (0);
}


char * dupstr (char* s)
{
    char *r;

    r = xmalloc (strlen (s) + 1);
    strcpy (r, s);
    return (r);
}


int execute_line ( char *line)
{
    register int i;
    COMMAND *command;
    char *word;

    /* Isolate the command word. */
    i = 0;
    while (line[i] && whitespace (line[i]))
        i++;
    word = line + i;

    while (line[i] && !whitespace (line[i]))
        i++;

    if (line[i])
        line[i++] = '\0';

    command = find_command (word);

    if (!command)
    {
        fprintf (stderr, "%s: No such command for SAS.\n", word);
        return (-1);
    }

    /* Get argument to command, if any. */
    while (whitespace (line[i]))
        i++;

    word = line + i;

    /* Call the function. */
    return ((*(command->func)) (word));
}

COMMAND * find_command ( char *name)
{
    register int i;

    for (i = 0; commands[i].name; i++)
        if (strcmp (name, commands[i].name) == 0)
            return (&commands[i]);

    return ((COMMAND *)NULL);
}

char * stripwhite ( char *string)
{
    register char *s, *t;

    for (s = string; whitespace (*s); s++)
        ;

    if (*s == 0)
        return (s);

    t = s + strlen (s) - 1;
    while (t > s && whitespace (*t))
        t--;
    *++t = '\0';

    return s;
}

/* **************************************************************** */
/*                                                                  */
/*                  Interface to Readline Completion                */
/*                                                                  */
/* **************************************************************** */

char *command_generator __P((const char *, int));
char **sas_shell_completion __P((const char *, int, int));

void initialize_readline ()
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = "SAS";

    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = sas_shell_completion;

    return ;
}

char ** sas_shell_completion ( const char *text, int start, int end)
{
    char **matches;

    matches = (char **)NULL;

    /* If this word is at the start of the line, then it is a command
       to complete.  Otherwise it is the name of a file in the current
       directory. */
    if (start == 0)
        matches = rl_completion_matches (text, command_generator);

    return (matches);
}

/* Generator function for command completion.  STATE lets us
   know whether to start from scratch; without any state
   (i.e. STATE == 0), then we start at the top of the list. */
char *  command_generator(const char *text, int state)    
{
    static int list_index, len;
    char *name;

    /* If this is a new word to complete, initialize now.  This
       includes saving the length of TEXT for efficiency, and
       initializing the index variable to 0. */
    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    /* Return the next name which partially matches from the
       command list. */
    while ((name = commands[list_index].name))
    {
        list_index++;

        if (strncmp (name, text, len) == 0)
            return (dupstr(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}


int sas_com_help ( char *arg)
{
    register int i;
    int printed = 0;

    for (i = 0; commands[i].name; i++)
    {
        if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
            printf ("%s\t\t%s.\n", commands[i].name, commands[i].doc);
            printed++;
        }
    }

    if (!printed)
    {
        printf ("No commands match `%s'.  Possibilties are:\n", arg);

        for (i = 0; commands[i].name; i++)
        {
            /* Print in six columns. */
            if (printed == 6)
            {
                printed = 0;
                printf ("\n");
            }

            printf ("%s\t", commands[i].name);
            printed++;
        }

        if (printed)
            printf ("\n");
    }

    return (0);
}


int sas_shell_quit ( char *arg)
{
    abort();
    return 0;
}


int valid_argument ( char *caller, char *arg)
{
    if (!arg || !*arg)
    {
        fprintf (stderr, "%s: Argument required.\n", caller);
        return (0);
    }

    return (1);
}


int scanf_hex(char *str, int *idx, unsigned int *num)
{
    int size;

    if (sscanf(str + *idx, "%x%n", num, &size) == 1)
    {
        *idx += size;
        return 0;
    }
    else
        return 1;
}

int scanf_int(char *str, int *idx, unsigned int *num)
{
    int size;

    if (sscanf(str + *idx, "%u%n", num, &size) == 1)
    {
        *idx += size;
        /* check for hex format starting with 0x */
        if (*num == 0 && str[*idx] == 'x') {
            (*idx)++;
            return scanf_hex(str, idx, num);
        }
        return true;
    }
    else
        return false;
}



