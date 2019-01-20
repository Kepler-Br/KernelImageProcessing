#ifndef MATRIXMISC_H
#define MATRIXMISC_H

#include <string>
#include <fstream>

#include "matrix.h"

namespace MatAlg
{

/*!
   \brief function that summs every element of matrix.
*/
template<typename Type>
Type elementSum(const Matrix<Type> &filter)
{
    double div = 0.0;
    for(uint x = 0; x < filter.getRowSize(); x++)
    {
        for(uint y = 0; y < filter.getColumnSize(); y++)
        {
            div += filter[x][y];
        }
    }
    return div;
}

/*!
   \brief function that multiplies every element of
    matrix with every element of another matrix.
*/
template<typename Type>
Type elementMul(const Matrix<Type> &first, const Matrix<Type> &second)
{
    double result = 0.0;
    uint columns = first.getColumnSize();
    uint rows = first.getRowSize();
    for(uint x = 0; x < rows; x++)
    {
        for(uint y = 0; y < columns; y++)
        {
            result += first[x][y]*second[x][y];
        }
    }
    return result;
}

/*!
   \brief save matrix to text file.
*/
template<typename Type>
void saveMatrix(const std::string &fileName, const Matrix<Type> &matrix)
{
    std::ofstream textFile;
    textFile.open(fileName);
    textFile << matrix.getRowSize() << " ";
    textFile << matrix.getColumnSize() << " ";
    for(std::size_t row = 0; row < matrix.getRowSize(); row++)
    {
        for(std::size_t column = 0; column < matrix.getColumnSize(); column++)
        {
            textFile << matrix[row][column] << " ";
        }
    }
    textFile.close();
}

/*!
   \brief load matrix from text file.
*/
template<typename Type>
Matrix<Type> loadMatrix(const std::string &fileName)
{
    std::ifstream textFile;
    textFile.open(fileName);

    std::size_t rows = 0;
    std::size_t columns = 0;
    textFile >> rows;
    textFile >> columns;
    if(textFile.fail())
        throw std::logic_error("Error reading rows/columns.");
    if(rows == 0 || columns == 0)
        throw std::logic_error("Error reading matrix from file (0 rows/columns)");
    Matrix<Type> matrix(rows, columns, Type(0.0));
    for(std::size_t row = 0; row < rows; row++)
    {
        for(std::size_t column = 0; column < columns; column++)
        {
            if(textFile.eof())
                throw std::logic_error("Unexpected end of matrix file.");
            if(textFile.fail())
                throw std::logic_error("Error reading matrix elements.");
            textFile >> matrix[row][column];
        }
    }
    textFile.close();
    return matrix;
}

/*!
   \brief convert matrix to human readable string.
*/
template <typename Type>
std::string toString(const Matrix<Type> &matrix)
{
    std::string output = "";
    for(std::size_t i = 0; i < matrix.getRowSize(); i++)
    {
        output += "[ ";
        for(std::size_t j = 0; j < matrix.getColumnSize(); j++)
        {
            output += std::to_string(matrix[i][j]);
            output += " ";
        }
        output += "]\n";
    }
    return output;
}

/*!
   \brief create matrix with every element equal to 1.
*/
template <typename Type>
Matrix<Type> eye(const uint dimension)
{
    Matrix<Type> eyeMatrix(dimension, dimension, Type(0.0));
    for(std::size_t i = 0; i < dimension; i++)
    {
       eyeMatrix[i][i] = Type(1.0);
    }
    return eyeMatrix;
}

template <typename Type>
Type dotProduct(const Matrix<Type> &l, const Matrix<Type> &r)
{
    Type product = Type(0.0);
    if(l.getColumnSize() == r.getColumnSize() && l.getColumnSize() > l.getRowSize())
    {
        for(std::size_t column = 0; column < l.getColumnSize(); column++)
        {
            product += l[0][column]*r[0][column];
        }
        return product;
    }
    else if(l.getRowSize() == r.getRowSize() && l.getRowSize() > l.getColumnSize())
    {
        for(std::size_t row = 0; row < l.getRowSize(); row++)
        {
            product += l[row][0]*r[row][0];
        }
        return product;
    }
    else
    {
        throw std::logic_error("Vectors should be same.");
    }
}

template <typename Type>
Matrix<Type> transpose(const Matrix<Type> &matrix)
{
    Matrix<Type> result(matrix.getColumnSize(), matrix.getRowSize(), Type(0.0));
    for(std::size_t row = 0; row < matrix.getRowSize(); row++)
    {
        for(std::size_t column = 0; column < matrix.getColumnSize(); column++)
        {
            result[column][row] = matrix[row][column];
        }
    }
    return result;
}
}

#endif // MATRIXMISC_H
