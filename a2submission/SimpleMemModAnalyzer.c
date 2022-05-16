#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DELIMS " \'{}()[];,=*" // to be used in parse()

// define node that contains token and corresponding line_num of token, to be implemented as a linked list of tokens returned in parse()
typedef struct token_node{

    char token[128];

    int line_num;

    struct token_node* next;
    struct token_node* prev;
}token_node;

// define node that contains information of 'variable', to be implemented as a linked list of variables returned in filter_tokens()
typedef struct var_node{

    char var[128];
    char type[128];
    char scope[128];
    char memory[128];
    char size[128];

    struct var_node* next;
}var_node;

// define node that contains function name and number of lines, to be implemented as a linked list of functions returned in filter_tokens()
typedef struct func_node{

    char name[128];
    int line_num;

    struct func_node* next;
}func_node;

// initialize a func node with name f_name and number of lines line, returns pointer to new func_node
func_node* new_func(char* f_name, int line){
    func_node* func_n = (func_node*)malloc(sizeof(func_node));

    strcpy(func_n->name, f_name);

    func_n -> line_num = line;
    func_n -> next = NULL;

    return func_n;
}

// appends func node new to the func node linked list head at head, returns the pointer to the head of the linked lists of function
func_node* append_func(func_node* head, func_node* new){

    if(head == NULL){
        return new;
    }

    func_node* curr = head;

    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = new;

    return head;
}

// prints content of the function linked list, used in debugging process of the code
void print_func_ll(func_node* head){
    
    printf("LIST OF FUNCTIONS\n\n");
    while(head != NULL){
        printf("Function Name: %s -- Number of lines: %d\n", head->name, head->line_num);
        head = head->next;
    }
    printf("================================\n");
}

// initialize var node, returns pointer to new func_node, to be called in function add_var
var_node* new_var(){

    var_node* var_n = (var_node*)malloc(sizeof(var_node));

    var_n->var[0] = '\0';
    var_n->type[0] = '\0';
    var_n->scope[0] = '\0';
    var_n->memory[0] = '\0';
    var_n->size[0] = '\0';

    var_n->next = NULL;

    return var_n;
}

// appends var node new to variable linked list with head at head, returns pointer to head of variable linked list, to becalled in function add_var
var_node* append_var(var_node* head, var_node* new){

    if(head == NULL){
        return new;
    }

    var_node* curr = head;

    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = new;

    return head;
}

// adds a var node with information(var, type, scope, memory, size) passed as argument to the linked list of variable with head at head
// calls helper function new_var and append_var
var_node* add_var(var_node* head, char* var, char* type, char* scope, char* memory, char* size){
    var_node* new = new_var();
    strcpy(new->var, var);
    strcpy(new->type, type);
    strcpy(new->scope, scope);
    strcpy(new->memory, memory);
    strcpy(new->size, size);  

    head = append_var(head, new);

    return head;   
}

// prints content of the variable linked list, used in debugging process of the code
void print_var_ll(var_node* head){

    printf("VARIABLES FOUND\n\n");
    while(head != NULL){
        printf("Variable Name: %s\n", head->var);
        printf("Type: %s\n", head->type);
        printf("Scope: %s\n", head->scope);
        printf("Memory: %s\n", head->memory);
        printf("Size: %s\n\n", head->size);
        head = head->next;
    }
    printf("================================\n");
}

// initialize a func node with name f_name and number of lines line, returns pointer to new func_node
token_node* new_token(char *token_s, int line_num){
    token_node* token_n = (token_node*)malloc(sizeof(token_node));
    strcpy(token_n->token,token_s);
    token_n->line_num = line_num;
    token_n->next = NULL;
    token_n->prev = NULL;

    return token_n;
}

// appends token node new to the token node linked list with head at head, returns the pointer to the head of the linked lists of tokens
token_node* append_token(token_node* head, token_node* new){

    if(head == NULL){
        return new;
    }

    token_node* curr = head;

    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = new;
    new->prev = curr;

    return head;
}

// prints content of the token linked list, used in debugging process of the code
void print_token_ll(token_node* head){
    while(head != NULL){
        printf("%s, line_num: %d\n", head->token, head->line_num);
        head = head->next;
    }
}

// check if string passed curr is a variable type (i.e. char, float, int, void), returns 0 if true and 1 if false (same like strcmp)
int is_type(char* curr){

    char* types[4] = {"int", "float", "char", "void"};

    for(int i=0; i<4; i++){

        if(strcmp(types[i], curr)==0){
            return 0; //true
        }
    }

    return 1; //false
}

// given a string type, returns an int of the corresponding size of type
int type_to_size(char *type){

    if(strstr(type,"int*")!=NULL){
        return sizeof(int*);
    }

    else if(strstr(type,"float*")!=NULL){
        return sizeof(float*);
    }

    else if(strstr(type,"char*")!=NULL){
        return sizeof(char*);
    }

    if(strcmp(type,"int")==0){
        return sizeof(int);
    }

    else if(strcmp(type,"float")==0){
        return sizeof(float);
    }

    else if(strcmp(type,"char")==0){
        return sizeof(char);
    }
    else{
        fprintf(stderr, "Error: Variable found with Invalid Type (size -1)\n");
        return -1;
    }


}

// given a variable of var_name, search through var_ll to extract the corresponding type which is assigned to temp_type
void search_type(var_node* var_ll, char* temp_type, char* var_name){
    while(var_ll != NULL){
        if(strcmp(var_ll->var, var_name)==0){
            strcpy(temp_type, var_ll->type);
        }
        var_ll = var_ll->next;
    }
    return;
}

// filters tokens in the tokens linked list passed as argument into variable nodes in variable linked list(returned) 
// and function nodes in function linked list (head of function linked list is assigned to *f_head)
// this is needed to bypass the restriction of only returning one value
var_node* filter_tokens(token_node* head, func_node** f_head){

    char temp_var[128];
    temp_var[0] = '\0';

    char temp_scope[128];
    strcpy(temp_scope, "global");

    char temp_type[128];
    temp_type[0] = '\0';

    char temp_holder[128];
    temp_holder[0] = '\0';

    char temp_holder2[128];
    temp_holder2[0] = '\0';

    char temp_size[128];
    temp_size[0] = '\0';

    //initialize temporary string holders

    int start_line, line_num;

    int c_counter = 0; // to keep track of { } to indicate if we are in a function or not

    var_node* var_ll = NULL;

    token_node* curr = head;

    token_node* traversal_helper; //to help use traverse to previous token

    while(curr != NULL){

        if(strchr(curr->token,'\"')!=NULL){
            //possible string literal assignment to existing variable
            traversal_helper = curr->prev;
            if(strcmp(traversal_helper->token, "=")==0){
                //then it is a string literal
                traversal_helper = traversal_helper->prev;
                //we are at the variable name
                search_type(var_ll, temp_type, traversal_helper->token);

                strcpy(temp_var, "*");
                strcat(temp_var, traversal_helper->token);

                sprintf(temp_size, "%d", (int)strlen(curr->token)-1);
                strcat(temp_size,"*sizeof(char)");

                // append to variable linked list for ROdata
                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "rodata", temp_size);


            }
            curr = curr ->next;

        }

        else if(strcmp(curr->token, "malloc")==0){
            // malloc not in initialization line case
            traversal_helper = curr->prev;
            while(strcmp(traversal_helper->token, "=")!=0){
                traversal_helper = traversal_helper->prev;
            }
            traversal_helper = traversal_helper->prev;
            // get the name of variable we are assigning malloc to

            search_type(var_ll, temp_type, traversal_helper->token);
            temp_type[strlen(temp_type)-1] = '\0';
            // get the type of the variable

            strcpy(temp_var, "*");
            strcat(temp_var, traversal_helper->token);

            curr = curr -> next; //we are in (
            curr = curr -> next; // we are inside the malloc argument
            temp_size[0] = '\0';
            strcpy(temp_size, curr->token);
            curr = curr -> next; //either ) or more tokens in argument
            while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                strcat(temp_size, curr->token);
                curr = curr->next;
            }

            // append to variable linked list for heap
            var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "heap", temp_size);
            curr = curr->next;
        }

        else if(strcmp(curr->token, "calloc")==0){
            // calloc not in initialization line case

            traversal_helper = curr->prev;
            while(strcmp(traversal_helper->token, "=")!=0){
                traversal_helper = traversal_helper->prev;
            }
            traversal_helper = traversal_helper->prev;
            // get the name of variable we are assigning calloc to

            search_type(var_ll, temp_type, traversal_helper->token);
            temp_type[strlen(temp_type)-1] = '\0';
            // get the type of the variable

            strcpy(temp_var, "*");
            strcat(temp_var, traversal_helper->token);

            curr = curr -> next; //we are in (
            curr = curr -> next; // we are inside the calloc argument
            temp_size[0] = '\0';
            strcpy(temp_size, curr->token);
            curr = curr -> next; // first tokens in first argument
            while(strcmp(curr->token, ",")!=0){
                strcat(temp_size, curr->token);
                curr = curr->next;
            }
            strcat(temp_size,"*");
            curr = curr->next;
            // tokens in second argument
            while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                strcat(temp_size, curr->token);
                curr = curr->next;
            }

            // append to variable linked list for heap
            var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "heap", temp_size);
            curr = curr->next;

        }

        else if(is_type(curr->token)==1){
            // token is not a type
            
            if(strcmp(curr->token, "{")==0){
                c_counter += 1;

                if(strcmp(temp_scope, "global")!=0 && c_counter == 1){
                    // beginning of a function contents
                    start_line = curr->line_num;
                }


            }
            else if(strcmp(curr->token, "}")==0){
                c_counter -= 1;

                if(c_counter == 0 && strcmp(temp_scope, "global")!=0){
                    // end of a function
                    line_num = curr->line_num - start_line - 1;
                    // line of end - line of beginning - 1

                    func_node* new_fn = new_func(temp_scope, line_num);
                    *f_head = append_func(*f_head,new_fn);
                    // add current function information to the function linked list
                    
                    strcpy(temp_scope, "global");
                    // set temp scope back to global
                }
                
            }

            curr = curr->next;
        }

        else{
            strcpy(temp_type, curr->token);

            curr = curr->next;

            strcpy(temp_holder2, temp_type);

            while(strcmp(curr->token,"*")==0){
                //append all the * to the type because our parse splits * into seperate tokens
                strcat(temp_type, curr->token);
                curr = curr->next;
            }

            if(strcmp(curr->token, ")")==0){
                //this is a type cast thus we ignore
                temp_type[0] = '\0';
                temp_var[0] = '\0';
                curr = curr->next;
            }
            else{
                
                strcpy(temp_var, curr->token);
                // store the name of the variable or function

                curr = curr -> next;

                if(strcmp(curr->token, "[")==0){
                    // append tokens of [N] to the type because our parse splits [N] into seperate tokens
                    strcpy(temp_holder, temp_type);
                    strcat(temp_type, curr->token);
                    curr = curr->next;
                    while(strcmp(curr->token,"]")!=0){
                        strcat(temp_size,curr->token);
                        curr = curr->next;
                    }
                    strcat(temp_type, curr->token);

                    curr = curr->next;
                }

                if(strcmp(curr->token, "(")==0){
                    // we have a function, set the temp scope to the name of the function
                    strcpy(temp_scope,temp_var);
                }
                else{

                    if(strcmp(temp_scope,"global")!=0 && c_counter == 0){
                        //We are in a function parameter, cannot sweep until semicolon because may have different types in 1 line
                        if(strchr(temp_type,'[')!=NULL){

                            strcpy(temp_type, temp_holder);
                            strcat(temp_type, "*");
                            //because arrays are passed as pointers in arguments
                        }
                        sprintf(temp_size, "%d", type_to_size(temp_type));
                        //get the size of the corresponding temp_type
                        
                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);

                    }

                    else{
                        //We are not in a function parameter (we can sweep all tokens until next semicolon because they all have same type)
                        if(strchr(temp_type,'[')!=NULL){
                            //Dealing with an array variable

                            if(temp_size[0]=='\0'){

                                // We have to count number of elements in the array as it is not specified
                                curr = curr->next; //we are now at "{" or " "text" "
                                int comma_counter = 0;

                                if(strchr(curr->token, '\"')!=NULL){
                                    sprintf(temp_size, "%d", (int)strlen(curr->token)-1);
                                    // length of array depends on length of string including null terminator
                                }

                                else{
                                    while(strcmp(curr->token,"}")!=0){
                                        if(strcmp(curr->token, ",")==0){
                                            comma_counter++;
                                        }
                                        curr = curr->next;
                                    }

                                    sprintf(temp_size, "%d", comma_counter+1);
                                    // length of array depends on number of elements
                                }
                                strcat(temp_size,"*sizeof(");
                                strcat(temp_size,temp_holder);
                                strcat(temp_size,")");

                                // append to variable linked list for static/stack
                                if(strcmp(temp_scope,"global") == 0){
                                    var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                }
                                else{
                                    var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                }

                                curr = curr -> next;

                            }
                            else{
                                // We already have our array size defined
                                strcat(temp_size,"*sizeof(");
                                strcat(temp_size,temp_holder);
                                strcat(temp_size,")");

                                // append to variable linked list for static/stack
                                if(strcmp(temp_scope,"global") == 0){
                                    var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                }
                                else{
                                    var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                }

                                if(strcmp(curr->token, "=")==0){
                                    curr = curr->next;

                                    if(strchr(curr->token, '\"')!=NULL){
                                        curr = curr->next; //now we should be either in a comma seperating multiple arrays initializer or a semicolon ";"
                                    }
                                    else{
                                        while(strcmp(curr->token,"}")!=0){
                                            curr = curr -> next;
                                        }
                                        curr = curr -> next; //now we should be either in a comma seperating multiple arrays initializer or a semicolon ";"
                                    }
                                }
                                //iterate to next token until , or ;
                            }

                            // deal with multiple variables declared in 1 lines
                            while(strcmp(curr->token, ";")!=0){
                                curr = curr -> next;

                                strcpy(temp_var, curr->token);

                                curr = curr -> next;
                                curr = curr -> next;
                                //curr->token should be either N or a ]

                                if(strcmp(curr->token, "]")==0){
                                    curr = curr -> next;
                                    // We have to count number of elements in the array
                                    curr = curr->next; //we are now at "{" or " "text" "
                                    int comma_counter = 0;

                                    if(strchr(curr->token, '\"')!=NULL){
                                        sprintf(temp_size, "%d", (int)strlen(curr->token)-1);
                                        // Length of array depends on size of string including null terminator
                                    }

                                    else{
                                        while(strcmp(curr->token,"}")!=0){
                                            if(strcmp(curr->token, ",")==0){
                                                comma_counter++;
                                            }
                                            curr = curr->next;
                                        }
                                        // Length of array depends on number of elements

                                        sprintf(temp_size, "%d", comma_counter+1);
                                    }
                                    strcat(temp_size,"*sizeof(");
                                    strcat(temp_size,temp_holder);
                                    strcat(temp_size,")");

                                    // append to variable linked list for static/stack
                                    if(strcmp(temp_scope,"global") == 0){
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                    }
                                    else{
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                    }

                                    curr = curr -> next;

                                }
                                else{
                                    // We have the size defined inside the [ ]

                                    temp_size[0] = '\0';
                                    while(strcmp(curr->token,"]")!=0){
                                        strcat(temp_size,curr->token);
                                        curr = curr->next;
                                    }
                                    curr = curr->next;

                                    strcat(temp_size,"*sizeof(");
                                    strcat(temp_size,temp_holder);
                                    strcat(temp_size,")");

                                    // append to variable linked list for static/stack
                                    if(strcmp(temp_scope,"global") == 0){
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                    }
                                    else{
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                    }

                                    if(strcmp(curr->token, "=")==0){
                                        curr = curr->next;

                                        if(strchr(curr->token, '\"')!=NULL){
                                            curr = curr->next; //now we should be either in a comma seperating multiple arrays initializer or a semicolon ";"
                                        }
                                        else{
                                            while(strcmp(curr->token,"}")!=0){
                                                curr = curr -> next;
                                            }
                                            curr = curr -> next; //now we should be either in a comma seperating multiple arrays initializer or a semicolon ";"
                                        }
                                    }
                                    //iterate to next token until , or ;
                                }
                            }
                        }
                        else if(strcmp(temp_type, "char*")==0){
                            //case for dealing char* types: literal, malloc or calloc
                            

                            sprintf(temp_size, "%d", type_to_size(temp_type));

                            //append to the variable linked list for the static/heap
                            if(strcmp(temp_scope,"global") == 0){
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                            }
                            else{
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                            }

                            //right now curr->token should point to either an equal, comma or a semicolon
                            if(strcmp(curr->token, "=")==0){
                                //possibility of malloc, calloc or literal
                                curr = curr -> next;
                                if(strchr(curr->token, '\"')!=NULL){
                                    //we have a literal
                                    strcpy(temp_holder, "*");
                                    strcat(temp_holder, temp_var);
                                    strcpy(temp_var, temp_holder);

                                    sprintf(temp_size, "%d", (int)strlen(curr->token)-1);
                                    strcat(temp_size,"*sizeof(char)");

                                    // append to variable linked list for rodata
                                    var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "rodata", temp_size);
                                    curr = curr->next;
                                }
                                else{
                                    // to iterate over type conversion if there is one
                                    while(strcmp(curr->token, ",")!=0 && strcmp(curr->token, ";")!=0 && strcmp(curr->token, "malloc")!=0 && strcmp(curr->token, "calloc")!=0){
                                        curr = curr -> next;
                                    }
                                    
                                    // deal with malloc case
                                    if(strcmp(curr->token, "malloc")==0){
                                        curr = curr -> next; //we are in (
                                        curr = curr -> next; // we are inside the malloc argument
                                        strcpy(temp_size, curr->token);
                                        curr = curr -> next; //either ) or more tokens in argument
                                        while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                            strcat(temp_size, curr->token);
                                            curr = curr->next;
                                        }
                                        strcpy(temp_holder, "*");
                                        strcat(temp_holder, temp_var);
                                        strcpy(temp_var, temp_holder);

                                        // append to variable linked list for heap
                                        var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                        curr = curr->next;
                                    }

                                    // deal with calloc case
                                    else if(strcmp(curr->token, "calloc")==0){
                                        curr = curr -> next; //we are in (
                                        curr = curr -> next; // we are inside the calloc argument
                                        strcpy(temp_size, curr->token);
                                        curr = curr -> next; // first tokens in first argument
                                        while(strcmp(curr->token, ",")!=0){
                                            strcat(temp_size, curr->token);
                                            curr = curr->next;
                                        }
                                        strcat(temp_size,"*");
                                        curr = curr->next;
                                        // tokens in second argument
                                        while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                            strcat(temp_size, curr->token);
                                            curr = curr->next;
                                        }

                                        strcpy(temp_holder, "*");
                                        strcat(temp_holder, temp_var);
                                        strcpy(temp_var, temp_holder);

                                        // append to variable linked list for heap
                                        var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                        curr = curr->next;
                                    }
                                }
                            }

                            // deal with the case of multiple variable declaration on the same line
                            while(strcmp(curr->token, ";")!=0){
                                    curr = curr -> next;
                                    curr = curr -> next;
                                    strcpy(temp_var, curr->token);
                                    // we are at variable name

                                    curr = curr -> next;

                                    sprintf(temp_size, "%d", type_to_size(temp_type));

                                    // append to variable linked list for static/stack
                                    if(strcmp(temp_scope,"global") == 0){
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                    }
                                    else{
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                    }
                                    //right now curr->token should point to either an equal, comma or a semicolon
                                    if(strcmp(curr->token, "=")==0){
                                        //possibility of malloc, calloc or literal
                                        curr = curr -> next;
                                        
                                        if(strchr(curr->token, '\"')!=NULL){
                                            //we have a literal
                                            strcpy(temp_holder, "*");
                                            strcat(temp_holder, temp_var);
                                            strcpy(temp_var, temp_holder);

                                            sprintf(temp_size, "%d", (int)strlen(curr->token)-1);
                                            strcat(temp_size,"*sizeof(char)");

                                            var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "rodata", temp_size);
                                            curr = curr->next;
                                        }
                                        else{

                                            //to get over possible type conversions
                                            while(strcmp(curr->token, ",")!=0 && strcmp(curr->token, ";")!=0 && strcmp(curr->token, "malloc")!=0 && strcmp(curr->token, "calloc")!=0){
                                                curr = curr -> next;
                                            }

                                            // malloc case
                                            if(strcmp(curr->token, "malloc")==0){
                                                curr = curr -> next; //we are in (
                                                curr = curr -> next; // we are inside the malloc argument
                                                strcpy(temp_size, curr->token);
                                                curr = curr -> next; //either ) or more tokens in argument
                                                while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                                    strcat(temp_size, curr->token);
                                                    curr = curr->next;
                                                }
                                                strcpy(temp_holder, "*");
                                                strcat(temp_holder, temp_var);
                                                strcpy(temp_var, temp_holder);

                                                // append to variable linked list for heap
                                                var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                                curr = curr->next;
                                            }

                                            // calloc case
                                            else if(strcmp(curr->token, "calloc")==0){
                                                curr = curr -> next; //we are in (
                                                curr = curr -> next; // we are inside the calloc argument
                                                strcpy(temp_size, curr->token);
                                                curr = curr -> next; // first tokens in first argument
                                                while(strcmp(curr->token, ",")!=0){
                                                    strcat(temp_size, curr->token);
                                                    curr = curr->next;
                                                }
                                                strcat(temp_size,"*");
                                                curr = curr->next;
                                                // tokens in second argument
                                                while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                                    strcat(temp_size, curr->token);
                                                    curr = curr->next;
                                                }

                                                strcpy(temp_holder, "*");
                                                strcat(temp_holder, temp_var);
                                                strcpy(temp_var, temp_holder);

                                                // append to variable linked list for heap
                                                var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                                curr = curr->next;
                                            }
                                        }
                                    }

                                }

                        }
                        else if(strchr(temp_type, '*')!=NULL){
                            // general pointer case, impossible to have literal

                            sprintf(temp_size, "%d", type_to_size(temp_type));
                            if(strcmp(temp_scope,"global") == 0){
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                            }
                            else{
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                            }
                            //right now curr->token should point to either an equal, comma or a semicolon
                            if(strcmp(curr->token, "=")==0){
                                //possibility of malloc, calloc
                                curr = curr -> next;
                                
                                // get over possible type conversion
                                while(strcmp(curr->token, ",")!=0 && strcmp(curr->token, ";")!=0 && strcmp(curr->token, "malloc")!=0 && strcmp(curr->token, "calloc")!=0){
                                    curr = curr -> next;
                                }
                                
                                // malloc case
                                if(strcmp(curr->token, "malloc")==0){
                                    curr = curr -> next; //we are in (
                                    curr = curr -> next; // we are inside the malloc argument
                                    strcpy(temp_size, curr->token);
                                    curr = curr -> next; //either ) or more tokens in argument
                                    while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                        strcat(temp_size, curr->token);
                                        curr = curr->next;
                                    }
                                    strcpy(temp_holder, "*");
                                    strcat(temp_holder, temp_var);
                                    strcpy(temp_var, temp_holder);

                                    var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                    curr = curr->next;
                                }

                                // calloc case
                                else if(strcmp(curr->token, "calloc")==0){
                                    curr = curr -> next; //we are in (
                                    curr = curr -> next; // we are inside the calloc argument
                                    strcpy(temp_size, curr->token);
                                    curr = curr -> next; // first tokens in first argument
                                    while(strcmp(curr->token, ",")!=0){
                                        strcat(temp_size, curr->token);
                                        curr = curr->next;
                                    }
                                    strcat(temp_size,"*");
                                    curr = curr->next;
                                    // tokens in second argument
                                    while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                        strcat(temp_size, curr->token);
                                        curr = curr->next;
                                    }

                                    strcpy(temp_holder, "*");
                                    strcat(temp_holder, temp_var);
                                    strcpy(temp_var, temp_holder);

                                    var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                    curr = curr->next;
                                }
                                
                            }

                            //dealing with multiple variables declared on the same line
                            while(strcmp(curr->token, ";")!=0){
                                    curr = curr -> next;
                                    curr = curr -> next;
                                    strcpy(temp_var, curr->token);
                                    // store new variable name

                                    curr = curr -> next;

                                    sprintf(temp_size, "%d", type_to_size(temp_type));
                                    if(strcmp(temp_scope,"global") == 0){
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                    }
                                    else{
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                    }

                                    //right now curr->token should point to either an equal, comma or a semicolon
                                    if(strcmp(curr->token, "=")==0){
                                        //possibility of malloc, calloc
                                        curr = curr -> next;
                                        
                                        // get over possible type conversion
                                        while(strcmp(curr->token, ",")!=0 && strcmp(curr->token, ";")!=0 && strcmp(curr->token, "malloc")!=0 && strcmp(curr->token, "calloc")!=0){
                                            curr = curr -> next;
                                        }

                                        // malloc case
                                        if(strcmp(curr->token, "malloc")==0){
                                            curr = curr -> next; //we are in (
                                            curr = curr -> next; // we are inside the malloc argument
                                            strcpy(temp_size, curr->token);
                                            curr = curr -> next; //either ) or more tokens in argument
                                            while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                                strcat(temp_size, curr->token);
                                                curr = curr->next;
                                            }
                                            strcpy(temp_holder, "*");
                                            strcat(temp_holder, temp_var);
                                            strcpy(temp_var, temp_holder);

                                            var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                            curr = curr->next;
                                        }

                                        // calloc case
                                        else if(strcmp(curr->token, "calloc")==0){
                                            curr = curr -> next; //we are in (
                                            curr = curr -> next; // we are inside the calloc argument
                                            strcpy(temp_size, curr->token);
                                            curr = curr -> next; // first tokens in first argument
                                            while(strcmp(curr->token, ",")!=0){
                                                strcat(temp_size, curr->token);
                                                curr = curr->next;
                                            }
                                            strcat(temp_size,"*");
                                            curr = curr->next;
                                            // tokens in second argument
                                            while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                                strcat(temp_size, curr->token);
                                                curr = curr->next;
                                            }

                                            strcpy(temp_holder, "*");
                                            strcat(temp_holder, temp_var);
                                            strcpy(temp_var, temp_holder);

                                            var_ll = add_var(var_ll, temp_var, temp_holder2, temp_scope, "heap", temp_size);
                                            curr = curr->next;
                                        }
                                        // token is now either , or ;
                                    }

                                }

                        }
                        else{
                            //we are working with either type int, float, char (basic types)
                            sprintf(temp_size, "%d", type_to_size(temp_type));

                            // handle case of possible multiple variable declaration in the same line
                            while(strcmp(curr->token, ";") != 0){
                                if(strcmp(curr->token, ",") == 0){
                                    // end of current variable, append to the variable linked list
                                    if(strcmp(temp_scope,"global") == 0){
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                                    }
                                    else{
                                        var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                                    }
                                }else if(strcmp(curr->token, "=") == 0){
                                    // iterate over until end of current variable
                                    while(strcmp(curr->next->token, ";")!=0 && strcmp(curr->next->token, ",")!=0){
                                        curr = curr -> next;
                                    }
                                }
                                else{
                                    // we are in the name of the variable, store in temp var
                                    strcpy(temp_var, curr->token);

                                }
                                curr=curr->next;
                            }

                            // append to linked list the last variable before ;
                            if(strcmp(temp_scope,"global") == 0){
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "static", temp_size);
                            }
                            else{
                                var_ll = add_var(var_ll, temp_var, temp_type, temp_scope, "stack", temp_size);
                            }
                        }
                    
                    }

                }

                // reset temporary string holders
                temp_type[0] = '\0';
                temp_var[0] = '\0';
                temp_size[0] = '\0';
                curr = curr->next;
            }
        }
    }
    return var_ll; //return head of the variable linked list
}

// given a file open for reading fp, parse (tokenize) through the file to extract tokens and appends into a token linked list whose
// head is returned by the function. Function also modifies integer pointed by line_len by assigning the number of lines in file fp  
token_node* parse(FILE* fp, int *line_len){
    char line[128];
    token_node* head = NULL;
    token_node* curr;
    int line_num=1;

    int empty =0; //bpolean if we are passed an empty file, to work around edge case


    while(fgets(line, 128, fp)!=NULL){
        empty =1; //not an empty file

        char* delim = DELIMS;
        int i = 0;
        char c = line[i];
        char tok[64];

        tok[0] ='\0'; //reset tok that temporarily stores token

        int string_exception = 0;
        int printskip = 0;

        while(c != '\0' && !(c == '/' && string_exception == 0)){ //if we meet / and its not in a string, ignore remaining tokens because its a comment

            printskip = 0;

            if(c == '"' && i>0){ //rely on the fact that we are testing a functional c file
                if(line[i-1] != '\\'){ 
                    if(string_exception==0){
                        string_exception=1; //start of string
                    }
                    else if(string_exception==1){
                        string_exception=0; //end of string
                        strncat(tok,&c,1);
                        curr = new_token(tok,line_num);
                        head = append_token(head, curr);
                        tok[0] ='\0';

                        printskip=1;

                    }
                }
            }

            if(string_exception==1){
                strncat(tok,&c,1); //we are in a string
            }

            else if(printskip!=1){
                if(strchr(delim, c)!=NULL || c == '\n'){ //we encounter a delimiter
                    
                    if(tok[0]!='\0'){ //non empty temp token, append to the linked list
                        curr = new_token(tok,line_num);
                        head = append_token(head, curr);
                        tok[0] ='\0';
                    }
                    if(c != '\n' && c!=' '){ //if not either these 2, we also store the character that triggered delimiter encounter into the linked list
                        strncat(tok,&c,1);
                        curr = new_token(tok,line_num);
                        head = append_token(head, curr);
                        tok[0] ='\0';
                    }
                }
                else{
                    strncat(tok,&c,1); //not delimiter, keep appending to temp token
                }
            }

            i += 1;
            c = line[i];
            if(c == '\0' && tok[0]!='\0'){ //deal with end of line edge case but temporary tok has some tokens
                curr = new_token(tok,line_num);
                head = append_token(head, curr);
            }
        }
        if(line[strlen(line)-1]=='\n'){
            line_num++; //increment number of lines
        }
    }
    if(empty == 0){
        *line_len = 0; //empty file
    }
    else{
        *line_len = line_num; //update file appropriately
    }
    return head; //return head to linked list
}

// prints the RO Data section of the output
void print_rodata(var_node* var_ll){
    fprintf(stdout, "### ROData ###       scope  type  size\n");
    while(var_ll!=NULL){
        if(strcmp(var_ll->memory, "rodata")==0){
            fprintf(stdout, "   %s   %s   %s   %s\n", var_ll->var, var_ll->scope, var_ll->type, var_ll->size);
        }
        var_ll = var_ll->next;
    }
    fprintf(stdout, "\n");
}

// prints the static section of the output
void print_static(var_node* var_ll){
    fprintf(stdout, "### static data ###\n");
    while(var_ll!=NULL){
        if(strcmp(var_ll->memory, "static")==0){
            fprintf(stdout, "   %s   %s   %s   %s\n", var_ll->var, var_ll->scope, var_ll->type, var_ll->size);
        }
        var_ll = var_ll->next;
    }
    fprintf(stdout, "\n");
}

// prints the heap section of the output
void print_heap(var_node* var_ll){
    fprintf(stdout, "### heap ###\n");
    while(var_ll!=NULL){
        if(strcmp(var_ll->memory, "heap")==0){
            fprintf(stdout, "   %s   %s   %s   %s\n", var_ll->var, var_ll->scope, var_ll->type, var_ll->size);
        }
        var_ll = var_ll->next;
    }
    fprintf(stdout, "\n");
}

// prints the 'unused' section of the output
void print_unused(){
    fprintf(stdout,"####################\n### unused space ###\n####################\n\n");
}

// prints the stack section of the output
void print_stack(var_node* var_ll){
    fprintf(stdout, "### stack ###\n");
    while(var_ll!=NULL){
        if(strcmp(var_ll->memory, "stack")==0){
            fprintf(stdout, "   %s   %s   %s   %s\n", var_ll->var, var_ll->scope, var_ll->type, var_ll->size);
        }
        var_ll = var_ll->next;
    }
    fprintf(stdout, "\n");
}

// prints the number of lines in file section of the output, under stats
void print_stats_line(int line_len){
    fprintf(stdout, "  - Total number of lines in the file: %d\n", line_len);
}

// returns the number of functions in the function linked lists passed as func_ll
int count_funcs(func_node* func_ll){
    int count = 0;
    while(func_ll != NULL){
        count++;
        func_ll = func_ll -> next;
    }
    return count;
}

// prints the number of function section of the output, under stats
void print_stats_num_func(func_node* func_ll){

    int num_of_funcs = count_funcs(func_ll);

    fprintf(stdout,"  - Total number of functions: %d\n", num_of_funcs);

    if(num_of_funcs >= 1){
        fprintf(stdout, "    ");
        fprintf(stdout, "%s", func_ll->name);

        func_ll = func_ll -> next;

        while(func_ll != NULL){
            fprintf(stdout, ", %s", func_ll->name);
            func_ll = func_ll -> next;
        }
        fprintf(stdout,"\n");
    }
}

// prints the number of lines per function section of the output, under stats
void print_stats_line_func(func_node* func_ll){

    fprintf(stdout, "  - Total number of lines per functions:\n");

    while(func_ll != NULL){

        fprintf(stdout, "    %s: %d\n",func_ll -> name, func_ll ->line_num);
        func_ll = func_ll -> next;
    }
}

// prints the number of variables per function section of the output, under stats
void print_stats_var_func(func_node* func_ll, var_node* var_ll){
    fprintf(stdout, "  - Total number of variables per functions:\n");

    while(func_ll != NULL){
        
        int var_count =0;
        var_node* curr = var_ll;

        while(curr!=NULL){
            if(strcmp(curr->scope, func_ll -> name)==0 && strcmp(curr->memory, "stack")==0){
                var_count++;
            }
            curr = curr -> next;
        }
        
        fprintf(stdout, "    %s: %d\n",func_ll -> name, var_count);
        func_ll = func_ll -> next;
    }
}

// free the nodes of function linked list, returns NULL to prevent dangling pointer pointing at the freed head node
func_node* free_funcll (func_node *head){
    func_node *curr=NULL;
    func_node *next=NULL;
    curr=head;
    while (curr!=NULL){
        next=curr->next;
        free(curr); 
        curr=next;
    }

    return NULL;
}

// free the nodes of variable linked list, returns NULL to prevent dangling pointer pointing at the freed head node
var_node* free_varll (var_node *head){
    var_node *curr=NULL;
    var_node *next=NULL;
    curr=head;
    while (curr!=NULL){
        next=curr->next;
        free(curr); 
        curr=next;
    }

    return NULL;
}

// free the nodes of token linked list, returns NULL to prevent dangling pointer pointing at the freed head node
token_node* free_tokenll (token_node *head){
    token_node *curr=NULL;
    token_node *next=NULL;
    curr=head;
    while (curr!=NULL){
        next=curr->next;
        free(curr); 
        curr=next;
    }

    return NULL;
}

void main(int argc, char* argv[]){
    if(argc == 1){
        fprintf(stderr, "Error: No File Passed as Argument\n");
    }
    else{
        FILE* fp = fopen(argv[1], "r");
        if(fp == NULL){
            fprintf(stderr, "Error: Specified File not Found\n");
        }
        else{
            fprintf(stdout, ">>> Memory Model Layout <<<\n\n***  exec // text ***\n   %s\n\n", argv[1]);
            int line_len = 0;
            token_node* token_ll = parse(fp, &line_len);

            func_node* func_ll = NULL;

            var_node* var_ll = filter_tokens(token_ll, &func_ll);

            print_rodata(var_ll);

            print_static(var_ll);

            print_heap(var_ll);

            print_unused();

            print_stack(var_ll);

            fprintf(stdout, "**** STATS ****\n");

            print_stats_line(line_len);

            print_stats_num_func(func_ll);

            print_stats_line_func(func_ll);

            print_stats_var_func(func_ll, var_ll);

            fprintf(stdout, "//////////////////////////////\n");

            func_ll = free_funcll(func_ll);

            token_ll = free_tokenll(token_ll);

            var_ll = free_varll(var_ll);

        }
    }
}