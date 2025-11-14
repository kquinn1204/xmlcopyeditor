/*
 * Copyright 2005-2007 Gerald Schmidt.
 * Copyright 2025 Contributors.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DITA_DETECTOR_H
#define DITA_DETECTOR_H

#include <wx/wx.h>
#include <string>

/**
 * DITA file type enumeration
 */
enum DitaFileType
{
	DITA_TYPE_TOPIC,    // DITA Topic file
	DITA_TYPE_MAP,      // DITA Map file
	DITA_TYPE_NONE      // Not a DITA file
};

/**
 * Detect DITA file type from file path
 *
 * Detection strategy:
 * 1. Check file extension (.dita, .ditamap)
 * 2. Read file content and check DOCTYPE
 * 3. Check root element name
 *
 * @param filePath path to file to analyze
 * @return DitaFileType enumeration value
 */
DitaFileType detectDitaType ( const wxString &filePath );

/**
 * Detect DITA file type from XML content
 *
 * Detection strategy:
 * 1. Parse XML and check root element
 * 2. Check DOCTYPE declaration
 * 3. Check for DITA-specific attributes
 *
 * @param xmlContent XML content as string
 * @return DitaFileType enumeration value
 */
DitaFileType detectDitaTypeFromContent ( const std::string &xmlContent );

#endif // DITA_DETECTOR_H
