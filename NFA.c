#include "NFA.h"

// Main Program Loop
int main(int argc, char **argv)
{
    // Ensure that all arguments are present
    if (argc < 2) 
    {
        printf("Usage: ./a1 NFA_FILENAME\n");
        exit(-1);
    }

    // Initialize a FiniteAutomaton struct
    FiniteAutomaton automaton;

    // Open the input file
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Failed to open input file");
        return 1;
    }

    // Parse the input format
    if (!parseAutomaton(input, &automaton)) {
        printf("Error: Failed to parse the input format.\n");
        // Close the input file in event of error
        fclose(input);
        return 1;
    }

    // Close the input file
    fclose(input);

    // Free all dynamically allocated memory in the automaton struct
    freeAutomaton(&automaton);

    return 0;
}

// Function to parse the input format and fill the FiniteAutomaton struct
int parseAutomaton(FILE* input, FiniteAutomaton* automaton) {
    char str[MAX_STRING_LEN];

    // Read alphabet size and alphabet list
    if (fscanf(input, "%d", &(automaton->alphabet_size)) != 1) return 0;
    printf("Alphabet size: %d\n", automaton->alphabet_size);
    printf("Alphabet: ");
    for (int i = 0; i < automaton->alphabet_size; i++) {
        if (fscanf(input, "%s", str) != 1) return 0;
        automaton->alphabet_list[i] = strdup(str);
        printf("%s ", automaton->alphabet_list[i]);
    }
    printf("\n");

    // Read number of states and state list
    if (fscanf(input, "%d", &(automaton->number_of_states)) != 1) return 0;
    printf("Number of states: %d\n", automaton->number_of_states);
    printf("States: ");
    for (int i = 0; i < automaton->number_of_states; i++) {
        if (fscanf(input, "%s", str) != 1) return 0;
        automaton->state_list[i] = strdup(str);
        printf("%s ", automaton->state_list[i]);
    }
    printf("\n");

    // Read starting state
    if (fscanf(input, "%s", str) != 1) return 0;
    automaton->starting_state = strdup(str);
    printf("Starting state: %s\n", automaton->starting_state);

    // Read accept state
    if (fscanf(input, "%s", str) != 1) return 0;
    automaton->accept_state = strdup(str);
    printf("Accept state: %s\n", automaton->accept_state);
    
    // Read number of inputs and input strings
    if (fscanf(input, "%d", &(automaton->num_inputs)) != 1) return 0;
    printf("Num inputs: %d\n", automaton->num_inputs);
    printf("Inputs: ");
    for (int i = 0; i < automaton->num_inputs; i++) {
        if (fscanf(input, "%s", str) != 1) return 0;
        automaton->input_string[i] = strdup(str);
        printf("%s ", automaton->input_string[i]);
    }
    printf("\n");

    // Read number of transitions
    if (fscanf(input, "%d", &(automaton->number_of_transitions)) != 1) return 0;
    printf("Number of transitions: %d\n", automaton->number_of_transitions);

    // Initialize transitions array structs
    automaton->transitions = malloc(automaton->number_of_transitions * sizeof(Transition));
    if (automaton->transitions == NULL) {
        perror("Failed to allocate memory for transitions");
        return 0;
    }

    // Read transitions
    printf("Transitions:\n");
    for (int i = 0; i < automaton->number_of_transitions; i++) {
        // Read from state, input symbol, and to state
        if (fscanf(input, "%s %s %s",
            automaton->transitions[i].from_state,
            &(automaton->transitions[i].input_symbol),
            automaton->transitions[i].to_state) != 3) return 0;
        printf("\t(%s, %c, %s)\n", automaton->transitions[i].from_state, automaton->transitions[i].input_symbol, automaton->transitions[i].to_state);
    }

    // Return 1 on success
    return 1; 
}

// Function to free the dynamic array of transitions
void freeTransitions(FiniteAutomaton* automaton) {
    free(automaton->transitions);
}

// Function to free dynamically allocated memory in the automaton struct
void freeAutomaton(FiniteAutomaton* automaton) {
    // Free alphabet
    for (int i =0; i < automaton->alphabet_size; i++) {
        free(automaton->alphabet_list[i]);
    }

    // Free states
    for (int i = 0; i < automaton->number_of_states; i++) {
        free(automaton->state_list[i]);
    }

    // Free input strings
    for (int i = 0; i < automaton->num_inputs; i++) {
        free(automaton->input_string[i]);
    }

    // Free starting and accept states
    free(automaton->starting_state);
    free(automaton->accept_state);

    // Free transitions array
    freeTransitions(automaton);
}


