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

#include "ditatopicmodel.h"
#include <libxml/xmlmemory.h>
#include <sstream>

DitaTopicModel::DitaTopicModel()
	: doc ( NULL )
	, rootNode ( NULL )
	, elementIdCounter ( 0 )
{
}

DitaTopicModel::~DitaTopicModel()
{
	if ( doc )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
	}
}

bool DitaTopicModel::loadFromXml ( const std::string &xml )
{
	// Clean up any existing document
	if ( doc )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		rootNode = NULL;
		elementIdMap.clear();
		elementIdCounter = 0;
	}

	// Parse XML string
	doc = xmlParseMemory ( xml.c_str(), xml.length() );
	if ( !doc )
	{
		return false; // Failed to parse XML
	}

	// Get root element
	rootNode = xmlDocGetRootElement ( doc );
	if ( !rootNode )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		return false; // No root element
	}

	// Validate that this is a DITA topic
	if ( !isValidTopic() )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		rootNode = NULL;
		return false; // Not a valid DITA topic
	}

	// Build element ID mapping
	buildElementIdMap();

	return true;
}

std::string DitaTopicModel::serializeToXml()
{
	if ( !doc )
		return "";

	// Serialize document to buffer
	xmlChar *xmlBuff;
	int bufferSize;

	xmlDocDumpFormatMemory ( doc, &xmlBuff, &bufferSize, 1 );

	if ( !xmlBuff )
		return "";

	// Convert to std::string
	std::string result ( reinterpret_cast<char*> ( xmlBuff ), bufferSize );

	// Free libxml2 buffer
	xmlFree ( xmlBuff );

	return result;
}

bool DitaTopicModel::isValidTopic()
{
	if ( !rootNode )
		return false;

	// Check if root element is "topic"
	if ( xmlStrcmp ( rootNode->name, BAD_CAST "topic" ) != 0 )
		return false;

	// A valid DITA topic must have an id attribute
	xmlChar *id = xmlGetProp ( rootNode, BAD_CAST "id" );
	if ( !id )
	{
		return false; // Topic must have an id
	}
	xmlFree ( id );

	// Check for required title element
	// Note: title is required but we'll be lenient for now
	// as we may be creating new topics

	return true;
}

std::string DitaTopicModel::getTitle()
{
	// TODO: TASK-004 - Implement title extraction
	return "";
}

void DitaTopicModel::setTitle ( const std::string &title )
{
	// TODO: TASK-004 - Implement title setting
}

bool DitaTopicModel::insertParagraph ( int position, const std::string &text )
{
	// TODO: Future implementation
	return false;
}

bool DitaTopicModel::insertList ( int position, bool ordered )
{
	// TODO: Future implementation
	return false;
}

bool DitaTopicModel::insertListItem ( const std::string &listId, const std::string &text )
{
	// TODO: Future implementation
	return false;
}

bool DitaTopicModel::transformElement ( const std::string &elementId, const std::string &newType )
{
	// TODO: Phase 3 - TASK-017
	return false;
}

bool DitaTopicModel::deleteElement ( const std::string &elementId )
{
	// TODO: Future implementation
	return false;
}

bool DitaTopicModel::canInsertElement ( const std::string &parentId, const std::string &elementType )
{
	// TODO: Phase 3 - TASK-019
	return false;
}

std::vector<std::string> DitaTopicModel::getValidChildElements ( const std::string &parentId )
{
	// TODO: Phase 3 - TASK-019
	std::vector<std::string> validElements;
	return validElements;
}

// Private helper methods

void DitaTopicModel::buildElementIdMap()
{
	elementIdMap.clear();
	elementIdCounter = 0;

	if ( !rootNode )
		return;

	// Recursively traverse the DOM and assign IDs
	buildElementIdMapRecursive ( rootNode );
}

void DitaTopicModel::buildElementIdMapRecursive ( xmlNodePtr node )
{
	if ( !node )
		return;

	// Only process element nodes
	if ( node->type == XML_ELEMENT_NODE )
	{
		std::string id = generateElementId();
		elementIdMap[id] = node;
	}

	// Recurse through children
	for ( xmlNodePtr child = node->children; child; child = child->next )
	{
		buildElementIdMapRecursive ( child );
	}
}

std::string DitaTopicModel::generateElementId()
{
	std::ostringstream oss;
	oss << "dita-elem-" << elementIdCounter++;
	return oss.str();
}

xmlNodePtr DitaTopicModel::getTitleNode()
{
	// TODO: TASK-004 - Find or create title node
	return NULL;
}

bool DitaTopicModel::validateInsertion ( xmlNodePtr parent, const std::string &elementType )
{
	// TODO: Phase 3 - Validation logic
	return false;
}
