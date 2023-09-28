/*
Name: Kyle Lukaszek
Student ID: 1113798
Class: CIS*3150
Assignment 1
Due Date: September 29 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALPHABET_SIZE  100   // Maximum number of characters in the alphabet + 1 for null-terminator
#define MAX_STATES         100
#define MAX_INPUTS         100   // Maximum number of input strings
#define MAX_STRING_LEN     11    // Maximum state name length + 1 for null-terminator

// Define a struct for a single transition
typedef struct {
    char from_state[MAX_STRING_LEN];
    char input_string[MAX_STRING_LEN];
    char to_state[MAX_STRING_LEN];
} Transition;

// Define a struct to represent the finite automaton
typedef struct {
    int alphabet_size;
    char* alphabet_list[MAX_ALPHABET_SIZE];
    int num_states;
    char* state_list[MAX_STATES];
    char* starting_state;
    char* accept_state;
    int num_inputs;
    char* input_strings[MAX_INPUTS]; // Up to 100 input strings
    int number_of_transitions;
    Transition* transitions; // Dynamic array of transitions
} FiniteAutomaton;

// Function to parse the input format and fill the FiniteAutomaton struct
int parseAutomaton(FILE* input, FiniteAutomaton* automaton);

// Parse transition from input format and fill the Transition struct in the FiniteAutomaton struct
int parseTransitions(FILE* input, FiniteAutomaton* automaton);

// Function to traverse the input string and return 0 or 1 depending on whether there is a final accept state active
int traverseAutomaton(FiniteAutomaton* automaton);

// Function to determine array of next states
void determineNextStates(FiniteAutomaton *automaton, char *input, int (*active_states)[automaton->num_states], int (*next_active_states)[automaton->num_states]);

// Function to handle epsilon transitions
void handleEpsilon(FiniteAutomaton *automaton, char *state, int (*next_active_states)[automaton->num_states]);

// Function to free dynamically allocated memory in the automaton struct
void freeAutomaton(FiniteAutomaton* automaton);

// Helper function to determine if pointer exist in array
int pointerInArray(char **array, char *pointer, int size);