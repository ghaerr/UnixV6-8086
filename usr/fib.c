#include "unix.h"

#define MAX_DIGITS 65
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

int n;
/* number of add operation */
uint8_t nAddOperation[MAX_DIGITS] = {0};

void inc_big_numbers(uint8_t *number)
{
    uint8_t carry = 1;
    uint8_t i;
    uint16_t sum;

    for (i = 0; i < MAX_DIGITS; i++)
    {
        sum = number[i] + carry;
        number[i] = sum % 10;
        carry = sum / 10;
    }
}

/* Function to add two large numbers represented as arrays of uint8_t */
void add_big_numbers(uint8_t *result, uint8_t *a, uint8_t *b)
{
    uint8_t carry = 0;
    uint8_t i;
    uint16_t sum;

    for (i = 0; i < MAX_DIGITS; i++)
    {
        sum = a[i] + b[i] + carry; /* Sum with carry */
        result[i] = sum % 10;      /* Store the single digit in result */
        carry = sum / 10;          /* Update the carry */
    }

    inc_big_numbers(nAddOperation);
}

/* Function to print a big number represented in reverse order */
void print_big_number(uint8_t *number)
{
    int i;
    int start_printing = 0;

    for (i = MAX_DIGITS - 1; i >= 0; i--)
    {
        if (number[i] != 0)
        {
            start_printing = 1;
        }
        if (start_printing)
        {
            printf("%u", number[i]);
        }
    }
    if (!start_printing)
    {
        printf("0"); /* Handle the case when number is 0 */
    }
    printf("\n");
}

void fibonacci_sequence(void)
{
    /* Arrays to store the big Fibonacci numbers as arrays of digits */
    uint8_t a[MAX_DIGITS] = {0};      /* First Fibonacci number */
    uint8_t b[MAX_DIGITS] = {0};      /* Second Fibonacci number */
    uint8_t result[MAX_DIGITS] = {0}; /* Result of addition */

    int count;

    /* Initialize the first two numbers */
    a[0] = 0;
    b[0] = 1;

    /* Print the first two Fibonacci numbers */
    print_big_number(a);
    print_big_number(b);

    /* Calculate and print subsequent Fibonacci numbers */
    for (count = 2; count <= 255; count++)
    {
        add_big_numbers(result, a, b);
        printf("fib(%d) = ", count);
        print_big_number(result);

        /* Update a and b for the next iteration */
        memcpy(a, b, MAX_DIGITS);      /* a = b */
        memcpy(b, result, MAX_DIGITS); /* b = result */
    }
}

void fibonacci(uint8_t n, uint8_t *result)
{
    uint8_t a[MAX_DIGITS], b[MAX_DIGITS];

    if (n == 0 || n == 1)
    {
        memset(result, 0, MAX_DIGITS);
        result[0] = n;
    }
    else
    {
        fibonacci(n - 1, a);
        fibonacci(n - 2, b);
        add_big_numbers(result, a, b);
    }
}

void fibonacci_print(uint8_t n)
{
    uint8_t result[MAX_DIGITS];
    fibonacci(n, result);
    printf("fib(%d) = ", n);
    print_big_number(result);
}

void catch (void)
{
    signal(SIGQIT, catch);
    printf("pid %d, fib %d, N_Add = ", getpid(), n);
    print_big_number(nAddOperation);
}

int main(int argc, char **argv)
{
    uint8_t result[MAX_DIGITS];

    signal(SIGQIT, catch);

    if (argc != 2)
    {
        printf("Usage: fib <number>\n");
        printf(" 0: Print Fibonacci sequence from 0 to 255 (fast algo).\n");
        printf(" n: Print the nth Fibonacci number (slow algo, CPU-intensive).\n");
        exit1(1);
    }
    n = atoi(argv[1]);
    if (n == 0)
    {
        fibonacci_sequence();
    }
    else
    {
        fibonacci_print(n);
    }

    return 0;
}
