#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct representing a term of the polynom
struct Expression {
    int coefficient;
    int exponent;
};

void polynomParser(const char* polynomialStr, struct Expression* polynomial, int* numTerms);
void addOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult);
void subOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult);
void MULTOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult);
void printResult(struct Expression* polynomial, int numTerms);

int main() {
    char input[128];
    struct Expression polynomial1[128];
    struct Expression polynomial2[128];
    struct Expression result[128];
    int numTerms1 = 0, numTerms2 = 0, numTermsResult = 0;

    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1]='\0';

    while (strncmp(input, "END", 3) != 0 && strncmp(input, "end", 3) != 0) {
        char* copy = strdup(input);

        char* polynomial1Str = strchr(copy, '(') + 1;
        polynomParser(polynomial1Str, polynomial1, &numTerms1);

        char* operation = strstr(copy, ")") + 1;

        char* polynomial2Str = strchr(operation, '(') + 1;
        polynomParser(polynomial2Str, polynomial2, &numTerms2);

        memset(result, 0, sizeof(struct Expression) * 128); // Clear the result array

        if (strncmp(operation, "ADD", 3) == 0) {
            addOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, result, &numTermsResult);
            printResult(result, numTermsResult);
        } else if (strncmp(operation, "SUB", 3) == 0) {
            subOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, result, &numTermsResult);
            printResult(result, numTermsResult);
        } else if (strncmp(operation, "MUL", 3) == 0) {
            MULTOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, result, &numTermsResult);
            printResult(result, numTermsResult);
        } else {
            printf("Invalid operation\n");
        }

        free(copy);

        // Clear the arrays and reset the term counters
        memset(polynomial1, 0, sizeof(struct Expression) * 128);
        memset(polynomial2, 0, sizeof(struct Expression) * 128);
        numTerms1 = 0;
        numTerms2 = 0;

        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1]='\0';
    }

    return 0;
}

// this function to parse the polynom string and initialize the polynom struct
void polynomParser(const char* polynomialStr, struct Expression* polynomial, int* numTerms) {
    char* copy = strdup(polynomialStr);
    char* token = strtok(copy, ":");
    int degree = (int)strtol(token, NULL, 10);
    polynomial[*numTerms].exponent = degree;
    token = strtok(NULL, ",");
    polynomial[*numTerms].coefficient = (int)strtol(token, NULL, 10);
    (*numTerms)++;
    for (int i = 0; token != NULL; i++) {
        token = strtok(NULL, ",");
        if (token != NULL) {
            degree--;
            if (degree >= 0) {
                polynomial[*numTerms].exponent = degree;
                polynomial[*numTerms].coefficient = (int)strtol(token, NULL, 10);
                (*numTerms)++;
            }
        }
    }
    free(copy);
}

// this function to make the add operation on the two polynoms
void addOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult) {
    int i, j, k = 0;

    for (i = 0, j = 0; i < numTerms1 && j < numTerms2; k++) {
        if (polynomial1[i].exponent > polynomial2[j].exponent) {
            result[k] = polynomial1[i];
            i++;
        } else if (polynomial1[i].exponent < polynomial2[j].exponent) {
            result[k] = polynomial2[j];
            j++;
        } else {
            result[k].exponent = polynomial1[i].exponent; //or result[k].exponent = polynomial2[i].exponent;
            result[k].coefficient = polynomial1[i].coefficient + polynomial2[j].coefficient;
            i++;
            j++;
        }
    }

    while (i < numTerms1) {
        result[k] = polynomial1[i];
        i++;
        k++;
    }

    while (j < numTerms2) {
        result[k] = polynomial2[j];
        j++;
        k++;
    }

    *numTermsResult = k;
}

// this function to make the sub operation on the two polynoms
void subOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult) {
    int i, j, k = 0;

    for (i = 0, j = 0; i < numTerms1 && j < numTerms2; k++ ) {
        if (polynomial1[i].exponent > polynomial2[j].exponent) {
            result[k] = polynomial1[i];
            i++;

        } else if (polynomial1[i].exponent < polynomial2[j].exponent) {
            result[k].exponent = polynomial2[j].exponent;
            result[k].coefficient = -polynomial2[j].coefficient;
            j++;
        } else {
            result[k].exponent = polynomial1[i].exponent;
            result[k].coefficient = polynomial1[i].coefficient - polynomial2[j].coefficient;
            i++;
            j++;
        }
    }
    while (i < numTerms1) {
        result[k] = polynomial1[i];
        i++;
        k++;
    }

    while (j < numTerms2) {
        result[k].exponent = polynomial2[j].exponent;
        result[k].coefficient = -polynomial2[j].coefficient;
        j++;
        k++;
    }

    *numTermsResult = k;
}

// this function to make the mult operation on the two polynoms
void MULTOperationOnPolynom(struct Expression* polynomial1, int numTerms1, struct Expression* polynomial2, int numTerms2, struct Expression* result, int* numTermsResult) {
    int maxExponent = polynomial1[0].exponent + polynomial2[0].exponent;
    int* coefficients = (int*)calloc(maxExponent + 1, sizeof(int));

    for (int i = 0; i < numTerms1; i++) {
        for (int j = 0; j < numTerms2; j++) {
            int exponent = polynomial1[i].exponent + polynomial2[j].exponent;
            coefficients[exponent] += polynomial1[i].coefficient * polynomial2[j].coefficient;
        }
    }

    int k = 0;
    for (int i = maxExponent; i >= 0; i--) {
        if (coefficients[i] != 0) {
            result[k].exponent = i;
            result[k].coefficient = coefficients[i];
            k++;
        }
    }

    *numTermsResult = k;

    free(coefficients);
}

// this function to print the result polynom
void printResult(struct Expression* polynomial, int numTerms) {
    for (int i = 0; i < numTerms; i++) {
        int coefficient = polynomial[i].coefficient;
        int exponent = polynomial[i].exponent;

        if (coefficient != 0) {
            if (i != 0 && coefficient > 0) {
                printf(" + ");
            } else if (coefficient < 0) {
                printf(" - ");
                coefficient = -coefficient;
            }

            if (coefficient != 1 || exponent == 0) {
                printf("%d", coefficient);
            }

            if (exponent != 0) {
                printf("x");

                if (exponent != 1) {
                    printf("^%d ", exponent);
                }
            }
        }
    }
    printf("\n");
}

//(2:3,2,1)SUB(3:2,4,0,-1)
//(2:3,2,1)ADD(3:2,4,0,-1)
//(2:3,2,1)MUL(3:2,4,0,-1)
