#include <omp.h>
#include <iostream>
#include <chrono>
using namespace std;


using namespace std;

double func(double x) {
	return 1.0 / (16 - sin(pow(x, 4)* sin(pow(x, 4))));
}

int main()
{
	setlocale(LC_CTYPE, "Russian");
	//Входные данные
	double a = -1, b = 1, num_threads, num_threads1, single, parallel, parallel1;
	int N = 1, i;
	double eps = 1.0E-8;
	cout << "Входные данные: " << endl;
	cout << " Функция f(x) = 1 / 16-x^4" << endl;
	cout << "Нижний предел интегрирования a = " << a << endl;
	cout << "Верзний предел интегрирования b = " << b << endl;
	cout << "Точность вычислений eps = " << eps << endl;
	double summa_2N = 0;
	double summa_N;

	//последовательный алгоритм

	auto start_time = chrono::high_resolution_clock::now();
	do {
		summa_N = summa_2N;
		summa_2N = func(a);
		double h = (b - a) / N;
		for (i = 1; i < N; i++) {
			summa_2N += func(a + i * h) * 2;
		}
		summa_2N += func(b);
		summa_2N *= h / 2;
		N *= 2;
	} while (
		(fabs((summa_N - summa_2N) / 3)) > eps
		);
	auto end_time = chrono::high_resolution_clock::now();
	single = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
	cout << "Время работы последовательного алгоритма: " << single/pow(10,6) << " секунд" << endl;

	//параллельный алгоритм
	summa_2N = 0;
	N = 1;
	start_time = chrono::high_resolution_clock::now();
	do {
		summa_N = summa_2N;
		summa_2N = func(a);
		double h = (b - a) / N;
#pragma omp parallel shared(a,h,N) private(i) reduction(+:summa_2N)
		{
#pragma omp sections nowait
			{
#pragma omp section 
				for (i = 1; i < N / 4; i++) {
					summa_2N += func(a + i * h) * 2;
				}
#pragma omp section
				for (i = N / 4; i < N / 2; i++) {
					summa_2N += func(a + i * h) * 2;
				}
#pragma omp section
				for (i = N / 2; i < N * 3 / 4; i++) {
					summa_2N += func(a + i * h) * 2;
				}
#pragma omp section
				for (i = N * 3 / 4; i < N; i++) {
					summa_2N += func(a + i * h) * 2;
				}
			}
			num_threads = omp_get_num_threads();
		}
		summa_2N += func(b);
		summa_2N *= h / 2;
		N *= 2;
	} while (
		(fabs((summa_N - summa_2N) / 3)) > eps);

	end_time = chrono::high_resolution_clock::now();
	parallel = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
	cout << "Время работы параллельного алгоритма с секциями: " << parallel/pow(10,6) << " секунд" << endl;


	//второй способ

	summa_2N = 0;
	N = 1;
	start_time = chrono::high_resolution_clock::now();

	do {
		summa_N = summa_2N;
		summa_2N = func(a);
		double h = (b - a) / N;
#pragma omp parallel for default(shared) private(i) reduction(+:summa_2N)
		for (i = 1; i < N; i++) {
			summa_2N += func(a + i * h) * 2;
			num_threads1 = omp_get_num_threads();
		}


		summa_2N += func(b);
		summa_2N *= h / 2;
		N *= 2;
	} while (
		(fabs((summa_N - summa_2N) / 3)) > eps
		);

	end_time = chrono::high_resolution_clock::now();
	parallel1 = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
	cout << "Время работы параллельного алгоритма цикла: " << parallel1/pow(10,6) << " секунд" << endl;

	cout << "Ускорение первого алгоритма: " << single / parallel << endl;
	cout << "Эффективность первого алгоритма: " << (single / parallel) / num_threads << endl;

	cout << "Ускорение второго алгоритма: " << single / parallel1 << endl;
	cout << "Эффективность второго алгоритма: " << (single / parallel1) / num_threads1 << endl;

	cout << "Результат вычислений: " << summa_2N;
}