/**
 * -----------------------------------------------------
 * File        resource.cpp
 * Authors     David O., Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 * This file was automatically generated by wxrc, do not edit by hand.
 * Copyright (C) 2013-2016 David O., Impact
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>

#if wxCHECK_VERSION(2,8,5) && wxABI_VERSION >= 20805
#define XRC_ADD_FILE(name, data, size, mime) \
	wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mime)
#else
#define XRC_ADD_FILE(name, data, size, mime) \
	wxMemoryFSHandler::AddFile(name, data, size)
#endif

static size_t xml_res_size_0 = 4286;
static unsigned char xml_res_file_0[] = {
	0, 0, 1, 0, 1, 0, 32, 32, 0, 0, 1, 0, 32, 0, 168, 16, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 32, 0, 0, 0, 64,
	0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 128, 16, 0, 0, 136, 19, 0, 0, 136, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 24, 0, 0, 0, 177, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 209, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 246, 0, 0, 0, 141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 194, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 106, 60, 255, 0, 143, 82, 255, 0, 150, 87, 255, 0, 175, 102, 255, 0, 163, 97, 255, 0,
	133, 77, 255, 0, 129, 74, 255, 0, 103, 58, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 121, 70, 255, 0, 145, 85, 255, 0, 162, 99, 255, 0, 155,
	95, 255, 0, 122, 70, 255, 0, 144, 88, 255, 0, 178, 105, 255, 0, 184, 109, 255, 0, 178, 105,
	255, 0, 178, 105, 255, 0, 136, 79, 255, 0, 121, 70, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 246, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 144, 86, 255, 0, 192, 118, 255, 0, 190, 114, 255, 0, 196, 118, 255, 0, 191, 118, 255, 0,
	189, 113, 255, 0, 187, 117, 255, 0, 188, 113, 255, 0, 185, 111, 255, 0, 183, 113, 255, 0, 185,
	115, 255, 0, 184, 112, 255, 0, 180, 111, 255, 0, 135, 80, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 147, 87, 255, 0, 118, 72, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 52,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 167,
	104, 255, 0, 211, 129, 255, 0, 202, 124, 255, 0, 199, 121, 255, 0, 197, 123, 255, 0, 196, 119,
	255, 0, 195, 120, 255, 0, 193, 117, 255, 0, 192, 116, 255, 0, 191, 116, 255, 0, 191, 116, 255,
	0, 189, 115, 255, 0, 190, 118, 255, 0, 191, 116, 255, 0, 194, 118, 255, 0, 155, 95, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 173, 105, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 246,
	0, 0, 0, 86, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 139,
	84, 255, 0, 183, 112, 255, 0, 220, 138, 255, 0, 206, 131, 255, 0, 209, 132, 255, 0, 209, 129,
	255, 0, 208, 131, 255, 0, 201, 124, 255, 0, 198, 121, 255, 0, 197, 121, 255, 0, 196, 120, 255,
	0, 197, 121, 255, 0, 196, 123, 255, 0, 196, 123, 255, 0, 196, 124, 255, 0, 198, 122, 255, 0,
	199, 127, 255, 0, 152, 93, 255, 0, 115, 68, 255, 0, 162, 98, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 175, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 210, 131, 255, 0, 178, 110, 255, 0, 221, 137, 255, 0, 212, 132,
	255, 0, 213, 134, 255, 0, 162, 100, 255, 0, 189, 117, 255, 0, 211, 134, 255, 0, 212, 132, 255,
	0, 213, 135, 255, 0, 209, 130, 255, 0, 205, 128, 255, 0, 202, 130, 255, 0, 202, 127, 255, 0,
	202, 126, 255, 0, 203, 130, 255, 0, 204, 129, 255, 0, 162, 99, 255, 0, 209, 135, 255, 0, 193,
	120, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 166, 106, 255, 0, 236, 153, 255, 0, 227,
	142, 255, 0, 222, 141, 255, 0, 217, 136, 255, 0, 216, 136, 255, 0, 220, 140, 255, 0, 223, 141,
	255, 0, 217, 136, 255, 0, 182, 113, 255, 0, 163, 101, 255, 0, 188, 117, 255, 0, 218, 137, 255,
	0, 214, 134, 255, 0, 212, 133, 255, 0, 208, 134, 255, 0, 208, 131, 255, 0, 213, 139, 255, 0,
	138, 85, 255, 0, 212, 133, 255, 0, 186, 119, 255, 0, 172, 111, 255, 0, 137, 84, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 25, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 223, 151, 255, 0, 234, 150, 255, 0, 227, 144, 255, 0, 223, 141, 255, 0, 221, 140,
	255, 0, 222, 140, 255, 0, 225, 148, 255, 0, 174, 111, 255, 0, 128, 79, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 145, 90, 255, 0, 183, 118, 255, 0, 228, 149, 255, 0, 228, 144, 255,
	0, 223, 141, 255, 0, 226, 143, 255, 0, 200, 127, 255, 0, 207, 131, 255, 0, 186, 120, 255, 0,
	167, 106, 255, 0, 204, 129, 255, 0, 157, 102, 255, 0, 143, 88, 255, 0, 161, 103, 255, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 158, 0, 0, 0, 0, 0, 0, 0, 0, 0, 236, 158, 255, 0, 234,
	152, 255, 0, 232, 152, 255, 0, 228, 145, 255, 0, 225, 144, 255, 0, 226, 146, 255, 0, 189, 120,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 187, 118, 255, 0, 203, 133, 255, 0, 167, 105, 255, 0, 243, 159, 255, 0, 138, 86, 255, 0,
	184, 117, 255, 0, 232, 150, 255, 0, 221, 145, 255, 0, 216, 138, 255, 0, 211, 134, 255, 0, 214,
	136, 255, 0, 164, 103, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 25,
	0, 0, 0, 86, 0, 237, 159, 255, 0, 236, 156, 255, 0, 234, 151, 255, 0, 230, 150, 255, 0, 231,
	154, 255, 0, 225, 144, 255, 0, 134, 84, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 224, 147, 255, 0, 158, 100, 255,
	0, 217, 139, 255, 0, 0, 0, 255, 0, 209, 139, 255, 0, 234, 150, 255, 0, 221, 145, 255, 0, 218,
	140, 255, 0, 219, 140, 255, 0, 206, 136, 255, 0, 174, 111, 255, 0, 148, 96, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 239, 161, 255, 0, 237, 160, 255, 0, 236,
	153, 255, 0, 233, 156, 255, 0, 236, 158, 255, 0, 188, 121, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 137, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 151,
	96, 255, 0, 255, 166, 255, 0, 136, 86, 255, 0, 0, 0, 255, 0, 204, 132, 255, 0, 238, 158, 255,
	0, 226, 149, 255, 0, 224, 144, 255, 0, 229, 148, 255, 0, 194, 126, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 241, 162, 255, 0, 241,
	161, 255, 0, 238, 159, 255, 0, 234, 157, 255, 0, 237, 154, 255, 0, 173, 113, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 192, 125, 255, 0, 218, 141, 255, 0, 0, 0, 255, 0, 157, 103, 255, 0, 226, 147, 255,
	0, 239, 155, 255, 0, 231, 150, 255, 0, 228, 148, 255, 0, 235, 154, 255, 0, 168, 107, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 244,
	163, 255, 0, 243, 160, 255, 0, 239, 160, 255, 0, 234, 153, 255, 0, 238, 161, 255, 0, 165, 109,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 118, 0, 0, 0, 16, 0, 0, 0, 76, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 150, 97, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 192, 124, 255, 0, 252,
	169, 255, 0, 240, 157, 255, 0, 236, 154, 255, 0, 233, 154, 255, 0, 238, 156, 255, 0, 183, 121,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 128, 82, 255, 0, 163,
	105, 255, 0, 246, 165, 255, 0, 242, 159, 255, 0, 238, 156, 255, 0, 234, 159, 255, 0, 236, 158,
	255, 0, 182, 122, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 226, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 204,
	137, 255, 0, 255, 172, 255, 0, 243, 159, 255, 0, 240, 157, 255, 0, 237, 155, 255, 0, 239, 156,
	255, 0, 201, 131, 255, 0, 212, 138, 255, 0, 146, 94, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 189, 126, 255, 0, 253, 167, 255, 0, 244, 161, 255, 0, 237, 161, 255, 0,
	234, 154, 255, 0, 233, 153, 255, 0, 228, 150, 255, 0, 136, 88, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 228, 157, 255, 0, 252, 167, 255, 0, 247, 163, 255, 0, 244, 167, 255,
	0, 241, 158, 255, 0, 241, 164, 255, 0, 243, 161, 255, 0, 214, 143, 255, 0, 202, 136, 255, 0,
	193, 126, 255, 0, 137, 91, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 184, 120, 255, 0, 214, 141, 255,
	0, 245, 169, 255, 0, 236, 156, 255, 0, 232, 153, 255, 0, 230, 154, 255, 0, 231, 153, 255, 0,
	189, 124, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 255, 171, 255, 0, 253, 169,
	255, 0, 249, 167, 255, 0, 248, 167, 255, 0, 245, 162, 255, 0, 243, 161, 255, 0, 242, 160, 255,
	0, 243, 161, 255, 0, 249, 164, 255, 0, 199, 134, 255, 0, 212, 145, 255, 0, 167, 109, 255, 0,
	202, 133, 255, 0, 226, 151, 255, 0, 167, 111, 255, 0, 251, 170, 255, 0, 239, 158, 255, 0, 232,
	154, 255, 0, 229, 152, 255, 0, 230, 152, 255, 0, 232, 153, 255, 0, 212, 141, 255, 0, 161, 110,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 212, 140, 255, 0, 147, 95, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 168, 113, 255, 0, 248, 168, 255, 0, 255, 178, 255, 0, 251, 168,
	255, 0, 251, 167, 255, 0, 249, 165, 255, 0, 251, 167, 255, 0, 249, 166, 255, 0, 247, 168, 255,
	0, 245, 164, 255, 0, 247, 164, 255, 0, 241, 162, 255, 0, 248, 164, 255, 0, 247, 164, 255, 0,
	249, 172, 255, 0, 0, 0, 255, 0, 197, 130, 255, 0, 243, 162, 255, 0, 234, 156, 255, 0, 228, 151,
	255, 0, 226, 150, 255, 0, 228, 151, 255, 0, 230, 157, 255, 0, 235, 156, 255, 0, 222, 152, 255,
	0, 172, 113, 255, 0, 197, 132, 255, 0, 255, 172, 255, 0, 152, 99, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 205, 138, 255, 0, 229, 160, 255, 0, 255, 183, 255, 0, 253, 174, 255, 0,
	255, 171, 255, 0, 255, 171, 255, 0, 172, 115, 255, 0, 226, 153, 255, 0, 255, 172, 255, 0, 252,
	168, 255, 0, 248, 165, 255, 0, 249, 166, 255, 0, 250, 166, 255, 0, 251, 172, 255, 0, 253, 169,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 198, 131, 255, 0, 237, 158, 255, 0, 230, 153, 255, 0, 226,
	150, 255, 0, 227, 156, 255, 0, 228, 157, 255, 0, 235, 161, 255, 0, 246, 165, 255, 0, 233, 155,
	255, 0, 227, 151, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 224,
	152, 255, 0, 225, 153, 255, 0, 255, 181, 255, 0, 255, 172, 255, 0, 255, 177, 255, 0, 227, 155,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 184, 121, 255, 0, 255, 175, 255, 0, 255, 172, 255, 0, 255,
	171, 255, 0, 255, 178, 255, 0, 255, 173, 255, 0, 255, 174, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 187, 125, 255, 0, 233, 155, 255, 0, 227, 154, 255, 0, 229, 152, 255, 0, 230, 153,
	255, 0, 234, 162, 255, 0, 246, 164, 255, 0, 255, 177, 255, 0, 162, 108, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 243, 165, 255, 0, 255, 183, 255, 0, 255,
	177, 255, 0, 255, 174, 255, 0, 255, 186, 255, 0, 217, 146, 255, 0, 0, 0, 255, 0, 155, 102, 255,
	0, 236, 158, 255, 0, 255, 184, 255, 0, 255, 173, 255, 0, 255, 174, 255, 0, 255, 183, 255, 0,
	255, 188, 255, 0, 255, 183, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 158,
	106, 255, 0, 220, 148, 255, 0, 238, 159, 255, 0, 238, 165, 255, 0, 239, 159, 255, 0, 250, 167,
	255, 0, 211, 145, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 255, 179, 255, 0, 255, 184, 255, 0, 255, 186, 255, 0, 255, 190, 255, 0, 255, 206,
	255, 0, 187, 125, 255, 0, 178, 120, 255, 0, 252, 177, 255, 0, 255, 178, 255, 0, 255, 184, 255,
	0, 255, 182, 255, 0, 255, 184, 255, 0, 255, 192, 255, 0, 255, 182, 255, 0, 193, 138, 255, 0,
	0, 0, 243, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 161, 107, 255,
	0, 208, 138, 255, 0, 252, 169, 255, 0, 255, 179, 255, 0, 190, 129, 255, 0, 186, 124, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 186, 129, 255, 0, 255, 181, 255, 0, 255,
	179, 255, 0, 255, 183, 255, 0, 197, 134, 255, 0, 223, 154, 255, 0, 217, 148, 255, 0, 255, 183,
	255, 0, 255, 182, 255, 0, 255, 179, 255, 0, 255, 180, 255, 0, 255, 185, 255, 0, 255, 199, 255,
	0, 212, 148, 255, 0, 220, 157, 255, 0, 244, 177, 255, 0, 0, 0, 75, 0, 0, 0, 226, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 179, 119, 255, 0, 201, 138,
	255, 0, 180, 124, 255, 0, 149, 101, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 207, 140, 255, 0, 255, 186, 255, 0, 255, 178, 255, 0, 255, 189, 255, 0, 174, 121, 255, 0,
	255, 177, 255, 0, 255, 189, 255, 0, 255, 188, 255, 0, 255, 180, 255, 0, 255, 182, 255, 0, 255,
	186, 255, 0, 255, 190, 255, 0, 177, 123, 255, 0, 253, 178, 255, 0, 193, 139, 255, 0, 0, 0, 255,
	0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 160, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 219, 146, 255, 0, 255, 193, 255, 0, 255, 178, 255, 0, 255, 179, 255, 0,
	255, 173, 255, 0, 255, 182, 255, 0, 255, 181, 255, 0, 255, 181, 255, 0, 255, 182, 255, 0, 255,
	185, 255, 0, 255, 198, 255, 0, 238, 166, 255, 0, 255, 180, 255, 0, 161, 113, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 192, 0, 0, 0, 255, 0, 0, 0, 255,
	0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 239, 161, 255, 0, 255, 184, 255, 0, 255, 179, 255, 0, 255, 181, 255,
	0, 255, 180, 255, 0, 255, 184, 255, 0, 255, 186, 255, 0, 255, 188, 255, 0, 255, 185, 255, 0,
	255, 190, 255, 0, 255, 200, 255, 0, 211, 148, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 192, 0, 0,
	0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 222,
	151, 255, 0, 188, 126, 255, 0, 255, 188, 255, 0, 255, 182, 255, 0, 255, 178, 255, 0, 255, 178,
	255, 0, 255, 183, 255, 0, 255, 185, 255, 0, 255, 182, 255, 0, 255, 187, 255, 0, 255, 192, 255,
	0, 255, 180, 255, 0, 192, 134, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
	0, 0, 0, 103, 0, 0, 0, 103, 0, 0, 0, 69, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 140, 93, 255,
	0, 223, 150, 255, 0, 247, 167, 255, 0, 255, 177, 255, 0, 255, 182, 255, 0, 255, 180, 255, 0,
	255, 179, 255, 0, 255, 188, 255, 0, 255, 188, 255, 0, 255, 198, 255, 0, 233, 160, 255, 0, 152,
	103, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 109, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 212, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 171, 114,
	255, 0, 235, 159, 255, 0, 213, 144, 255, 0, 255, 183, 255, 0, 255, 181, 255, 0, 255, 188, 255,
	0, 255, 184, 255, 0, 201, 141, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0,
	0, 255, 0, 0, 0, 209, 0, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 198, 133, 255, 0, 255, 189,
	255, 0, 255, 185, 255, 0, 245, 165, 255, 0, 182, 124, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0,
	255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 107, 0, 0, 0, 226, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 217, 151, 255, 0, 255, 182, 255, 0, 184, 125, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 158, 0, 0, 0, 255, 0, 0, 0, 255, 0,
	0, 0, 255, 0, 160, 108, 255, 0, 206, 138, 255, 0, 0, 0, 255, 0, 220, 149, 255, 0, 249, 168, 255,
	0, 161, 107, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 63, 0, 0, 0, 31, 0, 0, 0, 7, 0,
	0, 0, 7, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	128, 0, 0, 0, 224, 0, 0, 0, 248, 0, 0, 0, 254, 0, 0, 0, 255, 224, 0, 1, 255, 224, 0, 7, 255, 240,
	0, 15, 255, 248, 0, 15 };

static size_t xml_res_size_1 = 1916;
static unsigned char xml_res_file_1[] = {
	60, 63, 120, 109, 108, 32, 118, 101, 114, 115, 105, 111, 110, 61, 34, 49, 46, 48, 34, 32, 101,
	110, 99, 111, 100, 105, 110, 103, 61, 34, 85, 84, 70, 45, 56, 34, 63, 62, 10, 60, 114, 101,
	115, 111, 117, 114, 99, 101, 32, 120, 109, 108, 110, 115, 61, 34, 104, 116, 116, 112, 58,
	47, 47, 119, 119, 119, 46, 119, 120, 119, 105, 110, 100, 111, 119, 115, 46, 111, 114, 103,
	47, 119, 120, 120, 114, 99, 34, 32, 118, 101, 114, 115, 105, 111, 110, 61, 34, 50, 46, 53,
	46, 51, 46, 48, 34, 62, 10, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108, 97, 115, 115,
	61, 34, 119, 120, 70, 114, 97, 109, 101, 34, 32, 110, 97, 109, 101, 61, 34, 117, 112, 100,
	97, 116, 101, 70, 114, 97, 109, 101, 34, 62, 10, 32, 32, 32, 32, 60, 115, 116, 121, 108, 101,
	62, 119, 120, 67, 65, 80, 84, 73, 79, 78, 32, 124, 32, 119, 120, 67, 76, 79, 83, 69, 95, 66,
	79, 88, 32, 124, 32, 119, 120, 77, 73, 78, 73, 77, 73, 90, 69, 95, 66, 79, 88, 32, 124, 32, 119,
	120, 83, 89, 83, 84, 69, 77, 95, 77, 69, 78, 85, 60, 47, 115, 116, 121, 108, 101, 62, 10, 32,
	32, 32, 32, 60, 116, 105, 116, 108, 101, 62, 85, 112, 100, 97, 116, 101, 32, 67, 97, 108, 108,
	65, 100, 109, 105, 110, 32, 67, 108, 105, 101, 110, 116, 60, 47, 116, 105, 116, 108, 101,
	62, 10, 32, 32, 32, 32, 60, 99, 101, 110, 116, 101, 114, 101, 100, 62, 49, 60, 47, 99, 101,
	110, 116, 101, 114, 101, 100, 62, 10, 32, 32, 32, 32, 60, 105, 99, 111, 110, 62, 114, 101,
	115, 111, 117, 114, 99, 101, 46, 99, 112, 112, 36, 99, 97, 108, 108, 97, 100, 109, 105, 110,
	95, 99, 108, 105, 101, 110, 116, 95, 117, 112, 100, 97, 116, 101, 114, 46, 105, 99, 111, 60,
	47, 105, 99, 111, 110, 62, 10, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108,
	97, 115, 115, 61, 34, 119, 120, 80, 97, 110, 101, 108, 34, 32, 110, 97, 109, 101, 61, 34, 117,
	112, 100, 97, 116, 101, 80, 97, 110, 101, 108, 34, 62, 10, 32, 32, 32, 32, 32, 32, 60, 115,
	116, 121, 108, 101, 62, 119, 120, 78, 79, 95, 66, 79, 82, 68, 69, 82, 32, 124, 32, 119, 120,
	84, 65, 66, 95, 84, 82, 65, 86, 69, 82, 83, 65, 76, 60, 47, 115, 116, 121, 108, 101, 62, 10,
	32, 32, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108, 97, 115, 115, 61, 34, 119,
	120, 66, 111, 120, 83, 105, 122, 101, 114, 34, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111,
	114, 105, 101, 110, 116, 62, 119, 120, 86, 69, 82, 84, 73, 67, 65, 76, 60, 47, 111, 114, 105,
	101, 110, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99,
	108, 97, 115, 115, 61, 34, 115, 105, 122, 101, 114, 105, 116, 101, 109, 34, 62, 10, 32, 32,
	32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 112, 116, 105, 111, 110, 62, 48, 60, 47, 111, 112,
	116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 102, 108, 97, 103, 62,
	119, 120, 65, 76, 76, 32, 124, 32, 119, 120, 69, 88, 80, 65, 78, 68, 60, 47, 102, 108, 97, 103,
	62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 98, 111, 114, 100, 101, 114, 62, 53, 60,
	47, 98, 111, 114, 100, 101, 114, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 98,
	106, 101, 99, 116, 32, 99, 108, 97, 115, 115, 61, 34, 119, 120, 71, 97, 117, 103, 101, 34,
	32, 110, 97, 109, 101, 61, 34, 112, 114, 111, 103, 114, 101, 115, 115, 66, 97, 114, 34, 62,
	10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 116, 121, 108, 101, 62, 119, 120,
	71, 65, 95, 72, 79, 82, 73, 90, 79, 78, 84, 65, 76, 60, 47, 115, 116, 121, 108, 101, 62, 10,
	32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 97, 110, 103, 101, 62, 49, 48, 48, 60,
	47, 114, 97, 110, 103, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 118,
	97, 108, 117, 101, 62, 48, 60, 47, 118, 97, 108, 117, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32,
	32, 32, 32, 60, 47, 111, 98, 106, 101, 99, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47,
	111, 98, 106, 101, 99, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99,
	116, 32, 99, 108, 97, 115, 115, 61, 34, 115, 105, 122, 101, 114, 105, 116, 101, 109, 34, 62,
	10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 112, 116, 105, 111, 110, 62, 48, 60, 47,
	111, 112, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 102, 108,
	97, 103, 62, 119, 120, 65, 76, 73, 71, 78, 95, 67, 69, 78, 84, 69, 82, 32, 124, 32, 119, 120,
	65, 76, 76, 60, 47, 102, 108, 97, 103, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 98,
	111, 114, 100, 101, 114, 62, 49, 48, 60, 47, 98, 111, 114, 100, 101, 114, 62, 10, 32, 32, 32,
	32, 32, 32, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108, 97, 115, 115, 61, 34,
	119, 120, 83, 116, 97, 116, 105, 99, 84, 101, 120, 116, 34, 32, 110, 97, 109, 101, 61, 34,
	100, 108, 73, 110, 102, 111, 34, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60,
	102, 111, 110, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115,
	105, 122, 101, 62, 49, 54, 60, 47, 115, 105, 122, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32,
	32, 32, 32, 32, 32, 32, 60, 115, 116, 121, 108, 101, 62, 110, 111, 114, 109, 97, 108, 60, 47,
	115, 116, 121, 108, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60,
	119, 101, 105, 103, 104, 116, 62, 110, 111, 114, 109, 97, 108, 60, 47, 119, 101, 105, 103,
	104, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 117, 110, 100,
	101, 114, 108, 105, 110, 101, 100, 62, 48, 60, 47, 117, 110, 100, 101, 114, 108, 105, 110,
	101, 100, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 102, 111, 110, 116,
	62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 108, 97, 98, 101, 108, 62, 48, 107,
	66, 32, 111, 102, 32, 48, 75, 66, 32, 40, 48, 32, 107, 66, 47, 115, 41, 46, 32, 84, 105, 109,
	101, 58, 32, 48, 44, 48, 48, 32, 83, 101, 99, 111, 110, 100, 115, 60, 47, 108, 97, 98, 101,
	108, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 119, 114, 97, 112, 62, 45, 49,
	60, 47, 119, 114, 97, 112, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 111, 98, 106,
	101, 99, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 111, 98, 106, 101, 99, 116, 62,
	10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108, 97, 115, 115,
	61, 34, 115, 105, 122, 101, 114, 105, 116, 101, 109, 34, 62, 10, 32, 32, 32, 32, 32, 32, 32,
	32, 32, 32, 60, 111, 112, 116, 105, 111, 110, 62, 48, 60, 47, 111, 112, 116, 105, 111, 110,
	62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 102, 108, 97, 103, 62, 119, 120, 65, 76,
	73, 71, 78, 95, 67, 69, 78, 84, 69, 82, 32, 124, 32, 119, 120, 66, 79, 84, 84, 79, 77, 60, 47,
	102, 108, 97, 103, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 98, 111, 114, 100, 101,
	114, 62, 50, 48, 60, 47, 98, 111, 114, 100, 101, 114, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32,
	32, 32, 60, 111, 98, 106, 101, 99, 116, 32, 99, 108, 97, 115, 115, 61, 34, 119, 120, 83, 116,
	97, 116, 105, 99, 84, 101, 120, 116, 34, 32, 110, 97, 109, 101, 61, 34, 100, 108, 83, 116,
	97, 116, 117, 115, 34, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 102, 111,
	110, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 105, 122,
	101, 62, 49, 54, 60, 47, 115, 105, 122, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
	32, 32, 32, 32, 60, 115, 116, 121, 108, 101, 62, 110, 111, 114, 109, 97, 108, 60, 47, 115,
	116, 121, 108, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 119,
	101, 105, 103, 104, 116, 62, 110, 111, 114, 109, 97, 108, 60, 47, 119, 101, 105, 103, 104,
	116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 117, 110, 100, 101,
	114, 108, 105, 110, 101, 100, 62, 48, 60, 47, 117, 110, 100, 101, 114, 108, 105, 110, 101,
	100, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 102, 111, 110, 116, 62,
	10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 108, 97, 98, 101, 108, 62, 83, 116, 97,
	116, 117, 115, 58, 32, 68, 111, 119, 110, 108, 111, 97, 100, 105, 110, 103, 60, 47, 108, 97,
	98, 101, 108, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 119, 114, 97, 112,
	62, 45, 49, 60, 47, 119, 114, 97, 112, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47,
	111, 98, 106, 101, 99, 116, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 111, 98, 106, 101,
	99, 116, 62, 10, 32, 32, 32, 32, 32, 32, 60, 47, 111, 98, 106, 101, 99, 116, 62, 10, 32, 32,
	32, 32, 60, 47, 111, 98, 106, 101, 99, 116, 62, 10, 32, 32, 60, 47, 111, 98, 106, 101, 99, 116,
	62, 10, 60, 47, 114, 101, 115, 111, 117, 114, 99, 101, 62, 10 };

void InitXmlResource() {

	// Check for memory FS. If not present, load the handler:
	{
		wxMemoryFSHandler::AddFile(wxT("XRC_resource/dummy_file"), wxT("dummy one"));
		wxFileSystem fsys;
		wxFSFile *f = fsys.OpenFile(wxT("memory:XRC_resource/dummy_file"));
		wxMemoryFSHandler::RemoveFile(wxT("XRC_resource/dummy_file"));
		if (f) delete f;
		else wxFileSystem::AddHandler(new wxMemoryFSHandler);
	}

	XRC_ADD_FILE(wxT("XRC_resource/resource.cpp$calladmin_client_updater.ico"), xml_res_file_0, xml_res_size_0, wxT("image/x-icon"));
	XRC_ADD_FILE(wxT("XRC_resource/resource.cpp$._calladmin_client_updater.xrc"), xml_res_file_1, xml_res_size_1, wxT("text/xml"));
	wxXmlResource::Get()->Load(wxT("memory:XRC_resource/resource.cpp$._calladmin_client_updater.xrc"));
}
