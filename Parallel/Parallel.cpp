#include <omp.h>
#include <iostream>
#include <chrono>
#include <fstream>
using namespace std;

//инициализация матрицы, векторов

void ItemsInit(double** Matrix, double* Vector, double* Result, double* RowsRes, double* ColumnsRes, double* BlocksRes, int size) {

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			Matrix[i][j] = i + j + 2;
		}
	}

	for (int i = 0; i < size; i++) {
		Vector[i] = i + 1;
	}

	for (int i = 0; i < size; i++){
		Result[i] = 0;
		RowsRes[i] = 0;
		ColumnsRes[i] = 0;
		BlocksRes[i] = 0;
	}
}

//Стандартное умножение матрицы на вектор

void SerialProduct(double** Matrix, double* Vector, double* Result, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			Result[i] += Matrix[i][j] * Vector[j];
		}
	}
}

//Параллельное умножение по строкам

void ParallelProductRows(double** Matrix, double* Vector, double* RowsRes, int size) {
	int i, j;
#pragma omp parallel for private (j)
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			RowsRes[i] += Matrix[i][j] * Vector[j];
		}
	}
}

//Параллельное умножение по столбцам

void ParallelProductColumns(double** Matrix, double* Vector, double* ColumnsRes, int size) {
	double IterSum;
	for (int i = 0; i < size; i++) {
		IterSum = 0;
#pragma omp parallel for reduction (+:IterSum)
		for (int j = 0; j < size; j++) {
			IterSum += Matrix[j][i] * Vector[j];
		}
		ColumnsRes[i] = IterSum;
	}
}

//Параллельное умножение по блокам

void ParallelProductBlocks(double** pMatrix, double* pVector, double* blocksResult, int size) {
	int ThreadID;
	int GridThreadsNum = 4;
	int GridSize = int(sqrt(double(GridThreadsNum)));
	int BlockSize = size / GridSize;

#pragma omp parallel for private(ThreadID)
	for (int l = 0; l < 4; l++)
	{
		ThreadID = omp_get_thread_num();
		double* pThreadResult = new double[BlockSize];
		for (int i = 0; i < BlockSize; i++)
			pThreadResult[i] = 0;

		int i_start = (int(ThreadID / GridSize)) * BlockSize;
		int j_start = (ThreadID % GridSize) * BlockSize;

		for (int i = 0; i < BlockSize; i++) {
			for (int j = 0; j < BlockSize; j++)
				pThreadResult[i] += pMatrix[i + i_start][j + j_start] * pVector[j + j_start];
		}

#pragma omp critical
		for (int i = 0; i < BlockSize; i++)
			blocksResult[i + i_start] += pThreadResult[i];

		delete[] pThreadResult;
	}
}

void ProcessPrint(double* Result, int size) {
	cout << "Result [";
	for (int i = 0; i < size - 1; i++) {
		cout << Result[i] << ", ";
	}
	cout << Result[size - 1] << "]" << endl;
}

void ProcessTerminate(double** pMatrix, double* pVector, double* Result, double* rowsResult,
	double* columnsResult, double* blocksResult, int size) {
	for (int i = 0; i < size; i++) {
		delete[] pMatrix[i];
	}
	delete[] pMatrix;
	delete[] pVector;
	delete[] Result;
	delete[] rowsResult;
	delete[] columnsResult;
	delete[] blocksResult;
}

int main()
{
	setlocale(LC_CTYPE, "Russian");
	int size;
	cout << "Введите размер матрицы";
	cin >> size;

	double** Matrix = new double * [size];
	double* Vector = new double[size];
	double* Result = new double[size];
	double* RowsRes = new double[size];
	double* ColumnsRes = new double[size];
	double* BlocksRes = new double[size];

	for (int i = 0; i < size; i++) {
		Matrix[i] = new double[size];
	}

	ItemsInit(Matrix, Vector, Result, RowsRes, ColumnsRes, BlocksRes, size);

	auto start_time = chrono::high_resolution_clock::now();
	SerialProduct(Matrix, Vector, Result, size);
	auto end_time = chrono::high_resolution_clock::now();
	cout<<"Время работы последовательного алгоритма: "<<chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / pow(10, 9) << " сек" << endl;
	cout << "Вектор после умножения на матрицу:" << endl;
	//ProcessPrint(Result, size);

	start_time = chrono::high_resolution_clock::now();
	ParallelProductRows(Matrix, Vector, RowsRes, size);
	end_time = chrono::high_resolution_clock::now();
	cout << "Время работы алгоритма умножения на строки: " << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count()/pow(10,9) << " сек" << endl;
	cout << "Вектор после умножения на матрицу:" << endl;
	//ProcessPrint(Result, size);

	start_time = chrono::high_resolution_clock::now();
	ParallelProductColumns(Matrix, Vector, ColumnsRes, size);
	end_time = chrono::high_resolution_clock::now();
	cout << "Время работы алгоритма умножения на столбцы: " << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / pow(10, 9) << " сек" << endl;
	cout << "Вектор после умножения на матрицу:" << endl;
	//ProcessPrint(Result, size);

	start_time = chrono::high_resolution_clock::now();
	ParallelProductBlocks(Matrix, Vector, BlocksRes, size);
	end_time = chrono::high_resolution_clock::now();
	cout << "Время работы алгоритма умножения на блоки: " << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / pow(10, 9) << " сек" << endl;
	cout << "Вектор после умножения на матрицу:" << endl;
	//ProcessPrint(Result, size);



}