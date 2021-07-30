/***
 * File: homework_one_p6.c
 * Name: Joshua Early
 * Date: 1/25/18
 * Purpose: This program is designed to calculate factorial, Stirling's formula, and
 * compare the percentage of error between both.
 ***/
#include <stdio.h>
#include <math.h>

// named constants
#define PI 3.14159265
#define E 2.71828

/***
 * Purpose: Calculates Stirling's approximation for given input, n
 * Parameters: n: of type int
 * Precondition: n is the correct type
 *  Postcondition: Returns the approximation for the given input
***/
double stirling_approx(int n) {
    return pow( n, n ) * exp( -n ) * sqrt( 2.0 * PI * n );
}

/***
 * Purpose: Calculates factorial  for given input, n
 * Parameters: n: of type int
 * Precondition: n is the correct type
 *  Postcondition: Returns the factorial of the given input
***/
long int factorial(int input) {
    long int ans = 1;
    for (int i = 2; i <= input; i++)
        ans *= i;
    return ans;
}

/***
 * Purpose: Calculates the percent difference between factorial and Stirling's Formula
 * Parameters: n: of type int
 * Precondition: n is the correct type, and methods factorial() and stirling_approx() have
 * been defined.
 * Postcondition: Returns the percent difference/error of factorial and Stirling's Formula
***/
float percent_diff(int n) {
    long int fact = factorial(n);
    float stirling = stirling_approx(n);
    return ((fact - stirling) / fact) * 100;
}

int main(void) {
    printf(" n\tn!\t\t\t Stirling's Formula\t\tPercent Difference\n");
    for (int n = 1; n <= 20; n++) {
        printf("n: %-2d\t%-19ld\t %-22.2lf\t\t%f%\n", n, factorial(n), stirling_approx(n), percent_diff(n));
    }
    return 0;
}
