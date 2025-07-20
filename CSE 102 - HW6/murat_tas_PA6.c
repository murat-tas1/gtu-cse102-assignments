#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define gate types with integer constants
#define INPUT 0
#define OUTPUT 1
#define AND 2
#define OR 3
#define NOT 4
#define FLIPFLOP 5

// Forward declaration of the gate structure
struct gate;

// Structure to represent dynamic input connections
struct gate_input {
    struct gate *connected_gate;    // Pointer to the connected gate
    struct gate_input *next;        // Pointer to the next input connection
};

// Structure to represent a logic gate
struct gate {
    int type;    // Type of the gate (INPUT, OUTPUT, AND, etc)
    char name[20];    // Name identifier for the gate
    struct gate_input *inputs;    // Linked list of input connections
    int output;    // Current output value of the gate
    int former_out;    // Previous output value (used for FLIPFLOP)
    short evaluated;    // Flag to indicate if the gate has been evaluated
    // Pointer to the characteristic function based on gate type
    int (*characteristic_function)(int *, int, int *);
};

// Characteristic function for AND gate
int and_function(int *inputs, int count, int *former_out) {
    for (int i = 0; i < count; i++) {
        if (inputs[i] == 0) {
            return 0;    // If any input is 0, output is 0
        }
    }
    return 1;    // All inputs are 1, output is 1
}

// Characteristic function for OR gate
int or_function(int *inputs, int count, int *former_out) {
    for (int i = 0; i < count; i++) {
        if (inputs[i] == 1) {
            return 1;    // If any input is 1, output is 1
        }
    }
    return 0;    // All inputs are 0, output is 0
}

// Characteristic function for NOT gate
int not_function(int *inputs, int count, int *former_out) {
    if (inputs[0] == 0) {
        return 1;    // Invert input
    } else {
        return 0;
    }
}

// Characteristic function for FLIPFLOP gate
int flipflop_function(int *inputs, int count, int *former_out) {
    int result = inputs[0] ^ (*former_out);    // Toggle output based on input and former output
    *former_out = result;    // Update former output
    return result;
}

// Function to reset evaluation flags for all gates
void reset_gates(struct gate **input_gates, int input_count,
                 struct gate **output_gates, int output_count,
                 struct gate **other_gates, int other_count) {
    for (int i = 0; i < input_count; i++) {
        input_gates[i]->evaluated = 0;    // Reset INPUT gates
    }
    for (int i = 0; i < output_count; i++) {
        output_gates[i]->evaluated = 0;    // Reset OUTPUT gates
    }
    for (int i = 0; i < other_count; i++) {
        other_gates[i]->evaluated = 0;    // Reset other gates
    }
}

// Function to create a new gate and add it to the appropriate gate list
struct gate *create_gate(char *type_str, char *name,
                         struct gate ***input_gates, int *input_size, int *input_count,
                         struct gate ***output_gates, int *output_size, int *output_count,
                         struct gate ***other_gates, int *other_size, int *other_count) {
    // Allocate memory for the new gate
    struct gate *new_gate = (struct gate *)malloc(sizeof(struct gate));
    
    // Set the gate type based on the type string
    if (strcmp(type_str, "INPUT") == 0) {
        new_gate->type = INPUT;
    } else if (strcmp(type_str, "OUTPUT") == 0) {
        new_gate->type = OUTPUT;
    } else if (strcmp(type_str, "AND") == 0) {
        new_gate->type = AND;
    } else if (strcmp(type_str, "OR") == 0) {
        new_gate->type = OR;
    } else if (strcmp(type_str, "NOT") == 0) {
        new_gate->type = NOT;
    } else if (strcmp(type_str, "FLIPFLOP") == 0) {
        new_gate->type = FLIPFLOP;
    } else {
        new_gate->type = -1;    // Invalid type
    }

    // Initialize gate properties
    strcpy(new_gate->name, name);
    new_gate->output = 0;
    new_gate->former_out = 0;
    new_gate->evaluated = 0;
    new_gate->inputs = NULL;

    // Assign the appropriate characteristic function
    if (new_gate->type == AND) {
        new_gate->characteristic_function = and_function;
    } else if (new_gate->type == OR) {
        new_gate->characteristic_function = or_function;
    } else if (new_gate->type == NOT) {
        new_gate->characteristic_function = not_function;
    } else if (new_gate->type == FLIPFLOP) {
        new_gate->characteristic_function = flipflop_function;
    } else {
        new_gate->characteristic_function = NULL;    // No function for INPUT/OUTPUT
    }

    // Add the new gate to the corresponding gate list
    if (new_gate->type == INPUT) {
        // Resize the input gates array if necessary
        if (*input_count >= *input_size) {
            *input_size *= 2;
            *input_gates = (struct gate **)realloc(*input_gates, (*input_size) * sizeof(struct gate *));
        }
        (*input_gates)[(*input_count)++] = new_gate;    // Add to INPUT gates
    } else if (new_gate->type == OUTPUT) {
        // Resize the output gates array if necessary
        if (*output_count >= *output_size) {
            *output_size *= 2;
            *output_gates = (struct gate **)realloc(*output_gates, (*output_size) * sizeof(struct gate *));
        }
        (*output_gates)[(*output_count)++] = new_gate;    // Add to OUTPUT gates
    } else {
        // Resize the other gates array if necessary
        if (*other_count >= *other_size) {
            *other_size *= 2;
            *other_gates = (struct gate **)realloc(*other_gates, (*other_size) * sizeof(struct gate *));
        }
        (*other_gates)[(*other_count)++] = new_gate;    // Add to other gate types
    }

    return new_gate;    // Return the newly created gate
}

// Function to evaluate a gate's output recursively
int evaluate_gate(struct gate *g) {
    if (g->evaluated) {
        return g->output;    // Return cached output if already evaluated
    }

    if (g->type == INPUT) {
        g->evaluated = 1;    // Mark INPUT gate as evaluated
        return g->output;    // Return its output value
    }

    if (g->type == OUTPUT) {
        if (g->inputs) {
            g->output = evaluate_gate(g->inputs->connected_gate);    // OUTPUT gate takes input from its connected gate
        } else {
            g->output = 0;    // Default output if no connections
        }
        g->evaluated = 1;    // Mark as evaluated
        return g->output;
    }

    // Count the number of inputs connected to the gate
    int input_count = 0;
    struct gate_input *current_input = g->inputs;
    while (current_input) {
        input_count++;
        current_input = current_input->next;
    }

    // Allocate array to store input values
    int *input_values = (int *)malloc(input_count * sizeof(int));
    current_input = g->inputs;
    for (int i = 0; i < input_count; i++) {
        input_values[i] = evaluate_gate(current_input->connected_gate);    // Recursively evaluate each input gate
        current_input = current_input->next;
    }

    // Compute the gate's output using its characteristic function
    g->output = g->characteristic_function(input_values, input_count, &g->former_out);
    free(input_values);    // Free the allocated input values array
    g->evaluated = 1;    // Mark the gate as evaluated
    return g->output;    // Return the computed output
}

// Function to create a connection between two gates
void create_connection(char *from_name, char *to_name,
                       struct gate **input_gates, int input_count,
                       struct gate **output_gates, int output_count,
                       struct gate **other_gates, int other_count) {
    struct gate *to_gate = NULL, *from_gate = NULL;

    // Find the source gate by name from input gates
    for (int i = 0; i < input_count && !from_gate; i++) {
        if (strcmp(input_gates[i]->name, from_name) == 0) {
            from_gate = input_gates[i];
        }
    }
    // If not found, search in output gates
    for (int i = 0; i < output_count && !from_gate; i++) {
        if (strcmp(output_gates[i]->name, from_name) == 0) {
            from_gate = output_gates[i];
        }
    }
    // If still not found, search in other gates
    for (int i = 0; i < other_count && !from_gate; i++) {
        if (strcmp(other_gates[i]->name, from_name) == 0) {
            from_gate = other_gates[i];
        }
    }

    // Find the destination gate by name from output gates
    for (int i = 0; i < output_count && !to_gate; i++) {
        if (strcmp(output_gates[i]->name, to_name) == 0) {
            to_gate = output_gates[i];
        }
    }
    // If not found, search in other gates
    for (int i = 0; i < other_count && !to_gate; i++) {
        if (strcmp(other_gates[i]->name, to_name) == 0) {
            to_gate = other_gates[i];
        }
    }

    // If either gate is not found, exit the function
    if (!from_gate || !to_gate) {
        return;
    }

    // Create a new input connection and add it to the destination gate's inputs
    struct gate_input *new_input = (struct gate_input *)malloc(sizeof(struct gate_input));
    new_input->connected_gate = from_gate;    // Connect from_gate to to_gate
    new_input->next = to_gate->inputs;        // Insert at the beginning of the inputs list
    to_gate->inputs = new_input;              // Update the inputs pointer
}

// Function to free all allocated memory for gates
void free_gates(struct gate **gates, int count) {
    for (int i = 0; i < count; i++) {
        struct gate_input *current = gates[i]->inputs;
        // Free all input connections
        while (current) {
            struct gate_input *temp = current->next;
            free(current);
            current = temp;
        }
        free(gates[i]);    // Free the gate itself
    }
}

int main() {
    // Initial sizes for gate arrays
    int input_size = 10, output_size = 10, other_size = 10;
    int input_count = 0, output_count = 0, other_count = 0;

    // Allocate memory for gate lists
    struct gate **input_gates = (struct gate **)malloc(input_size * sizeof(struct gate *));
    struct gate **output_gates = (struct gate **)malloc(output_size * sizeof(struct gate *));
    struct gate **other_gates = (struct gate **)malloc(other_size * sizeof(struct gate *));

    // Open the circuit description file
    FILE *circuit_file = fopen("circuit.txt", "r");
    if (!circuit_file) {
        printf("Failed to open 'circuit.txt'.\n");
        free(input_gates);
        free(output_gates);
        free(other_gates);
        return 1;
    }

    char line[256], keyword[20], param1[20], param2[20];
    // Read and parse each line of the circuit file
    while (fgets(line, sizeof(line), circuit_file)) {
        sscanf(line, "%s %s %s", keyword, param1, param2);    // Extract keyword and parameters
        if (strcmp(keyword, "GATE") == 0) {
            // Create a new gate
            create_gate(param1, param2, &input_gates, &input_size, &input_count,
                        &output_gates, &output_size, &output_count,
                        &other_gates, &other_size, &other_count);
        } else if (strcmp(keyword, "CONNECTION") == 0) {
            // Create a connection between gates
            create_connection(param1, param2, input_gates, input_count, output_gates, output_count, other_gates, other_count);
        }
    }
    fclose(circuit_file);    // Close the circuit file

    // Open the input values file
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file) {
        printf("Failed to open 'input.txt'.\n");
        // Free all allocated gates before exiting
        free_gates(input_gates, input_count);
        free_gates(output_gates, output_count);
        free_gates(other_gates, other_count);
        free(input_gates);
        free(output_gates);
        free(other_gates);
        return 1;
    }

    // Process each line of input values
    while (fgets(line, sizeof(line), input_file)) {
        size_t len = strlen(line);
        // Remove newline character if present
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        // Reset evaluation flags before processing new input
        reset_gates(input_gates, input_count, output_gates, output_count, other_gates, other_count);

        // Set the output values for INPUT gates based on the input line
        for (int i = 0; i < input_count; i++) {
            if (i < len && line[i] == '1') {
                input_gates[i]->output = 1;
            } else {
                input_gates[i]->output = 0;
            }
        }

        // Evaluate and print the outputs for all OUTPUT gates
        for (int i = 0; i < output_count; i++) {
            int output_value = evaluate_gate(output_gates[i]);
            printf("%d", output_value);
        }
        printf("\n");    // Newline after each input line's output
    }
    fclose(input_file);    // Close the input file

    // Free all allocated memory for gates
    free_gates(input_gates, input_count);
    free_gates(output_gates, output_count);
    free_gates(other_gates, other_count);

    // Free the gate lists themselves
    free(input_gates);
    free(output_gates);
    free(other_gates);

    return 0;    // Exit the program
}
