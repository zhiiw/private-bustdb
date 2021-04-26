#pragma once

#include <memory>

namespace bustub {

template <typename T>
class Matrix {
 public:
  Matrix(int r, int c) {
    rows =r;
    cols =c;
    linear = new T[rows*cols];
    for (int i = 0; i <rows*cols; ++i) {
      linear[i]=0;
    }
  }
  // # of rows in the matrix
  int rows;
  // # of Columns in the matrix
  int cols;
  // Flattened array containing the elements of the matrix
  // TODO(P0) : Allocate the array in the constructor. Don't forget to free up
  // the array in the destructor.
  T *linear;
 public:
  // Return the # of rows in the matrix
  virtual int GetRows() = 0;
  // Return the # of columns in the matrix
  virtual int GetColumns() = 0;
  // Return the (i,j)th  matrix element
  virtual T GetElem(int i, int j) = 0;
  // Sets the (i,j)th  matrix element    val
  virtual void SetElem(int i, int j, T val) = 0;
  // Sets the matrix elements based on the array arr
  virtual void MatImport(T *arr) = 0;
  // TODO(P0): Add implementation
  virtual ~Matrix() { delete[] linear; }
};

template <typename T>
class RowMatrix : public Matrix<T> {
 public:
  using Matrix<T>::Matrix;

  // TODO(P0): Add implementation
  RowMatrix(int r, int c) : Matrix<T>(r, c){
    this->rows = r;
    this->cols=c;

    data_ = new T *[r];
    for( int i=0; i<r; i++ )
    {
        data_[i]= new T[c];
    }

  }

  // TODO(P0): Add implementation
  int GetRows() override {return this->rows;}
  // TODO(P0): Add implementation
  int GetColumns() override { return this->cols; }

  T GetElem(int i, int j) override {
    return this->data_[i][j];
  }

  // TODO(P0): Add implementation
  void SetElem(int i, int j, T val) override {
    this->data_[i][j] =val;

  }

  // TODO(P0): Add implementation
  void MatImport(T *arr) override {
    int rows = this->rows;
    int cols = this->cols;

    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        this->SetElem(i,j,*(arr +i*cols + j));
        this->linear[i*cols+j]=*(arr +i*cols + j);
      }
    }


  }

  // TODO(P0): Add implementation
  ~RowMatrix() override {

    delete[] data_;
  };

 public:
  // 2D array containing the elements of the matrix in row-major format
  // TODO(P0): Allocate the array of row pointers in the constructor. Use these pointers
  // to point to corresponding elements of the 'linear' array.
  // Don't forget to free up the array in the destructor.
  T **data_;
};

template <typename T>
class RowMatrixOperations {
 public:
  // Compute (mat1 + mat2) and return the result.
  // /Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> AddMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                   std::unique_ptr<RowMatrix<T>> mat2) {
    // TODO(P0): Add code

    RowMatrix<T> rm1 =*mat1;
    RowMatrix<T> rm2 =*mat2;

    int rows1 = rm1.GetRows();
    int cols1 = rm1.GetColumns();
    int rows2 = rm2.GetRows();
    int cols2 = rm2.GetColumns();
    if (!(rows1==rows2&&cols2==cols1)){
      return std::unique_ptr<RowMatrix<T>>(nullptr);
    }
    std::unique_ptr<RowMatrix<T>> matTemp{new RowMatrix<int>(rm1.GetRows(), rm2.GetColumns())};
    for (int i = 0; i < rows1; ++i) {                                                                                                                                                                                                     
      for (int j = 0; j < cols1; ++j) {
        matTemp->SetElem(i,j,(rm1.GetElem(i,j)+rm2.GetElem(i,j)));
      }
    }
    mat1.release();
    mat2.release();
    return matTemp;
  }

  // Compute matrix multiplication (mat1 * mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> MultiplyMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                        std::unique_ptr<RowMatrix<T>> mat2) {
    // TODO(P0): Add code
    RowMatrix<T> rm1 =*mat1;
    RowMatrix<T> rm2 =*mat2;
    std::unique_ptr<RowMatrix<T>> matTemp{new RowMatrix<int>(rm1.GetRows(), rm2.GetColumns())};
    int rows1 = rm1.GetRows();
    int cols1 = rm1.GetColumns();
    int rows2 = rm2.GetRows();
    int cols2 = rm2.GetColumns();
    if (!(rows1==cols2&&rows2==cols1)){
      return std::unique_ptr<RowMatrix<T>>(nullptr);
    }
    int multiResult=0;
    for (int i = 0; i < rows1; ++i) {
      for (int j = 0; j < cols2; ++j) {
        for (int k = 0; k < cols1; ++k) {
          multiResult+=rm1.GetElem(i,k)*rm2.GetElem(k,j);
        }
        matTemp->SetElem(i,j,multiResult);
        multiResult=0;
      }
    }
    mat1.release();
    mat2.release();
    return matTemp;
  }

  // Simplified GEMM (general matrix multiply) operation
  // Compute (matA * matB + matC). Return nullptr if dimensions mismatch for input matrices
  static std::unique_ptr<RowMatrix<T>> GemmMatrices(std::unique_ptr<RowMatrix<T>> matA,
                                                    std::unique_ptr<RowMatrix<T>> matB,
                                                    std::unique_ptr<RowMatrix<T>> matC) {
    // TODO(P0): Add code

    RowMatrix<T> rm1 =*(matA.release());
    RowMatrix<T> rm2 =*(matB.release());
    RowMatrix<T> rm3 =*(matC.release());


    int rows1 = rm1.GetRows();
    int cols1 = rm1.GetColumns();
    int rows2 = rm2.GetRows();
    int cols2 = rm2.GetColumns();
    int rows3 = rm3.GetRows();
    int cols3 = rm3.GetColumns();
    if ((!(rows1==cols2&&rows2==cols1))&&(!(rows1==cols3&&cols2==cols3))) {
      return std::unique_ptr<RowMatrix<T>>(nullptr);
    }
    for (int i = 0; i < rows1; ++i) {
      for (int j = 0; j < rows2; ++j) {

        rm1.SetElem(i,j,rm1.GetElem(i,j)*rm2.GetElem(j,i));
      }
    }
    for (int i = 0; i < rows1; ++i) {
      for (int j = 0; j < rows2; ++j) {

        rm1.SetElem(i,j,rm1.GetElem(i,j)+rm3.GetElem(i,j));
      }
    }
    matA.reset(&rm1);
    return matA;
  }
};
}  // namespace bustub
