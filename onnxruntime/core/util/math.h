/**
* Derived from caffe2, need copyright announcement here.
*/

/**
* Copyright (c) 2016-present, Facebook, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

// This is a simple translation from the old Caffe math interfaces. We aim to
// still keep it simple, so all platforms would be able to support it fairly
// easily.

#if defined(USE_MKLDNN) && defined(USE_MKLML)
// when both USE_MKLDNN and USE_MKLML are defined, use MKLML cblas for GEMM
#include "mkl_cblas.h"
#define CBLAS_ENUM_DEFINED_H
#else
// We include the cblas header here so that we can obtain the macros from cblas.
extern "C" {
#include "core/framework/cblas.h"
}
#endif

#include "core/common/common.h"
#include "core/framework/data_types.h"
#include "core/framework/tensor.h"

namespace onnxruntime {

enum StorageOrder {
  UNKNOWN = 0,
  NHWC = 1,
  NCHW = 2,
};

#define FLOAT_TYPE DataTypeImpl::GetType<float>()

namespace math {

template <typename T, class Provider>
void Exp(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void Log(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void Cos(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void Sin(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void SinCos(const int N, const T* x, T* ys, T* yc, Provider* provider);
template <typename T, class Provider>
void Abs(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void Sqrt(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void InvSqrt(const int N, const T* x, T* y, Provider* provider);
template <typename T, class Provider>
void Sqr(const int N, const T* x, T* y, Provider* provider);

template <typename T, class Provider>
void Not(const int N, const T* x, T* y, Provider* provider);

template <typename T, class Provider>
void Powx(const int N, const T* a, const T b, T* y, Provider* provider);

#define DECLARE_BINARY_OP_BINARY_RESULT(name)                                  \
  template <typename T, class Provider>                                        \
  void name(const int N, const T* a, const T* b, bool* y, Provider* provider); \
  template <typename T, class Provider>                                        \
  void name##ToRow(                                                            \
      const int M,                                                             \
      const int N,                                                             \
      const T* a,                                                              \
      const T* b,                                                              \
      bool* y,                                                                 \
      Provider* provider);

DECLARE_BINARY_OP_BINARY_RESULT(LT);
DECLARE_BINARY_OP_BINARY_RESULT(LE);
DECLARE_BINARY_OP_BINARY_RESULT(GT);
DECLARE_BINARY_OP_BINARY_RESULT(GE);

DECLARE_BINARY_OP_BINARY_RESULT(And);
DECLARE_BINARY_OP_BINARY_RESULT(Or);
DECLARE_BINARY_OP_BINARY_RESULT(Xor);

#undef DECLARE_BINARY_OP_BINARY_RESULT

#define DECLARE_BINARY_OP(name)                                             \
  template <typename T, class Provider>                                     \
  void name(const int N, const T* a, const T* b, T* y, Provider* provider); \
  template <typename T, class Provider>                                     \
  void name##ToRow(                                                         \
      const int M,                                                          \
      const int N,                                                          \
      const T* a,                                                           \
      const T* b,                                                           \
      T* y,                                                                 \
      Provider* provider);                                                  \
  template <typename T, class Provider>                                     \
  void name##ToRow(                                                         \
      const int M, const int N, const T* x, T* y, Provider* provider);      \
  template <typename T, class Provider>                                     \
  void name##ToCol(                                                         \
      const int M, const int N, const T* x, T* y, Provider* provider);

DECLARE_BINARY_OP(Add);
DECLARE_BINARY_OP(Sub);
DECLARE_BINARY_OP(Mul);
DECLARE_BINARY_OP(Div);

#undef DECLARE_BINARY_OP

template <typename T, class Provider>
void ReduceMin(
    const int N,
    const T* x,
    T* y,
    Tensor* scratch_ptr,
    Provider* provider);
template <typename T, class Provider>
void ReduceMax(
    const int N,
    const T* x,
    T* y,
    Tensor* scratch_ptr,
    Provider* provider);

// Adds batch sub-tensors elementwise to output. Stripe is the stripe length
// and N is the number of elements to add (size of Y).
template <typename T, class Provider>
void AddStripedBatch(
    const int N,
    const T* first,
    T* y,
    const int stripe,
    const int batch,
    Provider* provider);

// Compute the row-wise max of a N*D matrix X, and write it to a N
// dimensional vector y.
template <typename T, class Provider>
void RowwiseMax(const int N, const int D, const T* x, T* y,
                Provider* provider);

// Compute the column-wise max of a N*D matrix X, and write it to a D
// dimensional vector y.
template <typename T, class Provider>
void ColwiseMax(const int N, const int D, const T* x, T* y,
                Provider* provider);

// Elemwise maximum of vector x and vector y. z[i] = max(x[i], y[i])
template <typename T, class Provider>
void ElemwiseMax(const int N, const T* x, const T* y, T* z, Provider* provider);

// Elemwise maximum of vector x and scalar alpha. y[i] = max(x[i], alpha)
template <typename T, class Provider>
void Maximum(
    const int N,
    const float alpha,
    const T* x,
    T* y,
    Provider* provider);

// Decaf gemm provides a simpler interface to the gemm functions, with the
// limitation that the data has to be contiguous in memory.
template <typename T, class Provider>
void Gemm(
    const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB,
    const int64_t M,
    const int64_t N,
    const int64_t K,
    const float alpha,
    const T* A,
    const T* B,
    const float beta,
    T* C,
    Provider* provider,
    //Caffe2 use this type to control on GPU, what presicion do we want to do the calculation
    //But not sure is this a good design for us. Keep it here for now.
    MLDataType math_type = FLOAT_TYPE);

// We also provide a gemm that has explicit lda, ldb and ldc specified.
// In most cases you probably want to use the function above, though.
template <typename T, class Provider>
void GemmEx(
    const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB,
    const int M,
    const int N,
    const int K,
    const T alpha,
    const T* A,
    const int lda,
    const T* B,
    const int ldb,
    const T beta,
    T* C,
    const int ldc,
    Provider* provider);

// GemmBatched provides a simple abstraction into library routines
template <typename T, class Provider>
void GemmBatched(
    const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB,
    const int A_size,
    const int A_batches,
    const int B_size,
    const int B_batches,
    const int M,
    const int N,
    const int K,
    const float alpha,
    const T* A,
    const T* B,
    const float beta,
    T* C,
    Provider* provider,
    Tensor* scratch = nullptr,
    MLDataType math_type = DataTypeImpl::FLOAT_TYPE);

// Gemv always takes in a M*N matrix A, and depending on whether we set TransA
// to Trans, the output is:
// CblasNoTrans: x is an N dim vector and y is an M dim vector.
// CblasTrans:   x is an M dim vector and y is an N dim vector.
template <typename T, class Provider>
void Gemv(
    const CBLAS_TRANSPOSE TransA,
    const int M,
    const int N,
    const float alpha,
    const T* A,
    const T* x,
    const float beta,
    T* y,
    Provider* provider,
    MLDataType math_type = DataTypeImpl::FLOAT_TYPE);
template <typename T, class Provider>
void Set(const int64_t N, const T alpha, T* X, Provider* provider);

template <typename T, class Provider>
void RandUniform(const int n, const T a, const T b, T* r,
                 Provider* provider);

template <typename T, class Provider>
void RandUniformUnique(
    const size_t n,
    const T a,
    const T b,
    T* r,
    const size_t m,
    const T* avoid,
    Provider* provider);

template <typename T, class Provider>
void RandGaussian(
    const int n,
    const T mean,
    const T std,
    T* r,
    Provider* provider);

// Dot matrix of vector a and b, and writes the result to a single value y.
template <typename T, class Provider>
void Dot(const int N, const T* a, const T* b, T* y, Provider* provider);

// Sum of vector x, and writes the result to a single value y.
template <typename T, class Provider>
void Sum(const int N, const T* x, T* y, Provider* provider,
         Tensor* scratch_ptr = nullptr);

// Sum of squares of vector x, and writes the result to a single value y.
template <typename T, class Provider>
void SumSqr(
    const int N,
    const T* x,
    T* y,
    Provider* provider,
    Tensor* scratch_ptr = nullptr);

// Select does index selection of the rows a N*D matrix x, and gives the N
// dimensional vector y that contains the selected data.
template <typename T, class Provider>
void Select(const int N, const int D, const T* x, const int* idx, T* y,
            Provider* provider);

template <typename T, class Provider>
void Scale(const int N, const float alpha, const T* x, T* y, Provider* provider);

// Different from the Scale function above, if alpha is passed in
// as a pointer, we will assume that it lives on the correct execution provider,
// for example on GPU.
template <typename T, class Provider>
void Scale(const int N, const float* alpha, const T* x, T* y, Provider* provider);

template <typename T, class Provider>
void Axpy(const int N, const float alpha, const T* x, T* y, Provider* provider);

// Different from the Axpy function above, if alpha is passed in
// as a pointer, we will assume that it lives on the correct execution provider,
// for example on GPU.
template <typename T, class Provider>
void Axpy(const int N, const float* alpha, const T* x, T* y, Provider* provider);

template <typename T, class Provider>
void Axpby(
    const int N,
    const float alpha,
    const T* x,
    const T b,
    T* y,
    Provider* provider);

template <typename T, class Provider, int order>
void Im2colNd(
    const T* data_img,
    const int64_t* im_shape,
    const int64_t* col_shape,
    const int64_t img_size,
    const int64_t col_size,
    const int64_t* kernel_shape,
    const int64_t* stride,
    const int64_t* dilation,
    const int64_t* pad,
    const int64_t N,
    T* data_col,
    Provider* provider,
    bool accumulate_output = false);

template <typename T, class Provider, int order>
void Col2imNd(
    const T* data_col,
    const int64_t* img_shape,
    const int64_t* col_shape,
    const int64_t img_size,
    const int64_t col_size,
    const int64_t* kernel_shape,
    const int64_t* stride,
    const int64_t* dilation,
    const int64_t* pad,
    const int64_t N,
    T* data_img,
    Provider* provider);

template <typename T, class Provider, int order>
void Im2col(
    const T* data_im,
    const int64_t channels,
    const int64_t height,
    const int64_t width,
    const int64_t kernel_h,
    const int64_t kernel_w,
    const int64_t dilation_h,
    const int64_t dilation_w,
    const int64_t pad_t,
    const int64_t pad_l,
    const int64_t pad_b,
    const int64_t pad_r,
    const int64_t stride_h,
    const int64_t stride_w,
    T* data_col,
    Provider* provider);

template <typename T, class Provider, int order>
void Col2im(
    const T* data_col,
    const int64_t channels,
    const int64_t height,
    const int64_t width,
    const int64_t patch_h,
    const int64_t patch_w,
    const int64_t dilation_h,
    const int64_t dilation_w,
    const int64_t pad_t,
    const int64_t pad_l,
    const int64_t pad_b,
    const int64_t pad_r,
    const int64_t stride_h,
    const int64_t stride_w,
    T* data_im,
    Provider* provider);

// Todo: should we provide a better copy method signature?
using TypedCopy = void (*)(const void*, void*, size_t);

template <class Provider>
void CopyMatrix(
    const size_t item_size,
    const int M,
    const int N,
    const void* A,
    const int lda,
    void* B,
    const int ldb,
    Provider* provider,
    TypedCopy copy = nullptr);

template <typename T, class Provider>
void CopyVector(const int N, const T* A, T* B, Provider* provider);

uint32_t randomNumberSeed();

// Function uses casting from int64_t to uint64_t to compare if value of
// parameter a is greater or equal to zero and lower than value of
// parameter b. The b parameter is of type signed and is always
// positive,
// therefore its value is always lower than 0x800... where casting
// negative value of a parameter converts it to value higher than
// 0x800...
// The casting allows to use one condition instead of two.
inline bool is_a_ge_zero_and_a_lt_b(int64_t a, int64_t b) {
  return static_cast<uint64_t>(a) < static_cast<uint64_t>(b);
}

// Calculates ceil(a / b). User must be careful to ensure that there
// is no overflow or underflow in the calculation.
template <typename T>
constexpr T divUp(T a, T b) {
  return (a + b - (T)1) / b;
}

// Rounds a up to the next highest multiple of b. User must be careful
// to ensure that there is no overflow or underflow in the calculation
// of divUp.
template <typename T>
constexpr T roundUp(T a, T b) {
  return divUp<T>(a, b) * b;
}

// Returns true if the given integer type is a power-of-2 (positive only)
// Note(jiayq): windows reported an error per
//     https://github.com/caffe2/caffe2/issues/997
// and as a result will make it a macro.
#ifdef _MSC_VER
#define integerIsPowerOf2(v) ((v) && !((v) & ((v)-1)))
#else   // _MSC_VER
template <typename T>
constexpr bool integerIsPowerOf2(T v) {
  return (v && !(v & (v - 1)));
}
#endif  // _MSC_VER

// Returns log2(n) for a positive integer type
template <typename T>
constexpr int integerLog2(T n, int p = 0) {
  return (n <= 1) ? p : integerLog2(n / 2, p + 1);
}

// Returns the next highest power-of-2 for an integer type
template <typename T>
constexpr T integerNextHighestPowerOf2(T v) {
  return (integerIsPowerOf2(v) ? (T)2 * v : ((T)1 << (integerLog2(v) + 1)));
}

// Rounds a up to the next highest multiple of b, which is power-of-2. User must be careful
// to ensure that there is no overflow or underflow in the calculation
// of divUp.
template <typename T, T b>
constexpr T roundUpPow2(T a) {
  return (a + (b - 1)) & (~(b - 1));
}

uint16_t floatToHalf(float f);

float halfToFloat(uint16_t h);

}  // namespace math
}  // namespace onnxruntime
