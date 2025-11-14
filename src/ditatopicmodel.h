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

#ifndef DITA_TOPIC_MODEL_H
#define DITA_TOPIC_MODEL_H

#include <string>
#include <vector>
#include <map>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <boost/scoped_ptr.hpp>

/**
 * DitaTopicModel encapsulates all DITA Topic document manipulation logic.
 *
 * This class maintains the document object model (DOM) for a DITA Topic
 * and provides methods for loading, querying, modifying, and serializing
 * DITA Topic documents.
 *
 * Follows CQ4 principle: Separate Map and Topic Models
 */
class DitaTopicModel
{
public:
	/**
	 * Constructor
	 */
	DitaTopicModel();

	/**
	 * Destructor - cleans up libxml2 resources
	 */
	~DitaTopicModel();

	// Document lifecycle

	/**
	 * Load and parse DITA Topic XML from string
	 * @param xml XML content as string
	 * @return true if loaded and validated successfully, false otherwise
	 */
	bool loadFromXml ( const std::string &xml );

	/**
	 * Serialize the current model to XML string
	 * @return XML string representation of the topic
	 */
	std::string serializeToXml();

	// Structure queries

	/**
	 * Check if the loaded document is a valid DITA Topic
	 * @return true if valid topic structure, false otherwise
	 */
	bool isValidTopic();

	/**
	 * Get the topic title
	 * @return title text, or empty string if no title
	 */
	std::string getTitle();

	/**
	 * Set or update the topic title
	 * @param title new title text
	 */
	void setTitle ( const std::string &title );

	// Content manipulation

	/**
	 * Insert a paragraph at the specified position
	 * @param position index position (0-based)
	 * @param text paragraph content
	 * @return true if inserted successfully, false otherwise
	 */
	bool insertParagraph ( int position, const std::string &text );

	/**
	 * Insert a list (ordered or unordered) at the specified position
	 * @param position index position (0-based)
	 * @param ordered true for <ol>, false for <ul>
	 * @return true if inserted successfully, false otherwise
	 */
	bool insertList ( int position, bool ordered );

	/**
	 * Insert a list item into an existing list
	 * @param listId internal ID of the list element
	 * @param text list item content
	 * @return true if inserted successfully, false otherwise
	 */
	bool insertListItem ( const std::string &listId, const std::string &text );

	/**
	 * Transform an element to a different type
	 * @param elementId internal ID of the element
	 * @param newType new element type (e.g., "p", "li")
	 * @return true if transformed successfully, false otherwise
	 */
	bool transformElement ( const std::string &elementId, const std::string &newType );

	/**
	 * Delete an element from the document
	 * @param elementId internal ID of the element
	 * @return true if deleted successfully, false otherwise
	 */
	bool deleteElement ( const std::string &elementId );

	// Validation

	/**
	 * Check if an element type can be inserted as child of parent
	 * @param parentId internal ID of parent element
	 * @param elementType element type to insert (e.g., "p", "li")
	 * @return true if insertion is valid, false otherwise
	 */
	bool canInsertElement ( const std::string &parentId, const std::string &elementType );

	/**
	 * Get list of valid child element types for a given parent
	 * @param parentId internal ID of parent element
	 * @return vector of valid element type names
	 */
	std::vector<std::string> getValidChildElements ( const std::string &parentId );

private:
	// libxml2 document structure
	xmlDocPtr doc;
	xmlNodePtr rootNode;

	// Element ID mapping for fast lookups
	std::map<std::string, xmlNodePtr> elementIdMap;

	// Internal helper methods

	/**
	 * Initialize element ID mapping
	 */
	void buildElementIdMap();

	/**
	 * Generate unique ID for an element
	 * @return unique ID string
	 */
	std::string generateElementId();

	/**
	 * Find or create title element
	 * @return pointer to title element node
	 */
	xmlNodePtr getTitleNode();

	/**
	 * Validate element can be inserted at location
	 * @param parent parent node
	 * @param elementType type to insert
	 * @return true if valid
	 */
	bool validateInsertion ( xmlNodePtr parent, const std::string &elementType );

	// Counter for generating unique IDs
	int elementIdCounter;
};

#endif // DITA_TOPIC_MODEL_H
