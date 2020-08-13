#pragma once

#include <d3d11_3.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include <cmath>
#include <cassert>

#pragma warning(disable:4244)
#pragma warning(disable:4267)

namespace mth
{
	static long double Degree2Radian = 3.1415926535897932384626433832795028841971L / 180;
	const long double Pi = 3.14159265358979323846264332832795028841971L;
	double Rnd0();
	double Rnd1();

#pragma pack(push, 4)
	class vec2
	{
		double
			m_x{ 0 },
			m_y{ 0 };
	public:
		vec2() = default;
		vec2( double x, double y ) : m_x(x), m_y(y) {}
		explicit vec2( double x ) : m_x(x), m_y(x) {}

		double& x() { return m_x; }
		double c_x() const { return m_x; }

		double& y() { return m_y; }
		double c_y() const { return m_y; }

		vec2 operator-() const
		{ return vec2(-m_x, -m_y); }

		bool operator==( vec2 v ) { return (m_x == v.m_x && m_y == v.m_y); }
		bool operator!=( vec2 v ) { return !(v == *this); }

		vec2 operator+( const vec2 &v ) const { return vec2(m_x + v.m_x, m_y + v.m_y); }
		vec2 & operator+=( const vec2 &v );

		vec2 operator-( const vec2 &v ) const { return vec2(m_x - v.m_x, m_y - v.m_y); }
		vec2 & operator-=( const vec2 &v );

		vec2 operator*( const double s ) const { return vec2(m_x * s, m_y * s); }
		vec2 operator*( const vec2 &v) const { return vec2(m_x * v.m_x, m_y * v.m_y); }
		vec2 & operator*=( const vec2 &v );

		vec2 operator/( double N ) { return vec2(m_x / N, m_y / N); }

		double operator&( const vec2 &v ) const { return m_x * v.m_x + m_y * v.m_y; }
		double operator!() const { return *this & *this; }

		double operator~() const { return sqrt(*this & *this); }

		vec2 normal() const;
		vec2 & normalize();
	};

	class vec
	{
		friend class matr;
		double
			m_x{ 0 },
			m_y{ 0 },
			m_z{ 0 };
	public:
		vec() = default;
		vec( double A, double B, double C = 0 ) : m_x(A), m_y(B), m_z(C) {}
		explicit vec( double A ) : m_x(A), m_y(A), m_z(A) {}
		vec( double v[3] ) : m_x{ v[0] }, m_y{ v[1] }, m_z{ v[2] } {}

		double& x() { return m_x; }
		double c_x() const { return m_x; }

		double& y() { return m_y; }
		double c_y() const { return m_y; }

		double& z() { return m_z; }
		double c_z() const { return m_z; }

		bool operator==( const vec &v ) const { return (m_x == v.m_x && m_y == v.m_y && m_z == v.m_z); }
		bool operator!=( const vec &v ) const { return !(v == *this); }

		vec operator-() const { return vec(-m_x, -m_y, -m_z); }

		vec operator+( const vec &v ) const { return vec(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z); }
		vec & operator+=( const vec &v );

		vec operator-( const vec &v ) const { return vec(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z); }
		vec & operator-=( const vec &v );

		vec operator*( const double s ) const { return vec(m_x * s, m_y * s, m_z * s); }
		vec operator*( const vec &v ) const { return vec(m_x * v.m_x, m_y * v.m_y, m_z * v.m_z); }
		vec & operator*=( const vec &v );

		vec operator/( double N ) { return vec(m_x / N, m_y / N, m_z / N); }

		double operator&( const vec &v ) const { return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z; }
		vec operator%( const vec &v ) const { return vec(m_y * v.m_z - m_z * v.m_y, m_z * v.m_x - m_x * v.m_z, m_x * v.m_y - m_y * v.m_x); }

		double operator!() const { return *this & *this; }
		double operator~() const { return sqrt(!(*this)); }
		vec normal() const;
		vec & normalize();

		vec clamp( double min, double max ) const;
		static vec rnd();

		operator double*() { return &m_x; }
	};
#pragma pack(pop)
	class matr
	{
	private:
		double m_data[4][4];

	public:
		double *Value() { return m_data[0]; }

		matr() = default;

		matr( double A00, double A01, double A02, double A03,
				double A10, double A11, double A12, double A13,
				double A20, double A21, double A22, double A23,
				double A30, double A31, double A32, double A33 );

		static matr identity();
		static matr scale(vec s);

		static matr scale(double s);
		static matr translate( vec t );

		static matr rotateY( double AngleInDegree );
		static matr rotateX( double AngleInDegree);
		static matr rotate( double AngleInDegree, vec Axis );

		static matr view(const vec &Loc, const vec &At, const vec &Up);
		static matr frustum(
			const double &Left, const double &Right,
			const double &Bottom, const double &Top,
			const double &Near, const double &Far );
		matr operator*( const matr &m ) const;
		vec operator*(const vec& v) const;
		matr & operator*=( const matr &m );
		static double det3x3(
			double A00, double A01, double A02,
			double A10, double A11, double A12,
			double A20, double A21, double A22);
		static double det( double data[4][4] );
		bool inverse();
		void transpose();
		
		vec transformVector(const vec &v);
		vec transformPoint(const vec &v);
	};

	template<typename type>
	type clamp(const type x, const type min, const type max);

	class ray
	{
		vec m_org, m_dir;
	public:
		ray( const vec &org, const vec &dir ) : m_org(org), m_dir(dir.normal()) {}
		~ray() = default;

		inline vec& origin(){ return m_org; }
		inline vec& direction(){ return m_dir; }
		inline vec origin() const { return m_org; }
		inline vec direction() const { return m_dir; }
		inline vec cast( double T ) const { return m_org + m_dir * T; }
	};
}

namespace pt
{
	typedef mth::vec vec;
	typedef mth::vec2 vec2;
	typedef mth::matr matr;
	typedef mth::ray ray;
}
