#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1 COMPLETED

			const float dp{ float(Vector3::Dot(sphere.origin - ray.origin, ray.direction)) };
			const float tclSquared{ (sphere.origin - ray.origin).SqrMagnitude()};
			const float odSquared{ float(tclSquared - dp * dp) };


			if (odSquared <= sphere.radius * sphere.radius)
			{
				const float tca{ sqrtf(sphere.radius * sphere.radius - odSquared) };
				const float t0{ dp - tca };

				if (!(t0 > ray.min && t0 < ray.max))
					return false;

				if (!ignoreHitRecord)
				{
					hitRecord.origin = ray.origin + ray.direction * t0;
					hitRecord.normal = Vector3{ hitRecord.origin - sphere.origin } / sphere.radius;
					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.t = t0;
				}
				return true;
			}
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1 COMPLETED

			const float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			if (t > ray.min && t < ray.max)
			{
				const Vector3 p = ray.origin + t * ray.direction;
				if (t < hitRecord.t)
				{
					if (!ignoreHitRecord)
					{
						hitRecord.didHit = true;
						hitRecord.origin = ray.origin + ray.direction * t;
						hitRecord.materialIndex = plane.materialIndex;
						hitRecord.t = t;
						hitRecord.normal = plane.normal;
						if (Vector3::Dot(plane.normal, ray.direction) > 0)
						{
							hitRecord.normal = -plane.normal;
						}
					}
					return true;
				}
			}
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			////todo W5 FINISHED

			//// Normal VS Ray-Direction Check (Perpendicular?)
			//float dotNormalRaydir{ Vector3::Dot(triangle.normal, ray.direction) };

			//if (dotNormalRaydir == 0)
			//{
			//	return false;
			//}

			//// CullModeCheck
			//if (triangle.cullMode != TriangleCullMode::NoCulling)
			//{
			//	if (dotNormalRaydir > 0)
			//	{
			//		if (triangle.cullMode == TriangleCullMode::BackFaceCulling)
			//			return false;
			//	}
			//	else
			//	{
			//		if (triangle.cullMode == TriangleCullMode::FrontFaceCulling)
			//			return false;
			//	}
			//}

			//// Ray-Plane test(planedefinedbyTriangle) + T range check
			//Plane plane{ triangle.v0, triangle.normal };
			//HitRecord tempHitRecord{};
			//HitTest_Plane(plane, ray, tempHitRecord);

			//if (tempHitRecord.t > ray.max || tempHitRecord.t < ray.min)
			//	return false;

			//// CheckifhitpointisinsidetheTriangle
			//const Vector3 point{ tempHitRecord.origin };

			//Vector3 edgeA{ (triangle.v1 - triangle.v0) };
			//Vector3	pointToSideA{ point - triangle.v0 };
			//if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeA, pointToSideA)) < 0)
			//	return false;

			//Vector3 edgeB{ (triangle.v2 - triangle.v1) };
			//Vector3	pointToSideB{ point - triangle.v1 };
			//if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeB, pointToSideB)) < 0)
			//	return false;

			//Vector3 edgeC{ (triangle.v0 - triangle.v2) };
			//Vector3	pointToSideC{ point - triangle.v2 };
			//if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeC, pointToSideC)) < 0)
			//	return false;


			//// Fill-in HitRecord(if required)
			//if (!ignoreHitRecord)
			//{
			//	hitRecord.didHit = true;
			//	hitRecord.materialIndex = triangle.materialIndex;
			//	hitRecord.normal = triangle.normal;
			//	hitRecord.origin = point;
			//	hitRecord.t = tempHitRecord.t;
			//}
			//return true;

			// CullModeCheck
			if (triangle.cullMode != TriangleCullMode::NoCulling)
			{
				if (Vector3::Dot(triangle.normal, ray.direction) > 0)
				{
					if (triangle.cullMode == TriangleCullMode::BackFaceCulling)
						return false;
				}
				else
				{
					if (triangle.cullMode == TriangleCullMode::FrontFaceCulling)
						return false;
				}
			}

			const float EPSILON = 0.0000001f;
			Vector3 edge1, edge2, h, s, q;
			float a, f, u, v;
			edge1 = triangle.v1 - triangle.v0;
			edge2 = triangle.v2 - triangle.v0;
			h = Vector3::Cross(ray.direction, edge2);
			a = Vector3::Dot(edge1, h);
			if (a > -EPSILON && a < EPSILON)
				return false;    // This ray is parallel to this triangle.
			f = 1.0f / a;
			s = ray.origin - triangle.v0;
			u = f * Vector3::Dot(s, h);
			if (u < 0.0f || u > 1.0f)
				return false;
			q = Vector3::Cross(s, edge1);
			v = f * Vector3::Dot(ray.direction, q);
			if (v < 0.0f || u + v > 1.0f)
				return false;
			// At this stage we can compute t to find out where the intersection point is on the line.
			float t = f * Vector3::Dot(edge2, q);
			if (t > ray.max || t < ray.min)
				return false;
			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				hitRecord.origin = ray.origin + ray.direction * t;
				hitRecord.t = t;
			}
			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//SlabTest
			if (!SlabTest_TriangleMesh(mesh, ray)) {
				return false;
			}


			//todo W5 COMPLETED

			bool hasHit{false};
			HitRecord tempRecord{};
			float closestT{ FLT_MAX };

			Triangle triangle{};
			triangle.cullMode = mesh.cullMode;
			triangle.materialIndex = mesh.materialIndex;

			for (int idx{}; idx < mesh.indices.size(); idx += 3)
			{
				triangle.v0 = mesh.transformedPositions[mesh.indices[idx]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[idx + 1]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[idx + 2]];

				triangle.normal = mesh.transformedNormals[idx / 3];

				if (HitTest_Triangle(triangle, ray, tempRecord, ignoreHitRecord))
				{
					hasHit = true;
				}

				if (closestT >= tempRecord.t)
				{
					closestT = tempRecord.t;
					if (!ignoreHitRecord)
					{
						hitRecord = tempRecord;
					}
				}
		
			}
			return hasHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}

#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& /*light*/, const Vector3 /*origin*/)
		{
			//todo W3 (NOT USED)
			assert(false && "No Implemented Yet!");
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3 COMPLETED

			
			float rSquared{ (light.origin - target).SqrMagnitude() };
		
			return (light.color * ColorRGB{ light.intensity/rSquared, light.intensity /rSquared, light.intensity /rSquared });
		}
	}	

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				normal.Normalize();

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}