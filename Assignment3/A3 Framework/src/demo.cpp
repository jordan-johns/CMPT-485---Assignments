
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

#include "GL3/gl3w.h"
#include <GL/glext.h>
#include <cstdio>
#include <cstring>

#include "GML/gml.h"
#include "Objects/Models/sphere.h"

#include "glUtils.h"
#include "demo.h"


DemoProgram::DemoProgram() : UI::Callbacks()
{
	m_scene = 0;
	m_nObjects = 0;
	m_geometry = 0;
	m_nGeometry = 0;

	m_sRGBframebuffer = glIsEnabled(GL_FRAMEBUFFER_SRGB_EXT) == GL_TRUE;
	m_renderWireframe = false;
}

DemoProgram::~DemoProgram()
{
	if (m_scene)
	{
		for (GLuint i=0; i<m_nObjects; i++)
		{
			if (m_scene[i]) delete m_scene[i];
		}
		delete[] m_scene;
	}
	if (m_geometry)
	{
		for (GLuint i=0; i<m_nGeometry; i++)
		{
			if (m_geometry[i]) delete m_geometry[i];
		}
		delete[] m_geometry;
	}
}

bool DemoProgram::init()
{
	// Initialize the shader manager
	if ( !m_shaderManager.init() )
	{
		fprintf(stderr, "ERROR! Could not initialize Shader Manager.\n");
		return false;
	}

	// Create geometry
	m_nGeometry = 1;
	m_geometry = new Object::Geometry*[m_nGeometry];
	memset(m_geometry, 0x00, sizeof(Object::Geometry*)*m_nGeometry);

	m_geometry[0] = new Object::Models::Sphere();
	if ( !m_geometry[0] || !((Object::Models::Sphere*)m_geometry[0])->init(3) || isGLError() )
	{
		return false;
	}

	// Create some material properties for the object
	Material::Material mat; // material for the object
	mat.setShaderType(Material::SIMPLE); // simple shader
	mat.setSurfReflectance(gml::vec3_t(0.4, 0.5, 0.6)); // set the surface reflectance

	// Create the object
	m_nObjects = 1;
	m_scene = new Object::Object*[m_nObjects];
	m_scene[0] = new Object::Object(m_geometry[0], mat, gml::identity4());
	// Note: Simple shader does not do any geometry transformation.
	//  So, there's no point in giving anything other than identity for the transformation

	printf(
			"Controls:\n"
			"  [g] -- Toggle sRGB framebuffer\n"
			"  [f] -- Toggle wireframe rendering\n"
			"  [ESC] -- Quit\n"
	);

	return true;
}


void DemoProgram::windowResize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void DemoProgram::specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state)
{
	switch (key)
	{
	case UI::KEY_ESC:
		if (state == UI::BUTTON_UP)
		{
			UI::exitMainLoop();
		}
		break;
	case UI::KEY_G:
		if (state == UI::BUTTON_DOWN)
		{
			if (m_sRGBframebuffer)
			{
				glDisable(GL_FRAMEBUFFER_SRGB_EXT);
			}
			else
			{
				glEnable(GL_FRAMEBUFFER_SRGB_EXT);
			}
			m_sRGBframebuffer = glIsEnabled(GL_FRAMEBUFFER_SRGB_EXT) == GL_TRUE;
			if (m_sRGBframebuffer)
			{
				printf("sRGB framebuffer enabled\n");
			}
			else
			{
				printf("sRGB framebuffer disabled\n");
			}
		}
		break;
	case UI::KEY_F:
		if (state == UI::BUTTON_DOWN)
		{
			m_renderWireframe = !m_renderWireframe;
		}
		break;
	default:
		break;
	}
}

void DemoProgram::repaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	Shader::GLProgUniforms shaderUniforms;


	// Query is disabled
	// When enabled, it can be a useful debugging tool.
	//  -- The queries will tell you how many primitives & fragments were rendered.

	//#define QUERY
#if QUERY
	GLuint query[2];
	glGenQueries(2, query);

	glBeginQuery(GL_SAMPLES_PASSED, query[0]);
	glBeginQuery(GL_PRIMITIVES_GENERATED, query[1]);
#endif

	// Set up values for uniforms that are constant to the world
	shaderUniforms.m_ambientRad = gml::vec3_t(0.6, 0.6, 0.6);
	// note: Normally we wouldn't set the ambient radiance so high.
	//  in this demo it is the only light, so I turned it up.

	for (GLuint i=0; i<m_nObjects; i++)
	{
		// Get the shader for this object
		const Shader::Shader *shader = m_shaderManager.getShader(m_scene[i]->getMaterial());

		// bind the shader to the OpenGL context.
		// must always bind a shader before you can use it, and only one shader can be bound
		// at a time.
		shader->bindGL();
		if (isGLError()) return;

		// Set up values for Object-specific uniforms
		shaderUniforms.m_surfRefl = gml::vec3_t(0.8, 0.4, 0.2); // surface reflectance

		// Load the uniform values into the shader program
		if ( !shader->setUniforms(shaderUniforms) || isGLError() ) return;

		// render the object
		m_scene[i]->rasterize();
		if (isGLError()) return;

		// Unbind the shader program from the OpenGL context
		shader->unbindGL();
	}

#if QUERY
	glEndQuery(GL_SAMPLES_PASSED);
	glEndQuery(GL_PRIMITIVES_GENERATED);

	GLuint nSamples=-1, nPrims=-1;
	glGetQueryObjectuiv(query[0], GL_QUERY_RESULT, &nSamples);
	glGetQueryObjectuiv(query[1], GL_QUERY_RESULT, &nPrims);

	glDeleteQueries(2, query);
	printf("nSamples = %u, nPrims = %u\n", nSamples, nPrims);
#endif


	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Force everything outstanding to complete
	glFinish();
}
