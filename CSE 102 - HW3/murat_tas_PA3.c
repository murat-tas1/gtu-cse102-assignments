#include <stdio.h> 
#include <string.h>
#include <math.h>

#define SIZE 1000

 /*Parses monomial data from the polynomial string and stores in  arrays*/
int parse_monomials(const char* polyn, double coef[], int sign[], int power_x[], int power_y[]) {
    int pos = 0, m_idx = 0,status,len;
    char monomial[SIZE];
    int i, exponent;
    double c;
    char ch;
    len = strlen(polyn);

    while (pos < len) {
       /* Determine the sign of the monomial*/
        if (polyn[pos] == '+') {
            sign[m_idx] = 1;
            pos++;
        } else if (polyn[pos] == '-') {
            sign[m_idx] = -1;
            pos++;
        } else {
            sign[m_idx] = 1;
        }

        /*Extract the monomial untill + or - sign*/
        i = 0;
        while (pos < len && polyn[pos] != '+' && polyn[pos] != '-') {
            monomial[i] = polyn[pos];
            ++i; ++pos;
        }
        monomial[i] = '\0';

        /*read coefficient*/
         status = sscanf(monomial, "%lf", &c);
        if (status == 0) { /*Coefficient automatically is 1*/
            c = 1.0;
        }
        coef[m_idx] = c;

        /*Set default powers for x and y to zero*/
        power_x[m_idx] = 0;
        power_y[m_idx] = 0;

        /*Parse powers of x and y in the monomial*/
        const char* mon_ptr = monomial;
        while (*mon_ptr && *mon_ptr != 'x' && *mon_ptr != 'y') {
            ++mon_ptr;
        }
        /*Iterate through each character to find x/y and its exponent*/
        while (*mon_ptr != '\0') {
            status = sscanf(mon_ptr, "%c^%d", &ch, &exponent);
            if (status == 2) {
                if (ch == 'x') {
                    power_x[m_idx] = exponent;
                } else if (ch == 'y') {
                    power_y[m_idx] = exponent;
                }
                mon_ptr += 3;
            } else if (status == 1) {
                if (ch == 'x') {
                    power_x[m_idx] = 1;
                } else if (ch == 'y') {
                    power_y[m_idx] = 1;
                }
                mon_ptr += 1;
            }
        }
        m_idx++;
    }
    return m_idx; /*Returns the number of monomials parsed*/
}

/*Evaluates the polynomial based on given monomial data, x and y values*/
double evaluate_polynomial(int monomial_count, double coef[], int sign[], int power_x[], int power_y[], double x, double y) {
    double result = 0.0;
    int i;
    for (i = 0; i < monomial_count; ++i) {
        result += sign[i] * coef[i] * pow(x, power_x[i]) * pow(y, power_y[i]);
    }
    return result;
}

int main() {
     /* Open input files for (x, y) values and polynomial strings, and output file for evaluations*/
    FILE *fp_v = fopen("values.txt", "r");
    FILE *fp_pol = fopen("polynomials.txt", "r");
    FILE *fp_ev = fopen("evaluations.txt", "w");
    /* Check if files opened successfully*/
    if (fp_v == NULL || fp_pol == NULL || fp_ev ==NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    double x, y;
    char polyn[SIZE];
    double coef[200];
    int sign[200],power_x[200],power_y[200],monomial_count;
    
    /*Read all (x, y) pairs from values.txt and store in an array*/
    double values[200][2];
    int value_count = 0;
    while (fscanf(fp_v, "%lf %lf", &values[value_count][0], &values[value_count][1]) == 2) {
        value_count++;
    }
    fclose(fp_v);  /*close the file*/
    int i;

    /* Read each polynomial, parse monomial details, and evaluate for each (x, y) pair*/
    while (fgets(polyn, SIZE, fp_pol) != NULL) {
        if(polyn[0] == '\n')
        continue; /*Skip empty lines*/
        /*Parse monomial data from the polynomial string*/
         monomial_count = parse_monomials(polyn, coef, sign, power_x, power_y);
        
        /* For each x-y pair  evaluate the polynomial and write the result*/
        for (i = 0; i < value_count; ++i) {
            x = values[i][0];
            y = values[i][1];
            if(monomial_count != 0){
            double result = evaluate_polynomial(monomial_count, coef, sign, power_x, power_y, x, y);
            fprintf(fp_ev, "%.2lf ", result);
            }
        }
        fprintf(fp_ev, "\n"); /* new line after each polynomial's evaluation for all pairs*/
    
    }
    /*close the files*/
    fclose(fp_pol);
    fclose(fp_ev);

    return 0;
}
