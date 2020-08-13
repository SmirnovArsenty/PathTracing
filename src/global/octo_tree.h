#pragma once

#include <vector>
#include <Windows.h>
#include "mth.h"

namespace pt
{
	class octo_tree
	{
	public:
		struct box
		{
		private:
			vec m_v0, v1;
			vec m_position, m_size;
		public:
			box( vec m_v0, vec v1 ) : m_v0(m_v0), v1(v1)
			{
				m_position = vec((m_v0.x() + v1.x()) / 2, (m_v0.y() + v1.y()) / 2, (m_v0.z() + v1.z()) / 2);
				m_size = vec(abs(m_v0.x() - v1.x()), abs(m_v0.y() - v1.y()), abs(m_v0.z() - v1.z()));
			}
			vec size() const { return m_size; }
			vec position() const { return m_position; }
			vec V0() const { return m_v0; }
			vec V1() const { return v1; }
		};

	private:
		// subtrees
		octo_tree *m_northEastNear = nullptr;
		octo_tree *m_northWestNear = nullptr;
		octo_tree *m_northEastFar = nullptr;
		octo_tree *m_northWestFar = nullptr;
		octo_tree *m_southEastNear = nullptr;
		octo_tree *m_southWestNear = nullptr;
		octo_tree *m_southEastFar = nullptr;
		octo_tree *m_southWestFar = nullptr;
		/* Axis alined bounding box */
		box AABB;
		/* Triangles vector */
		std::vector<triangle> m_triangles;
	public:
		octo_tree( box AABB ) : AABB{ AABB } {}
	private:
		void subdivide()
		{
			vec O = vec((AABB.V0().x() + AABB.V1().x()) / 2, (AABB.V0().y() + AABB.V1().y()) / 2, (AABB.V0().z() + AABB.V1().z()) / 2);
			vec v1 = vec(min(AABB.V0().x(), AABB.V1().x()), min(AABB.V0().y(), AABB.V1().y()), min(AABB.V0().z(), AABB.V1().z()));
			vec v2 = vec(max(AABB.V0().x(), AABB.V1().x()), min(AABB.V0().y(), AABB.V1().y()), min(AABB.V0().z(), AABB.V1().z()));
			vec v3 = vec(min(AABB.V0().x(), AABB.V1().x()), max(AABB.V0().y(), AABB.V1().y()), min(AABB.V0().z(), AABB.V1().z()));
			vec v4 = vec(min(AABB.V0().x(), AABB.V1().x()), min(AABB.V0().y(), AABB.V1().y()), max(AABB.V0().z(), AABB.V1().z()));

			vec v5 = vec(max(AABB.V0().x(), AABB.V1().x()), max(AABB.V0().y(), AABB.V1().y()), min(AABB.V0().z(), AABB.V1().z()));
			vec v6 = vec(max(AABB.V0().x(), AABB.V1().x()), min(AABB.V0().y(), AABB.V1().y()), max(AABB.V0().z(), AABB.V1().z()));
			vec v7 = vec(min(AABB.V0().x(), AABB.V1().x()), max(AABB.V0().y(), AABB.V1().y()), max(AABB.V0().z(), AABB.V1().z()));
			vec v8 = vec(max(AABB.V0().x(), AABB.V1().x()), max(AABB.V0().y(), AABB.V1().y()), max(AABB.V0().z(), AABB.V1().z()));
			if (m_northEastNear == nullptr || m_northWestNear == nullptr ||
				m_northEastFar == nullptr || m_northWestFar == nullptr ||
				m_southEastNear == nullptr || m_southWestNear == nullptr ||
				m_southEastFar == nullptr || m_southWestFar == nullptr)
			{
				m_southWestNear = new octo_tree(box(O, v1));
				m_southEastNear = new octo_tree(box(O, v2));
				m_southEastFar = new octo_tree(box(O, v3));
				m_southWestFar = new octo_tree(box(O, v4));

				m_northEastNear = new octo_tree(box(O, v5));
				m_northWestNear = new octo_tree(box(O, v6));
				m_northWestFar = new octo_tree(box(O, v7));
				m_northEastFar = new octo_tree(box(O, v8));
			}
			else
				return;
		}

		bool isTriInBox( triangle Tri )
		{
			if (Tri.V0().position.x() < min(AABB.V0().x(), AABB.V1().x()) ||
				Tri.V1().position.x() < min(AABB.V0().x(), AABB.V1().x()) ||
				Tri.V2().position.x() < min(AABB.V0().x(), AABB.V1().x()))
				return false;
			if (Tri.V0().position.y() < min(AABB.V0().y(), AABB.V1().y()) ||
				Tri.V1().position.y() < min(AABB.V0().y(), AABB.V1().y()) ||
				Tri.V2().position.y() < min(AABB.V0().y(), AABB.V1().y()))
				return false;
			if (Tri.V0().position.z() < min(AABB.V0().z(), AABB.V1().z()) ||
				Tri.V1().position.z() < min(AABB.V0().z(), AABB.V1().z()) ||
				Tri.V2().position.z() < min(AABB.V0().z(), AABB.V1().z()))
				return false;

			if (Tri.V0().position.x() > max(AABB.V0().x(), AABB.V1().x()) ||
				Tri.V1().position.x() > max(AABB.V0().x(), AABB.V1().x()) ||
				Tri.V2().position.x() > max(AABB.V0().x(), AABB.V1().x()))
				return false;
			if (Tri.V0().position.y() > max(AABB.V0().y(), AABB.V1().y()) ||
				Tri.V1().position.y() > max(AABB.V0().y(), AABB.V1().y()) ||
				Tri.V2().position.y() > max(AABB.V0().y(), AABB.V1().y()))
				return false;
			if (Tri.V0().position.z() > max(AABB.V0().z(), AABB.V1().z()) ||
				Tri.V1().position.z() > max(AABB.V0().z(), AABB.V1().z()) ||
				Tri.V2().position.z() > max(AABB.V0().z(), AABB.V1().z()))
				return false;
			return true;
		}

	public:
		bool Insert( triangle Tri, unsigned int Depth = 0 )
		{
			subdivide();
			if (!isTriInBox(Tri))
				return false;

			if (Depth >= 5 ||
				!m_northWestNear->Insert(Tri, Depth + 1) && !m_northWestFar->Insert(Tri, Depth + 1) &&
				!m_northEastFar->Insert(Tri, Depth + 1) && !m_northEastNear->Insert(Tri, Depth + 1) &&
				!m_southWestNear->Insert(Tri, Depth + 1) && !m_southWestFar->Insert(Tri, Depth + 1) &&
				!m_southEastFar->Insert(Tri, Depth + 1) && !m_southEastNear->Insert(Tri, Depth + 1))
				m_triangles.push_back(Tri);
			return true;
		}

	private:
		bool Boxintersect( const ray &Ray )
		{
			double
				x_min, x_max,
				y_min, y_max,
				z_min, z_max;
			int
				x_inv = 1,
				y_inv = 1,
				z_inv = 1;
			x_min = -(Ray.origin().x() - (AABB.position().x() - AABB.size().x())) / Ray.direction().x();
			x_max = -(Ray.origin().x() - (AABB.position().x() + AABB.size().x())) / Ray.direction().x();
			if (x_min > x_max)
			{
				double tmp = x_min;
				x_min = x_max;
				x_max = tmp;
				x_inv = -1;
			}
			y_min = -(Ray.origin().y() - (AABB.position().y() - AABB.size().y())) / Ray.direction().y();
			y_max = -(Ray.origin().y() - (AABB.position().y() + AABB.size().y())) / Ray.direction().y();
			if (y_min > y_max)
			{
				double tmp = y_min;
				y_min = y_max;
				y_max = tmp;
				y_inv = -1;
			}
			z_min = -(Ray.origin().z() - (AABB.position().z() - AABB.size().z())) / Ray.direction().z();
			z_max = -(Ray.origin().z() - (AABB.position().z() + AABB.size().z())) / Ray.direction().z();
			if (z_min > z_max)
			{
				double tmp = z_min;
				z_min = z_max;
				z_max = tmp;
				z_inv = -1;
			}
			if (max(max(x_min, y_min), z_min) < min(min(x_max, y_max), z_max))
			{
				double t = max(max(x_min, y_min), z_min);
				return true;
			}
			return false;
		}

	public:
		void clear()
		{
			if (m_northEastFar == nullptr)
				return;
			m_northEastFar->clear();
			m_northEastNear->clear();
			m_northWestNear->clear();
			m_northWestFar->clear();

			m_southEastFar->clear();
			m_southEastNear->clear();
			m_southWestNear->clear();
			m_southWestFar->clear();

			delete this;
		}

		struct octo
		{
			vec v1, v2;
			int FirstTriangle;
			int NoofTriangles;
			int Child[8];
		};

		int createStructuredBuffer(std::vector<octo> &O, std::vector<triangle> &T)
		{
			if (m_northEastFar != nullptr)
			{
				octo O_tmp;
				ZeroMemory(&O_tmp, sizeof octo);
				int number = O.size();

				O.push_back(O_tmp);

				O[number].v1 = AABB.V0();
				O[number].v2 = AABB.V1();

				if (m_triangles.size() > 0)
				{
					O[number].FirstTriangle = T.size();
					O[number].NoofTriangles = m_triangles.size();
					T.insert(T.end(), m_triangles.begin(), m_triangles.end());
				}
				else
				{
					O[number].FirstTriangle = -1;
					O[number].NoofTriangles = 0;
				}

				O[number].Child[0] = m_northEastFar->createStructuredBuffer(O, T);
				O[number].Child[1] = m_northEastNear->createStructuredBuffer(O, T);
				O[number].Child[2] = m_northWestFar->createStructuredBuffer(O, T);
				O[number].Child[3] = m_northWestNear->createStructuredBuffer(O, T);

				O[number].Child[4] = m_southEastFar->createStructuredBuffer(O, T);
				O[number].Child[5] = m_southEastNear->createStructuredBuffer(O, T);
				O[number].Child[6] = m_southWestFar->createStructuredBuffer(O, T);
				O[number].Child[7] = m_southWestNear->createStructuredBuffer(O, T);
				return number;
			}
			return -1;
		}
	};
}
