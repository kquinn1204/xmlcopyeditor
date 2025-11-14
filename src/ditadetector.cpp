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

#include "ditadetector.h"
#include <wx/filename.h>
#include <wx/textfile.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

DitaFileType detectDitaType ( const wxString &filePath )
{
	// Strategy 1: Check file extension
	wxFileName fileName ( filePath );
	wxString ext = fileName.GetExt().Lower();

	if ( ext == wxT ( "ditamap" ) )
	{
		return DITA_TYPE_MAP;
	}
	else if ( ext == wxT ( "dita" ) )
	{
		return DITA_TYPE_TOPIC;
	}

	// Strategy 2: Read file content and analyze
	wxTextFile file;
	if ( !file.Open ( filePath ) )
	{
		return DITA_TYPE_NONE; // Cannot read file
	}

	// Read file content
	wxString content;
	for ( wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine() )
	{
		content += line + wxT ( "\n" );
	}
	file.Close();

	// Convert to std::string and detect from content
	std::string xmlContent ( content.mb_str ( wxConvUTF8 ) );
	return detectDitaTypeFromContent ( xmlContent );
}

DitaFileType detectDitaTypeFromContent ( const std::string &xmlContent )
{
	if ( xmlContent.empty() )
		return DITA_TYPE_NONE;

	// Strategy 1: Quick check for DOCTYPE containing "topic" or "map"
	if ( xmlContent.find ( "topic.dtd" ) != std::string::npos )
	{
		return DITA_TYPE_TOPIC;
	}
	if ( xmlContent.find ( "map.dtd" ) != std::string::npos )
	{
		return DITA_TYPE_MAP;
	}

	// Strategy 2: Parse XML and check root element
	xmlDocPtr doc = xmlParseMemory ( xmlContent.c_str(), xmlContent.length() );
	if ( !doc )
	{
		return DITA_TYPE_NONE; // Failed to parse XML
	}

	xmlNodePtr root = xmlDocGetRootElement ( doc );
	DitaFileType result = DITA_TYPE_NONE;

	if ( root )
	{
		// Check root element name
		if ( xmlStrcmp ( root->name, BAD_CAST "topic" ) == 0 )
		{
			result = DITA_TYPE_TOPIC;
		}
		else if ( xmlStrcmp ( root->name, BAD_CAST "map" ) == 0 )
		{
			result = DITA_TYPE_MAP;
		}
		// Check for specialized topic types (concept, task, reference)
		else if ( xmlStrcmp ( root->name, BAD_CAST "concept" ) == 0 ||
		          xmlStrcmp ( root->name, BAD_CAST "task" ) == 0 ||
		          xmlStrcmp ( root->name, BAD_CAST "reference" ) == 0 )
		{
			result = DITA_TYPE_TOPIC; // Specialized topics are still topics
		}
	}

	xmlFreeDoc ( doc );
	return result;
}
