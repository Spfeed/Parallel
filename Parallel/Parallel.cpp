#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");

    int m, n;
    cout << "Введите размерность матрицы: m, n" << endl;
    cin >> m >> n;

    int** A = new int* [m];

    for (int i = 0; i < m; i++) {
        A[i] = new int[n];
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = i + j + 1;
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

    int sum = 0;

#pragma omp parallel num_threads(m)
    {
        int thread_id = omp_get_thread_num(); // Получаем номер текущей нити

        // Суммируем элементы строки, если номер строки кратен номеру нити
#pragma omp for reduction(+:sum)
        for (int i = 0; i < m; i++) {
            cout << i + 1 << " " << thread_id + 1 << endl;
            if ((i + 1) % (thread_id + 1) == 0) {
                for (int j = 0; j < n; j++) {
                    sum += A[i][j];
                }
            }
        }
    }

    // Вывод результата
    std::cout << "Сумма элементов строк, кратных номеру нити: " << sum << std::endl;

    // Очистка памяти
    for (int i = 0; i < m; i++) {
        delete[] A[i];
    }
    delete[] A;

    return 0;
}
