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
				const float tca{ float(sqrt(sphere.radius * sphere.radius - odSquared)) };
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
			//todo W5 BUSY

			// Normal VS Ray-Direction Check (Perpendicular?)
			Vector3 a = triangle.v1 - triangle.v0;
			Vector3	b = triangle.v2 - triangle.v0;
			Vector3	normal = Vector3::Cross(a, b);

			if (Vector3::Dot(normal, ray.direction) == 0)
			{
				return false;
			}

			// CullModeCheck
			TriangleCullMode cullMode{ triangle.cullMode };
			if (cullMode != TriangleCullMode::NoCulling)
			{
				if (ignoreHitRecord)
				{
					if (cullMode == TriangleCullMode::BackFaceCulling)
						cullMode = TriangleCullMode::FrontFaceCulling;
					else if (cullMode == TriangleCullMode::FrontFaceCulling)
						cullMode = TriangleCullMode::BackFaceCulling;
				}

				if (Vector3::Dot(normal, ray.direction) > 0)
				{
					if (cullMode == TriangleCullMode::BackFaceCulling)
						return false;
				}
				else if (Vector3::Dot(normal, ray.direction) < 0)
				{
					if (cullMode == TriangleCullMode::FrontFaceCulling)
						return false;
				}
			}

			// Ray-Plane test(planedefinedbyTriangle) + T range check
			Plane plane{ triangle.v0, normal };
			HitRecord tempHitRecord{};
			HitTest_Plane(plane, ray, tempHitRecord);

			if (tempHitRecord.t > ray.max || tempHitRecord.t < ray.min)
				return false;

			// CheckifhitpointisinsidetheTriangle
			Vector3 point{ tempHitRecord.origin };

			Vector3 edgeA{ (triangle.v1 - triangle.v0) };
			Vector3	pointToSideA{ point - triangle.v0 };
			if (Vector3::Dot(normal, Vector3::Cross(edgeA, pointToSideA)) < 0)
				return false;

			Vector3 edgeB{ (triangle.v2 - triangle.v1) };
			Vector3	pointToSideB{ point - triangle.v1 };
			if (Vector3::Dot(normal, Vector3::Cross(edgeB, pointToSideB)) < 0)
				return false;

			Vector3 edgeC{ (triangle.v0 - triangle.v2) };
			Vector3	pointToSideC{ point - triangle.v2 };
			if (Vector3::Dot(normal, Vector3::Cross(edgeC, pointToSideC)) < 0)
				return false;


			// Fill-in HitRecord(if required)
			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = normal;
				hitRecord.origin = point;
				hitRecord.t = tempHitRecord.t;
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
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
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
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
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

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}