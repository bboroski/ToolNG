#include "rcme.h"
#include <math.h>
#include <stdint.h>

static rcme_status storage_validate(const rcme_matrix *m, size_t *span_bytes) {
    size_t required, bytes;
    uintptr_t start;
    if (m == NULL || m->data == NULL || m->rows == 0 || m->cols == 0 ||
        m->stride < m->cols) return RCME_INVALID_ARGUMENT;
    if (m->rows - 1 > (SIZE_MAX - m->cols) / m->stride)
        return RCME_INVALID_ARGUMENT;
    required = (m->rows - 1) * m->stride + m->cols;
    if (m->capacity < required) return RCME_INSUFFICIENT_CAPACITY;
    if (required > SIZE_MAX / sizeof(double)) return RCME_NUMERIC_OVERFLOW;
    bytes = required * sizeof(double);
    start = (uintptr_t)m->data;
    if (bytes > UINTPTR_MAX - start) return RCME_INVALID_ARGUMENT;
    if (span_bytes != NULL) *span_bytes = bytes;
    return RCME_OK;
}

static int views_overlap(const rcme_matrix *a, const rcme_matrix *b) {
    size_t a_bytes = 0, b_bytes = 0;
    uintptr_t a_start, b_start;
    if (storage_validate(a, &a_bytes) != RCME_OK ||
        storage_validate(b, &b_bytes) != RCME_OK) return 1;
    a_start = (uintptr_t)a->data;
    b_start = (uintptr_t)b->data;
    return a_start < b_start + b_bytes && b_start < a_start + a_bytes;
}

rcme_status rcme_matrix_validate(const rcme_matrix *m) {
    size_t i, j;
    rcme_status status = storage_validate(m, NULL);
    if (status != RCME_OK) return status;
    for (i = 0; i < m->rows; ++i)
        for (j = 0; j < m->cols; ++j)
            if (!isfinite(m->data[i * m->stride + j]))
                return RCME_NON_FINITE_VALUE;
    return RCME_OK;
}

static int same_shape(const rcme_matrix *a, const rcme_matrix *b) {
    return a->rows == b->rows && a->cols == b->cols;
}

rcme_status rcme_multiply(const rcme_matrix *a, const rcme_matrix *b,
                          rcme_matrix *out) {
    size_t i, j, k;
    rcme_status status;
    if ((status = rcme_matrix_validate(a)) != RCME_OK ||
        (status = rcme_matrix_validate(b)) != RCME_OK) return status;
    if ((status = storage_validate(out, NULL)) != RCME_OK) return status;
    if (a->cols != b->rows || out->rows != a->rows || out->cols != b->cols)
        return RCME_DIMENSION_MISMATCH;
    if (views_overlap(out, a) || views_overlap(out, b))
        return RCME_OVERLAPPING_STORAGE;
    for (i = 0; i < out->rows; ++i) {
        for (j = 0; j < out->cols; ++j) {
            double sum = 0.0;
            for (k = 0; k < a->cols; ++k) {
                double term = a->data[i * a->stride + k] *
                              b->data[k * b->stride + j];
                if (!isfinite(term) || !isfinite(sum + term))
                    return RCME_NUMERIC_OVERFLOW;
                sum += term;
            }
            out->data[i * out->stride + j] = sum;
        }
    }
    return RCME_OK;
}

rcme_status rcme_transpose(const rcme_matrix *in, rcme_matrix *out) {
    size_t i, j;
    rcme_status status;
    if ((status = rcme_matrix_validate(in)) != RCME_OK) return status;
    if ((status = storage_validate(out, NULL)) != RCME_OK) return status;
    if (out->rows != in->cols || out->cols != in->rows)
        return RCME_DIMENSION_MISMATCH;
    if (views_overlap(in, out)) return RCME_OVERLAPPING_STORAGE;
    for (i = 0; i < in->rows; ++i)
        for (j = 0; j < in->cols; ++j)
            out->data[j * out->stride + i] = in->data[i * in->stride + j];
    return RCME_OK;
}

rcme_status rcme_normalize_frobenius(rcme_matrix *m) {
    size_t i, j;
    double scale = 0.0, sum = 1.0;
    rcme_status status = rcme_matrix_validate(m);
    if (status != RCME_OK) return status;
    for (i = 0; i < m->rows; ++i) {
        for (j = 0; j < m->cols; ++j) {
            double x = fabs(m->data[i * m->stride + j]);
            if (x != 0.0) {
                if (scale < x) {
                    double q = scale / x;
                    sum = 1.0 + sum * q * q;
                    scale = x;
                } else {
                    double q = x / scale;
                    sum += q * q;
                }
            }
        }
    }
    if (scale == 0.0) return RCME_OK;
    {
        double norm = scale * sqrt(sum);
        if (!isfinite(norm) || norm == 0.0) return RCME_NUMERIC_OVERFLOW;
        for (i = 0; i < m->rows; ++i)
            for (j = 0; j < m->cols; ++j)
                m->data[i * m->stride + j] /= norm;
    }
    return RCME_OK;
}

rcme_status rcme_recursive_step(const rcme_matrix *m, const rcme_matrix *a,
                                const rcme_matrix *b, rcme_matrix *out,
                                rcme_matrix *wa, rcme_matrix *wb) {
    size_t i, j, index, other, n;
    rcme_status status;
    const rcme_matrix *all[] = {m, a, b, out, wa, wb};
    for (index = 0; index < sizeof(all) / sizeof(all[0]); ++index) {
        if ((status = storage_validate(all[index], NULL)) != RCME_OK)
            return status;
    }
    if ((status = rcme_matrix_validate(m)) != RCME_OK ||
        (status = rcme_matrix_validate(a)) != RCME_OK ||
        (status = rcme_matrix_validate(b)) != RCME_OK) return status;
    n = m->rows;
    for (index = 0; index < 6; ++index)
        if (all[index]->rows != n || all[index]->cols != n)
            return RCME_DIMENSION_MISMATCH;
    /* All three write views must be disjoint from every other view. */
    for (index = 3; index < 6; ++index)
        for (other = 0; other < 6; ++other)
            if (other != index && views_overlap(all[index], all[other]))
                return RCME_OVERLAPPING_STORAGE;
    if ((status = rcme_multiply(m, a, wa)) != RCME_OK ||
        (status = rcme_transpose(m, wb)) != RCME_OK ||
        (status = rcme_multiply(wa, wb, out)) != RCME_OK) return status;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            double value = out->data[i * out->stride + j] +
                           b->data[i * b->stride + j];
            if (!isfinite(value)) return RCME_NUMERIC_OVERFLOW;
            out->data[i * out->stride + j] = value;
        }
    }
    return rcme_normalize_frobenius(out);
}

double rcme_residual_l2(const rcme_matrix *a, const rcme_matrix *b,
                        rcme_status *status) {
    size_t i, j;
    double scale = 0.0, sum = 1.0;
    rcme_status result = rcme_matrix_validate(a);
    if (result == RCME_OK) result = rcme_matrix_validate(b);
    if (result != RCME_OK || !same_shape(a, b)) {
        if (status != NULL)
            *status = result == RCME_OK ? RCME_DIMENSION_MISMATCH : result;
        return NAN;
    }
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            double x = fabs(a->data[i * a->stride + j] -
                            b->data[i * b->stride + j]);
            if (!isfinite(x)) {
                if (status != NULL) *status = RCME_NUMERIC_OVERFLOW;
                return NAN;
            }
            if (x != 0.0) {
                if (scale < x) {
                    double q = scale / x;
                    sum = 1.0 + sum * q * q;
                    scale = x;
                } else {
                    double q = x / scale;
                    sum += q * q;
                }
            }
        }
    }
    if (status != NULL) *status = RCME_OK;
    return scale == 0.0 ? 0.0 : scale * sqrt(sum);
}
