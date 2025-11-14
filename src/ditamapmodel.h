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

#ifndef DITA_MAP_MODEL_H
#define DITA_MAP_MODEL_H

#include <string>
#include <vector>
#include <map>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <boost/scoped_ptr.hpp>

/**
 * TopicRef structure represents a DITA Map topicref element
 */
struct TopicRef
{
	std::string id;           // Internal ID for mapping
	std::string href;         // File reference
	std::string navtitle;     // Navigation title
	std::string format;       // Format (dita, html, etc.)
	std::vector<std::string> childIds;  // IDs of child topicrefs

	TopicRef()
		: id ( "" )
		, href ( "" )
		, navtitle ( "" )
		, format ( "dita" )
	{
	}
};

/**
 * DitaMapModel encapsulates all DITA Map document manipulation logic.
 *
 * This class maintains the document object model (DOM) for a DITA Map
 * and provides methods for loading, querying, modifying, and serializing
 * DITA Map documents.
 *
 * Follows CQ4 principle: Separate Map and Topic Models
 */
class DitaMapModel
{
public:
	/**
	 * Constructor
	 */
	DitaMapModel();

	/**
	 * Destructor - cleans up libxml2 resources
	 */
	~DitaMapModel();

	// Document lifecycle

	/**
	 * Load and parse DITA Map XML from string
	 * @param xml XML content as string
	 * @return true if loaded and validated successfully, false otherwise
	 */
	bool loadFromXml ( const std::string &xml );

	/**
	 * Serialize the current model to XML string
	 * @return XML string representation of the map
	 */
	std::string serializeToXml();

	// Structure queries

	/**
	 * Check if the loaded document is a valid DITA Map
	 * @return true if valid map structure, false otherwise
	 */
	bool isValidMap();

	/**
	 * Get all topicref elements from the map
	 * @return vector of TopicRef structures
	 */
	std::vector<TopicRef> getTopicRefs();

	/**
	 * Get a specific topicref by internal ID
	 * @param id internal topicref ID
	 * @return TopicRef structure, or empty TopicRef if not found
	 */
	TopicRef getTopicRefById ( const std::string &id );

	/**
	 * Get child topicrefs of a parent
	 * @param parentId internal ID of parent topicref
	 * @return vector of child TopicRef structures
	 */
	std::vector<TopicRef> getChildren ( const std::string &parentId );

	// Manipulation

	/**
	 * Add a new topicref element
	 * @param parentId internal ID of parent (empty for root level)
	 * @param ref TopicRef structure with href and navtitle
	 * @return true if added successfully, false otherwise
	 */
	bool addTopicRef ( const std::string &parentId, const TopicRef &ref );

	/**
	 * Remove a topicref element
	 * @param id internal ID of topicref to remove
	 * @return true if removed successfully, false otherwise
	 */
	bool removeTopicRef ( const std::string &id );

	/**
	 * Move a topicref to a new parent and position
	 * @param id internal ID of topicref to move
	 * @param newParentId internal ID of new parent (empty for root)
	 * @param position position index (0-based)
	 * @return true if moved successfully, false otherwise
	 */
	bool moveTopicRef ( const std::string &id, const std::string &newParentId, int position );

	/**
	 * Update a topicref's attributes
	 * @param id internal ID of topicref
	 * @param updatedRef TopicRef with new values
	 * @return true if updated successfully, false otherwise
	 */
	bool updateTopicRef ( const std::string &id, const TopicRef &updatedRef );

	// Validation

	/**
	 * Check if a topicref can be moved to a new location
	 * @param id internal ID of topicref to move
	 * @param targetParentId internal ID of target parent
	 * @return true if move is valid, false otherwise
	 */
	bool canMoveTopicRef ( const std::string &id, const std::string &targetParentId );

private:
	// libxml2 document structure
	xmlDocPtr doc;
	xmlNodePtr mapNode;

	// Topicref ID mapping for fast lookups
	std::map<std::string, xmlNodePtr> topicRefIdMap;

	// Internal helper methods

	/**
	 * Initialize topicref ID mapping
	 */
	void buildTopicRefIdMap();

	/**
	 * Recursively build topicref ID mapping
	 * @param node current node to process
	 */
	void buildTopicRefIdMapRecursive ( xmlNodePtr node );

	/**
	 * Generate unique ID for a topicref
	 * @return unique ID string
	 */
	std::string generateTopicRefId();

	/**
	 * Parse a topicref node into TopicRef structure
	 * @param node topicref XML node
	 * @return TopicRef structure
	 */
	TopicRef parseTopicRef ( xmlNodePtr node );

	/**
	 * Find topicref node by internal ID
	 * @param id internal ID
	 * @return XML node pointer or NULL if not found
	 */
	xmlNodePtr findTopicRefNode ( const std::string &id );

	/**
	 * Check if moving would create circular reference
	 * @param nodeToMove node being moved
	 * @param targetParent target parent node
	 * @return true if would create cycle
	 */
	bool wouldCreateCycle ( xmlNodePtr nodeToMove, xmlNodePtr targetParent );

	// Counter for generating unique IDs
	int topicRefIdCounter;
};

#endif // DITA_MAP_MODEL_H
