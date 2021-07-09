// libraries
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// constants
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define DIGITS "0123456789"
#define MINUS "-"
#define NON_STATE_CHARS ",.]["
#define SEPERATOR ","
#define ENDOFLINE "."
#define OPENBLOCK "["
#define CLOSEBLOCK "]"
#define COMMENT_START "{"
#define COMMENT_END "}"
#define QUOTATION "\""
#define UNDERSCORE "_"
#define SPACE " "
char KEYWORDS[20][20] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub", "from", "newline"};

/**
Instead of initializing a state string variable and modifying it every time;
I defined a STATE struct with a name member.
I change the state when needed and access their name property to find the source_state(current state)
**/

// State struct
typedef struct {
	char* name;
} STATE;

// States
STATE default_state = {"default"};
STATE key_iden_state = {"keyword and identifier"};
STATE str_const_state = {"string constant"};
STATE comment_state = {"comment"};
STATE int_const_state = {"integer constant"};

// Variables
char *variables[1000];
int variableIndex = 0;
int values[1000] = { 0 };
int line = 1;

/**
PROTOTYPES
**/

// Utilities
int isKeyword(char *str);
char *substring(char *string, int position, int length);
bool variableExists(char *variable);
void undeclarationError(char identifier[]);
void syntaxError(char type[], char expected[], char previous[]);
int getIndex (char *name);

// Operations
void move(char *token[]);
void out(char *tokens[], int tokens_size);
void declaration(char *tokens[]);
void add(char *token[]);
void sub(char *tokens[]);
void loop(char *tokens[], int tokens_size, char *type);

// Execution prototype
void evaluate(char *statement);

// Main and Lexical Analyzer
void main(int argc, char *argv[]) {
	char readFilename[256];
	char writeFilename[256];
	snprintf(readFilename, sizeof(readFilename), "%s.ba", argv[1]);
	snprintf(writeFilename, sizeof(writeFilename), "%s.lx", argv[1]);

	STATE source_state = default_state; // FSA starts from default state

	char token_stack[100]; // tokens are written to file from this string
	token_stack[0] = '\0'; // empty string

	char statement_stack[1000];
	statement_stack[0] = '\0';

	char statements_arr[100][1000] = {};
	int statements_arr_len = 0;

	int loop_detector = 0;

	FILE *readFilePointer = fopen(readFilename, "r"); // read files from here
	FILE *writeFilePointer = fopen(writeFilename, "w"); // write to this file
	char ch;

	if (readFilePointer == NULL) // check if file exists
	{
		printf("ERROR: File is not available\n");
	}
	else
	{
		// iterate number of characters in file, assign current char to ch
		while ((ch = fgetc(readFilePointer)) != EOF)
		{
            if (ch == '\n') {
                line++;
            }
			// check if char assigned to ch is valid
            // (ascii -126) used in code
            // (ascii 126-) may only appear in a comment or string
			if (ch<0 || ch>125) {
                if (source_state.name == "comment" || source_state.name == "string constant") {

                }
                else {
                    printf("An unrecognized character is detected in code!\n");
					break;
				}
			}

			char ch_str[2] = {ch, '\0'}; // create string from ch to use it in string.h functions

			/**
			NESTED IF ELSE STATEMENTS TO CATCH EVERY POSSIBLE STATE-INPUT COMBINATION;
			APPLY THE OPERATIONS DEFINED FOR THAT PARTICULAR STATE-INPUT COMBINATION;
			POSSIBLE OPERATIONS
			-add char to token_stack --> strcat(token_stack, ch_str);
			-empty token_stack --> token_stack[0] = '\0';
			-change state --> source_state = ..._state;
			-write to file --> fprintf(writeFilePointer, "...");
			-print error message and break loop --> printf("..."); break;
 			**/
			if (source_state.name == "default") { // CHECK
				if (strstr(ALPHABET, ch_str)) { // DEFAULT_STATE-ALPHABET_INPUT COMBINATION
					strcat(token_stack, ch_str);
					source_state = key_iden_state;
				}
				else if (strstr(DIGITS MINUS, ch_str)) {
					strcat(token_stack, ch_str);
					source_state = int_const_state;
				}
				else if (strstr(NON_STATE_CHARS, ch_str)) { // ] [ , .
					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
						strcat(statement_stack, "EndOfLine ");
                        if (loop_detector == 0) {
                            strcpy(statements_arr[statements_arr_len], statement_stack);
                            evaluate(statements_arr[statements_arr_len]);
                            statements_arr_len++;
                            statement_stack[0] = '\0';
                        }
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
						strcat(statement_stack, "Seperator ");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
						strcat(statement_stack, "OpenBlock ");
						loop_detector++;
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
						strcat(statement_stack, "CloseBlock ");
						loop_detector--;
						if (loop_detector == 0 ) {
							strcpy(statements_arr[statements_arr_len], statement_stack);
							evaluate(statements_arr[statements_arr_len]);
							statements_arr_len++;
							statement_stack[0] = '\0';
						}
					}
				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
				else if (strstr(COMMENT_END UNDERSCORE, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
				else if (strstr(QUOTATION, ch_str)) {
					strcat(token_stack, ch_str);
					source_state = str_const_state;
				}
				else if (strstr(SPACE, ch_str)) {
					continue;
				}
                else if (ch == '\n'){
                    //
                }
                else {
                    printf("ERROR: Invalid token: %s\n", ch_str);
					break;
                }
			}
			else if (source_state.name == "keyword and identifier") {
				// check if identifier in token_stack is out of bound
				if (strlen(token_stack) > 20 ) {
					printf("Length of variable name is out of bound\n");
					break;
				}

				// check inputs
				if (strstr(ALPHABET DIGITS UNDERSCORE, ch_str)) {
					strcat(token_stack, ch_str);
				}
				else if (strstr(NON_STATE_CHARS SPACE, ch_str)) { // ] [ , . or space

					if (isKeyword(token_stack)) {
						fprintf(writeFilePointer, "Keyword %s\n", token_stack);
						strcat(statement_stack, "Keyword ");
						strcat(statement_stack, token_stack);
						strcat(statement_stack, " ");
					}
					else {
						fprintf(writeFilePointer, "Identifier %s\n", token_stack);
						strcat(statement_stack, "Identifier ");
						strcat(statement_stack, token_stack);
						strcat(statement_stack, " ");
					}

					token_stack[0] = '\0';

					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
						strcat(statement_stack, "EndOfLine ");
						if (loop_detector == 0) {
                            strcpy(statements_arr[statements_arr_len], statement_stack);
                            evaluate(statements_arr[statements_arr_len]);
                            statements_arr_len++;
                            statement_stack[0] = '\0';
                        }
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
						strcat(statement_stack, "Seperator ");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
						strcat(statement_stack, "OpenBlock ");
						loop_detector++;
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
						strcat(statement_stack, "CloseBlock ");
						loop_detector--;
						if (loop_detector == 0 ) {
							strcpy(statements_arr[statements_arr_len], statement_stack);
							evaluate(statements_arr[statements_arr_len]);
							statements_arr_len++;
							statement_stack[0] = '\0';
						}
					}
					source_state = default_state;
				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
				else if (strstr(QUOTATION MINUS COMMENT_END, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
                else if (ch == '\n'){
                    //
                }
                else {
                    printf("ERROR: Invalid token: %s\n", ch_str);
					break;
                }
			}
			else if (source_state.name == "string constant") {
				// check inputs
				if (strstr(QUOTATION, ch_str)) {
					strcat(token_stack, ch_str);
					fprintf(writeFilePointer, "StringConstant %s\n", token_stack);
					strcat(statement_stack, "StringConstant ");
					strcat(statement_stack, token_stack);
					strcat(statement_stack, " ");
					token_stack[0] = '\0';
					source_state = default_state;
				}
				else {
					strcat(token_stack, ch_str);
				}
			}
			else if (source_state.name == "comment") {
				// check inputs
				if (strstr(COMMENT_END, ch_str)) {
					token_stack[0] = '\0';
					source_state = default_state;
				}
				else {
					// do nothing
				}
			}
			else if (source_state.name == "integer constant") {
				// check if integer constant is out of bound
				if (strlen(token_stack) > 100 ) {
					printf("Length of integer constant is out of bound\n");
					break;
				}

				// check inputs
				if (strstr(ALPHABET QUOTATION COMMENT_END MINUS UNDERSCORE, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
				else if (strstr(DIGITS, ch_str)) {
					strcat(token_stack, ch_str);
				}
				else if (strstr(NON_STATE_CHARS SPACE, ch_str)) { // ] [ , . or space
					fprintf(writeFilePointer, "IntConstant %s\n", token_stack);
					strcat(statement_stack, "IntConstant ");
					strcat(statement_stack, token_stack);
					strcat(statement_stack, " ");

                    ///// CHECK IF

					token_stack[0] = '\0';

					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
						strcat(statement_stack, "EndOfLine ");
						if (loop_detector == 0) {
                            strcpy(statements_arr[statements_arr_len], statement_stack);
                            evaluate(statements_arr[statements_arr_len]);
                            statements_arr_len++;
                            statement_stack[0] = '\0';
                        }
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
						strcat(statement_stack, "Seperator ");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
						strcat(statement_stack, "OpenBlock ");
						loop_detector++;
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
						strcat(statement_stack, "CloseBlock ");
						loop_detector--;
						if (loop_detector == 0 ) {
							strcpy(statements_arr[statements_arr_len], statement_stack);
							evaluate(statements_arr[statements_arr_len]);
							statements_arr_len++;
							statement_stack[0] = '\0';
						}
					}
					source_state = default_state;
				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
                else if (ch == '\n'){
                    //
                }
                else {
                    printf("ERROR: Invalid token: %s\n", ch_str);
                    break;
                }
			}
		}
		// end of file
		// check if file ended in a comment or string
		if (source_state.name == "comment" || source_state.name == "string constant") {
			printf("ERROR: A big lexeme is just left open\n");
		}
	}

	fclose(readFilePointer);
	fclose(writeFilePointer);
}

/**
FUNCTION BODY
**/

int isKeyword(char *str) {
    int keywords_len = 11;
    for (int i = 0; i < keywords_len; i++ ) {
        if (!strcmp(KEYWORDS[i], str)) {
            return true;
        }
    }
    return false;
}

char *substring(char *string, int position, int length)
{
   char *p;
   int c;

   p = malloc(length+1);

   if (p == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }

   for (c = 0; c < length; c++)
   {
      *(p+c) = *(string+position-1);
      string++;
   }

   *(p+c) = '\0';

   return p;
}

bool variableExists(char *variable){
    if(variableIndex == 0)
        return false;
    for(int i = 0; i < (variableIndex); i++)
        if(!strcmp(variable, variables[i]))
            return true;
    return false;
}

void undeclarationError(char identifier[]) {
    printf("ERROR: line: %d | Identifier %s called before declaration!\n", line, identifier);
    exit(0);
}

void syntaxError(char type[], char expected[], char previous[]) {
    printf("ERROR: line: %d | Unvalid %s statement => %s expected after %s.\n", line, type, expected, previous);
    exit(0);
}

// Getting the index of a variable by the name of it
int getIndex (char *name)
{
    if(variableIndex == 0)
        return -1;
    for(int i = 0; i < (variableIndex); i++)
        if(!strcmp(name, variables[i]))
            return i;
    return -1;
}

// Assignment functionality
void move(char *token[])
{
    int index = getIndex(token[7]);
    if(!strcmp(token[2], "Identifier"))
            values[index] = values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[index] = atoi(token[3]);

    // printf("Moved %d to %s\n", values[index], variables[index]);
}

// Print functionality
void out(char *tokens[], int tokens_size)
{
    char new_tokens[1000][1000];
    int new_tokens_len = 0;

    char string_holder[1000];
    int holder_len = 0;

    int str = 0;

    for (int i = 0; i < tokens_size; i++) {

        if (str == 0) {
            if (strstr(tokens[i],"\"")) {
                strcat(string_holder, tokens[i]);
                holder_len++;
                str++;
            }
            else {
                strcpy(new_tokens[new_tokens_len], tokens[i]);
            }
        }
        else if (str == 1) {
            if (strstr(tokens[i],"\"")) {
                strcat(string_holder, tokens[i]);
                strcpy(new_tokens[new_tokens_len], string_holder);
                string_holder[0] = '\0';
                holder_len = 0;
                str--;
            }
            else {
                strcat(string_holder, tokens[i]);
            }
        }
        else {
            printf("never should have come here");
        }
    }
    int counter = 0;
    // Looping through the array and execute all the possible executables
    while (tokens[counter] != NULL)
    {
        if(!strcmp(tokens[counter], "Seperator"))
        {
            counter++;
            continue;
        }
        else if(!strcmp(tokens[counter], "Identifier"))
            printf("%d ", values[getIndex(tokens[counter + 1])]);
        else if(!strcmp(tokens[counter], "IntConstant"))
            printf("%s ", tokens[counter + 1]);
        else if(!strcmp(tokens[counter], "newline"))
            printf("\n");
        else if(!strcmp(tokens[counter], "StringConstant"))
        {
            while (tokens[counter] != NULL && strcmp(tokens[counter + 1], "Seperator"))
            {
               counter++;
               if(!strcmp(tokens[counter], "EndOfLine"))
                    break;
               if(holder_len == 1 && (!strcmp(tokens[counter + 1], "Seperator") || !strcmp(tokens[counter], "EndOfLine")))
               {
                  char *temp = substring(tokens[counter], 2, strlen(tokens[counter]) - 2);
                  printf("%s ",temp);
                  temp = "";
               }

               else
               {
                   if(counter == 6)
                   {
                       char *temp = substring(tokens[counter], 2, strlen(tokens[counter]));
                       printf("%s ",temp);
                   }
                   else if((!strcmp(tokens[counter + 1], "Seperator") || !strcmp(tokens[counter], "EndOfLine")))
                   {
                       char *temp = substring(tokens[counter], 1, strlen(tokens[counter]) - 1);
                       printf("%s ",temp);
                   }
                   else
                   {
                       printf("%s ", tokens[counter]);
                   }
               }

            }
        }
        counter++;
    }
}

// Declaration functionality
void declaration(char *tokens[])
{
    variables[variableIndex] = tokens[3]; // 0 when using, then 1
    values[variableIndex] = 0; // 1
    variableIndex++;
}

// Addition functionality
void add(char *token[])
{
    int index = getIndex(token[7]);
    if(!strcmp(token[2], "Identifier"))
            values[index] += values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[index] += atoi(token[3]);
}

// Subtraction functionality
void sub(char *tokens[])
{
    int index = getIndex(tokens[7]);

    if(!strcmp(tokens[2], "Identifier"))
            values[index] -= values[getIndex(tokens[3])];

    if(!strcmp(tokens[2], "IntConstant"))
            values[index]-= atoi(tokens[3]);
}

// Loop functionality
void loop(char *tokens[], int tokens_size, char *type)
{
    char inside_loop_matris[100][1000];
    int lines = 0;
    int times = 0;

    char identifier[21];
    int identifier_index;

    // Finding the valid time for the specific command
    if(!strcmp(tokens[2], "Identifier")) {
        strcpy(identifier, tokens[3]);
        identifier_index = getIndex(tokens[3]);
        times = values[identifier_index];
    }
    else if(!strcmp(tokens[2], "IntConstant"))
        times = atoi(tokens[3]);

    if (!strcmp("oneline", type)) {
        for (int i = 6; i < tokens_size; i++) {
            strcat(inside_loop_matris[0], tokens[i]);
            if (i != tokens_size-1) {
                strcat(inside_loop_matris[0], " ");
            }
        }
        lines = 1;
    }
    else if (!strcmp("block", type)) {
        int looper = 0;
        for (int i = 7; i < tokens_size-1; i++)
        {
            strcat(inside_loop_matris[lines], tokens[i]);
            if (i != 6) {
                strcat(inside_loop_matris[lines], " ");
            }

            if (!strcmp(tokens[i],"OpenBlock")){
                looper++;
            }
            else if (!strcmp(tokens[i],"CloseBlock")){
                looper--;
                lines++;
            }
            else if(!strcmp("EndOfLine", tokens[i]) && looper == 0){
                lines++;
            }
        }
    }

    char run[1000][1000];
    int run_len = times * lines;


    for (int i = 0; i < run_len;) {
        for (int j = 0; j < lines; j++) {
            strcat(run[i], inside_loop_matris[j]);
            i++;
        }
    }

    for (int i = 0; i < run_len; i++) {
        evaluate(run[i]);
        if(!strcmp(tokens[2], "Identifier") && i%(lines) == lines - 1) {
            values[identifier_index]--;
        }
    }
}

// Evaluation and Execution
void evaluate(char *statement) {
    int strconst = 0;
    char *tokens[1000] = {'\0'};
    int tokens_size = 0;
    int wordCount = 0;
    char *splitedWords = strtok(statement, " ");
    while (splitedWords != NULL) {
        tokens[wordCount++] = splitedWords;
        splitedWords = strtok(NULL, " ");
        tokens_size++;
    }

    char type[20] = {"\0"};

    if (!strcmp(tokens[0], "Keyword")){
        // declaration
        if (!strcmp(tokens[1],"int")) {
            strcpy(type, "Declaration");
            if (!strcmp(tokens[2], "Identifier")) {
                if (variableExists(tokens[3])) {
                    printf("ERROR at line %d: Double declaration of variable %s\n", line, tokens[3]);
                    exit(0);
                }
                if (!strcmp(tokens[4], "EndOfLine")) {
                    declaration(tokens);
                } else {
                    syntaxError(type, "EndOfLine", tokens[3]);
                }
            }
            else {
                syntaxError(type, "Identifier", tokens[1]);
            }
        }
        // assignment
        else if (!strcmp(tokens[1],"move")) {
            strcpy(type, "Assignment");
            if (!strcmp(tokens[2], "Identifier") || !strcmp(tokens[2], "IntConstant")) {
                if (!strcmp(tokens[2], "Identifier") && !variableExists(tokens[3]) ) {
                    undeclarationError(tokens[3]);
                }
                if (!strcmp(tokens[4], "Keyword") && !strcmp(tokens[5], "to")) {
                    if (!strcmp(tokens[6], "Identifier")) {
                        if (!variableExists(tokens[7])) {
                            undeclarationError(tokens[7]);
                        }
                        if (!strcmp(tokens[8], "EndOfLine")) {
                            move(tokens);
                        } else {
                            syntaxError(type, "EndOfLine", tokens[7]);
                        }
                    } else {
                        syntaxError(type, "Identifier", tokens[5]);
                    }
                } else {
                    syntaxError(type, "Keyword to", tokens[2]);
                }
            } else {
                syntaxError(type, "Identifier or IntConstant", tokens[1]);
            }
        }
        // addition
        else if (!strcmp(tokens[1],"add")) {
            strcpy(type, "Addition");
            if (!strcmp(tokens[2], "Identifier") || !strcmp(tokens[2], "IntConstant")) {
                if (!strcmp(tokens[2], "Identifier") && !variableExists(tokens[3]) ) {
                    undeclarationError(tokens[3]);
                }
                if (!strcmp(tokens[4], "Keyword") && !strcmp(tokens[5], "to")) {
                    if (!strcmp(tokens[6], "Identifier")) {
                        if (!variableExists(tokens[7])) {
                            undeclarationError(tokens[7]);
                        }
                        if (!strcmp(tokens[8], "EndOfLine")) {
                            add(tokens);
                        } else {
                            syntaxError(type, "EndOfLine", tokens[7]);
                        }
                    } else {
                        syntaxError(type, "Identifier", tokens[5]);
                    }
                } else {
                    syntaxError(type, "Keyword to", tokens[3]);
                }
            } else {
                syntaxError(type, "Identifier or IntConstant", tokens[1]);
            }
        }
        // subtraction
        else if (!strcmp(tokens[1],"sub")) {
            strcpy(type, "Subtraction");
            if (!strcmp(tokens[2], "Identifier") || !strcmp(tokens[2], "IntConstant")) {
                if (!strcmp(tokens[2], "Identifier") && !variableExists(tokens[3]) ) {
                    undeclarationError(tokens[3]);
                }
                if (!strcmp(tokens[4], "Keyword") && !strcmp(tokens[5], "from")) {
                    if (!strcmp(tokens[6], "Identifier")) {
                        if (!variableExists(tokens[7])) {
                            undeclarationError(tokens[7]);
                        }
                        if (!strcmp(tokens[8], "EndOfLine")) {
                            sub(tokens);
                        } else {
                            syntaxError(type, "EndOfLine", tokens[7]);
                        }
                    } else {
                        syntaxError(type, "Identifier", tokens[5]);
                    }
                } else {
                    syntaxError(type, "Keyword from", tokens[3]);
                }
            } else {
                syntaxError(type, "Identifier or IntConstant", tokens[1]);
            }
        }
        // output
    else if (!strcmp(tokens[1],"out")) {
        int counter = 2;
        while (tokens[counter] != NULL)
        {
            if(!strcmp(tokens[counter],"Keyword"))
            {
                if(!strcmp(tokens[counter + 1],"newline"))
                {
                    if(!strcmp(tokens[counter + 2],"EndOfLine") || !strcmp(tokens[counter + 2],"Seperator"))
                    {
                        counter += 2;
                        continue;
                    }
                    else
                    {
                        printf("ERROR: line: %d | Invalid out syntax\n", line);
                        exit(0);
                    }

                }
                else
                {
                    printf("ERROR: line: %d | Invalid out syntax\n", line);
                    exit(0);
                }
            }
            else if(!strcmp(tokens[counter],"IntConstant"))
            {
                if(atoi(tokens[counter + 1]) != NULL)
                {
                    if(!strcmp(tokens[counter + 2],"EndOfLine") || !strcmp(tokens[counter + 2],"Seperator"))
                    {
                        counter += 2;
                        continue;
                    }
                    else
                    {
                        printf("ERROR: line: %d | Invalid out syntax\n", line);
                        exit(0);
                    }
                }
                else
                {
                    printf("ERROR: line: %d | Invalid out syntax\n", line);
                    exit(0);
                }

            }
            else if(!strcmp(tokens[counter],"Identifier"))
            {
                if(variableExists(tokens[counter + 1]))
                {
                    if(!strcmp(tokens[counter + 2],"EndOfLine") || !strcmp(tokens[counter + 2],"Seperator"))
                    {
                        counter += 2;
                        continue;
                    }
                    else
                    {
                        printf("ERROR: line: %d | Invalid out syntax\n", line);
                        exit(0);
                    }
                }
                else
                {
                    printf("ERROR: line: %d | Variable does not existed!\n", line);
                    exit(0);
                }
            }
            else if(!strcmp(tokens[counter],"StringConstant"))
            {
                counter++;
                while (strcmp(tokens[counter], "EndOfLine"))
                {

                    if(!strcmp(tokens[counter], "Seperator"))
                    {
                        counter++;
                        break;
                    }
                    counter++;

                }
                if(!strcmp(tokens[counter], "Identifier") || !strcmp(tokens[counter], "Keyword") ||
                    !strcmp(tokens[counter], "IntConstant") || !strcmp(tokens[counter], "StringConstant") ||
                    !strcmp(tokens[counter], "EndOfLine") || !strcmp(tokens[counter], "Seperator"))
                {
                    continue;
                }
                else
                {
                    printf("ERROR: line: %d | Seprater does not exist!\n", line);
                    exit(0);
                }
            }
            counter++;
        }

        if(!strcmp(tokens[counter - 1], "EndOfLine") && strcmp(tokens[counter - 2], "Seperator"))
           out(tokens, tokens_size);

        else
           printf("ERROR: line: %d | Invalid out syntax\n", line);
    }
        // loop
        else if (!strcmp(tokens[1],"loop")) {
            if (!strcmp(tokens[2], "Identifier") || !strcmp(tokens[2], "IntConstant")) {
                if (!strcmp(tokens[2], "Identifier") && !variableExists(tokens[3]) )
                    undeclarationError(tokens[3]);

                if (!strcmp(tokens[5], "times")) {
                    if (!strcmp(tokens[6], "OpenBlock")) {
                        loop(tokens, tokens_size, "block");
                    }
                    else if (!strcmp(tokens[6], "Keyword")) {
                        loop(tokens, tokens_size, "oneline");
                    }
                    else
                        syntaxError(type, "OpenBlock or a starting Keyword", tokens[5]);
                }
                else
                    syntaxError(type, "Keyword times", tokens[4]);

            }
            else {
                syntaxError(type, "Identifier or IntConstant", tokens[1]);
            }
        }
    }
    else {
        printf("ERROR at line %d: Statement should start with a valid keyword!\n", line);
        exit(0);
    }
}
