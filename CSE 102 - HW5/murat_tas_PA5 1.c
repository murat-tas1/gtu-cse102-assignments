#include <stdio.h>
#include <math.h>

#define COUNT_FILE 5           /*This is number of files between 1 and 100 */
#define FILENAME_LENGTH 20     /*Length of filename*/
#define TOTAL_BI_GRAMS 729     /*27 * 27  possible number of bigrams*/
#define SPACE_INDEX 26       /*This is index of space character */

void reset_array(int *a, int size) {  /*this function resets all elements of array zero*/
    int i;
    for (i = 0; i < size; ++i)
        a[i] = 0;
}

int find_bigram_index(char curr, char prev) { /* This function finds index of characters which read from file*/
    int index,first_index, second_index;
    if (prev == ' ')
        first_index = SPACE_INDEX;
    else
        first_index = prev - 'a';
    if (curr == ' ')
        second_index = SPACE_INDEX;
    else
        second_index = curr - 'a';

    index = first_index * 27 + second_index;  /*it finds index*/
    return index;
}

void process_file(const char *filename, int *a) { /*This function reads characters from the files and calculates the frequency of bigrams */
    int index;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return;
    }
    char curr, prev; /*current character and previous character*/

    /*we need to take char for prev at first to compare*/
    do {   /*this loop continues if character that we read from the file is outside of our given characters*/
        prev = getc(fp); /*reading character by character from file and assign it into prev*/
    } while ((prev < 'a' || prev > 'z') && prev != ' ' && prev != EOF);

    if (prev == EOF) {
        fclose(fp);
        return;
    }
    /*The loop continues until the end of the file is reached.*/
    while ((curr = getc(fp)) != EOF) {
        if ((curr < 'a' || curr > 'z') && curr != ' ') /*if the character is not a lowercase letter (a-z) or not a space*/
            continue;    /* skip this character and continue to the next iteration of the loop */
        if (!(curr == ' ' && prev == ' ')) { /*if the current character (curr) and the previous character (prev) are not both spaces, process the bi-gram (space-space pairs are ignored)*/
             index = find_bigram_index(curr, prev); /* Calculate the index in the array corresponding to the current bi-gram*/
            ++a[index];  /*Increment the count for the bi-gram at the calculated index.*/
        }
        prev = curr; /* Set the current character (curr) as the previous character (prev) */
    }
    fclose(fp);  /* Close the file after reading is complete */
}

int find_bigram_count(int *x, int size) { /*This function calculates the total count of all bi-grams.*/
    int i,count = 0;
    for(i = 0; i < size; ++i)
        count += x[i];  /* sum up the counts of all bi-grams */
    return count;     /* Return the total count of bi-grams */
}

double calculate_dissimilarity(int *x, int *other_language, int total_bigrams_x, int size) { /*This function calculates the dissimilarity score between two languages*/
    int total_bigrams_others = find_bigram_count(other_language, size); /*Calculate the total number of bi-grams in the other language.*/
    double diss = 0.0;
    int i;
    if(total_bigrams_others == 0){ /* If the other language file is empty or has no valid bi-grams,*/
        diss = -1;                 /*discard this file by returning a dissimilarity score of -1.*/
        return diss;               /* This avoids a division by zero scenario (0 / 0)*/
    }

    for (i = 0; i < size; ++i) { /*loop through all bi-grams and calculate the dissimilarity score by using the formula*/
        diss += fabs(((double)x[i] / total_bigrams_x) - ((double)other_language[i] / total_bigrams_others));
    }
    return diss;
}

int main() {
    char files[COUNT_FILE][FILENAME_LENGTH]; /*Create an array to store the filenames of the language files*/
    int i;
    for (i = 0; i < COUNT_FILE; ++i)  /*Generate the filenames dynamically in the format "language_1.txt", "language_2.txt", ...,*/
       sprintf(files[i], "language_%d.txt", i + 1);  /* Store these filenames in the files array.*/

    /*Arrays to store the bi-gram counts for language_x and for other languages being compared*/
    int count_bigrams_x[TOTAL_BI_GRAMS] = {0}, count_bigrams_others[TOTAL_BI_GRAMS];

    process_file("language_x.txt", count_bigrams_x); /*Process the "language_x.txt" file to calculate the bi-gram counts and store them in the count_bigrams_x array.*/

    int total_bigrams_x = find_bigram_count(count_bigrams_x, TOTAL_BI_GRAMS); /* Calculate the total number of bi-grams in "language_x.txt"*/
    double diss;

    for (i = 0; i < COUNT_FILE; ++i) {
        reset_array(count_bigrams_others, TOTAL_BI_GRAMS); /*Reset the bi-gram counts array for the other language before processing the next file.*/
        process_file(files[i], count_bigrams_others); /* Process the current language file to calculate its bi-gram counts*/
        diss = calculate_dissimilarity(count_bigrams_x, count_bigrams_others, total_bigrams_x, TOTAL_BI_GRAMS); /* Calculate the dissimilarity score between "language_x.txt" and the current language file*/
        if(diss != -1) /*this means file is not empty if diss is -1 then do not print anything just skip that file discard it*/
        printf("%lf\n", diss);
    }
    return 0;
}
