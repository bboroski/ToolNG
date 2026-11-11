#ifndef RCME_H
#define RCME_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define RCME_ABI_VERSION 1u
typedef enum {
    RCME_OK = 0, RCME_INVALID_ARGUMENT, RCME_DIMENSION_MISMATCH,
    RCME_NON_FINITE_VALUE, RCME_NUMERIC_OVERFLOW,
    RCME_INSUFFICIENT_CAPACITY, RCME_OVERLAPPING_STORAGE
} rcme_status;
/* Non-owning view. capacity and stride are measured in double elements. */
typedef struct {
    size_t rows;
    size_t cols;
    size_t stride;
    size_t capacity;
    double *data;
} rcme_matrix;
rcme_status rcme_matrix_validate(const rcme_matrix *matrix);
/* Destination is not read before writes. On overflow it may be partially written. */
rcme_status rcme_multiply(const rcme_matrix *left, const rcme_matrix *right,
                          rcme_matrix *output);
rcme_status rcme_transpose(const rcme_matrix *input, rcme_matrix *output);
rcme_status rcme_normalize_frobenius(rcme_matrix *matrix);
/*
 * One bounded iteration: normalize(M * A * transpose(M) + B).
 * All views are square and equal-sized. Output and scratch views must be
 * capacity-valid and disjoint from each other and all inputs. M, A and B
 * may share read-only storage. No allocation or I/O is performed.
 */
rcme_status rcme_recursive_step(const rcme_matrix *matrix,
                                const rcme_matrix *transform,
                                const rcme_matrix *bias,
                                rcme_matrix *output,
                                rcme_matrix *scratch_a,
                                rcme_matrix *scratch_b);
double rcme_residual_l2(const rcme_matrix *left, const rcme_matrix *right,
                        rcme_status *status);
const char *rcme_domain_name(void);
rcme_status rcme_domain_weights(double *weights, size_t count);
rcme_status rcme_domain_measure(const rcme_matrix *matrix, double *measure);
#ifdef __cplusplus
}
#endif
#endif
