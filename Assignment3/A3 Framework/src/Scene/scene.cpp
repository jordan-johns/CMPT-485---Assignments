
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

#include "scene.h"
#include "../glUtils.h"

#include <cstdio>
#include <cstring>
#include <cfloat>

namespace Scene
{

// Number of Object::Object* 's to allocate at a time
const GLuint N_PTRS = 20;

Scene::Scene()
{
	m_scene = 0;
	m_nObjects = 0;
	m_nObjPtrsAlloced = 0;
	// Position of a point light: (0,0,0)
	m_lightPos = gml::vec4_t(0.0f,0.0f,0.0f,1.0f);
	// Radiance of the point light
	m_lightRad = gml::vec3_t(0.6f,0.6f,0.6f);
	// Ambient radiance
	m_ambientRad = gml::vec3_t(0.025f, 0.025f, 0.025f);
}

Scene::~Scene()
{
	if (m_nObjPtrsAlloced > 0)
	{
		for (GLuint i=0; i<m_nObjects; i++)
			delete m_scene[i];
		delete[] m_scene;
	}
}

bool Scene::init()
{
	// Initialize the shader manager
	if ( !m_shaderManager.init() )
	{
		fprintf(stderr, "ERROR! Could not initialize Shader Manager.\n");
		return false;
	}
	return true;
}

bool Scene::addObject(Object::Object *obj)
{
	if (m_nObjPtrsAlloced == 0)
	{
		m_nObjPtrsAlloced = N_PTRS;
		m_scene = new Object::Object*[m_nObjPtrsAlloced];
		if (m_scene == 0) return false;
	}
	else if (m_nObjPtrsAlloced == m_nObjects)
	{
		m_nObjPtrsAlloced += N_PTRS;
		Object::Object **temp = new Object::Object*[m_nObjPtrsAlloced];
		if (temp == 0) return false;
		memcpy(temp, m_scene, sizeof(Object::Object*)*m_nObjects);
		delete[] m_scene;
		m_scene = temp;
	}

	m_scene[m_nObjects++] = obj;
	return true;
}

void Scene::rasterizeDepth(const gml::mat4x4_t &worldView, const gml::mat4x4_t &projection)
{
	const Shader::Shader *depthShader = m_shaderManager.getDepthShader();

	Shader::GLProgUniforms shaderUniforms;
	shaderUniforms.m_projection = projection;

	depthShader->bindGL(false);
	for (GLuint i=0; i<m_nObjects; i++)
	{
		shaderUniforms.m_modelView = gml::mul(worldView, m_scene[i]->getObjectToWorld());

		if ( !depthShader->setUniforms(shaderUniforms, false) ) return;

		m_scene[i]->rasterize();
		if ( isGLError() ) return;
	}
}

void Scene::rasterize(const gml::mat4x4_t &worldView, const gml::mat4x4_t &projection, const bool useShadows)
{
	// Struct used to pass data values for GLSL uniform variables to
	// the shader program
	Shader::GLProgUniforms shaderUniforms;


	// Set up uniforms constant to the world
	shaderUniforms.m_lightPos = gml::extract3( gml::mul( worldView, m_lightPos ) );
	shaderUniforms.m_lightRad = m_lightRad;
	shaderUniforms.m_ambientRad = m_ambientRad;
	shaderUniforms.m_projection = projection;

	for (GLuint i=0; i<m_nObjects; i++)
	{
		// Fetch the Shader object from the ShaderManager that will perform the
		// shading calculations for this object
		const Shader::Shader *shader = m_shaderManager.getShader(m_scene[i]->getMaterial());

		if (shader->getIsReady(useShadows))
		{
			shader->bindGL(useShadows); // Bind the shader to the OpenGL context
			if (isGLError()) return;

			// Object-specific uniforms
			shaderUniforms.m_modelView = gml::mul(worldView, m_scene[i]->getObjectToWorld());
			shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );
			// If the surface material is not using a texture for Lambertian surface reflectance
			if (m_scene[i]->getMaterial().getLambSource() == Material::CONSTANT)
			{
				shaderUniforms.m_surfRefl = m_scene[i]->getMaterial().getSurfRefl();
			}
			else
			{
				m_scene[i]->getMaterial().getTexture()->bindGL(GL_TEXTURE0); // Set up texture
			}
			// Set up the specular components of the uniforms struct if the material
			// is specular
			if (m_scene[i]->getMaterial().hasSpecular())
			{
				shaderUniforms.m_specExp = m_scene[i]->getMaterial().getSpecExp();
				shaderUniforms.m_specRefl = m_scene[i]->getMaterial().getSpecRefl();
			}

			// Set the shader uniform variables
			if ( !shader->setUniforms(shaderUniforms, useShadows) || isGLError() ) return;

			// Rasterize the object
			m_scene[i]->rasterize();
			if (isGLError()) return;

			// Unbind the shader from the OpenGL context
			shader->unbindGL();
		}
	}
}

bool Scene::rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	// TODO
	//   Find the closest intersection of the ray in the distance range [t0,t1].
	// Return true if an intersection was found, false otherwise
	// Return true if an intersection was found, false otherwise
	RayTracing::HitInfo_t tmpInfo;

	hitinfo.hitDist = t1;
	tmpInfo.hitDist = t1;
	bool retVal = false;

	// Go through each object in the scene to check for intersections with the ray.
	for (unsigned int i = 0; i < m_nObjects; i++)
	{
			// If it intersects...
			if (m_scene[i]->rayIntersects(ray, t0, t1, tmpInfo))
			{
					if (tmpInfo.hitDist < hitinfo.hitDist )
					{
							hitinfo = tmpInfo;
							retVal = true;
					}
			}
	}

	return retVal;
}

bool Scene::shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	// TODO
	//  Determine whether or not the ray intersects an object in the distance range [t0,t1].
	//  Note: Just need to know whether it intersects _an_ object, not the nearest.

	// Return true if the ray intersects an object, false otherwise.
	for (unsigned i = 0; i < m_nObjects; i++)
	{
			if (m_scene[i]->shadowsRay(ray, t0, t1))
			{
					return true;
			}
	}

	// Note: Having this return false will effectively disable/ignore shadows
	return false;
}


void Scene::hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const
{
	// You may use this function if you wish, but it is not necessary.
}

gml::vec3_t Scene::shadeRay(const RayTracing::Ray_t &ray, RayTracing::HitInfo_t &hitinfo, const int remainingRecursionDepth) const
{
	// TODO!

	// Calculate the shade/radiance/color of the given ray. Return the calculated color
	//  - Information about the ray's point of nearest intersection is located
	// in 'hitinfo'
	//  - If remainingRecursionDepth is 0, then _no_ recursive rays (mirror or indirect lighting) should be cast

	// Note: You will have to set up the values for a RayTracing::ShaderValues object, and then
	// pass the object to a shader object to do the appropriate shading.
	//   Use m_shaderManager.getShader() to get an appropriate Shader object for shading
	//  the point based on material properties of the object intersected.

	// When implementing shadows, then the direct lighting component of the
	// calculated ray color will be black if the point is in shadow.

	//ml::vec3_t shade(0.5, 0.5, 0.5);
	//RayTracing::ShaderValues sv(hitinfo.objHit->getMaterial());

	// Note: For debugging your rayIntersection() function, this function
	// returns some non-black constant color at first. When you actually implement
	// this function, then initialize shade to black (0,0,0).

	gml::vec3_t shade(0.0, 0.0, 0.0);
	gml::vec2_t texCoord;
	gml::vec3_t normal;

	hitinfo.objHit->hitProperties(hitinfo, normal, texCoord);

	RayTracing::ShaderValues shaderVal(hitinfo.objHit->getMaterial());
	shaderVal.n = normal;
	shaderVal.p = gml::add(ray.o, gml::scale(hitinfo.hitDist, ray.d));
	shaderVal.e = gml::normalize(gml::scale(-1.0f, ray.d));
	shaderVal.tex = texCoord;
	shaderVal.lightDir = gml::normalize(gml::sub(gml::extract3(m_lightPos), shaderVal.p));
	shaderVal.lightRad = m_lightRad;

	float distToLight = gml::length(gml::sub(gml::extract3(m_lightPos), shaderVal.p));

	// test if in shadow
	RayTracing::Ray_t shadowRay;
	shadowRay.o = shaderVal.p;
	shadowRay.d = shaderVal.lightDir;
	if (!shadowsRay(shadowRay, 0.001, distToLight))
	{
			// direct lighting
			shade = m_shaderManager.getShader(hitinfo.objHit->getMaterial())->shade(shaderVal);
	}
	else
	{
			shade = gml::vec3_t(0,0,0);

	}



	// Only proceed if the recursion depth limit has not been met.
	if (remainingRecursionDepth > 0)
	{
			// Mirror checks, and shading.
			if (hitinfo.objHit->getMaterial().isMirror())
			{
					// Ray for mirrors.
					RayTracing::Ray_t mirrorRay;
					mirrorRay.o = shaderVal.p;
					mirrorRay.d = gml::normalize(gml::scale(-1, gml::reflect(ray.d, normal)));

					RayTracing::HitInfo_t mirrorHitInfo;

					// If intersection, get the mirror shading color and apply it.
					if (this->rayIntersects(mirrorRay, 0.001f, FLT_MAX, mirrorHitInfo))
					{
							gml::vec3_t mirrorShade = shadeRay(mirrorRay, mirrorHitInfo, remainingRecursionDepth - 1);
							shade = gml::add(shade, gml::mul(hitinfo.objHit->getMaterial().getMirrorRefl(), mirrorShade));
					}
			}

			// Setup indirect lighting.
			RayTracing::Ray_t indirectRay;
			indirectRay.o = shaderVal.p;
			indirectRay.randomDirection(shaderVal.n);

			RayTracing::HitInfo_t indirectHitInfo;

			// If intersection, apply indirect ray for indirect lighting.
			if (this->rayIntersects(indirectRay, 0.001f, FLT_MAX, indirectHitInfo))
			{

					shaderVal.lightDir = indirectRay.d;
					shaderVal.lightRad = shadeRay(indirectRay, indirectHitInfo, remainingRecursionDepth - 1);

					gml::vec3_t indirectShade = m_shaderManager.getShader(hitinfo.objHit->getMaterial())->shade(shaderVal);

					// Add together to the cumulative color.
					shade = gml::add(shade, indirectShade);
			}
	}

	// Note: For debugging your rayIntersection() function, this function
	// returns some non-black constant color at first. When you actually implement
	// this function, then initialize shade to black (0,0,0).

	// Return the cumulative color for the point.
	return shade;

}

}
