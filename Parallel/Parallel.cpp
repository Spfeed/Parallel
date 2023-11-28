#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

// Функция, для которой вычисляется интеграл
double f(double x) {
    return sin(x);
}

// Функция для вычисления интеграла методом Симпсона с использованием OpenMP
double integrateSimpson(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = f(a) + f(b);

#pragma omp parallel for reduction(+:sum)
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += (i % 2 == 0) ? 2 * f(x) : 4 * f(x);
    }

    return h / 3 * sum;
}

int main() {
    setlocale(LC_ALL, "ru");

    double a, b;
    int n;

    cout << "Введите нижний предел интегрирования (a): ";
    cin >> a;

    cout << "Введите верхний предел интегрирования (b): ";
    cin >> b;

    cout << "Введите количество разбиений (n): ";
    cin >> n;

    // Вычисление интеграла
    double result = integrateSimpson(a, b, n);

    // Вывод результата
    cout << "Значение интеграла: " << result << endl;

    return 0;
}
