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

#include "ditamapmodel.h"
#include <libxml/xmlmemory.h>
#include <sstream>
#include <algorithm>

DitaMapModel::DitaMapModel()
	: doc ( NULL )
	, mapNode ( NULL )
	, topicRefIdCounter ( 0 )
{
}

DitaMapModel::~DitaMapModel()
{
	if ( doc )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
	}
}

bool DitaMapModel::loadFromXml ( const std::string &xml )
{
	// Clean up any existing document
	if ( doc )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		mapNode = NULL;
		topicRefIdMap.clear();
		topicRefIdCounter = 0;
	}

	// Parse XML string
	doc = xmlParseMemory ( xml.c_str(), xml.length() );
	if ( !doc )
	{
		return false; // Failed to parse XML
	}

	// Get root element
	mapNode = xmlDocGetRootElement ( doc );
	if ( !mapNode )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		return false; // No root element
	}

	// Validate that this is a DITA map
	if ( !isValidMap() )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		mapNode = NULL;
		return false; // Not a valid DITA map
	}

	// Build topicref ID mapping
	buildTopicRefIdMap();

	return true;
}

std::string DitaMapModel::serializeToXml()
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

bool DitaMapModel::isValidMap()
{
	if ( !mapNode )
		return false;

	// Check if root element is "map"
	if ( xmlStrcmp ( mapNode->name, BAD_CAST "map" ) != 0 )
		return false;

	// Maps may have optional title, but we'll be lenient
	return true;
}

std::vector<TopicRef> DitaMapModel::getTopicRefs()
{
	std::vector<TopicRef> refs;

	if ( !mapNode )
		return refs;

	// Traverse all topicref elements
	for ( xmlNodePtr child = mapNode->children; child; child = child->next )
	{
		if ( child->type == XML_ELEMENT_NODE &&
		     xmlStrcmp ( child->name, BAD_CAST "topicref" ) == 0 )
		{
			refs.push_back ( parseTopicRef ( child ) );
		}
	}

	return refs;
}

TopicRef DitaMapModel::getTopicRefById ( const std::string &id )
{
	xmlNodePtr node = findTopicRefNode ( id );
	if ( node )
	{
		return parseTopicRef ( node );
	}

	return TopicRef(); // Return empty TopicRef
}

std::vector<TopicRef> DitaMapModel::getChildren ( const std::string &parentId )
{
	std::vector<TopicRef> children;

	xmlNodePtr parentNode = findTopicRefNode ( parentId );
	if ( !parentNode )
		return children;

	// Find child topicrefs
	for ( xmlNodePtr child = parentNode->children; child; child = child->next )
	{
		if ( child->type == XML_ELEMENT_NODE &&
		     xmlStrcmp ( child->name, BAD_CAST "topicref" ) == 0 )
		{
			children.push_back ( parseTopicRef ( child ) );
		}
	}

	return children;
}

bool DitaMapModel::addTopicRef ( const std::string &parentId, const TopicRef &ref )
{
	// TODO: Future implementation
	return false;
}

bool DitaMapModel::removeTopicRef ( const std::string &id )
{
	// TODO: Future implementation
	return false;
}

bool DitaMapModel::moveTopicRef ( const std::string &id, const std::string &newParentId, int position )
{
	// TODO: Phase 4 - TASK-025
	return false;
}

bool DitaMapModel::updateTopicRef ( const std::string &id, const TopicRef &updatedRef )
{
	// TODO: Future implementation
	return false;
}

bool DitaMapModel::canMoveTopicRef ( const std::string &id, const std::string &targetParentId )
{
	// TODO: Phase 4 - Validation
	return false;
}

// Private helper methods

void DitaMapModel::buildTopicRefIdMap()
{
	topicRefIdMap.clear();
	topicRefIdCounter = 0;

	if ( !mapNode )
		return;

	// Recursively traverse the DOM and assign IDs to topicrefs
	buildTopicRefIdMapRecursive ( mapNode );
}

void DitaMapModel::buildTopicRefIdMapRecursive ( xmlNodePtr node )
{
	if ( !node )
		return;

	// Only process topicref elements
	if ( node->type == XML_ELEMENT_NODE &&
	     xmlStrcmp ( node->name, BAD_CAST "topicref" ) == 0 )
	{
		std::string id = generateTopicRefId();
		topicRefIdMap[id] = node;
	}

	// Recurse through children
	for ( xmlNodePtr child = node->children; child; child = child->next )
	{
		buildTopicRefIdMapRecursive ( child );
	}
}

std::string DitaMapModel::generateTopicRefId()
{
	std::ostringstream oss;
	oss << "dita-topicref-" << topicRefIdCounter++;
	return oss.str();
}

TopicRef DitaMapModel::parseTopicRef ( xmlNodePtr node )
{
	TopicRef ref;

	if ( !node )
		return ref;

	// Find the ID for this node in our map
	for ( std::map<std::string, xmlNodePtr>::const_iterator it = topicRefIdMap.begin();
	      it != topicRefIdMap.end(); ++it )
	{
		if ( it->second == node )
		{
			ref.id = it->first;
			break;
		}
	}

	// Extract href attribute
	xmlChar *href = xmlGetProp ( node, BAD_CAST "href" );
	if ( href )
	{
		ref.href = reinterpret_cast<char*> ( href );
		xmlFree ( href );
	}

	// Extract navtitle attribute
	xmlChar *navtitle = xmlGetProp ( node, BAD_CAST "navtitle" );
	if ( navtitle )
	{
		ref.navtitle = reinterpret_cast<char*> ( navtitle );
		xmlFree ( navtitle );
	}

	// Extract format attribute (defaults to "dita" if not present)
	xmlChar *format = xmlGetProp ( node, BAD_CAST "format" );
	if ( format )
	{
		ref.format = reinterpret_cast<char*> ( format );
		xmlFree ( format );
	}

	// Find child topicref IDs
	for ( xmlNodePtr child = node->children; child; child = child->next )
	{
		if ( child->type == XML_ELEMENT_NODE &&
		     xmlStrcmp ( child->name, BAD_CAST "topicref" ) == 0 )
		{
			// Find ID for this child
			for ( std::map<std::string, xmlNodePtr>::const_iterator it = topicRefIdMap.begin();
			      it != topicRefIdMap.end(); ++it )
			{
				if ( it->second == child )
				{
					ref.childIds.push_back ( it->first );
					break;
				}
			}
		}
	}

	return ref;
}

xmlNodePtr DitaMapModel::findTopicRefNode ( const std::string &id )
{
	std::map<std::string, xmlNodePtr>::iterator it = topicRefIdMap.find ( id );
	if ( it != topicRefIdMap.end() )
	{
		return it->second;
	}
	return NULL;
}

bool DitaMapModel::wouldCreateCycle ( xmlNodePtr nodeToMove, xmlNodePtr targetParent )
{
	// TODO: Phase 4 - Cycle detection
	return false;
}
