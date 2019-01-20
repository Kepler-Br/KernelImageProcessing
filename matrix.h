#ifndef PTRMATRIX_H
#define PTRMATRIX_H

#include <vector>
#include <ostream>

#include <string>

namespace MatAlg
{
template <typename Type>
class Matrix
{
private:
    uint rows;
    uint columns;
    std::vector<std::vector<Type>> matrix;

    /*!
       \brief check if row and column is in matrix range.
    */
    void rangeCheck(const uint &row, const uint &column) const
    {
        if(row >= this->rows)
            throw std::out_of_range("Matrix::rangeCheck: row (which is " + std::to_string(row) +") >= this->rows() (which is " + std::to_string(this->rows) + ")");
        if(column >= this->columns)
            throw std::out_of_range("Matrix::rangeCheck: column (which is " + std::to_string(column) +") >= this->columns() (which is " + std::to_string(this->columns) + ")");
    }

    /*!
       \brief check sanity of initializer list.
    */
    void checkInitListSize(const std::initializer_list<std::initializer_list<Type>> &initList)
    {
        uint columnSize = initList.begin()->size();
        for(auto row: initList)
            if(row.size() != columnSize)
                throw std::range_error("Matrix row size should be same in initializer list.");
    }

    /*!
       \brief init every matrix element with specific value.
    */
    void initMatrixValue(const uint row, const uint column, const Type value)
    {
        this->matrix.clear();
        this->rows = row;
        this->columns = column;
        for(uint i = 0; i < rows; i++)
        {
            std::vector<Type> row;
            for(uint j = 0; j < columns; j++)
            {
                row.push_back(value);
            }
            matrix.push_back(row);
        }
    }

public:
    Matrix(const uint &row,
           const uint &column,
           const Type &value):
        rows(row),
        columns(column)
    {
        initMatrixValue(row, column, value);
    }

    /*!
       \brief create matrix using initializer list. Easy!
    */
    Matrix(const std::initializer_list<std::initializer_list<Type>> &initList)
    {
        checkInitListSize(initList);
        rows = initList.size();
        columns = initList.begin()->size();

        for(const auto &row: initList)
        {
            std::vector<Type> matrixRow;
            for(const auto &element: row)
            {
                matrixRow.push_back(element);
            }
            matrix.push_back(row);
        }
    }

    /*!
       \brief copy matrix from another matrix.
    */
    Matrix(const Matrix &r)
    {
        rows = r.getRowSize();
        columns = r.getColumnSize();
        matrix.clear();
        for(uint row = 0; row < rows; row++)
        {
            std::vector<Type> matrixRow;
            for(uint column = 0; column < columns; column++)
            {
                matrixRow.push_back(r[row][column]);
            }
            matrix.push_back(matrixRow);
        }
    }

    uint getRowSize() const
    {
        return this->rows;
    }

    uint getColumnSize() const
    {
        return this->columns;
    }

    /*!
       \brief safe and slow way to get element.
    */
    const Type at(const uint &row, const uint &column) const
    {
        rangeCheck(row, column);
        return matrix[row][column];
    }

    /*!
       \brief safe and slow way to set element.
    */
    void assign(const uint &row, const uint &column, const Type &value)
    {
        rangeCheck(row, column);
        matrix[row][column] = value;
    }

    Type* operator[](const uint& row)
    {
        return &matrix[row][0];
    }

    const Type* operator[](const uint& row) const
    {
        return &matrix[row][0];
    }

    //Scalar operators
    void operator+=(const Type &r)
    {
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] += r;
            }
        }
    }

    void operator-=(const Type &r)
    {
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] -= r[i][j];
            }
        }
    }

    void operator/=(const Type &r)
    {
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] /= r;
            }
        }
    }

    void operator*=(const Type &r)
    {
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] *= r;
            }
        }
    }

    // Matrix operators
    void operator+=(const Matrix<Type>& r)
    {
        if(this->columns != r.getColumnSize() ||
                this->rows != r.getRowSize())
            throw std::range_error("Matrix::operator+=:  Matrices with different dimensions cannot be summed."
                                   "this dimensions (columns: " + this->getColumnSize() + ", rows: " + this->getRowSize() +") != "
                                   "r dimensions (columns: " + r.getColumnSize() + ", rows: " + r.getRowSize() +")");
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] += r[i][j];
            }
        }
    }

    void operator-=(const Matrix<Type>& r)
    {
        if(this->columns != r.getColumnSize() ||
                this->rows != r.getRowSize())
            throw std::range_error("Matrix::operator-=:  Matrices with different dimensions cannot be subtracted."
                                   "this dimensions (columns: " + this->getColumnSize() + ", rows: " + this->getRowSize() +") != "
                                   "r dimensions (columns: " + r.getColumnSize() + ", rows: " + r.getRowSize() +")");
        for(uint i = 0; i < columns; i++)
        {
            for(uint j = 0; j < rows; j++)
            {
                matrix[i][j] -= r[i][j];
            }
        }
    }

    void operator*=(const Matrix<Type>& r)
    {
        if(this->getColumnSize() != r.getRowSize())
            throw std::range_error("Matrix::operator*=: this row size(which is "+ std::to_string(this->getRowSize()) +
                                   ") should be == r column size(which is " + std::to_string(r.getColumnSize()) + ").");
        Matrix<Type> l = *this;
        if(rows != l.getRowSize() || columns != r.getColumnSize())
            initMatrixValue(l.getRowSize(), r.getColumnSize(), Type(0.0));

        for(uint row = 0; row < l.getRowSize(); row++)
        {
            for(uint rColumn = 0; rColumn < r.getColumnSize(); rColumn++)
            {
                int num = 0;
                for(uint rRow = 0; rRow < r.getRowSize(); rRow++)
                {
                    num += l[row][rRow] * r[rRow][rColumn];
                }
                matrix[row][rColumn] = num;
            }
        }
    }
};


//Scalar binary matrix operators
template<typename Type>
Matrix<Type> operator+(const Matrix<Type>& l, const Matrix<Type>& r)
{
    if(l.getColumnSize() != r.getColumnSize() ||
            l.getRowSize() != r.getRowSize())
        throw std::range_error("Matrix::operator+:  Matrices with different dimensions cannot be summed."
                               "l dimensions (columns: " + l.getColumnSize() + ", rows: " + l.getRowSize() +") != "
                               "r dimensions (columns: " + r.getColumnSize() + ", rows: " + r.getRowSize() +")");

    Matrix<Type> out = Matrix<Type>(l.getRowSize(), l.getColumnSize(), Type());
    for(uint i = 0; i < l.getColumnSize(); i++)
    {
        for(uint j = 0; j < l.getRowSize(); j++)
        {
            out[i][j] = l[i][j] + r[i][j];
        }
    }
    return out;
}

template <typename Type>
Matrix<Type> operator*(const Matrix<Type> &l, const Matrix<Type> &r)
{
    if(l.getColumnSize() != r.getRowSize())
        throw std::range_error("Matrix::operator*: l row size"
                               "(which is "+ std::to_string(l.getRowSize()) + ") should be == r "
                               "column size(which is " + std::to_string(r.getColumnSize()) + ").");

    Matrix<Type> result(l.getRowSize(), r.getColumnSize(), Type(0.0));
    for(uint row = 0; row < l.getRowSize(); row++)
    {
        for(uint rColumn = 0; rColumn < r.getColumnSize(); rColumn++)
        {
            int num = 0;
            for(uint rRow = 0; rRow < r.getRowSize(); rRow++)
            {
                num += l[row][rRow] * r[rRow][rColumn];
            }
            result[row][rColumn] = num;
        }
    }
    return result;
}

template <typename Type>
bool operator==(const Matrix<Type> &l, const Matrix<Type> &r)
{
    if(l.getColumnSize() != r.getColumnSize())
        return false;
    if(l.getRowSize() != r.getRowSize())
        return false;
    uint rowSize = l.getRowSize();
    uint columnSize = l.getColumnSize();
    for(uint row = 0; row < rowSize; row++)
    {
        for(uint column = 0; column < columnSize; column++)
        {
            if(l[row][column] != r[row][column])
                return false;
        }
    }
    return true;
}

template <typename Type>
bool operator!=(const Matrix<Type> &l, const Matrix<Type> &r)
{
    if(l.getColumnSize() != r.getColumnSize())
        return true;
    if(l.getRowSize() != r.getRowSize())
        return true;
    uint rowSize = l.getRowSize();
    uint columnSize = l.getColumnSize();
    for(uint row = 0; row < rowSize; row++)
    {
        for(uint column = 0; column < columnSize; column++)
        {
            if(l[row][column] != r[row][column])
                return true;
        }
    }
    return false;
}

//Scalar binary scalar operators
template <typename Type>
Matrix<Type> operator+(const Matrix<Type> &l, const Type &r)
{
    Matrix<Type> matrix = l;
    uint columns = matrix.getColumnSize();
    uint rows = matrix.getRowSize();
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < columns; j++)
        {
            matrix[i][j] += r;
        }
    }
    return matrix;
}

template <typename Type>
Matrix<Type> operator-(const Matrix<Type> &l, const Type &r)
{
    Matrix<Type> matrix = l;
    uint columns = matrix.getColumnSize();
    uint rows = matrix.getRowSize();
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < columns; j++)
        {
            matrix[i][j] -= r;
        }
    }
    return matrix;
}

template <typename Type>
Matrix<Type> operator/(const Matrix<Type> &l, const Type &r)
{
    Matrix<Type> matrix = l;
    uint columns = matrix.getColumnSize();
    uint rows = matrix.getRowSize();
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < columns; j++)
        {
            matrix[i][j] /= r;
        }
    }
    return matrix;
}

template <typename Type>
Matrix<Type> operator*(const Matrix<Type> &l, const Type &r)
{
    Matrix<Type> matrix = l;
    uint columns = matrix.getColumnSize();
    uint rows = matrix.getRowSize();
    for(uint i = 0; i < rows; i++)
    {
        for(uint j = 0; j < columns; j++)
        {
            matrix[i][j] *= r;
        }
    }
    return matrix;
}

}
#endif // PTRMATRIX_H
