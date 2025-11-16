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

#ifndef DITA_MAP_TREE_CTRL_H
#define DITA_MAP_TREE_CTRL_H

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <map>
#include <string>
#include "ditamapmodel.h"

/**
 * Tree item data to associate topicref IDs with tree items
 */
class DitaMapTreeItemData : public wxTreeItemData
{
public:
	DitaMapTreeItemData(const std::string& topicRefId)
		: refId(topicRefId)
	{
	}

	std::string getRefId() const { return refId; }

private:
	std::string refId;
};

/**
 * DitaMapTreeCtrl displays DITA Map structure in a tree control
 * with drag-and-drop support for reorganizing topicref elements.
 */
class DitaMapTreeCtrl : public wxTreeCtrl
{
public:
	/**
	 * Constructor
	 * @param parent parent window
	 * @param id window ID
	 * @param model pointer to DitaMapModel
	 * @param mapFilePath full path to the map file (for resolving relative hrefs)
	 */
	DitaMapTreeCtrl(
		wxWindow* parent,
		wxWindowID id,
		DitaMapModel* model,
		const wxString& mapFilePath = wxEmptyString
	);

	/**
	 * Destructor
	 */
	virtual ~DitaMapTreeCtrl();

	/**
	 * Set the model for this tree control
	 * @param model pointer to DitaMapModel
	 */
	void setModel(DitaMapModel* model);

	/**
	 * Build tree from the current model
	 * Populates the tree with topicref hierarchy
	 */
	void buildTreeFromModel();

	/**
	 * Refresh tree to reflect model changes
	 */
	void refreshTree();

	/**
	 * Get topicref ID for a tree item
	 * @param item tree item
	 * @return topicref ID, or empty string if not found
	 */
	std::string getTopicRefIdForItem(const wxTreeItemId& item);

	/**
	 * Get tree item for a topicref ID
	 * @param refId topicref ID
	 * @return tree item, or invalid item if not found
	 */
	wxTreeItemId getItemForTopicRefId(const std::string& refId);

private:
	// Model reference
	DitaMapModel* m_model;

	// Map file path (for resolving relative hrefs)
	wxString m_mapFilePath;

	// Root tree item
	wxTreeItemId m_rootItem;

	// Mapping: topicref ID -> tree item ID
	std::map<std::string, wxTreeItemId> m_refIdToItem;

	// Drag-and-drop state
	wxTreeItemId m_draggedItem;
	bool m_isDragging;

	// Event handlers
	void OnBeginDrag(wxTreeEvent& event);
	void OnEndDrag(wxTreeEvent& event);
	void OnRightClick(wxTreeEvent& event);
	void OnItemActivated(wxTreeEvent& event);

	// Helper methods

	/**
	 * Recursively build tree from topicref
	 * @param parentItem parent tree item
	 * @param parentRefId parent topicref ID (empty for root)
	 */
	void buildTreeRecursive(const wxTreeItemId& parentItem, const std::string& parentRefId);

	/**
	 * Add a single topicref to the tree
	 * @param parentItem parent tree item
	 * @param ref TopicRef structure
	 * @return tree item ID for the added item
	 */
	wxTreeItemId addTopicRefToTree(const wxTreeItemId& parentItem, const TopicRef& ref);

	/**
	 * Clear all tree mappings
	 */
	void clearMappings();

	/**
	 * Validate drop operation
	 * @param draggedItem item being dragged
	 * @param targetItem proposed drop target
	 * @return true if drop is valid
	 */
	bool validateDrop(const wxTreeItemId& draggedItem, const wxTreeItemId& targetItem);

	/**
	 * Perform drop operation
	 * @param draggedItem item being dragged
	 * @param targetItem drop target
	 * @return true if operation succeeded
	 */
	bool performDrop(const wxTreeItemId& draggedItem, const wxTreeItemId& targetItem);

	DECLARE_EVENT_TABLE()
};

#endif // DITA_MAP_TREE_CTRL_H
