#include <algorithm>

#include "mth.h"

using namespace mth;

double mth::Rnd0() { return 1. * rand() / RAND_MAX; }
double mth::Rnd1() { return 2. * rand() / RAND_MAX - 1; }

vec2& vec2::operator+=(const vec2& v)
{
	*this = *this + v;
	return *this;
}

vec2 & vec2::operator-=( const vec2 &v )
{
	*this = *this - v;
	return *this;
}

vec2 & vec2::operator*=( const vec2 &v )
{
	*this = *this * v;
	return *this;
}

vec2 mth::vec2::normal() const
{
	double len = *this & *this;
	if (len != 0 && len != 1)
	{
		len = sqrt(len);
		return vec2(m_x / len, m_y / len);
	}
	return *this;
}

vec2 & mth::vec2::normalize()
{
	double len = *this & *this;
	if (len != 0 && len != 1)
	{
		len = sqrt(len);
		m_x /= len;
		m_y /= len;
	}
	return *this;
}

vec& mth::vec::operator+=(const vec& v)
{
	*this = *this + v;
	return *this;
}

vec& mth::vec::operator-=(const vec& v)
{
	*this = *this - v;
	return *this;
}

vec& mth::vec::operator*=(const vec& v)
{
	*this = *this * v;
	return *this;
}

vec vec::normal() const
{
	double len = *this & *this;
	if (len != 0)
	{
		len = sqrt(len);
		return vec(m_x / len, m_y / len, m_z / len);
	}
	return *this;
}

vec& vec::normalize()
{
	double len = *this & *this;
	if (len != 0 && len != 1)
	{
		len = sqrt(len);
		m_x /= len;
		m_y /= len;
		m_z /= len;
	}
	return *this;
}

vec vec::clamp( double min, double max ) const
{
	return vec(
			m_x < min ? min : m_x > max ? max : m_x,
			m_y < min ? min : m_y > max ? max : m_y,
			m_z < min ? min : m_z > max ? max : m_z);
}

vec vec::rnd() { return vec(rand() * 2.0 / RAND_MAX - 1, rand() * 2.0 / RAND_MAX - 1, rand() * 2.0 / RAND_MAX - 1); }

matr::matr( double A00, double A01, double A02, double A03,
		double A10, double A11, double A12, double A13,
		double A20, double A21, double A22, double A23,
		double A30, double A31, double A32, double A33 )
{
	m_data[0][0] = A00, m_data[0][1] = A01, m_data[0][2] = A02, m_data[0][3] = A03;
	m_data[1][0] = A10, m_data[1][1] = A11, m_data[1][2] = A12, m_data[1][3] = A13;
	m_data[2][0] = A20, m_data[2][1] = A21, m_data[2][2] = A22, m_data[2][3] = A23;
	m_data[3][0] = A30, m_data[3][1] = A31, m_data[3][2] = A32, m_data[3][3] = A33;
}

matr matr::identity()
{
	return matr(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

matr matr::scale(vec s)
{
	return matr(
		s.m_x,	0,		0,		0,
		0,		s.m_y,	0,		0,
		0,		0,		s.m_z,	0,
		0,		0,		0,		1);
}

matr matr::translate(vec t)
{
	return matr(
		1,	0,	0,	t.m_x,
		0,	1,	0,	t.m_y,
		0,	0,	1,	t.m_z,
		0,	0,	0,	1);
}

matr matr::view(const vec& Loc, const vec& At, const vec& Up1)
{
	vec Dir, Right, Up;
	Dir = (At - Loc).normal();
	Right = (Dir % Up1).normal();
	Up = (Right % Dir).normal();

	matr m = matr(
		Right.m_x,		Up.m_x,			-Dir.m_x,		0,
		Right.m_y,		Up.m_y,			-Dir.m_y,		0,
		Right.m_z,		Up.m_z,			-Dir.m_z,		0,
		-(Loc & Right),	-(Loc & Up),	(Loc & Dir),	1);
	return m;
}

matr matr::frustum(
	const double& Left, const double& Right,
	const double& Bottom, const double& Top,
	const double& Near, const double& Far)
{
	return matr(
		2 * Near / (Right - Left),			0,									0,								0,
		0,									2 * Near / (Top - Bottom),			0,								0,
		(Right + Left) / (Right - Left),	(Top + Bottom) / (Top - Bottom),	-(Far + Near) / (Far - Near),	-1,
		0,									0,									-2 * Near * Far / (Far - Near),	0);
}

matr mth::matr::operator*(const matr& m) const
{
	return matr(
		m_data[0][0] * m.m_data[0][0] + m_data[0][1] * m.m_data[1][0] + m_data[0][2] * m.m_data[2][0] + m_data[0][3] * m.m_data[3][0],
		m_data[0][0] * m.m_data[0][1] + m_data[0][1] * m.m_data[1][1] + m_data[0][2] * m.m_data[2][1] + m_data[0][3] * m.m_data[3][1],
		m_data[0][0] * m.m_data[0][2] + m_data[0][1] * m.m_data[1][2] + m_data[0][2] * m.m_data[2][2] + m_data[0][3] * m.m_data[3][2],
		m_data[0][0] * m.m_data[0][3] + m_data[0][1] * m.m_data[1][3] + m_data[0][2] * m.m_data[2][3] + m_data[0][3] * m.m_data[3][3],

		m_data[1][0] * m.m_data[0][0] + m_data[1][1] * m.m_data[1][0] + m_data[1][2] * m.m_data[2][0] + m_data[1][3] * m.m_data[3][0],
		m_data[1][0] * m.m_data[0][1] + m_data[1][1] * m.m_data[1][1] + m_data[1][2] * m.m_data[2][1] + m_data[1][3] * m.m_data[3][1],
		m_data[1][0] * m.m_data[0][2] + m_data[1][1] * m.m_data[1][2] + m_data[1][2] * m.m_data[2][2] + m_data[1][3] * m.m_data[3][2],
		m_data[1][0] * m.m_data[0][3] + m_data[1][1] * m.m_data[1][3] + m_data[1][2] * m.m_data[2][3] + m_data[1][3] * m.m_data[3][3],

		m_data[2][0] * m.m_data[0][0] + m_data[2][1] * m.m_data[1][0] + m_data[2][2] * m.m_data[2][0] + m_data[2][3] * m.m_data[3][0],
		m_data[2][0] * m.m_data[0][1] + m_data[2][1] * m.m_data[1][1] + m_data[2][2] * m.m_data[2][1] + m_data[2][3] * m.m_data[3][1],
		m_data[2][0] * m.m_data[0][2] + m_data[2][1] * m.m_data[1][2] + m_data[2][2] * m.m_data[2][2] + m_data[2][3] * m.m_data[3][2],
		m_data[2][0] * m.m_data[0][3] + m_data[2][1] * m.m_data[1][3] + m_data[2][2] * m.m_data[2][3] + m_data[2][3] * m.m_data[3][3],

		m_data[3][0] * m.m_data[0][0] + m_data[3][1] * m.m_data[1][0] + m_data[3][2] * m.m_data[2][0] + m_data[3][3] * m.m_data[3][0],
		m_data[3][0] * m.m_data[0][1] + m_data[3][1] * m.m_data[1][1] + m_data[3][2] * m.m_data[2][1] + m_data[3][3] * m.m_data[3][1],
		m_data[3][0] * m.m_data[0][2] + m_data[3][1] * m.m_data[1][2] + m_data[3][2] * m.m_data[2][2] + m_data[3][3] * m.m_data[3][2],
		m_data[3][0] * m.m_data[0][3] + m_data[3][1] * m.m_data[1][3] + m_data[3][2] * m.m_data[2][3] + m_data[3][3] * m.m_data[3][3]);
}

vec matr::operator*(const vec& v) const
{
	vec res;
	const double w = v.m_x * m_data[3][0] + v.m_y * m_data[3][1] + v.m_z * m_data[3][2] + m_data[3][3];
	res.m_x = (v.m_x * m_data[0][0] + v.m_y * m_data[0][1] + v.m_z * m_data[0][2] + m_data[0][3]) / w;
	res.m_y = (v.m_x * m_data[1][0] + v.m_y * m_data[1][1] + v.m_z * m_data[1][2] + m_data[1][3]) / w;
	res.m_z = (v.m_x * m_data[2][0] + v.m_y * m_data[2][1] + v.m_z * m_data[2][2] + m_data[2][3]) / w;
	return res;
}

matr& matr::operator*=(const matr& m)
{
	static matr Result;

	Result.m_data[0][0] =
		m_data[0][0] * m.m_data[0][0] +
		m_data[0][1] * m.m_data[1][0] +
		m_data[0][2] * m.m_data[2][0];
	Result.m_data[1][0] =
		m_data[1][0] * m.m_data[0][0] +
		m_data[1][1] * m.m_data[1][0] +
		m_data[1][2] * m.m_data[2][0];
	Result.m_data[2][0] =
		m_data[2][0] * m.m_data[0][0] +
		m_data[2][1] * m.m_data[1][0] +
		m_data[2][2] * m.m_data[2][0];
	Result.m_data[3][0] =
		m_data[3][0] * m.m_data[0][0] +
		m_data[3][1] * m.m_data[1][0] +
		m_data[3][2] * m.m_data[2][0] +
		m.m_data[3][0];

	Result.m_data[0][1] =
		m_data[0][0] * m.m_data[0][1] +
		m_data[0][1] * m.m_data[1][1] +
		m_data[0][2] * m.m_data[2][1];
	Result.m_data[1][1] =
		m_data[1][0] * m.m_data[0][1] +
		m_data[1][1] * m.m_data[1][1] +
		m_data[1][2] * m.m_data[2][1];
	Result.m_data[2][1] =
		m_data[2][0] * m.m_data[0][1] +
		m_data[2][1] * m.m_data[1][1] +
		m_data[2][2] * m.m_data[2][1];
	Result.m_data[3][1] =
		m_data[3][0] * m.m_data[0][1] +
		m_data[3][1] * m.m_data[1][1] +
		m_data[3][2] * m.m_data[2][1] +
		m.m_data[3][1];

	Result.m_data[0][2] =
		m_data[0][0] * m.m_data[0][2] +
		m_data[0][1] * m.m_data[1][2] +
		m_data[0][2] * m.m_data[2][2];
	Result.m_data[1][2] =
		m_data[1][0] * m.m_data[0][2] +
		m_data[1][1] * m.m_data[1][2] +
		m_data[1][2] * m.m_data[2][2];
	Result.m_data[2][2] =
		m_data[2][0] * m.m_data[0][2] +
		m_data[2][1] * m.m_data[1][2] +
		m_data[2][2] * m.m_data[2][2];
	Result.m_data[3][2] =
		m_data[3][0] * m.m_data[0][2] +
		m_data[3][1] * m.m_data[1][2] +
		m_data[3][2] * m.m_data[2][2] +
		m.m_data[3][2];

	Result.m_data[0][3] = 0;
	Result.m_data[1][3] = 0;
	Result.m_data[2][3] = 0;
	Result.m_data[3][3] = 1;

	*this = Result;
	return *this;
}

double mth::matr::det3x3(
	double A00, double A01, double A02,
	double A10, double A11, double A12,
	double A20, double A21, double A22)
{
	return A00 * A11 * A22 +
			A01 * A12 * A20 +
			A02 * A10 * A21 -
			A00 * A12 * A21 -
			A01 * A10 * A22 -
			A02 * A11 * A20;
}

double mth::matr::det(double data[4][4])
{
	return
		(data[0][0] * det3x3(
			data[1][1], data[1][2], data[1][3],
			data[2][1], data[2][2], data[2][3],
			data[3][1], data[3][2], data[3][3]) -
		data[0][1] * det3x3(
			data[1][0], data[1][2], data[1][3],
			data[2][0], data[2][2], data[2][3],
			data[3][0], data[3][2], data[3][3]) +
		data[0][2] * det3x3(
			data[1][0], data[1][1], data[1][3],
			data[2][0], data[2][1], data[2][3],
			data[3][0], data[3][1], data[3][3]) -
		data[0][3] * det3x3(
			data[1][0], data[1][1], data[1][2],
			data[2][0], data[2][1], data[2][2],
			data[3][0], data[3][1], data[3][2]));
}

bool mth::matr::inverse()
{
	double determinant = det(m_data);
	matr m;

	if (determinant == 0)
		return false;
	m.m_data[0][0] =
		det3x3(m_data[1][1], m_data[1][2], m_data[1][3],
				m_data[2][1], m_data[2][2], m_data[2][3],
				m_data[3][1], m_data[3][2], m_data[3][3]) / determinant;
	m.m_data[0][1] =
		det3x3(m_data[0][1], m_data[0][2], m_data[0][3],
				m_data[2][1], m_data[2][2], m_data[2][3],
				m_data[3][1], m_data[3][2], m_data[3][3]) / -determinant;
	m.m_data[0][2] =
		det3x3(m_data[0][1], m_data[0][2], m_data[0][3],
				m_data[1][1], m_data[1][2], m_data[1][3],
				m_data[3][1], m_data[3][2], m_data[3][3]) / determinant;
	m.m_data[0][3] =
		det3x3(m_data[0][1], m_data[0][2], m_data[0][3],
				m_data[1][1], m_data[1][2], m_data[1][3],
				m_data[2][1], m_data[2][2], m_data[2][3]) / -determinant;
	
	m.m_data[1][0] =
		det3x3(m_data[1][0], m_data[1][2], m_data[1][3],
				m_data[2][0], m_data[2][2], m_data[2][3],
				m_data[3][0], m_data[3][2], m_data[3][3]) / -determinant;
	m.m_data[1][1] =
		det3x3(m_data[0][0], m_data[0][2], m_data[0][3],
				m_data[2][0], m_data[2][2], m_data[2][3],
				m_data[3][0], m_data[3][2], m_data[3][3]) / determinant;
	m.m_data[1][2] =
		det3x3(m_data[0][0], m_data[0][2], m_data[0][3],
				m_data[1][0], m_data[1][2], m_data[1][3],
				m_data[3][0], m_data[3][2], m_data[3][3]) / -determinant;
	m.m_data[1][3] =
		det3x3(m_data[0][0], m_data[0][2], m_data[0][3],
				m_data[1][0], m_data[1][2], m_data[1][3],
				m_data[2][0], m_data[2][2], m_data[2][3]) / determinant;
	
	m.m_data[2][0] =
		det3x3(m_data[1][0], m_data[1][1], m_data[1][3],
				m_data[2][0], m_data[2][1], m_data[2][3],
				m_data[3][0], m_data[3][1], m_data[3][3]) / determinant;
	m.m_data[2][1] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][3],
				m_data[2][0], m_data[2][1], m_data[2][3],
				m_data[3][0], m_data[3][1], m_data[3][3]) / -determinant;
	m.m_data[2][2] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][3],
				m_data[1][0], m_data[1][1], m_data[1][3],
				m_data[3][0], m_data[3][1], m_data[3][3]) / determinant;
	m.m_data[2][3] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][3],
				m_data[1][0], m_data[1][1], m_data[1][3],
				m_data[2][0], m_data[2][1], m_data[2][3]) / -determinant;
	
	m.m_data[3][0] =
		det3x3(m_data[1][0], m_data[1][1], m_data[1][2],
				m_data[2][0], m_data[2][1], m_data[2][2],
				m_data[3][0], m_data[3][1], m_data[3][2]) / -determinant;
	m.m_data[3][1] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][2],
				m_data[2][0], m_data[2][1], m_data[2][2],
				m_data[3][0], m_data[3][1], m_data[3][2]) / determinant;
	m.m_data[3][2] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][2],
				m_data[1][0], m_data[1][1], m_data[1][2],
				m_data[3][0], m_data[3][1], m_data[3][2]) / -determinant;
	m.m_data[3][3] =
		det3x3(m_data[0][0], m_data[0][1], m_data[0][2],
				m_data[1][0], m_data[1][1], m_data[1][2],
				m_data[2][0], m_data[2][1], m_data[2][2]) / determinant;
	memcpy(m_data, m.m_data, sizeof(float) * 4 * 4);
	return true;
}

void mth::matr::transpose()
{
	std::swap(m_data[1][0], m_data[0][1]);
	std::swap(m_data[2][0], m_data[0][2]);
	std::swap(m_data[3][0], m_data[0][3]);
	std::swap(m_data[1][2], m_data[2][1]);
	std::swap(m_data[1][3], m_data[3][1]);
	std::swap(m_data[2][3], m_data[2][3]);
}

vec mth::matr::transformVector(const vec& v)
{
	return vec(
		(v.m_x * m_data[0][0] + v.m_y * m_data[1][0] + v.m_z * m_data[2][0] + 1 * m_data[3][0]),
		(v.m_x * m_data[0][1] + v.m_y * m_data[1][1] + v.m_z * m_data[2][1] + 1 * m_data[3][1]),
		(v.m_x * m_data[0][2] + v.m_y * m_data[1][2] + v.m_z * m_data[2][2] + 1 * m_data[3][2]));
}

vec mth::matr::transformPoint(const vec& v)
{
	double w = v.m_x * m_data[0][3] + v.m_y * m_data[1][3] + v.m_z * m_data[2][3] + m_data[3][3];
	return vec(
		(v.m_x * m_data[0][0] + v.m_y * m_data[1][0] + v.m_z * m_data[2][0] + 1 * m_data[3][0]) / w,
		(v.m_x * m_data[0][1] + v.m_y * m_data[1][1] + v.m_z * m_data[2][1] + 1 * m_data[3][1]) / w,
		(v.m_x * m_data[0][2] + v.m_y * m_data[1][2] + v.m_z * m_data[2][2] + 1 * m_data[3][2]) / w);
}

matr matr::scale(double s)
{
	return matr(
		s, 0, 0, 0,
		0, s, 0, 0,
		0, 0, s, 0,
		0, 0, 0, 1);
}

matr matr::rotateY(double AngleInDegree)
{
	double rad = AngleInDegree * Degree2Radian;
	double
		sine{ sin(rad) },
		cosine{ cos(rad) };

	//__asm
	//{
	//	/* FST(0) Angle (from degree to radian) */
	//	fld AngleInDegree
	//	fmul Degree2Radian
	//	/* FST(0) - cos, FST(1) - sin */
	//	fsincos
	//	fstp cosine /* cos -> cosine */
	//	fstp sine /* sin -> sine */
	//}
	return matr(
		cosine,	0,	-sine,	0,
		0,		1,	0,		0,
		sine,	0,	cosine,	0,
		0,		0,	0,		1);
}

matr matr::rotateX(double AngleInDegree)
{
	double rad = AngleInDegree * Degree2Radian;
	double
		sine{ sin(rad) },
		cosine{ cos(rad) };
	return matr(
		1,	0,		0,		0,
		0,	cosine,	sine,	0,
		0,	-sine,	cosine,	0,
		0,	0,		0,		1);
}

matr matr::rotate(double AngleInDegree, vec Axis)
{
	double rad = AngleInDegree * Degree2Radian;
	double
		sine{ sin(rad) },
		cosine{ cos(rad) };
	return matr(
		cosine + Axis.m_x * Axis.m_x * (1 - cosine),
		Axis.m_x * Axis.m_y * (1 - cosine) - Axis.m_z * sine,
		Axis.m_z * Axis.m_x * (1 - cosine) + Axis.m_y * sine,
		0,

		Axis.m_y * Axis.m_x * (1 - cosine) + Axis.m_z * sine,
		cosine + Axis.m_y * Axis.m_y * (1 - cosine),
		Axis.m_z * Axis.m_y * (1 - cosine) - Axis.m_x * sine,
		0,

		Axis.m_z * Axis.m_x * (1 - cosine) - Axis.m_y * sine,
		Axis.m_z * Axis.m_y * (1 - cosine) + Axis.m_x * sine,
		cosine + Axis.m_z * Axis.m_z * (1 - cosine),
		0,
		
		0, 0, 0, 1);
}

template<typename type>
type mth::clamp(const type x, const type min, const type max)
{
	if (x <= min) return min;
	if (x >= max) return max;
	return x;
}

//template<class type>
//inline pt::stock::stock(const stock& copyData)
//	: m_realSize(copyData.m_realSize)
//	, m_maxSize(copyData.m_maxSize)
//	, m_data(new type[m_maxSize])
//{
//	for (int i = 0; i < m_realSize; ++i)
//		m_data[i] = copyData.m_data[i];
//}
//
//template<class type>
//pt::stock& pt::stock::operator=(const stock& copyData)
//{
//	if (m_maxSize < copyData.m_realSize)
//	{
//		delete[] m_data;
//		m_data = new type[copyData.m_realSize];
//		m_maxSize = copyData.m_realSize;
//	}
//	for (int i = 0; i < copyData.m_realSize; i++)
//		m_data[i] = copyData.m_data[i];
//	m_realSize = copyData.m_realSize;
//	return *this;
//}
//
//template<class type>
//pt::stock& pt::stock::add(const type& newData)
//{
//	if (m_realSize == m_maxSize)
//	{
//		if (m_maxSize == 0)
//			m_maxSize = 1;
//		else
//			m_maxSize *= 2;
//		type *pNewArray = new type[m_maxSize];
//		if (m_data != nullptr)
//		{
//			for (int i = 0; i < m_realSize; i++)
//				pNewArray[i] = m_data[i];
//			delete[] m_data;
//		}
//		m_data = pNewArray;
//	}
//	m_data[m_realSize++] = newData;
//	return *this;
//}
//
//template<class type>
//type& pt::stock::operator[](int index)
//{
//	assert(index >= 0 && index < m_realSize);
//	return m_data[index];
//}
//
//template<class type>
//pt::stock& pt::stock::clear()
//{
//	m_realSize = 0;
//	m_maxSize = 0;
//	if (m_data != nullptr)
//		delete[] m_data;
//	return *this;
//}
//
//template<class type>
//pt::stock& pt::stock::deleteLast()
//{
//	if (m_realSize > 0)
//		--m_realSize;
//	return *this;
//}
//
//template<class type>
//	template<class typeW>
//	inline void pt::stock::walk(typeW & walker)
//	{
//		for (int i = 0; i < m_realSize; i++)
//			walker(m_data[i]);
//	}
