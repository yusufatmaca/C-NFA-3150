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

    int result = traverseAutomaton(&automaton);

    if (result == 0)
    {
        printf("reject\n");
    }
    else if (result == 1)
    {
        printf("accept\n");
    }
    else
    {
        printf("Error: Failed to traverse the automaton.\n");

        // Free all dynamically allocated memory in the automaton struct
        freeAutomaton(&automaton);

        return 1;
    }

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
            automaton->transitions[i].input_string,
            automaton->transitions[i].to_state) != 3) return 0;
        printf("\t(%s, %s, %s)\n", automaton->transitions[i].from_state, automaton->transitions[i].input_string, automaton->transitions[i].to_state);
    }
    printf("\n");

    // Return 1 on success
    return 1; 
}

// Function to traverse the input string and return 0 or 1 depending on whether there is a final accept state active
int traverseAutomaton(FiniteAutomaton* automaton) {

    int num_inputs = automaton->num_inputs;
    int active_states[automaton->number_of_states];
    int next_active_states[automaton->number_of_states];

    // Initialize active states array and next active states array
    for (int i = 0; i < automaton->number_of_states; i++)
    {
        active_states[i] = 0;
        next_active_states[i] = 0;
    }

    // Set the starting state to active
    for (int i = 0; i < automaton->number_of_states; i++)
    {
        if (strcmp(automaton->state_list[i], automaton->starting_state) == 0)
        {
            active_states[i] = 1;
            break;
        }
    }

    // Iterate through each input string
    for (int i = 0; i < num_inputs; i++)
    {
        char* input = automaton->input_string[i];

        /* 
        Iterate through the transitions and keep track of active states in the active_states array
        by copying the results of the next_active_states array to the active_states array after each iteration
        */

        determineNextStates(automaton, input, &active_states, &next_active_states);
    
        // Copy the next_active_states array to the active_states array
        for (int i = 0; i < automaton->number_of_states; i++)
        {
            active_states[i] = next_active_states[i];
            next_active_states[i] = 0;
        }

        // Print the input string
        printf("%s  ", input);
        
        for (int i = 0; i < automaton->number_of_states; i++)
        {
            printf("%d ", active_states[i]);
        }
        printf("\n");
    }

    // Check if the accept state is active, return 1 if it is
    for (int i = 0; i < automaton->number_of_states; i++)
    {
        if (strcmp(automaton->state_list[i], automaton->accept_state) == 0 && active_states[i] == 1)
        {
            return 1;
        }
    }

    // Return 0 if the accept state is not active
    return 0;
}

// Function to determine array of next states
void determineNextStates(FiniteAutomaton *automaton, char *input, int (*active_states)[automaton->number_of_states], int (*next_active_states)[automaton->number_of_states])
{
    // Iterate through each transition to update the next_active_states array
    for (int i = 0; i < automaton->number_of_transitions; i++)
    {
        Transition* transition = &(automaton->transitions[i]);

        // Iterate through the active state list
        for (int j = 0; j < automaton->number_of_states; j++)
        {

            // Make sure the current state is active, else we go to the next one
            if ((*active_states)[j] == 1)
            {

                // Check if the current transition "from_state" matches the current state we are on, and check if the input string matches the required transition input string
                if (strcmp(transition->from_state, automaton->state_list[j]) == 0 && strcmp(transition->input_string, input) == 0)
                {

                    // We have to iterate through the state name list again to find the index of the next state
                    for (int k = 0; k < automaton->number_of_states; k++)
                    {

                        // Set the next state to active based on the string in the to_state field of the transition
                        if (strcmp(transition->to_state, automaton->state_list[k]) == 0)
                        {
                            (*next_active_states)[k] = 1;

                            char *state = automaton->state_list[k];

                            // If the state we set to active has an epsilon transition, we handle it here
                            handleEpsilon(automaton, state, next_active_states);
                        }
                    }
                }
            }
        }
    }
}

// Function to handle epsilon transitions
void handleEpsilon(FiniteAutomaton *automaton, char *state, int (*next_active_states)[automaton->number_of_states])
{
    int num_transitions = automaton->number_of_transitions;

    // Iterate through all transitions to find epsilon transitions
    for (int i = 0; i < num_transitions; i++)
    {
        Transition *epsilon_transition = &(automaton->transitions[i]);

        // Check if the transition is an "e" epsilon transition, and check if the transition "from state" matches the passed "state" string
        if (strcmp(epsilon_transition->input_string, "e") == 0 && strcmp(epsilon_transition->from_state, state) == 0)
        {

            // Iterate through the state list to find the index of the next state
            for (int j = 0; j < automaton->number_of_states; j++)
            {
                if (strcmp(epsilon_transition->to_state, automaton->state_list[j]) == 0)
                {
                    // Set the next state to active if the current state has an epsilon transition
                    (*next_active_states)[j] = 1;
                }
            }
        }
    }
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


