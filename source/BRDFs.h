#pragma once
#include <cassert>
#include "Math.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3 COMPLETED
			return cd * kd / float(M_PI);
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3 COMPLETED
			return cd * kd / float(M_PI);
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//todo: W3 COMPLETED
			Vector3 r{ l - 2 * Vector3::Dot(n , l) * n };
			float color{ abs(ks * powf(Vector3::Dot(r, v), exp)) };
			ColorRGB RGBcolor{ color,color,color };
			return RGBcolor;
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//todo: W3 COMPLETED
			float powerthing{ 1 - std::max(Vector3::Dot(h, v), 0.f) };
			ColorRGB Frgb{ f0 + (ColorRGB{1,1,1} - f0) * powerthing * powerthing * powerthing * powerthing * powerthing };
			return Frgb;
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			//todo: W3 COMPLETED
			float a{ roughness * roughness };
			float powerThing{ std::max(Vector3::Dot(n, h),0.f) * std::max(Vector3::Dot(n, h),0.f) * (a * a - 1) + 1 };
			powerThing *= powerThing;
			float Dnfloat{ float((a * a) / (float(M_PI) * powerThing)) };
			return Dnfloat;
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			//todo: W3 COPLETED
			float a{ roughness * roughness };
			float k{ (a + 1) * (a + 1) / 8 };
			float Gfloat{ std::max(Vector3::Dot(n, v), 0.f) / (std::max(Vector3::Dot(n, v), 0.f) * (1 - k) + k) };
			return Gfloat;
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			//todo: W3 COMPLETED
			float Gsmith{ BRDF::GeometryFunction_SchlickGGX(n, v, roughness) * BRDF::GeometryFunction_SchlickGGX(n, l, roughness) };
			return Gsmith;
		}

	}
}