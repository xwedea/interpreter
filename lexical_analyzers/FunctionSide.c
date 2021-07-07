#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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

// Addition functionality


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

void loop(char *token[])
{
    int times = 0;

    // Finding the valid time for the specific command
    if(!strcmp(token[2], "Identifier"))
        times = values[getIndex(token[3])];

    else if(!strcmp(token[2], "IntConstant"))
        times = atoi(token[3]);

    // Main loop
    for(times; times >= 0; times--)
    {
        // Update the variable if we loop with a variable
        if(!strcmp(token[2], "Identifier"))
            values[getIndex(token[3])] = times;

        if(!strcmp(token[2], "OpenBlock"))
        {
            // Operations when it's a code block
            // Parsing code recursively
        }
        else
        {
            // Operations when it's not a code block
            // Parsing code recursively
            printf("* %d\n", times);
        }
    }
}

// Declaration functionality
void declaration(char *tokens[])
{
    // 0
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

// Subtraction functionality
void sub(char *tokens[])
{
    int index = getIndex(tokens[7]);

    if(!strcmp(tokens[2], "Identifier"))
            values[index] -= values[getIndex(tokens[3])];

    if(!strcmp(tokens[2], "IntConstant"))
            values[index]-= atoi(tokens[3]);

    printf("subbed %s from %s | new value = %d\n", tokens[3], variables[index], values[index]);

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

    char type[20] = {"\0"}; 

    // declaration
    if (!strcmp(tokens[1],"int")) {
        strcpy(type, "Declaration");
        // type = "Declaration";
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
        int outsize = (tokens_size - 2) / 3; // how many inputs does output statement have
        for (int i = 1; i <= outsize; i++) {
            printf("%s\n", tokens[i*3 + 1]);
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
            else if (i != outsize && strcmp(tokens[i*3 + 1], "Seperator")) {
                printf("ERROR: line: %d | Out inputs should be seperated by commas.\n", line);
                exit(0);
            }
            else if (i == outsize && strcmp(tokens[i*3 + 1], "EndOfLine")) {
                printf("ERROR: line: %d | Statement should end with EndOfLine\n", line);
                exit(0);
            }
            else if (i != outsize && !strcmp(tokens[i*3 + 1], "EndOfLine")) {
                printf("ERROR: line: %d | Invalid usage of EndOfLine\n", line);
                exit(0);
            }
            else if (strcmp(tokens[i*3 + 1], "Seperator") && strcmp(tokens[i*3 + 1], "EndOfLine")) {
                if (strcmp(tokens[i*3 +1], "Seperator"))
                    printf("ERROR: line: %d | Out inputs should be seperated by commas.\n", line);
                else if (strcmp(tokens[i*3 +1], "EndOfLine")){
                    printf("ERROR: line: %d | Out inputs should end with \".\"\n", line);
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
                    if (!strcmp(tokens[6], "OpenBlock")) {
                        if (!strcmp(tokens[tokens_size-1], "OpenBlock")) {
                            printf("call loop()");
                        } 
                        else {
                            printf("ERROR at line %d: \n", line);
                            exit(0);
                        }
                    } 
                    else
                        syntaxError(type, "OpenBlock", tokens[5]);
                } 
                else
                    syntaxError(type, "OpenBlock", tokens[5]);
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


int main()
{   
    char test[7][1000] = {
        "Keyword int Identifier firstVar EndOfLine",
        "Keyword int Identifier second EndOfLine",
        "Keyword move IntConstant 10 Keyword to Identifier firstVar EndOfLine",
        "Keyword add IntConstant 5 Keyword to Identifier second EndOfLine",
        "Keyword sub Identifier second Keyword from Identifier firstVar EndOfLine",
        "Keyword out Identifier firstVar Seperator Keyword newline Seperator StringConstant \"lmao\" Keyword newline EndOfLine",
        "Keyword out Identifier firstVar Seperator IntConstant 78 Seperator StringConstant \"lmao\" Seperator Keyword newline Seperator StringConstant mmm EndOfLine",
    };

    evaluate(test[0]);
    evaluate(test[1]);
    evaluate(test[2]);
    evaluate(test[3]);
    evaluate(test[4]);
    evaluate(test[5]);
    // evaluate(test[6]);


    char command[7][1000] = {"Keyword int Identifier firstVar EndOfLine",
                            "Keyword move IntConstant 10 Keyword to Identifier firstVar EndOfLine",
                            "Keyword int Identifier second EndOfLine",
                            // "Keyword move IntConstant 5 Keyword to Identifier firstVar EndOfLine",
                            "Keyword move IntConstant 2 Keyword to Identifier second EndOfLine",
                            "Keyword sub IntConstant 1 Keyword from Identifier firstVar EndOfLine",
                            "Keyword out Identifier firstVar Seperator IntConstant 78 Seperator StringConstant \"lmao\" Seperator newline StringConstant mmm EndOfLine",
                            "Keyword loop Identifier firstVar Keyword times out \"*\""};
   
    // for(int i=0; i<7; i++)
    // {

    // }
    // for(int i=0; i<7; i++)
    // {
    //     // Splitting ritual and creating a readable command-set for the functions
    //     char *words[1000] = {'\0'};
    //     int wordCount = 0;
    //     char *splitedWords = strtok(command[i], " ");
    //     while (splitedWords != NULL) {
    //         words[wordCount++] = splitedWords;
    //         splitedWords = strtok(NULL, " ");
    //     }
    //     if(i >= 0 && i < 2){
    //         declaration(words);
    //         printf("CREATED VAR IS: %s\n",variables[i]);
    //         printf("CREATED VALUE IS: %d\n\n",values[i]);
    //     }
    //     if(i >= 2 && i < 4){
    //         move(words);
    //         printf("firstVar: %d\n",values[0]);
    //         printf("second: %d\n",values[1]);
    //     }
    //     if(i == 4)
    //     {
    //         sub(words);
    //         printf("firstVar: %d\n",values[0]);
    //         printf("second: %d\n",values[1]);
    //     }
    //     if(i == 5)
    //     {
    //         printf("\n");
    //         out(words);
    //     }
    //     if(i == 6)
    //     {
    //         printf("\n");
    //         loop(words);
    //     }
    // }

    printf("\n");
    return 0;
}
