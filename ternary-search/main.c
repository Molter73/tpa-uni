#include <stddef.h>
#include <stdio.h>

/**
 * This is a private method, you should call 'ternary_search' instead.
 *
 * Look for needle in a haystack, bounding to a set of lower and upper bounds.
 *
 * Parameters:
 *   needle: The value we will be looking for.
 *   haystack: The array we will try to find the needle in.
 *   lower_bound: The lower limit in the haystack.
 *   upper_bound: The upper limit in the haystack.
 *
 * Returns:
 *   Index for the needle in the haystack if found, -1 otherwise.
 */
int _ternary_search(int needle, int haystack[], int lower_bound, int upper_bound) {
    if (lower_bound == upper_bound) {
        return haystack[lower_bound] == needle ? lower_bound : -1;
    } else if (lower_bound > upper_bound) {
        return -1;
    }

    size_t chunk_size  = (upper_bound - lower_bound) / 3;
    size_t lower_pivot = lower_bound + chunk_size;
    size_t upper_pivot = upper_bound - chunk_size;

    if (needle == haystack[lower_pivot]) {
        return lower_pivot;
    } else if (needle == haystack[upper_pivot]) {
        return upper_pivot;
    } else if (needle < haystack[lower_pivot]) {
        return _ternary_search(needle, haystack, lower_bound, lower_pivot - 1);
    } else if (needle > haystack[upper_pivot]) {
        return _ternary_search(needle, haystack, upper_pivot + 1, upper_bound);
    } else {
        return _ternary_search(needle, haystack, lower_pivot + 1, upper_pivot - 1);
    }
}

/**
 * Look for needle in a haystack.
 *
 * Parameters:
 *   needle: The value we will be looking for.
 *   haystack: The array we will try to find the needle in.
 *   haystack_size: The amount of elements in the haystack.
 *
 * Returns:
 *   Index for the needle in the haystack if found, -1 otherwise.
 */
int ternary_search(int needle, int haystack[], size_t haystack_size) {
    if (haystack == NULL || haystack_size == 0) {
        return -1;
    }

    return _ternary_search(needle, haystack, 0, haystack_size - 1);
}

typedef struct {
    int needle;
    int index;
    int* haystack;
    size_t haystack_size;
} test_case;

/**
 * Run a test case for ternary_search.
 *
 * Returns 0 if the test succeeds, 1 otherwise
 */
int execute_test(test_case* t) {
    printf("Expect needle '%d' at '%d' - haystack '%p' - size '%zu': ", t->needle, t->index, t->haystack,
           t->haystack_size);

    int index = ternary_search(t->needle, t->haystack, t->haystack_size);
    if (t->index != index) {
        printf("Error!!\n\tGot index '%d'\n", index);
        return 1;
    }

    printf("OK\n");
    return 0;
}

int main(int argc, char* argv[]) {
    int haystack[]       = {-28, -10, -4, 0, 5, 10, 20, 140, 1000};
    size_t haystack_size = sizeof(haystack) / sizeof(typeof(*haystack));

    printf("Starting tests...\n");

    test_case test_cases[] = {{-28, 0, haystack, haystack_size},  {-10, 1, haystack, haystack_size},
                              {-4, 2, haystack, haystack_size},   {0, 3, haystack, haystack_size},
                              {5, 4, haystack, haystack_size},    {10, 5, haystack, haystack_size},
                              {20, 6, haystack, haystack_size},   {140, 7, haystack, haystack_size},
                              {1000, 8, haystack, haystack_size}, {-20, -1, haystack, haystack_size},
                              {-5, -1, haystack, haystack_size},  {-2, -1, haystack, haystack_size},
                              {2, -1, haystack, haystack_size},   {8, -1, haystack, haystack_size},
                              {15, -1, haystack, haystack_size},  {50, -1, haystack, haystack_size},
                              {500, -1, haystack, haystack_size}, {-243, -1, haystack, haystack_size},
                              {-10, -1, NULL, haystack_size},     {20, -1, haystack, 0}};
    size_t test_cases_size = sizeof(test_cases) / sizeof(test_case);

    int failures = 0;
    for (int i = 0; i < test_cases_size; i++) {
        failures += execute_test(&test_cases[i]);
    }

    printf("%d out of %zu tests failed\n", failures, test_cases_size);

    return failures;
}
