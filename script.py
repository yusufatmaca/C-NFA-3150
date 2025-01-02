import random

def generate_nfa_transitions(num_transitions, output_file):
    states = [f"s{i}" for i in range(1, 28)]  # States: s1 to s27
    inputs = [chr(i) for i in range(ord('a'), ord('z') + 1)] + ['e']  # Inputs: a to z and epsilon

    transitions = []

    for _ in range(num_transitions):
        start_state = random.choice(states)
        input_symbol = random.choice(inputs)
        end_state = random.choice(states)
        transitions.append(f"{start_state} {input_symbol} {end_state}")

    with open(output_file, "w") as file:
        file.write("\n".join(transitions))

if __name__ == "__main__":
    num_transitions = int(input("Enter the number of transitions: "))
    output_file = "transitions.txt"
    generate_nfa_transitions(num_transitions, output_file)
    print(f"Generated transitions written to {output_file}")
