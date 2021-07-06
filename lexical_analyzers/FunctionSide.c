#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

char *variables[1000];
int variableIndex = 0;
int values[1000] = { 0 };


void evaluate(char *statement) {
    char *tokens[1000] = {'\0'};
    int wordCount = 0;
    char *splitedWords = strtok(command[i], " ");
    while (splitedWords != NULL) {
        tokens[wordCount++] = splitedWords;
        splitedWords = strtok(NULL, " ");
    }

    if (!strcmp(tokens[0], "Keyword") && !strcmp(tokens[1],"int")) {
        if (!strcmp(tokens[2], "Identifier")) {
            if (int i; i < variableIndex; i++)
            if (!strcmp(tokens[4], "EndOfLine")) {
                
            }   
        }
    } else {
        printf("ERROR: Statement should start with a keyword!");
    }
}

// !!SPLIT FUNCTION IS NOT FULLY WORKING!!
// Split function
void split(char *token[], char *target[1000]){
    int wordCount = 0;
    char *splitedWords = strtok(token, " ");
    while (splitedWords != NULL) {
        target[wordCount++] = splitedWords;
        splitedWords = strtok(NULL, " ");
    }
}

// Getting the index of a variable by the name of it
int getIndex (char *name)
{
    if(variableIndex == 0)
        return -1;
    for(int i = 0; i < (variableIndex + 1); i++)
        if(!strcmp(name, variables[i]))
            return i;
    return -1;
}

// Declaration functionality
void declaration(char *token[])
{
    variables[variableIndex++] = token[3];
    values[variableIndex] = 0;
}

// Assignment functionality
void move(char *token[])
{
    if(!strcmp(token[2], "Identifier"))
            values[getIndex(token[7])] = values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[getIndex(token[7])] = atoi(token[3]);
}

// Addition functionality
void add(char *token[])
{
    if(!strcmp(token[2], "Identifier"))
            values[getIndex(token[7])] += values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[getIndex(token[7])] += atoi(token[3]);
}

// Subtraction functionality
void sub(char *token[])
{
    if(!strcmp(token[2], "Identifier"))
            values[getIndex(token[7])] -= values[getIndex(token[3])];

    if(!strcmp(token[2], "IntConstant"))
            values[getIndex(token[7])] -= atoi(token[3]);
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


int main()
{
    char command[7][1000] = {"Keyword int Identifier firstVar EndOfLine",
                            "Keyword int Identifier second EndOfLine",
                            "Keyword move IntConstant 5 Keyword to Identifier firstVar EndOfLine",
                            "Keyword move IntConstant 2 Keyword to Identifier second EndOfLine",
                            "Keyword sub IntConstant 1 Keyword from Identifier firstVar EndOfLine",
                            "Keyword out Identifier firstVar Seperator IntConstant 78 Seperator StringConstant \"lmao\" Seperator newline StringConstant mmm EndOfLine",
                            "Keyword loop Identifier firstVar Keyword times out \"*\""};
    for(int i=0; i<7; i++)
    {
        // Splitting ritual and creating a readable command-set for the functions
        char *words[1000] = {'\0'};
        int wordCount = 0;
        char *splitedWords = strtok(command[i], " ");
        while (splitedWords != NULL) {
            words[wordCount++] = splitedWords;
            splitedWords = strtok(NULL, " ");
        }
        if(i >= 0 && i < 2){
            declaration(words);
            printf("CREATED VAR IS: %s\n",variables[i]);
            printf("CREATED VALUE IS: %d\n\n",values[i]);
        }
        if(i >= 2 && i < 4){
            move(words);
            printf("firstVar: %d\n",values[0]);
            printf("second: %d\n",values[1]);
        }
        if(i == 4)
        {
            sub(words);
            printf("firstVar: %d\n",values[0]);
            printf("second: %d\n",values[1]);
        }
        if(i == 5)
        {
            printf("\n");
            out(words);
        }
        if(i == 6)
        {
            printf("\n");
            loop(words);
        }
    }

    printf("\n");
    return 0;
}
