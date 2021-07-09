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
#define NON_STATE_CHARS ",.][" // I DEFINED SUCH CONSTANT BECAUSE THEY HAVE SIMILAR BEHAVIOR PATTERN
#define SEPERATOR ","
#define ENDOFLINE "."
#define OPENBLOCK "["
#define CLOSEBLOCK "]"
#define COMMENT_START "{"
#define COMMENT_END "}"
#define QUOTATION "\""
#define UNDERSCORE "_"
#define SPACE " "
#define KEYWORDS "int move to loop times out newline add sub from newline"

/**
Instead of initializing a state string variable and modifying it every time;
I defined a STATE struct with a name member.
I change the state when needed and access their name property to find the source_state(current state)
**/
typedef struct {
	char* name;
} STATE;

STATE default_state = {"default"};
STATE key_iden_state = {"keyword and identifier"};
STATE str_const_state = {"string constant"};
STATE comment_state = {"comment"};
STATE int_const_state = {"integer constant"};


char *variables[1000];
int variableIndex = 0;
int values[1000] = { 0 };
int line = 0;

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


// !!SPLIT FUNCTION IS NOT FULLY WORKING!!
// Split function
// void split(char *token[], char *target[1000]){
//     int wordCount = 0;
//     char *splitedWords = strtok(token, " ");
//     while (splitedWords != NULL) {
//         target[wordCount++] = splitedWords;
//         splitedWords = strtok(NULL, " ");
//     }
// }

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

    printf("Moved %d to %s\n", values[index], variables[index]);
}

// Print functionality
void out(char *token[])
{
    int counter = 0;
    // Looping through the array and execute all the possible executables
    while (token[counter] != NULL)
    {
        if(!strcmp(token[counter], "Seperator"))
        {
            counter++;
            continue;
        }
        else if(!strcmp(token[counter], "Identifier"))
            printf("%d ", values[getIndex(token[counter + 1])]);
        else if(!strcmp(token[counter], "IntConstant"))
            printf("%s ", token[counter + 1]);
        else if(!strcmp(token[counter], "newline"))
            printf("\n");
        else if(!strcmp(token[counter], "StringConstant"))
            printf("%s ", token[counter + 1]);

        counter++;
    }
}

// Declaration functionality
void declaration(char *tokens[])
{
	// printf("%s\n", tokens[3]);
    variables[variableIndex] = tokens[3]; // 0 when using, then 1
    values[variableIndex] = 0; // 1
    printf("declared %s with value %d\n", variables[variableIndex], values[variableIndex]);
    variableIndex++;
}

void add(char *token[])
{
    int index = getIndex(token[7]);
    if(!strcmp(token[2], "Identifier"))
            values[index] += values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[index] += atoi(token[3]);

    printf("added %s to %s | new value = %d\n", token[3], variables[index], values[index]);
}

void sub(char *tokens[])
{
    int index = getIndex(tokens[7]);

    if(!strcmp(tokens[2], "Identifier"))
            values[index] -= values[getIndex(tokens[3])];

    if(!strcmp(tokens[2], "IntConstant"))
            values[index]-= atoi(tokens[3]);

    printf("subbed %s from %s | new value = %d\n", tokens[3], variables[index], values[index]);
}

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
        // printf("%s\n", inside_loop_matris[0]);
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
            // printf("looper: %d | token:  %s \n", looper, tokens[i]);
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
        // printf("statement: %s\n", run[i]);
        evaluate(run[i]);
        if(!strcmp(tokens[2], "Identifier") && i%(lines) == lines - 1) {
            values[identifier_index]--;
        }
    }
}

void evaluate(char *statement) {
    char *tokens[1000] = {'\0'};
    int tokens_size = 0;
    int wordCount = 0;
    char *splitedWords = strtok(statement, " ");
    while (splitedWords != NULL) {
        tokens[wordCount++] = splitedWords;
        splitedWords = strtok(NULL, " ");
        tokens_size++;
    }

    // for (int i = 0; i < tokens_size; i++) {
    //     printf("%s ", tokens[i]);
    // }
    // printf("tokens[3]: %s\n", tokens[3]);

    char type[20] = {"\0"};

    // declaration
    if (!strcmp(tokens[1],"int")) {
        strcpy(type, "Declaration");
        // type = "Declaration";
        // statement_stack = "Keyword int Identifier varname endofline"
        if (!strcmp(tokens[2], "Identifier")) {
               // printf("%\n%s\n",tokens[3]);
               // printf("%\n%d\n",variableExists(tokens[3]));
               // printf("var %s \n", variables[0]);
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
        if ((tokens_size -2) % 3 != 0){
            printf("ERROR: line: %d | Invalid out syntax\n", line);
            exit(0);
        }
        int outsize = (tokens_size - 2) / 3; // how many inputs does output statement have
        for (int i = 1; i <= outsize; i++) {
            if (i != outsize && !strcmp(tokens[i*3 + 1], "Seperator")) {
                if(!strcmp(tokens[i*3 -1], "Identifier") || !strcmp(tokens[i*3 -1], "Keyword") || !strcmp(tokens[i*3 -1], "StringConstant") || !strcmp(tokens[i*3 -1], "IntConstant")) {
                    if (!variableExists(tokens[i*3]) && !strcmp(tokens[i * 3 - 1], "Identifier"))
                        undeclarationError(tokens[i * 3]);
                    if (!strcmp(tokens[i*3 -1], "Keyword") && strcmp(tokens[i*3], "newline")){
                        printf("ERROR: line: %d | Invalid Keyword in out statement\n", line, tokens[i*3]);
                        exit(0);
                    }
                    continue;
                }
            }
            else if (i == outsize && !strcmp(tokens[i*3 + 1], "EndOfLine")) {
                if(!strcmp(tokens[i*3 -1], "Identifier") || !strcmp(tokens[i*3 -1], "Keyword") || !strcmp(tokens[i*3 -1], "StringConstant") || !strcmp(tokens[i*3 -1], "IntConstant")) {
                    if (!variableExists(tokens[i*3]) && !strcmp(tokens[i * 3 - 1], "Identifier"))
                        undeclarationError(tokens[i * 3]);
                    if (!strcmp(tokens[i*3 -1], "Keyword") && strcmp(tokens[i*3], "newline")){
                        printf("ERROR: line: %d | Keyword %s is not a valid input for out statement\n", line, tokens[i*3]);
                        exit(0);
                    }
                    out(tokens);
                }
            }
            else {
                if (strcmp(tokens[i*3 +1], "Seperator"))
                    printf("ERROR: line: %d | Out inputs should be seperated by commas.\n", line);
                else if (strcmp(tokens[i*3 +1], "EndOfLine")){
                    printf("ERROR: line: %d | Out inputs should end with EndOfLine\n", line);
                }
                exit(0);
            }
        }
    }
    // loop
    else if (!strcmp(tokens[1],"loop")) {
        if (!strcmp(tokens[2], "Identifier") || !strcmp(tokens[2], "IntConstant")) {
            if (!strcmp(tokens[2], "Identifier") && !variableExists(tokens[3]) )
                undeclarationError(tokens[3]);

            if (!strcmp(tokens[5], "times")) {
                if (!strcmp(tokens[6], "OpenBlock")) {
                    loop(tokens, tokens_size, "block");
                    // if (!strcmp(tokens[6], "OpenBlock")) {
                    //     if (!strcmp(tokens[tokens_size-1], "CloseBlock")) {
                    //         printf("call loop()");
                    //     }
                    //     else {
                    //         printf("ERROR at line %d: \n", line);
                    //         exit(0);
                    //     }
                    // }
                    // else
                    //     syntaxError(type, "OpenBlock", tokens[5]);
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
    else {
        printf("ERROR at line %d: Statement should start with a valid keyword!\n", line);
        exit(0);
    }
}

void main(int argc, char *argv[]) {
    // handle given filename argument
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
				// if (strstr(ALPHABET QU ch);
				//     break;
                // }
			}

			char ch_str[2] = {ch, '\0'}; // create string from ch to use it in string.h functions 

			// printf("%s | %s -> ", ch_str, source_state.name);

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
					
					if (strstr(KEYWORDS, token_stack)) {
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
				
			}

		}
		// end of file
		// check if file ended in a comment or string
		if (source_state.name == "comment" || source_state.name == "string constant") {
			printf("ERROR: A big lexeme is just left open\n");
		}
	}
	
	// close files
	fclose(readFilePointer);
	fclose(writeFilePointer);
}
