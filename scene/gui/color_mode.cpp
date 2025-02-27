/*************************************************************************/
/*  color_mode.cpp                                                       */
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

#include "color_mode.h"

#include "core/math/color.h"
#include "scene/gui/slider.h"
#include "thirdparty/misc/ok_color.h"

ColorMode::ColorMode(ColorPicker *p_color_picker) {
	color_picker = p_color_picker;
}

String ColorModeRGB::get_slider_label(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 3, String(), "Couldn't get slider label.");
	return labels[idx];
}

float ColorModeRGB::get_slider_max(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider max value.");
	Color color = color_picker->get_pick_color();
	return next_power_of_2(MAX(255, color.components[idx] * 255.0)) - 1;
}

float ColorModeRGB::get_slider_value(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider value.");
	return color_picker->get_pick_color().components[idx] * 255;
}

Color ColorModeRGB::get_color() const {
	Vector<float> values = color_picker->get_active_slider_values();
	Color color;
	for (int i = 0; i < 4; i++) {
		color.components[i] = values[i] / 255.0;
	}
	return color;
}

void ColorModeRGB::slider_draw(int p_which) {
	Vector<Vector2> pos;
	pos.resize(4);
	Vector<Color> col;
	col.resize(4);
	HSlider *slider = color_picker->get_slider(p_which);
	Size2 size = slider->get_size();
	Color left_color;
	Color right_color;
	Color color = color_picker->get_pick_color();
	const real_t margin = 16 * color_picker->get_theme_default_base_scale();

	if (p_which == ColorPicker::SLIDER_COUNT) {
		slider->draw_texture_rect(color_picker->get_theme_icon(SNAME("sample_bg"), SNAME("ColorPicker")), Rect2(Point2(0, 0), Size2(size.x, margin)), true);

		left_color = color;
		left_color.a = 0;
		right_color = color;
		right_color.a = 1;
	} else {
		left_color = Color(
				p_which == 0 ? 0 : color.r,
				p_which == 1 ? 0 : color.g,
				p_which == 2 ? 0 : color.b);
		right_color = Color(
				p_which == 0 ? 1 : color.r,
				p_which == 1 ? 1 : color.g,
				p_which == 2 ? 1 : color.b);
	}

	col.set(0, left_color);
	col.set(1, right_color);
	col.set(2, right_color);
	col.set(3, left_color);
	pos.set(0, Vector2(0, 0));
	pos.set(1, Vector2(size.x, 0));
	pos.set(2, Vector2(size.x, margin));
	pos.set(3, Vector2(0, margin));

	slider->draw_polygon(pos, col);
}

String ColorModeHSV::get_slider_label(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 3, String(), "Couldn't get slider label.");
	return labels[idx];
}

float ColorModeHSV::get_slider_max(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider max value.");
	return slider_max[idx];
}

float ColorModeHSV::get_slider_value(int idx) const {
	switch (idx) {
		case 0:
			return color_picker->get_pick_color().get_h() * 360.0;
		case 1:
			return color_picker->get_pick_color().get_s() * 100.0;
		case 2:
			return color_picker->get_pick_color().get_v() * 100.0;
		case 3:
			return Math::round(color_picker->get_pick_color().components[3] * 255.0);
		default:
			ERR_FAIL_V_MSG(0, "Couldn't get slider value.");
	}
}

Color ColorModeHSV::get_color() const {
	Vector<float> values = color_picker->get_active_slider_values();
	Color color;
	color.set_hsv(values[0] / 360.0, values[1] / 100.0, values[2] / 100.0, values[3] / 255.0);
	return color;
}

void ColorModeHSV::slider_draw(int p_which) {
	Vector<Vector2> pos;
	pos.resize(4);
	Vector<Color> col;
	col.resize(4);
	HSlider *slider = color_picker->get_slider(p_which);
	Size2 size = slider->get_size();
	Color left_color;
	Color right_color;
	Color color = color_picker->get_pick_color();
	const real_t margin = 16 * color_picker->get_theme_default_base_scale();

	if (p_which == ColorPicker::SLIDER_COUNT) {
		slider->draw_texture_rect(color_picker->get_theme_icon(SNAME("sample_bg"), SNAME("ColorPicker")), Rect2(Point2(0, 0), Size2(size.x, margin)), true);

		left_color = color;
		left_color.a = 0;
		right_color = color;
		right_color.a = 1;
	} else if (p_which == 0) {
		Ref<Texture2D> hue = color_picker->get_theme_icon(SNAME("color_hue"), SNAME("ColorPicker"));
		slider->draw_set_transform(Point2(), -Math_PI / 2, Size2(1.0, 1.0));
		slider->draw_texture_rect(hue, Rect2(Vector2(margin * -1, 0), Vector2(margin, size.x)), false);
		return;
	} else {
		Color s_col;
		Color v_col;
		s_col.set_hsv(color.get_h(), 0, color.get_v());
		left_color = (p_which == 1) ? s_col : Color(0, 0, 0);
		s_col.set_hsv(color.get_h(), 1, color.get_v());
		v_col.set_hsv(color.get_h(), color.get_s(), 1);
		right_color = (p_which == 1) ? s_col : v_col;
	}
	col.set(0, left_color);
	col.set(1, right_color);
	col.set(2, right_color);
	col.set(3, left_color);
	pos.set(0, Vector2(0, 0));
	pos.set(1, Vector2(size.x, 0));
	pos.set(2, Vector2(size.x, margin));
	pos.set(3, Vector2(0, margin));

	slider->draw_polygon(pos, col);
}

String ColorModeRAW::get_slider_label(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 3, String(), "Couldn't get slider label.");
	return labels[idx];
}

float ColorModeRAW::get_slider_max(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider max value.");
	return slider_max[idx];
}

float ColorModeRAW::get_slider_value(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider value.");
	return color_picker->get_pick_color().components[idx];
}

Color ColorModeRAW::get_color() const {
	Vector<float> values = color_picker->get_active_slider_values();
	Color color;
	for (int i = 0; i < 4; i++) {
		color.components[i] = values[i];
	}
	return color;
}

void ColorModeRAW::slider_draw(int p_which) {
	Vector<Vector2> pos;
	pos.resize(4);
	Vector<Color> col;
	col.resize(4);
	HSlider *slider = color_picker->get_slider(p_which);
	Size2 size = slider->get_size();
	Color left_color;
	Color right_color;
	Color color = color_picker->get_pick_color();
	const real_t margin = 16 * color_picker->get_theme_default_base_scale();

	if (p_which == ColorPicker::SLIDER_COUNT) {
		slider->draw_texture_rect(color_picker->get_theme_icon(SNAME("sample_bg"), SNAME("ColorPicker")), Rect2(Point2(0, 0), Size2(size.x, margin)), true);

		left_color = color;
		left_color.a = 0;
		right_color = color;
		right_color.a = 1;

		col.set(0, left_color);
		col.set(1, right_color);
		col.set(2, right_color);
		col.set(3, left_color);
		pos.set(0, Vector2(0, 0));
		pos.set(1, Vector2(size.x, 0));
		pos.set(2, Vector2(size.x, margin));
		pos.set(3, Vector2(0, margin));

		slider->draw_polygon(pos, col);
	}
}

bool ColorModeRAW::apply_theme() const {
	for (int i = 0; i < 4; i++) {
		HSlider *slider = color_picker->get_slider(i);
		slider->remove_theme_icon_override("grabber");
		slider->remove_theme_icon_override("grabber_highlight");
		slider->remove_theme_style_override("slider");
		slider->remove_theme_constant_override("grabber_offset");
	}

	return true;
}

String ColorModeOKHSL::get_slider_label(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 3, String(), "Couldn't get slider label.");
	return labels[idx];
}

float ColorModeOKHSL::get_slider_max(int idx) const {
	ERR_FAIL_INDEX_V_MSG(idx, 4, 0, "Couldn't get slider max value.");
	return slider_max[idx];
}

float ColorModeOKHSL::get_slider_value(int idx) const {
	switch (idx) {
		case 0:
			return color_picker->get_pick_color().get_ok_hsl_h() * 360.0;
		case 1:
			return color_picker->get_pick_color().get_ok_hsl_s() * 100.0;
		case 2:
			return color_picker->get_pick_color().get_ok_hsl_l() * 100.0;
		case 3:
			return Math::round(color_picker->get_pick_color().components[3] * 255.0);
		default:
			ERR_FAIL_V_MSG(0, "Couldn't get slider value.");
	}
}

Color ColorModeOKHSL::get_color() const {
	Vector<float> values = color_picker->get_active_slider_values();
	Color color;
	color.set_ok_hsl(values[0] / 360.0, values[1] / 100.0, values[2] / 100.0, values[3] / 255.0);
	return color;
}

void ColorModeOKHSL::slider_draw(int p_which) {
	HSlider *slider = color_picker->get_slider(p_which);
	Size2 size = slider->get_size();
	const real_t margin = 16 * color_picker->get_theme_default_base_scale();

	if (p_which == 0) { // H
		Ref<Texture2D> hue = color_picker->get_theme_icon(SNAME("color_hue"), SNAME("ColorPicker"));
		slider->draw_set_transform(Point2(), -Math_PI / 2, Size2(1.0, 1.0));
		slider->draw_texture_rect(hue, Rect2(Vector2(margin * -1, 0), Vector2(margin, size.x)), false);
		return;
	}

	Vector<Vector2> pos;
	Vector<Color> col;
	Color left_color;
	Color right_color;
	Color color = color_picker->get_pick_color();

	if (p_which == 2) { // L
		pos.resize(6);
		col.resize(6);
		left_color = Color(0, 0, 0);
		Color middle_color;
		middle_color.set_ok_hsl(color.get_ok_hsl_h(), color.get_ok_hsl_s(), 0.5);
		right_color.set_ok_hsl(color.get_ok_hsl_h(), color.get_ok_hsl_s(), 1);

		col.set(0, left_color);
		col.set(1, middle_color);
		col.set(2, right_color);
		col.set(3, right_color);
		col.set(4, middle_color);
		col.set(5, left_color);
		pos.set(0, Vector2(0, 0));
		pos.set(1, Vector2(size.x * 0.5, 0));
		pos.set(2, Vector2(size.x, 0));
		pos.set(3, Vector2(size.x, margin));
		pos.set(4, Vector2(size.x * 0.5, margin));
		pos.set(5, Vector2(0, margin));
	} else { // A / S
		pos.resize(4);
		col.resize(4);

		if (p_which == ColorPicker::SLIDER_COUNT) {
			slider->draw_texture_rect(color_picker->get_theme_icon(SNAME("sample_bg"), SNAME("ColorPicker")), Rect2(Point2(0, 0), Size2(size.x, margin)), true);

			left_color = color;
			left_color.a = 0;
			right_color = color;
			right_color.a = 1;
		} else {
			left_color.set_ok_hsl(color.get_ok_hsl_h(), 0, color.get_ok_hsl_l());
			right_color.set_ok_hsl(color.get_ok_hsl_h(), 1, color.get_ok_hsl_l());
		}

		col.set(0, left_color);
		col.set(1, right_color);
		col.set(2, right_color);
		col.set(3, left_color);
		pos.set(0, Vector2(0, 0));
		pos.set(1, Vector2(size.x, 0));
		pos.set(2, Vector2(size.x, margin));
		pos.set(3, Vector2(0, margin));
	}

	slider->draw_polygon(pos, col);
}
