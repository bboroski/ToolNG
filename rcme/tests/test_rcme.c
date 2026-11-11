#include "rcme.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#define MATRIX(r, c, s, cap, ptr) { (r), (c), (s), (cap), (ptr) }
int main(void) {
    double md[4] = {1, 0, 0, 1}, ad[4] = {1, 0, 0, 1};
    double bd[4] = {0, 0, 0, 0}, od[4] = {NAN, NAN, NAN, NAN};
    double x[4] = {NAN, NAN, NAN, NAN}, y[4] = {NAN, NAN, NAN, NAN};
    double weights[8], measure = 0.0, residual;
    rcme_status status = RCME_OK;
    rcme_matrix m = MATRIX(2, 2, 2, 4, md), a = MATRIX(2, 2, 2, 4, ad);
    rcme_matrix b = MATRIX(2, 2, 2, 4, bd), out = MATRIX(2, 2, 2, 4, od);
    rcme_matrix wa = MATRIX(2, 2, 2, 4, x), wb = MATRIX(2, 2, 2, 4, y);
    rcme_matrix too_small = MATRIX(2, 2, 2, 3, md);
    rcme_matrix alias = MATRIX(2, 2, 2, 4, md);
    double bad_data[1] = {NAN}, big_data[1] = {DBL_MAX};
    double two_data[1] = {2.0}, untouched[1] = {123.0};
    rcme_matrix bad = MATRIX(1, 1, 1, 1, bad_data);
    rcme_matrix big = MATRIX(1, 1, 1, 1, big_data);
    rcme_matrix two = MATRIX(1, 1, 1, 1, two_data);
    rcme_matrix out_one = MATRIX(1, 1, 1, 1, untouched);

    assert(rcme_matrix_validate(&m) == RCME_OK);
    assert(rcme_matrix_validate(&too_small) == RCME_INSUFFICIENT_CAPACITY);
    assert(rcme_matrix_validate(&bad) == RCME_NON_FINITE_VALUE);
    /* Destination buffers are not inspected before being written. */
    assert(rcme_multiply(&m, &a, &out) == RCME_OK);
    assert(od[0] == 1.0 && od[1] == 0.0 && od[2] == 0.0 && od[3] == 1.0);
    assert(rcme_multiply(&m, &a, &alias) == RCME_OVERLAPPING_STORAGE);
    assert(rcme_recursive_step(&m, &a, &b, &out, &wa, &wb) == RCME_OK);
    assert(fabs(od[0] - 0.7071067811865475) < 1e-12);
    assert(fabs(od[3] - 0.7071067811865475) < 1e-12);
    residual = rcme_residual_l2(&out, &out, &status);
    assert(status == RCME_OK && residual == 0.0);
    residual = rcme_residual_l2(&out, &b, &status);
    assert(status == RCME_OK && fabs(residual - 1.0) < 1e-12);
    assert(rcme_domain_weights(weights, 8) == RCME_OK);
    assert(rcme_domain_measure(&out, &measure) == RCME_OK && isfinite(measure));
    assert(rcme_domain_name() != NULL);
    assert(rcme_domain_weights(NULL, 4) == RCME_INVALID_ARGUMENT);
    assert(rcme_multiply(&big, &two, &out_one) == RCME_NUMERIC_OVERFLOW);
    assert(untouched[0] == 123.0);
    puts("RCME native tests: PASS");
    return 0;
}
