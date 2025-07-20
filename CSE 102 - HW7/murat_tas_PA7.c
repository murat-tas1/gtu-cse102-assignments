#include <stdio.h>
#include <stdlib.h>

#define MAX_LABELS 128

// Function to create a 2D grid with n rows and m columns
int **create_grid(int n, int m){
    int **s = (int**)malloc(n * sizeof(int*)); // Allocate memory for row pointers
    int i;
    for(i = 0; i < n; ++i){
        s[i] = (int*)malloc(m * sizeof(int)); // Allocate memory for each row
    }
    return s;
}

// Function to find the root label of a given label
int find_label(int equivalence[], int label) {
    while (equivalence[label] != label) { // Traverse until the root label is found
        label = equivalence[label];
    }
    return label; // Return the root label
}

// Function to union two label groups
void union_groups(int *equivalence, int label1, int label2) {
    int root1 = find_label(equivalence, label1); // Find root of first label
    int root2 = find_label(equivalence, label2); // Find root of second label

    if (root1 != root2) {
        equivalence[root2] = root1; // Merge the two groups by updating the root
    }
}

// Function to label connected groups in the grid
void label_groups(int **a, int n, int m){
    int i, j, label = 1;
    int left, up;

    // Allocate memory for equivalence array to keep track of label equivalences
    int *equivalence = (int*)malloc((n*m) * sizeof(int));
    if (equivalence == NULL) {
        printf("Error: Could not allocate memory for equivalence array.\n");
        exit(1);
    }

    // Initialize each label to be its own root
    for (i = 0; i <n*m; ++i) {
        equivalence[i] = i;
    }

    // First pass: Assign temporary labels and record equivalences
    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            if (a[i][j] == 1) { // If the cell is part of a group
                // Handle the top-left corner
                if (i == 0 && j == 0) {
                    a[i][j] = label++;
                }
                // Handle the first row
                else if (i == 0 && j != 0) {
                    if (a[i][j-1] > 0) {
                        a[i][j] = a[i][j-1]; // Use the label from the left cell
                    } else {
                        a[i][j] = label++; // Assign a new label
                    }
                }
                // Handle the first column
                else if (i != 0 && j == 0) {
                    if (a[i-1][j] > 0) {
                        a[i][j] = a[i-1][j]; // Use the label from the above cell
                    } else {
                        a[i][j] = label++; // Assign a new label
                    }
                }
                // Handle all other cells
                else {
                    left = a[i][j-1]; // Label from the left cell
                    up   = a[i-1][j]; // Label from the above cell

                    if (left > 0 && up > 0) { // If both neighbors are labeled
                        if (left == up) {
                            a[i][j] = left; // Same label, use it
                        }
                        else {
                            union_groups(equivalence, left, up); // Merge different labels
                            if(left < up)
                                a[i][j] = left; // Use the smaller label
                            else
                                a[i][j] = up;
                        }
                    }
                    else if (left > 0) {
                        a[i][j] = left; // Use the label from the left
                    }
                    else if (up > 0) {
                        a[i][j] = up; // Use the label from above
                    }
                    else {
                        a[i][j] = label++; // Assign a new label
                    }
                }
            }
        }
    }

    // Second pass assign final labels and ensure they are consecutive
    int *root_to_new_label = (int*)calloc(n*m , sizeof(int)); // Map roots to new labels
    if (root_to_new_label == NULL) {
        printf("Error: Could not allocate memory for root_to_new_label array.\n");
        free(equivalence);
        exit(1);
    }
    int new_label = 1, flag = 0;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            if (a[i][j] > 0) { // If the cell is labeled
                int root = find_label(equivalence, a[i][j]); // Find its root label
                if (root_to_new_label[root] == 0) { // If root hasn't been assigned a new label
                    if (new_label > MAX_LABELS -1 && flag == 0) { // Check label limit
                        printf("Exceeded maximum number of groups 127 !!!!\n");
                        flag = 1;
                    }
                    root_to_new_label[root] = new_label++; // Assign a new consecutive label
                }
                a[i][j] = root_to_new_label[root]; // Update cell with the new label
            }
        }
    }
    free(equivalence); // Free equivalence array
    free(root_to_new_label); // Free root_to_new_label array
}

// Function to free the memory allocated for the grid
void deallocate_grid(int **a, int n){
    int i;
    for(i = 0; i < n; ++i){
        free(a[i]); // Free each row
    }
    free(a); // Free the row pointers
}

// Function to read a PPM file and store it in a grid
void readPPMFile(const char *filename, int ***grid, int *rows, int *cols){
    FILE *fp = fopen(filename, "r"); // Open the PPM file
    if (fp == NULL) {
        printf("Error: Could not open PPM input file: %s\n", filename);
        exit(1);
    }

    char format[3];
    fscanf(fp, "%s", format); // Read the PPM format
    if (format[0] != 'P' || format[1] != '1') { // Check for P1 format
        printf("Invalid PPM format!!!\n");
        fclose(fp);
        exit(1);
    }

    fscanf(fp, "%d %d", cols, rows); // Read the image dimensions
    *grid = create_grid(*rows, *cols); // Create the grid

    int i, j;
    for (i = 0; i < *rows; ++i) { // Read each pixel
        for (j = 0; j < *cols; ++j) {
            int pixel;
            fscanf(fp, "%d", &pixel); // Read pixel value (0 or 1)
            (*grid)[i][j] = pixel;   // Store in the grid
        }
    }

    fclose(fp); // Close the file
}

// Function to write the grid to a PPM file with colors
int writePPMFile(const char *filename, int **grid, int rows, int cols) {
    FILE *fp = fopen(filename, "w"); // Open the output PPM file
    if (fp == NULL) {
        printf("Error: Could not open PPM output file: %s\n", filename);
        return 1;
    }
    // Write the PPM header
    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", cols, rows); // Image width and height
    fprintf(fp, "255\n");              // Maximum color value

    int i, j, r, g, b;
    // Write each pixel with a unique color based on its group ID
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            int groupID = grid[i][j];
            if (groupID == 0) {
                // Black pixel for background
                r = g = b = 0;
            } else {
                // Generate colors based on group ID
                r = (groupID * 37) % 256; // Red component
                g = (groupID * 73) % 256; // Green component
                b = (groupID * 109) % 256; // Blue component
            }

            fprintf(fp, "%d %d %d ", r, g, b); // Write RGB values
        }
        fprintf(fp, "\n"); // New line after each row
    }
    fclose(fp); // Close the file
    return 0;
}

int main(){
    int n, m, i, j;
    FILE *inp, *outp;
    
    // Open the input text file
    inp = fopen("input.txt","r");
    if (inp == NULL){
        printf("FILE opening error!!!\n");
        return 1; // Exit if file cannot be opened
    }
    
    // Read the matrix dimensions from the file
    fscanf(inp, "%d %d", &n, &m);

    // Create the grid
    int **a = create_grid(n, m);
    
    // Read the matrix data from the file
    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            fscanf(inp, "%d", &a[i][j]);
        }
    }
    fclose(inp); // Close the input file
    
    // Perform labeling on the grid
    label_groups(a, n, m);

    // Open the output text file for writing
    outp = fopen("output.txt","w");
    if (outp == NULL){
        printf("Could not open output file.\n");
        deallocate_grid(a, n); // Free the grid memory
        return 1;
    }
    
    // Write the labeled grid to the output file
    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            fprintf(outp, "%d ", a[i][j]);
        }
        fprintf(outp, "\n"); // New line after each row
    }
    fclose(outp); // Close the output file
    // Free the grid memory
    deallocate_grid(a, n);
    /* Bonus part: Process PPM image files */
    int row, col;
    int **ppm_grid;
    // Read the input PPM file
    readPPMFile("input.ppm", &ppm_grid, &row, &col);
    // Perform labeling on the PPM grid
    label_groups(ppm_grid, row, col);
    // Write the labeled PPM grid to the output PPM file
    writePPMFile("output.ppm", ppm_grid, row, col);
    // Free the PPM grid memory
    deallocate_grid(ppm_grid, row);
    return 0; // Exit the program
}
