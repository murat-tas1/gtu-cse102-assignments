#include <stdio.h>
#include <string.h> 

/* Define module structure */
typedef struct {     
    char ident[30];  /* Module name */
    double coeff;    /* Coefficient of the module */
} module_t;

/* Function to remove spaces from the string */
void remove_spaces(char *a){   
    int i, j = 0, len;
    len = strlen(a);
    for(i = 0; i < len; ++i){
        if(a[i] != ' '){   /* If the character is not a space, store it in the array */
            a[j] = a[i];
            ++j; 
        }
    }
    a[j] = '\0';      /* to make it a string */
}

/* Function to remove newline characters from the input */
void clear_new_line(char *a){    
    int i, len = strlen(a);
    for(i = 0; i < len; ++i){       /* We are removing the newline character because fgets includes spaces */
        if(a[i] == '\n'){     /* If the character is a newline, replace it with a null character and break the loop */
            a[i] = '\0';
            break;
        }
    }
}

/* Function to evaluate the module, calculating its value based on defined terms */
double eval(char *name) {
    char def[3000];   
    char *token, *pos;
    int number_of_terms = 0, i;
    double sum = 0.0;    /* Initialize sum to 0 */
    module_t array_of_modules[100];  /* Array to hold the modules */
    
    clear_new_line(name);  /* Clear any newline character in the module name */
    printf("Define %s:\n", name);
    fgets(def, 3000, stdin);  /* Get the module definition from user input */
    remove_spaces(def);  /* Remove any spaces from the module definition */

    /* Process the first module definition */

    token = strtok(def, "+");          /* split the def string by '+' signs and gets the terms*/
    while (token != NULL){

        pos = strchr(token, '*');  /*if pos is not NULL then after '*' there is a module and
                                    before the '*' there is a scalar */
        if (pos != NULL) { 
            *pos = '\0';       /* *pos indicates the content of adress of '*' then we assigned there null char
                                  to obtain scalar value as a string*/
            sscanf(token, "%lf", &array_of_modules[number_of_terms].coeff);  /* Parse the scalar value */
            strcpy(array_of_modules[number_of_terms].ident, pos + 1);  /* Copy the variable name after '*' */
        } else {
            sscanf(token, "%lf", &array_of_modules[number_of_terms].coeff);  /* If no '*' present, parse the scalar value */
            strcpy(array_of_modules[number_of_terms].ident, "");  /* Set variable name to empty string it means there is no module */
        }

        ++number_of_terms;        /* Increment the number of terms */
        token = strtok(NULL, "+");  /* Get the next term */
    }

    /* Evaluate the total sum of terms */
    for (i = 0; i < number_of_terms; ++i) {

        if (strlen(array_of_modules[i].ident) > 0) {  /* If there is a variable name it means there is a module */
            double r = eval(array_of_modules[i].ident);  /* Recursively evaluate the module */
            sum += r * array_of_modules[i].coeff;  /* Add the product of coefficient and evaluated value */
        }
         else {   /*this is base case:  the string is empty it means there is no module */
            sum += array_of_modules[i].coeff;  /* just add the coefficient */
        }
    }

    return sum;  /* return the computed sum */
}

int main() {    
    char ident[30];  /* Variable to store module name */
    printf("Module name?:\n");
    fgets(ident,30,stdin);
    double result = eval(ident);  /* call eval function to calculate the result */
    printf("%.2f\n", result);  /* print the result*/
    
    return (0);
}
