#ifndef MATRIX2D_HPP
#define MATRIX2D_HPP

#include <iostream>
#include <vector>

using RealType = float;

class Matrix2D
{
private:
    std::vector<std::vector<RealType>> m_data;

public:
    Matrix2D() = default;

    Matrix2D(int rows, int cols) :
        m_data(std::vector<std::vector<RealType>>(rows, std::vector<RealType>(cols, 0)))
    {
        if (rows == 0 || cols == 0)
        {
            throw std::invalid_argument("Cannot create matrix with 0 rows or 0 columns!");
        }
    }

    size_t size() const { return m_data.size(); }

    bool operator==(const Matrix2D& rhs){ return m_data == rhs.m_data; }
    bool operator!=(const Matrix2D& rhs){ return !(*this == rhs); }

    std::vector<RealType>& operator[](int row)
    {
        return m_data[row];
    }

    void print(std::ostream& os)
    {
        for (auto row : m_data)
        {
            bool first = true;
            for (auto cell : row)
            {
                os << (first ? "" : " ") << cell;
                first = false;
            }
            os << std::endl;
        }
    }

    Matrix2D naiveMultiplication(const Matrix2D& other) const
    {
        Matrix2D result(m_data.size(), m_data.size());
        size_t size = m_data.size();
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                for (int k = 0; k < size; k++)
                {
                    result.m_data[i][j] += m_data[i][k] * other.m_data[k][j];
                }
            }
        }
        return result;
    }

    Matrix2D cacheEfficientMultiplication(const Matrix2D& other) const
    {
        Matrix2D result(m_data.size(), other.m_data[0].size());
        size_t size = m_data.size();
        constexpr int BLOCK_SIZE = 4;
        const int blockCount = size / BLOCK_SIZE;
        int subMatrixA[BLOCK_SIZE][BLOCK_SIZE];
        int subMatrixB[BLOCK_SIZE][BLOCK_SIZE];

        for (int resBlockY = 0; resBlockY < blockCount; resBlockY++)
        {
            for (int resBlockX = 0; resBlockX < blockCount; resBlockX++)
            {
                for (int blockIndex = 0; blockIndex < blockCount; blockIndex++)
                {
                    for (int i = 0; i < BLOCK_SIZE; i++)
                    {
                        for (int j = 0; j < BLOCK_SIZE; j++)
                        {
                            subMatrixA[i][j] = m_data[resBlockY * BLOCK_SIZE + i][blockIndex * BLOCK_SIZE + j];
                            subMatrixB[i][j] = other.m_data[blockIndex * BLOCK_SIZE + i][resBlockX * BLOCK_SIZE + j];
                        }
                    }

                    for (int i = 0; i < BLOCK_SIZE; i++)
                    {
                        for (int j = 0; j < BLOCK_SIZE; j++)
                        {
                            RealType sum = 0;
                            for (int k = 0; k < BLOCK_SIZE; k++)
                            {
                                sum += subMatrixA[i][k] * subMatrixB[k][j];
                            }
                            result.m_data[resBlockY * BLOCK_SIZE + i][resBlockX * BLOCK_SIZE + j] += sum;
                        }
                    }
                }
            }
        }

        return result;
    }

    Matrix2D transposedMultiplication(const Matrix2D& other) const
    {
        Matrix2D result(m_data.size(), other.m_data[0].size());
        size_t size = m_data.size();

        Matrix2D transposed(size, size);
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                transposed.m_data[i][j] = other.m_data[j][i];
            }
        }

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                RealType sum = 0;
                for (int k = 0; k < size; k++)
                {
                    sum += m_data[i][k] * transposed.m_data[j][k];
                }
                result.m_data[i][j] = sum;
            }
        }

        return result;
    }
};

#endif // MATRIX2D_HPP
