/*************************************************************************/
/*  config.cpp                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifdef GLES3_ENABLED

#include "config.h"
#include "core/config/project_settings.h"
#include "core/templates/vector.h"

#ifdef ANDROID_ENABLED
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

using namespace GLES3;

#define _GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

Config *Config::singleton = nullptr;

Config::Config() {
	singleton = this;

	{
		GLint max_extensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &max_extensions);
		for (int i = 0; i < max_extensions; i++) {
			const GLubyte *s = glGetStringi(GL_EXTENSIONS, i);
			if (!s) {
				break;
			}
			extensions.insert((const char *)s);
		}
	}

	bptc_supported = extensions.has("GL_ARB_texture_compression_bptc") || extensions.has("EXT_texture_compression_bptc");
#ifdef GLES_OVER_GL
	float_texture_supported = true;
	etc2_supported = false;
	s3tc_supported = true;
	rgtc_supported = true; //RGTC - core since OpenGL version 3.0
#else
	float_texture_supported = extensions.has("GL_ARB_texture_float") || extensions.has("GL_OES_texture_float");
	etc2_supported = true;
#if defined(ANDROID_ENABLED) || defined(IOS_ENABLED)
	// Some Android devices report support for S3TC but we don't expect that and don't export the textures.
	// This could be fixed but so few devices support it that it doesn't seem useful (and makes bigger APKs).
	// For good measure we do the same hack for iOS, just in case.
	s3tc_supported = false;
#else
	s3tc_supported = extensions.has("GL_EXT_texture_compression_dxt1") || extensions.has("GL_EXT_texture_compression_s3tc") || extensions.has("WEBGL_compressed_texture_s3tc");
#endif
	rgtc_supported = extensions.has("GL_EXT_texture_compression_rgtc") || extensions.has("GL_ARB_texture_compression_rgtc") || extensions.has("EXT_texture_compression_rgtc");
#endif

#ifdef GLES_OVER_GL
	use_rgba_2d_shadows = false;
#else
	use_rgba_2d_shadows = !(float_texture_supported && extensions.has("GL_EXT_texture_rg"));
#endif

	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_vertex_texture_image_units);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_buffer_size);
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &max_viewport_size);

	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment);

	// the use skeleton software path should be used if either float texture is not supported,
	// OR max_vertex_texture_image_units is zero
	use_skeleton_software = (float_texture_supported == false) || (max_vertex_texture_image_units == 0);

	support_anisotropic_filter = extensions.has("GL_EXT_texture_filter_anisotropic");
	if (support_anisotropic_filter) {
		glGetFloatv(_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropic_level);
		anisotropic_level = MIN(float(1 << int(GLOBAL_GET("rendering/textures/default_filters/anisotropic_filtering_level"))), anisotropic_level);
	}

	multiview_supported = extensions.has("GL_OVR_multiview2") || extensions.has("GL_OVR_multiview");
#ifdef ANDROID_ENABLED
	if (multiview_supported) {
		eglFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
		if (eglFramebufferTextureMultiviewOVR == nullptr) {
			multiview_supported = false;
		}
	}
#endif

	force_vertex_shading = false; //GLOBAL_GET("rendering/quality/shading/force_vertex_shading");
	use_nearest_mip_filter = GLOBAL_GET("rendering/textures/default_filters/use_nearest_mipmap_filter");

	use_depth_prepass = bool(GLOBAL_GET("rendering/driver/depth_prepass/enable"));
	if (use_depth_prepass) {
		String vendors = GLOBAL_GET("rendering/driver/depth_prepass/disable_for_vendors");
		Vector<String> vendor_match = vendors.split(",");
		String renderer = (const char *)glGetString(GL_RENDERER);
		for (int i = 0; i < vendor_match.size(); i++) {
			String v = vendor_match[i].strip_edges();
			if (v == String()) {
				continue;
			}

			if (renderer.findn(v) != -1) {
				use_depth_prepass = false;
			}
		}
	}

	max_renderable_elements = GLOBAL_GET("rendering/limits/opengl/max_renderable_elements");
	max_renderable_lights = GLOBAL_GET("rendering/limits/opengl/max_renderable_lights");
	max_lights_per_object = GLOBAL_GET("rendering/limits/opengl/max_lights_per_object");
}

Config::~Config() {
	singleton = nullptr;
}

#endif // GLES3_ENABLED
