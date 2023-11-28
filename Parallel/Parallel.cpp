#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");

    int n, m;
    cout << "Введите размерность матрицы: n, m" << endl;
    cin >> n >> m;

    int** A = new int* [n];

    for (int i = 0; i < n; i++) {
        A[i] = new int[m];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            A[i][j] = i + j + 1;
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

    int sum = 0;

#pragma omp parallel num_threads(m)
    {
        int thread_id = omp_get_thread_num(); // Получаем номер текущей нити
        int i = thread_id + 1; // Номер нити (индексация начинается с 0)

        // Локальная переменная для хранения суммы текущей нити
        int local_sum = 0;

        // Проверка, что столбец кратен номеру нити
        if (i <= m && m % i == 0) {
            // Суммируем элементы столбца
#pragma omp parallel for reduction(+:local_sum)
            for (int j = 0; j < n; ++j) {
                local_sum += A[j][i - 1];
            }
        }

        // Критическая секция для обновления общей суммы
#pragma omp critical
        sum += local_sum;
    }

    // Вывод результата
    std::cout << "Сумма элементов столбцов, кратных номеру нити: " << sum << std::endl;

    // Очистка памяти
    for (int i = 0; i < n; i++) {
        delete[] A[i];
    }
    delete[] A;

    return 0;
}
