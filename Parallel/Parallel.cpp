#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

void fillMatrix(vector<vector<int>>& matrix, int k, int n) {
#pragma omp parallel for collapse(2)
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            int value = min(min(i, j), min(k - 1 - i, n - 1 - j)) + 1;//значение вычисляется как расстояние до границы матрицы+1
            matrix[i][j] = value;
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    int k, n;
    cout << "Введите размерность матрицы k, n: ";
    cin >> k >> n;

    // Создание матрицы
    vector<vector<int>> matrix(k, vector<int>(n, 0));

    // Заполнение матрицы
    fillMatrix(matrix, k, n);

    // Вывод матрицы
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
