HOW TO RUN THE CODE
$ gcc SimpleMemModAnalyzer.c -o SimpleMemModAnalyzer
$ ./SimpleMemModAnalyzer myProg.c

Program analyzes the c code stored at myProg.c 

Possible Error Messages:
if no argument is passed -- "Error: No File Passed as Argument" printed at Standard Error (stderr)
if file does not exist in same directory as code -- "Error: Specified File not Found" printed at Standard Error (stderr)

Note that the analyzer relies on the passed c file being an actual functional c file with types restricted to the following
        int, float, char
        int *, float *, char *
        int [], float [], char[]
	(exception to char** argv)

If there is a row with multiple variable declarations, then every variable in that row has to be the same type

only // comments are allowed in the code



SAMPLE OUTPUT (OUTPUT TO STANDARD OUTPUT (STDOUT))

<<<prog1.c>>> (file starts at line below)
// evil global variables
int evil_glob_var_1;
float evil_glob_var_2;

void fun1(int x)
{
  int y;
  int z;
  printf("%d \n", x+y+z); 
}

int fun2(float z)
{
   float x;
   return (int)(z+x);
}

int main(int argc, char** argv)
{
  int w;

  fun1(w);
  fun2();

  return 0;
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   prog1.c

### ROData ###       scope  type  size

### static data ###
   evil_glob_var_1   global   int   4
   evil_glob_var_2   global   float   4

### heap ###

####################
### unused space ###
####################

### stack ###
   x   fun1   int   4
   y   fun1   int   4
   z   fun1   int   4
   z   fun2   float   4
   x   fun2   float   4
   argc   main   int   4
   argv   main   char**   8
   w   main   int   4

**** STATS ****
  - Total number of lines in the file: 26
  - Total number of functions: 3
    fun1, fun2, main
  - Total number of lines per functions:
    fun1: 3
    fun2: 2
    main: 6
  - Total number of variables per functions:
    fun1: 3
    fun2: 2
    main: 3
//////////////////////////////

<<<prog2.c>>> (file starts at line below)
void f1(int *i)
{
 i = malloc(sizeof(int));
}


int f2()
{
  
  float x[5];
}

int main(int argc, char **argv)
{
  int i;
  int j;

  i = f2();
  f1(j);
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   prog2.c

### ROData ###       scope  type  size

### static data ###
   evil_glob_var_1   global   int   4
   evil_glob_var_2   global   float   4

### heap ###

####################
### unused space ###
####################

### stack ###
   x   fun1   int   4
   y   fun1   int   4
   z   fun1   int   4
   z   fun2   float   4
   x   fun2   float   4
   argc   main   int   4
   argv   main   char**   8
   w   main   int   4

**** STATS ****
  - Total number of lines in the file: 26
  - Total number of functions: 3
    fun1, fun2, main
  - Total number of lines per functions:
    fun1: 3
    fun2: 2
    main: 6
  - Total number of variables per functions:
    fun1: 3
    fun2: 2
    main: 3
//////////////////////////////

<<<prog3.c>>> (file starts at line below)
int x = 10;
int y;
 
int f(int p, int q)
{
    int j = 5;

    x = 5;
    return p * q + j;
}
 
int main()
{
   int i = x;
 
   y = f(i, i);
   return 0;
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   prog3.c

### ROData ###       scope  type  size

### static data ###
   x   global   int   4
   y   global   int   4

### heap ###

####################
### unused space ###
####################

### stack ###
   p   f   int   4
   q   f   int   4
   j   f   int   4
   i   main   int   4

**** STATS ****
  - Total number of lines in the file: 18
  - Total number of functions: 2
    f, main
  - Total number of lines per functions:
    f: 4
    main: 4
  - Total number of variables per functions:
    f: 3
    main: 1
//////////////////////////////

<<<prog4.c>>> (file starts at line below)
int x = 10;
int y;
 
void f(int *p, int q)
{
    *p = 5;
}
 
int main()
{
    int i = x;

    f(&i, i);
    return 0;
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   prog4.c

### ROData ###       scope  type  size

### static data ###
   x   global   int   4
   y   global   int   4

### heap ###

####################
### unused space ###
####################

### stack ###
   p   f   int*   8
   q   f   int   4
   i   main   int   4

**** STATS ****
  - Total number of lines in the file: 15
  - Total number of functions: 2
    f, main
  - Total number of lines per functions:
    f: 1
    main: 4
  - Total number of variables per functions:
    f: 2
    main: 1
//////////////////////////////

<<<string_tools.c>>> (file starts at line below)
char *concat_wrong(char *s1, char *s2) 
{
  char result[70];

  strcpy (result, s1);
  strcat (result, s2);

  return result;
}


char *concat(char *s1, char *s2)
{
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1);
  if (result == NULL) {
      printf ("Error: malloc failed\n");
      exit(1);
  }

  strcpy (result, s1);
  strcat (result, s2);

  return result;
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   string_tools.c

### ROData ###       scope  type  size

### static data ###

### heap ###
   *result   concat   char   strlen(s1)+strlen(s2)+1

####################
### unused space ###
####################

### stack ###
   s1   concat_wrong   char*   8
   s2   concat_wrong   char*   8
   result   concat_wrong   char[]   70*sizeof(char)
   s1   concat   char*   8
   s2   concat   char*   8
   result   concat   char*   8

**** STATS ****
  - Total number of lines in the file: 26
  - Total number of functions: 2
    concat_wrong, concat
  - Total number of lines per functions:
    concat_wrong: 6
    concat: 12
  - Total number of variables per functions:
    concat_wrong: 3
    concat: 3
//////////////////////////////

<<<sumArray.c>>> (file starts at line below)
int sum(int *a, int size)
{
   int i, s = 0;

   for(i = 0; i < size; i++) 
       s += a[i];

   return s;
}


int main()
{
  int N = 5;
  int i[N] = {10, 9, 8, 7, 6};

  printf("sum is %d\n", sum(i,N));

  return 0;
}

Output from running "SimpleMemModAnalyzer":
>>> Memory Model Layout <<<

***  exec // text ***
   sumArray.c

### ROData ###       scope  type  size

### static data ###

### heap ###

####################
### unused space ###
####################

### stack ###
   a   sum   int*   8
   size   sum   int   4
   i   sum   int   4
   s   sum   int   4
   N   main   int   4
   i   main   int[]   N*sizeof(int)

**** STATS ****
  - Total number of lines in the file: 20
  - Total number of functions: 2
    sum, main
  - Total number of lines per functions:
    sum: 6
    main: 6
  - Total number of variables per functions:
    sum: 4
    main: 2
//////////////////////////////


LIST OF STRUCTS AND FUNCTIONS

// define node that contains token and corresponding line_num of token, to be implemented as a linked list of tokens returned in parse()
typedef struct token_node

// define node that contains information of 'variable', to be implemented as a linked list of variables returned in filter_tokens()
typedef struct var_node

// define node that contains function name and number of lines, to be implemented as a linked list of functions returned in filter_tokens()
typedef struct func_node

// initialize a func node with name f_name and number of lines line, returns pointer to new func_node
func_node* new_func(char* f_name, int line)

// appends func node new to the func node linked list head at head, returns the pointer to the head of the linked lists of function
func_node* append_func(func_node* head, func_node* new)

// initialize var node, returns pointer to new func_node, to be called in function add_var
var_node* new_var()

// appends var node new to variable linked list with head at head, returns pointer to head of variable linked list, to becalled in function add_var
var_node* append_var(var_node* head, var_node* new)

// adds a var node with information(var, type, scope, memory, size) passed as argument to the linked list of variable with head at head
// calls helper function new_var and append_var
var_node* add_var(var_node* head, char* var, char* type, char* scope, char* memory, char* size)

// prints content of the variable linked list, used in debugging process of the code
void print_var_ll(var_node* head)

// initialize a func node with name f_name and number of lines line, returns pointer to new func_node
token_node* new_token(char *token_s, int line_num)

// appends token node new to the token node linked list with head at head, returns the pointer to the head of the linked lists of tokens
token_node* append_token(token_node* head, token_node* new)

// prints content of the token linked list, used in debugging process of the code
void print_token_ll(token_node* head)

// check if string passed curr is a variable type (i.e. char, float, int, void), returns 0 if true and 1 if false (same like strcmp)
int is_type(char* curr)

// given a string type, returns an int of the corresponding size of type
int type_to_size(char *type)

// given a variable of var_name, search through var_ll to extract the corresponding type which is assigned to temp_type
void search_type(var_node* var_ll, char* temp_type, char* var_name)

// filters tokens in the tokens linked list passed as argument into variable nodes in variable linked list(returned) 
// and function nodes in function linked list (head of function linked list is assigned to *f_head)
// this is needed to bypass the restriction of only returning one value
var_node* filter_tokens(token_node* head, func_node** f_head)

// given a file open for reading fp, parse (tokenize) through the file to extract tokens and appends into a token linked list whose
// head is returned by the function. Function also modifies integer pointed by line_len by assigning the number of lines in file fp  
token_node* parse(FILE* fp, int *line_len)

// prints the RO Data section of the output
void print_rodata(var_node* var_ll)

// prints the static section of the output
void print_static(var_node* var_ll)

// prints the heap section of the output
void print_heap(var_node* var_ll)

// prints the 'unused' section of the output
void print_unused()

// prints the stack section of the output
void print_stack(var_node* var_ll)

// prints the number of lines in file section of the output, under stats
void print_stats_line(int line_len)

// returns the number of functions in the function linked lists passed as func_ll
int count_funcs(func_node* func_ll)

// prints the number of function section of the output, under stats
void print_stats_num_func(func_node* func_ll)

// prints the number of lines per function section of the output, under stats
void print_stats_line_func(func_node* func_ll)

// prints the number of variables per function section of the output, under stats
void print_stats_var_func(func_node* func_ll, var_node* var_ll)

// free the nodes of function linked list, returns NULL to prevent dangling pointer pointing at the freed head node
func_node* free_funcll (func_node *head)

// free the nodes of variable linked list, returns NULL to prevent dangling pointer pointing at the freed head node
var_node* free_varll (var_node *head)

// free the nodes of token linked list, returns NULL to prevent dangling pointer pointing at the freed head node
token_node* free_tokenll (token_node *head)



HOW DID I SOLVE THE PROBLEM
- open the file passed as argument and read content of the file line by line
- in each line, split the contents into tokens based on delimiter characters '{}()[];,=* and empty space (parsing step)
(example: char *c; will be split to the following tokens {char, *, c, ;} with each element being stored as a string)
- exception to strings in between " " then the entire string including " " will be one token (example: s = "something" then the tokens are {s, = , "something"})
- iterate through the linked list of tokens and if the token is a type (either void, char, int, float) perform the following
	- go to next token which may be *, if * then append to the previous token to form the type
	- go to next set of tokens which may be [N], if so then append to the type
  - if it is a close bracket, then it is simple type casting so we ignore and repeat iteration on next token
	- if it is an open bracket "(", then we encountered a function, update the temporary scope to the function name and add the function to the function linked list
	- else if not open bracket, then we are working with a variable
		- variable cases are first divided into 2 (inside a parameter or not a parameter), to see if it is a parameter, our scope is no longer global but we have not encountered any {

      - if parameter, analyze first variable that is seperated by commas, then loop back to the top of the iteration after adding the variable's relevant information to a variable node 
        then append the node to variable linked list

      - if not parameter, analyze each variable that is seperated by commas because they will be of the same type
        add each variable's information to their respective variable node, and append each node to the variable linked list

        note if we meet a malloc or calloc then add another node to the variable linked list represting data stored in heap
        note if we meet a string literal then add another node to the variable linked list represting data stored in ROData
- throughout iteration, we may encounter malloc calloc or literals not in the same line as a variable declaration
- for literal, check if the token predescessor is "=", if so then it is a literal and add another node to the variable linked list represting data stored in ROData
- for calloc malloc, then add another node to the variable linked list represting data stored in heap
- when outputting the information about memory, we iterate through our variable linked list, filtering and outputting relevant information to standard output
- for outputting stats, we refer to the information stored in both function linked list and variable linked list
- in the end, free all the nodes in each 3 linked list to prevent memory leak