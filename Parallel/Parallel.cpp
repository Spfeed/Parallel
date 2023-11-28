#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

// Тип для матрицы
typedef vector<vector<double>> Matrix;

// Тип для вектора
typedef vector<double> Vector;

// Функция для создания и заполнения матрицы случайными значениями
Matrix createRandomMatrix(int rows, int cols) {
    Matrix mat(rows, Vector(cols));
#pragma omp parallel for
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = static_cast<double>(rand()) / RAND_MAX;
        }
    }
    return mat;
}

// Функция для вывода матрицы
void printMatrix(const Matrix& mat) {
    for (const auto& row : mat) {
        for (double value : row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

// Функция для вывода вектора
void printVector(const Vector& vec) {
    for (double value : vec) {
        cout << value << " ";
    }
    cout << endl;
}

// Функция для вычисления выражения b * (A - C) * b^T
Matrix calculateExpression(const Vector& b, const Matrix& A, const Matrix& C) {
    int rows_A = A.size();
    int cols_A = A[0].size();
    int rows_b = b.size();

    Matrix result(rows_b, Vector(rows_b, 0.0));

#pragma omp parallel for
    for (int i = 0; i < rows_b; ++i) {
        for (int j = 0; j < rows_b; ++j) {
            double sum = 0.0;
            for (int k = 0; k < cols_A; ++k) {
                sum += b[k] * (A[k][j] - C[k][j]);
            }
            result[i][j] = sum;
        }
    }

    return result;
}

int main() {
    setlocale(LC_ALL, "ru");

    srand(static_cast<unsigned int>(time(0)));

    // Задаем размерности матриц и вектора
    int rows_A = 3, cols_A = 3;
    int rows_b = 3, cols_b = 1;

    // Создаем и заполняем матрицы A, C и вектор b случайными значениями
    Matrix A = createRandomMatrix(rows_A, cols_A);
    Matrix C = createRandomMatrix(rows_A, cols_A);
    Vector b(rows_b);
#pragma omp parallel for
    for (int i = 0; i < rows_b; ++i) {
        b[i] = static_cast<double>(rand()) / RAND_MAX;
    }

    // Вывод начальных матриц и векторов
    cout << "Матрица A:\n";
    printMatrix(A);

    cout << "\nМатрица C:\n";
    printMatrix(C);

    cout << "\nВектор b:\n";
    printVector(b);

    // Последовательное вычисление
    Matrix result_sequential = calculateExpression(b, A, C);
    cout << "\nРезультат (последовательно):\n";
    printMatrix(result_sequential);

    // Параллельное вычисление
    Matrix result_parallel = calculateExpression(b, A, C);
    cout << "\nРезультат (параллельно):\n";
    printMatrix(result_parallel);

    return 0;
}
