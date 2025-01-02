/*
Name: Kyle Lukaszek
Student ID: 1113798
Class: CIS*3150
Assignment 1
Due Date: September 29 2023
*/

#include "NFA.h"
#include <omp.h>

// Main Program Loop
int main(int argc, char **argv)
{
    double start_time = omp_get_wtime();
    // Ensure that all arguments are present
    if (argc < 2)
    {
        printf("Usage: ./a1 NFA_FILENAME\n");
        exit(-1);
    }

    // Initialize a FiniteAutomaton struct
    FiniteAutomaton automaton;

    // Open the input file
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        perror("Failed to open input file");
        return 1;
    }

    // Parse the input format
    if (!parseAutomaton(input, &automaton))
    {
        printf("Error: Failed to parse the input format.\n");
        freeAutomaton(&automaton);
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

        // Free all dynamically allocated memory in the automaton struct on error
        freeAutomaton(&automaton);

        return 1;
    }

    // Free all dynamically allocated memory in the automaton struct
    freeAutomaton(&automaton);

    double end_time = omp_get_wtime();
    printf("Execution time: %f seconds\n", end_time - start_time);
    return 0;
}

// Function to parse the input format and fill the FiniteAutomaton struct
int parseAutomaton(FILE *input, FiniteAutomaton *automaton)
{
    // Initialize a temporary string to read into
    char str[MAX_STRING_LEN];

    // Initialize the automaton struct defaults
    automaton->alphabet_size = 0;
    automaton->num_states = 0;
    automaton->starting_state = NULL;
    automaton->accept_state = NULL;
    automaton->num_inputs = 0;
    automaton->number_of_transitions = 0;
    automaton->transitions = NULL;

    // Read alphabet size
    if (fscanf(input, "%d", &(automaton->alphabet_size)) != 1)
        return 0;
    printf("Alphabet size: %d\n", automaton->alphabet_size);

    // Initialize alphabet list to NULL
    for (int i = 0; i < automaton->alphabet_size; i++)
    {
        automaton->alphabet_list[i] = NULL;
    }

    // Read alphabet into character array
    printf("Alphabet: ");
    for (int i = 0; i < automaton->alphabet_size; i++)
    {
        if (fscanf(input, "%s", str) != 1)
            return 0;

        // Make sure alphabet string is 10 characters or less as per assignment requirements
        if (strlen(str) > 10)
        {
            printf("Error: Alphabet string \"%s\" is too long.\n", str);
            return 0;
        }

        // Add null terminator to the end of the string
        str[strlen(str)] = '\0';

        // Duplicate the string and add it to the alphabet list
        automaton->alphabet_list[i] = strdup(str);

        printf("%s ", automaton->alphabet_list[i]);
    }
    // Add null terminator to the end of the alphabet list
    automaton->alphabet_list[automaton->alphabet_size + 1] = '\0';
    printf("\n");

    // Read number of states
    if (fscanf(input, "%d", &(automaton->num_states)) != 1)
        return 0;
    printf("Number of states: %d\n", automaton->num_states);

    // Initialize state list to NULL
    for (int i = 0; i < automaton->num_states; i++)
    {
        automaton->state_list[i] = NULL;
    }

    // Read states into state list
    printf("States: ");
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (fscanf(input, "%s", str) != 1)
            return 0;

        // Make sure state name is 10 characters or less as per assignment requirements
        if (strlen(str) > 10)
        {
            printf("Error: State name \"%s\" is too long.\n", str);
            return 0;
        }

        str[strlen(str)] = '\0';

        // Duplicate the state name and add it to the state list
        automaton->state_list[i] = strdup(str);

        printf("%s ", automaton->state_list[i]);
    }
    printf("\n");

    // Read starting state
    if (fscanf(input, "%s", str) != 1)
        return 0;
    automaton->starting_state = strdup(str);
    printf("Starting state: %s\n", automaton->starting_state);

    // Check if starting state is in the state list
    int starting_state_found = 0;
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (strcmp(automaton->starting_state, automaton->state_list[i]) == 0)
        {
            starting_state_found = 1;
            break;
        }
    }

    // Print error if starting state is not in the state list
    if (!starting_state_found)
    {
        printf("Error: Starting state \"%s\" is not in the state list.\n", automaton->starting_state);
        return 0;
    }

    // Read accept state
    if (fscanf(input, "%s", str) != 1)
        return 0;
    automaton->accept_state = strdup(str);
    printf("Accept state: %s\n", automaton->accept_state);

    // Check if accept state is in the state list
    int accept_state_found = 0;
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (strcmp(automaton->accept_state, automaton->state_list[i]) == 0)
        {
            accept_state_found = 1;
            break;
        }
    }

    // Print error if accept state is not in the state list
    if (!accept_state_found)
    {
        printf("Error: Accept state \"%s\" is not in the state list.\n", automaton->accept_state);
        return 0;
    }

    // Read number of inputs
    if (fscanf(input, "%d", &(automaton->num_inputs)) != 1)
        return 0;
    printf("Num inputs: %d\n", automaton->num_inputs);

    // Make sure number of inputs is less than or equal to MAX_INPUTS
    if (automaton->num_inputs > MAX_INPUTS)
    {
        printf("Error: Number of inputs exceeds maximum of %d.\n", MAX_INPUTS);
        return 0;
    }

    // Initialize input strings to NULL
    for (int i = 0; i < automaton->num_inputs; i++)
    {
        automaton->input_strings[i] = NULL;
    }

    // Read input strings into input strings array
    printf("Inputs: ");
    for (int i = 0; i < automaton->num_inputs; i++)
    {
        if (fscanf(input, "%s", str) != 1)
            return 0;

        // Make sure input string is 10 characters or less as per assignment requirements
        if (strlen(str) > 10)
        {
            printf("Error: Input string \"%s\" is too long.\n", str);
            return 0;
        }

        // Add null terminator to the end of the input string
        str[strlen(str)] = '\0';

        // Check if input string is "e", we don't need to check if it is in the alphabet
        if (!strcmp(str, "e") == 0)
        {
            // Used to count the number of characters present in the alphabet from the input string
            int count = 0;

            // Iterate through dynamic alphabet array to check if the input string is a subset of the alphabet
            int input_string_found = 0;

            // Check if the input string is in the alphabet
            input_string_found = pointerInArray(automaton->alphabet_list, str, automaton->alphabet_size);

            // If the input string is not a string in the alphabet, we check if it is a subset of characters from the alphabet
            if (!input_string_found)
            {
                for (int j = 0; j < automaton->alphabet_size; j++)
                {
                    // Check if the current alphabet string is a single character
                    if (strlen(automaton->alphabet_list[j]) == 1)
                    {
                        // Check if the current character alphabet character exists in the input string, increment count if it is
                        if (strchr(str, automaton->alphabet_list[j][0]))
                        {
                            count++;
                        }
                    }
                }

                // If the current character index is equal to the length of the input string, we know that the input string is a subset of the alphabet
                if (count == strlen(str))
                {
                    input_string_found = 1;
                }
            }

            // Print error if input string is not in the alphabet
            if (!input_string_found)
            {
                printf("Error: Input string \"%s\" is not in the alphabet.\n", str);
                return 0;
            }
        }

        // Duplicate the input string and add it to the input strings array
        automaton->input_strings[i] = strdup(str);

        printf("%s ", automaton->input_strings[i]);
    }
    printf("\n");

    // Read number of transitions
    if (fscanf(input, "%d", &(automaton->number_of_transitions)) != 1)
        return 0;
    printf("Number of transitions: %d\n", automaton->number_of_transitions);

    // Initialize transitions array structs
    automaton->transitions = malloc(automaton->number_of_transitions * sizeof(Transition));
    if (automaton->transitions == NULL)
    {
        printf("Failed to allocate memory for transitions");
        return 0;
    }

    int result = parseTransitions(input, automaton);

    if (!result)
    {
        printf("Error: Failed to parse transitions.\n");
        return 0;
    }

    printf("\n");

    // Return 1 on success
    return 1;
}

// Parse transition from input format and fill the Transition struct in the FiniteAutomaton struct
// This function is called from parseAutomaton()
int parseTransitions(FILE *input, FiniteAutomaton *automaton)
{
    // Read transitions
    printf("Transitions:\n");
    for (int i = 0; i < automaton->number_of_transitions; i++)
    {

        char from_state[MAX_STRING_LEN];
        char input_string[MAX_STRING_LEN];
        char to_state[MAX_STRING_LEN];

        // Read from state, input symbol, and to state
        if (fscanf(input, "%s %s %s", from_state, input_string, to_state) != 3)
            return 0;

        // Make sure from state, input string, and to state are 10 characters or less as per assignment requirements
        if (strlen(from_state) > 10)
        {
            printf("Error: Transition %d from state \"%s\" is too long.\n", i, from_state);
            return 0;
        }

        if (strlen(input_string) > 10)
        {
            printf("Error: Transition %d input string \"%s\" is too long.\n", i, input_string);
            return 0;
        }

        if (strlen(to_state) > 10)
        {
            printf("Error: Transition %d to state \"%s\" is too long.\n", i, to_state);
            return 0;
        }

        // Add null terminators to the end of the strings
        from_state[strlen(from_state)] = '\0';
        input_string[strlen(input_string)] = '\0';
        to_state[strlen(to_state)] = '\0';

        // Make sure from_state is in the state list
        int from_state_found = 0;
        for (int j = 0; j < automaton->num_states; j++)
        {
            if (strcmp(from_state, automaton->state_list[j]) == 0)
            {
                from_state_found = 1;
            }
        }

        // Print error if from_state is not in the state list
        if (!from_state_found)
        {
            printf("\nError: Transition %d (%s, %s, %s), from_state \"%s\" is not in the state list.\n", i, from_state, input_string, to_state, from_state);
            return 0;
        }

        // Make sure to_state is in the state list
        int to_state_found = 0;
        for (int j = 0; j < automaton->num_states; j++)
        {
            if (strcmp(to_state, automaton->state_list[j]) == 0)
            {
                to_state_found = 1;
            }
        }

        // Print error if to_state is not in the state list
        if (!to_state_found)
        {
            printf("\nError: Transition %d (%s, %s, %s), to_state \"%s\" is not in the state list.\n", i, from_state, input_string, to_state, to_state);
            return 0;
        }

        // If input_string is "e", we don't need to check if it is in the alphabet
        if (!strcmp(input_string, "e") == 0)
        {
            // Used to track the number of characters present in the alphabet from the input string
            int count = 0;

            // Iterate through dynamic alphabet array to check if the input string is a subset of the alphabet
            int input_string_found = 0;

            // Check if the input string is in the alphabet
            input_string_found = pointerInArray(automaton->alphabet_list, input_string, automaton->alphabet_size);

            // If the input string is not a string in the alphabet, we check if it is a subset of characters from the alphabet
            if (!input_string_found)
            {
                for (int j = 0; j < automaton->alphabet_size; j++)
                {
                    // Check if the current alphabet string is a single character
                    if (strlen(automaton->alphabet_list[j]) == 1)
                    {
                        // Check if the current character alphabet character exists in the input string, increment count if it is
                        if (strchr(input_string, automaton->alphabet_list[j][0]))
                        {
                            count++;
                        }
                    }
                }

                // If the current character index is equal to the length of the input string, we know that the input string is a subset of the alphabet
                if (count == strlen(input_string))
                    input_string_found = 1;
            }

            // Print error if input string is not in the alphabet
            if (!input_string_found)
            {
                printf("\nError: Transition %d (%s, %s, %s), input string \"%s\" is not in the alphabet.\n", i, from_state, input_string, to_state, input_string);
                return 0;
            }
        }

        // Copy the strings to the transitions struct array
        strcpy(automaton->transitions[i].from_state, from_state);
        strcpy(automaton->transitions[i].input_string, input_string);
        strcpy(automaton->transitions[i].to_state, to_state);

        printf("\t(%s, %s, %s)\n", automaton->transitions[i].from_state, automaton->transitions[i].input_string, automaton->transitions[i].to_state);
    }

    return 1;
}

int getStateIndex(const FiniteAutomaton *automaton, const char *state)
{
    int found_index = -1;
    #pragma omp parallel for shared(found_index)
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (strcmp(automaton->state_list[i], state) == 0)
        {
                if (found_index == -1)
                { // Ensure only one thread sets the value
                    found_index = i;
                }
            
        }
    }
    return found_index;
}

// Function to traverse the input string and return 0 or 1 depending on whether there is a final accept state active
int traverseAutomaton(FiniteAutomaton *automaton)
{
    int num_inputs = automaton->num_inputs;
    int active_states[automaton->num_states];
    int next_active_states[automaton->num_states];

    #pragma omp parallel for schedule(static) // chuck size = total iter / num threads
    // Initialize active states array and next active states array
    for (int i = 0; i < automaton->num_states; i++)
    {
        active_states[i] = 0;
        next_active_states[i] = 0;
    }

    int start_index = getStateIndex(automaton, automaton->starting_state);
    if (start_index != -1)
    {
        active_states[start_index] = 1;
        handleEpsilon(automaton, automaton->state_list[start_index], &active_states);
    }

    // Iterate through each input string
    for (int i = 0; i < num_inputs; i++)
    {
        char *input = automaton->input_strings[i];

        /*
        Iterate through the transitions and keep track of active states in the active_states array
        by copying the results of the next_active_states array to the active_states array after each iteration
        */

        determineNextStates(automaton, input, &active_states, &next_active_states);

        // Copy the next_active_states array to the active_states array
        for (int i = 0; i < automaton->num_states; i++)
        {
            active_states[i] = next_active_states[i];
            next_active_states[i] = 0;
        }

        // Print the input string
        printf("%s  ", input);

        for (int i = 0; i < automaton->num_states; i++)
        {
            printf("%d ", active_states[i]);
        }
        printf("\n");
    }

    int found2 = 0;
    #pragma omp parallel for shared(found2)
    // Check if the accept state is active, return 1 if it is
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (strcmp(automaton->state_list[i], automaton->accept_state) == 0 && active_states[i] == 1)
        {
            found2 = 1;
        }
    }

    // Return 0 if the accept state is not active
    return found2;
}

// Function to determine array of next states
void determineNextStates(FiniteAutomaton *automaton, char *input, int (*active_states)[automaton->num_states], int (*next_active_states)[automaton->num_states])
{
#pragma omp parallel for
    // Iterate through each transition to update the next_active_states array
    for (int i = 0; i < automaton->number_of_transitions; i++)
    {
        Transition *transition = &(automaton->transitions[i]);

        // Iterate through the active state list
        for (int j = 0; j < automaton->num_states; j++)
        {

            // Make sure the current state is active, else we go to the next one
            if ((*active_states)[j] == 1)
            {

                // Check if the current transition "from_state" matches the current state we are on, and check if the input string matches the required transition input string
                if (strcmp(transition->from_state, automaton->state_list[j]) == 0 && strcmp(transition->input_string, input) == 0)
                {

                    // We have to iterate through the state name list again to find the index of the next state
                    for (int k = 0; k < automaton->num_states; k++)
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
void handleEpsilon(FiniteAutomaton *automaton, char *state, int (*next_active_states)[automaton->num_states])
{
    int num_transitions = automaton->number_of_transitions;

    #pragma omp parallel for
    // Iterate through all transitions to find epsilon transitions
    for (int i = 0; i < num_transitions; i++)
    {
        Transition *epsilon_transition = &(automaton->transitions[i]);

        // Check if the transition is an "e" epsilon transition, and check if the transition "from state" matches the passed "state" string
        if (strcmp(epsilon_transition->input_string, "e") == 0 && strcmp(epsilon_transition->from_state, state) == 0)
        {
            // Iterate through the state list to find the index of the next state
            for (int j = 0; j < automaton->num_states; j++)
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

// Function to free dynamically allocated memory in the automaton struct
void freeAutomaton(FiniteAutomaton *automaton)
{
    // Free alphabet
    for (int i = 0; i < automaton->alphabet_size; i++)
    {
        if (automaton->alphabet_list[i] != NULL)
            free(automaton->alphabet_list[i]);
    }

    // Free states
    for (int i = 0; i < automaton->num_states; i++)
    {
        if (automaton->state_list[i] != NULL)
            free(automaton->state_list[i]);
    }

    // Free input strings
    for (int i = 0; i < automaton->num_inputs; i++)
    {
        if (automaton->input_strings[i] != NULL)
            free(automaton->input_strings[i]);
    }

    // Free starting and accept states
    if (automaton->starting_state != NULL)
        free(automaton->starting_state);
    if (automaton->accept_state != NULL)
        free(automaton->accept_state);

    // Free transitions dynamic array
    if (automaton->transitions != NULL)
        free(automaton->transitions);
}

// Helper function to determine if pointer exist in array, return true or false
int pointerInArray(char **array, char *pointer, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(array[i], pointer) == 0)
        {
            return 1;
        }
    }

    return 0;
}