/*********************************************************************
 * \file   Calc4x4.h
 * \brief  4x4行列の計算関数
 * 
 * \author Harukichimaru
 * \date   November 2024
 * \note   
 *********************************************************************/
//NOTE:04_02_09を見ると関数分けがわかるぞ

#pragma once
#include <stdexcept>
#include "Matrix4x4.h"

/**----------------------------------------------------------------------------
 * \brief  Add4x4 行列の加算
 * \param  m1 
 * \param  m2 
 * \return Matrix4x4
 * \note   
 */
inline Matrix4x4 Add4x4(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

/**----------------------------------------------------------------------------
 * \brief  Subtract4x4 行列の減算
 * \param  m1 
 * \param  m2 
 * \return Matrix4x4
 * \note   
 */
inline Matrix4x4 Subtract4x4(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

/**----------------------------------------------------------------------------
 * \brief  Multiply4x4 行列の乗算
 * \param  m1
 * \param  m2
 * \return Matrix4x4
 * \note
 */
inline Matrix4x4 Multiply4x4(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	// 行列の各成分を直接計算して結果を求める
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

	return result;
}


// 余因子行列の計算に必要なサブ行列を計算するヘルパー関数
inline Matrix4x4 Cofactor4x4(const Matrix4x4& matrix);

// 余因子行列の計算に必要な小行列式を計算するヘルパー関数
inline float Minor(const Matrix4x4& matrix, int row, int col);

/**----------------------------------------------------------------------------
 * \brief  Inverse4x4 逆行列を求める
 * \param  matrix 
 * \return Matrix4x4
 * \note   
 */
inline Matrix4x4 Inverse4x4(const Matrix4x4& matrix) {
	// 行列式を計算
	float det =
		matrix.m[0][0] * ( matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] -
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2] -
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] ) -
		matrix.m[0][1] * ( matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] -
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2] -
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] ) +
		matrix.m[0][2] * ( matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] +
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] ) -
		matrix.m[0][3] * ( matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2] +
			matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] +
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2] );

	// 行列式が0の場合は逆行列は存在しない
	if (det == 0) {
		throw std::runtime_error("Matrix is singular and cannot be inverted.");
	}

	Matrix4x4 cofactorMatrix = Cofactor4x4(matrix);
	Matrix4x4 adjugateMatrix;
	// 余因子行列の転置を求める
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			adjugateMatrix.m[i][j] = cofactorMatrix.m[j][i];
		}
	}

	Matrix4x4 inverseMatrix;
	// 逆行列を求める
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			inverseMatrix.m[i][j] = adjugateMatrix.m[i][j] / det;
		}
	}

	return inverseMatrix;
}

/**----------------------------------------------------------------------------
 * \brief  Cofactor4x4 余因子行列を求める
 * \param  matrix
 * \return 
 * \note   
 */
inline Matrix4x4 Cofactor4x4(const Matrix4x4& matrix) {
	Matrix4x4 cofactorMatrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float minor = Minor(matrix, i, j);
			// 余因子行列の符号付き小行列行列式
			cofactorMatrix.m[i][j] = ( ( i + j ) % 2 == 0 ? 1 : -1 ) * minor;
		}
	}
	return cofactorMatrix;
}

/**----------------------------------------------------------------------------
 * \brief  Minor 小行列式を求める
 * \param  matrix
 * \param  row
 * \param  col
 * \return 
 * \note   
 */
inline float Minor(const Matrix4x4& matrix, int row, int col) {
	float subMatrix[3][3];
	int subRow = 0;
	for (int i = 0; i < 4; i++) {
		if (i == row) continue;
		int subCol = 0;
		for (int j = 0; j < 4; j++) {
			if (j == col) continue;
			subMatrix[subRow][subCol] = matrix.m[i][j];
			subCol++;
		}
		subRow++;
	}

	return subMatrix[0][0] * ( subMatrix[1][1] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][1] ) -
		subMatrix[0][1] * ( subMatrix[1][0] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][0] ) +
		subMatrix[0][2] * ( subMatrix[1][0] * subMatrix[2][1] - subMatrix[1][1] * subMatrix[2][0] );
}

/**----------------------------------------------------------------------------
 * \brief  Transpose4x4 転置行列を求める
 * \param  matrix
 * \return 
 * \note   
 */
inline Matrix4x4 Transpose4x4(const Matrix4x4& matrix) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = matrix.m[j][i];
		}
	}
	return result;
}

/**----------------------------------------------------------------------------
 * \brief  Identity4x4 単位行列を生成する
 * \return 
 * \note   
 */
inline Matrix4x4 Identity4x4() {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (i == j) {
				result.m[i][j] = 1.0f;
			} else {
				result.m[i][j] = 0.0f;
			}
		}
	}
	return result;
}
