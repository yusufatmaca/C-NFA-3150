#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALPHABET_SIZE  100
#define MAX_STATES         100
#define MAX_INPUTS         100   // Maximum number of input strings
#define MAX_STRING_LEN     11    // Maximum state name length + 1 for null-terminator

// Define a struct for a single transition
typedef struct Transition {
    char from_state[MAX_STRING_LEN];
    char input_symbol;
    char to_state[MAX_STRING_LEN];
} Transition;

// Define a struct to represent the finite automaton
typedef struct {
    int alphabet_size;
    char* alphabet_list[MAX_ALPHABET_SIZE]; // Each string can be up to 10 characters long
    int number_of_states;
    char* state_list[MAX_STATES];
    char* starting_state;
    char* accept_state;
    int num_inputs;
    char* input_string[MAX_INPUTS]; // Up to 100 input strings
    int number_of_transitions;
    Transition* transitions; // Dynamic array of transitions
} FiniteAutomaton;

// Function to free the dynamic array of transitions
void freeTransitions(FiniteAutomaton* automaton);

// Function to parse the input format and fill the FiniteAutomaton struct
int parseAutomaton(FILE* input, FiniteAutomaton* automaton);

// Function to free dynamically allocated memory in the automaton struct
void freeAutomaton(FiniteAutomaton* automaton);