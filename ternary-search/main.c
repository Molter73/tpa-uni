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
 *   upper_bound: The uuper limit in the haystack.
 *
 * Returns:
 *   Index for the needle in the haystack if found, -1 otherwise.
 */
int _ternary_search(int needle, int haystack[], int lower_bound, int upper_bound) {
    if (lower_bound >= upper_bound) {
        return haystack[lower_bound] == needle ? lower_bound : -1;
    }

    int lower_pivot = (upper_bound + lower_bound + 1) / 3;
    int upper_pivot = lower_pivot * 2;

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
    int haystack[]       = {-10, -4, 0, 5, 20, 140};
    size_t haystack_size = sizeof(haystack) / sizeof(typeof(*haystack));

    printf("Starting tests...\n");

    test_case test_cases[] = {
        {-10, 0, haystack, haystack_size},   {-4, 1, haystack, haystack_size}, {0, 2, haystack, haystack_size},
        {5, 3, haystack, haystack_size},     {20, 4, haystack, haystack_size}, {140, 5, haystack, haystack_size},
        {-243, -1, haystack, haystack_size}, {-10, -1, NULL, haystack_size},   {20, -1, haystack, 0}};
    size_t test_cases_size = sizeof(test_cases) / sizeof(test_case);

    int failures = 0;
    for (int i = 0; i < test_cases_size; i++) {
        failures += execute_test(&test_cases[i]);
    }

    printf("%d out of %zu tests failed\n", failures, test_cases_size);

    return failures;
}
