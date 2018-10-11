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
    Matrix2D(int rows, int cols) :
        m_data(std::vector<std::vector<RealType>>(rows, std::vector<RealType>(cols, 0)))
    {
        if (rows == 0 || cols == 0)
        {
            throw std::invalid_argument("Cannot create matrix with 0 rows or 0 columns!");
        }
    }

    size_t size() const { return m_data.size(); }

    void print(std::ostream& os)
    {
        for (auto row : m_data)
        {
            bool first = true;
            for (auto cell : row)
            {
                os << (first ? "" : " ") << cell;
            }
            os << std::endl;
        }
    }

    Matrix2D naiveMultiplication(const Matrix2D& other)
    {
        Matrix2D result(m_data.size(), other.m_data[0].size());
        size_t size = m_data.size();
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                for (int k = 0; k < size; k++)
                {
                    result.m_data[i][j] += m_data[i][k] * other.m_data[k][i];
                }
            }
        }
        return result;
    }

    Matrix2D cacheEfficientMultiplication(const Matrix2D& other)
    {
        Matrix2D result(m_data.size(), other.m_data[0].size());
        size_t size = m_data.size();
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                for (int k = 0; k < size; k++)
                {
                    result.m_data[i][j] += m_data[i][k] * other.m_data[k][i];
                }
            }
        }

        return result;
    }
};

#endif // MATRIX2D_HPP
