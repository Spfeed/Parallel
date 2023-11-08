#include <omp.h>
#include <iostream>
#include <chrono>
#include <fstream>
using namespace std;

//инициализация матрицы, векторов

void ItemsInit(double** Matrix, double** Matrix2, double** Result, double** ParallelRes, double**BlockRes ,int size) {

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			Matrix[i][j] = i + j + 2;
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			Matrix2[i][j] = i + 1;
		}
	}

	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++) {
			Result[i][j] = 0;
			ParallelRes[i][j] = 0;
			BlockRes[i][j] = 0;
		}
	}

	
}

//Стандартное умножение матрицы на вектор

void SerialProduct(double** Matrix, double** Matrix2, double** Result, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				Result[i][j] += Matrix[i][k] * Matrix2[k][j];
			}
		}
	}
}

//Параллельное умножение по строкам

void ParallelProduct(double** Matrix, double** Matrix2, double** ParallelRes, int size) {
	int k;
#pragma omp parallel for private (k)
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				ParallelRes[i][j] += Matrix[i][k] * Matrix2[k][j];
			}
		}
	}
}

//Параллельное умножение по блокам

void ParallelProductBlocks(double** pMatrix, double** Matrix2, double** BlocksRes, int size) {
	int GridThreadsNum = 4;
	int GridSize = int(sqrt(double(GridThreadsNum)));
	int BlockSize = size / GridSize;
	omp_set_num_threads(GridThreadsNum);
	int ThreadID;

#pragma omp parallel for private(ThreadID)
	for (int l = 0; l < 4; l++)
	{
		ThreadID = omp_get_thread_num();
		ThreadID = omp_get_thread_num();
		int i_start = (int(ThreadID / GridSize)) * BlockSize;
		int j_start = (ThreadID % GridSize) * BlockSize;

		for (int i = i_start; i < i_start + BlockSize; ++i) {
			for (int j = j_start; j < j_start + BlockSize; ++j) {
				double sum = 0.0;
				for (int k = 0; k < size; ++k) {
					sum += pMatrix[i][k] * Matrix2[k][j];
				}
#pragma omp atomic
				BlocksRes[i][j] += sum;
			}
		}
	}
}

void ProcessPrint(double** Result, int size) {
	cout << "Result:" << endl;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cout << Result[i][j] << " ";
		}
		cout << endl;
	}
}

void ProcessTerminate(double** pMatrix, double** pMatrix2, double** Result, double** paralResult,
	double** blocksResult, int size) {
	for (int i = 0; i < size; i++) {
		delete[] pMatrix[i];
		delete[] pMatrix2[i];
		delete[] Result[i];
		delete[] paralResult[i];
		delete[] blocksResult[i];
	}
	delete[] pMatrix;
	delete[] pMatrix2;
	delete[] Result;
	delete[] paralResult;
	delete[] blocksResult;
}

int main()
{
	setlocale(LC_CTYPE, "Russian");
	int size;
	cout << "Введите размер матрицы";
	cin >> size;

	double** Matrix = new double * [size];
	double** Matrix2 = new double * [size];
	double** Result = new double * [size];
	double** ParallelRes = new double * [size];
	double** BlocksRes = new double * [size];

	for (int i = 0; i < size; i++) {
		Matrix[i] = new double[size];
		Matrix2[i] = new double[size];
		Result[i] = new double[size];
		ParallelRes[i] = new double[size];
		BlocksRes[i] = new double[size];
	}

	ItemsInit(Matrix, Matrix2, Result, ParallelRes, BlocksRes, size);

	auto start_time = chrono::high_resolution_clock::now();
	SerialProduct(Matrix, Matrix2, Result, size);
	auto end_time = chrono::high_resolution_clock::now();
	cout<<"Время работы последовательного алгоритма: "<<chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / pow(10, 9) << " сек" << endl;
	cout << "Матрицы после умножения: " << endl;
	//ProcessPrint(Result, size);

	start_time = chrono::high_resolution_clock::now();
	ParallelProduct(Matrix, Matrix2, ParallelRes, size);
	end_time = chrono::high_resolution_clock::now();
	cout << "Время работы алгоритма параллельного умножения: " << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count()/pow(10,9) << " сек" << endl;
	cout << "Матрицы после умножения: " << endl;
	//ProcessPrint(Result, size);


	start_time = chrono::high_resolution_clock::now();
	ParallelProductBlocks(Matrix, Matrix2, BlocksRes, size);
	end_time = chrono::high_resolution_clock::now();
	cout << "Время работы алгоритма умножения по блокам: " << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / pow(10, 9) << " сек" << endl;
	cout << "Матрицы после умножения: " << endl;
	//ProcessPrint(Result, size);



}