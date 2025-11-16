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

#include "ditamaptreectrl.h"
#include <wx/msgdlg.h>
#include <wx/filename.h>

BEGIN_EVENT_TABLE(DitaMapTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(wxID_ANY, DitaMapTreeCtrl::OnBeginDrag)
	EVT_TREE_END_DRAG(wxID_ANY, DitaMapTreeCtrl::OnEndDrag)
	EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, DitaMapTreeCtrl::OnRightClick)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, DitaMapTreeCtrl::OnItemActivated)
END_EVENT_TABLE()

DitaMapTreeCtrl::DitaMapTreeCtrl(
	wxWindow* parent,
	wxWindowID id,
	DitaMapModel* model,
	const wxString& mapFilePath
)
	: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize,
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_EDIT_LABELS)
	, m_model(model)
	, m_mapFilePath(mapFilePath)
	, m_isDragging(false)
{
	// Initialize tree with root item if model is provided
	if (m_model)
	{
		buildTreeFromModel();
	}
}

DitaMapTreeCtrl::~DitaMapTreeCtrl()
{
	clearMappings();
}

void DitaMapTreeCtrl::setModel(DitaMapModel* model)
{
	m_model = model;
	if (m_model)
	{
		buildTreeFromModel();
	}
	else
	{
		DeleteAllItems();
		clearMappings();
	}
}

void DitaMapTreeCtrl::buildTreeFromModel()
{
	if (!m_model)
		return;

	// Clear existing tree
	DeleteAllItems();
	clearMappings();

	// Create root item
	m_rootItem = AddRoot(wxT("DITA Map"));

	// Build tree recursively starting from root level topicrefs
	buildTreeRecursive(m_rootItem, "");

	// Expand root by default
	if (m_rootItem.IsOk())
	{
		Expand(m_rootItem);
	}
}

void DitaMapTreeCtrl::refreshTree()
{
	buildTreeFromModel();
}

std::string DitaMapTreeCtrl::getTopicRefIdForItem(const wxTreeItemId& item)
{
	if (!item.IsOk())
		return "";

	DitaMapTreeItemData* data = dynamic_cast<DitaMapTreeItemData*>(GetItemData(item));
	if (data)
	{
		return data->getRefId();
	}

	return "";
}

wxTreeItemId DitaMapTreeCtrl::getItemForTopicRefId(const std::string& refId)
{
	std::map<std::string, wxTreeItemId>::iterator it = m_refIdToItem.find(refId);
	if (it != m_refIdToItem.end())
	{
		return it->second;
	}

	return wxTreeItemId();
}

void DitaMapTreeCtrl::buildTreeRecursive(const wxTreeItemId& parentItem, const std::string& parentRefId)
{
	if (!m_model || !parentItem.IsOk())
		return;

	// Get children of this parent
	std::vector<TopicRef> children;

	if (parentRefId.empty())
	{
		// Root level - get all top-level topicrefs
		std::vector<TopicRef> allRefs = m_model->getTopicRefs();

		// Filter to only root-level topicrefs (those without parents in the tree)
		// For now, we'll add all and let the recursive structure handle it
		// This may need refinement based on how the model tracks parent relationships
		children = allRefs;
	}
	else
	{
		// Get children of specific parent
		children = m_model->getChildren(parentRefId);
	}

	// Add each child to the tree
	for (size_t i = 0; i < children.size(); ++i)
	{
		const TopicRef& ref = children[i];

		// Add this topicref to the tree
		wxTreeItemId childItem = addTopicRefToTree(parentItem, ref);

		// Recursively add children
		if (childItem.IsOk() && !ref.childIds.empty())
		{
			buildTreeRecursive(childItem, ref.id);
		}
	}
}

wxTreeItemId DitaMapTreeCtrl::addTopicRefToTree(const wxTreeItemId& parentItem, const TopicRef& ref)
{
	if (!parentItem.IsOk())
		return wxTreeItemId();

	// Create display text
	wxString displayText;
	if (!ref.navtitle.empty())
	{
		displayText = wxString::FromUTF8(ref.navtitle.c_str());
	}
	else if (!ref.href.empty())
	{
		displayText = wxString::FromUTF8(ref.href.c_str());
	}
	else
	{
		displayText = wxT("[Untitled]");
	}

	// Add href as additional info if we have navtitle
	if (!ref.navtitle.empty() && !ref.href.empty())
	{
		displayText += wxT(" (") + wxString::FromUTF8(ref.href.c_str()) + wxT(")");
	}

	// Create tree item with custom data
	DitaMapTreeItemData* data = new DitaMapTreeItemData(ref.id);
	wxTreeItemId item = AppendItem(parentItem, displayText, -1, -1, data);

	// Store mapping
	m_refIdToItem[ref.id] = item;

	return item;
}

void DitaMapTreeCtrl::clearMappings()
{
	m_refIdToItem.clear();
}

bool DitaMapTreeCtrl::validateDrop(const wxTreeItemId& draggedItem, const wxTreeItemId& targetItem)
{
	if (!draggedItem.IsOk() || !targetItem.IsOk() || !m_model)
		return false;

	// Can't drop on self
	if (draggedItem == targetItem)
		return false;

	// Can't drop on root
	if (targetItem == m_rootItem)
		return false;

	// Can't drop on own descendant (would create cycle)
	wxTreeItemId checkItem = targetItem;
	while (checkItem.IsOk())
	{
		if (checkItem == draggedItem)
			return false;
		checkItem = GetItemParent(checkItem);
	}

	// Get topicref IDs
	std::string draggedId = getTopicRefIdForItem(draggedItem);
	std::string targetId = getTopicRefIdForItem(targetItem);

	if (draggedId.empty() || targetId.empty())
		return false;

	// Ask model to validate
	return m_model->canMoveTopicRef(draggedId, targetId);
}

bool DitaMapTreeCtrl::performDrop(const wxTreeItemId& draggedItem, const wxTreeItemId& targetItem)
{
	if (!validateDrop(draggedItem, targetItem))
		return false;

	// Get topicref IDs
	std::string draggedId = getTopicRefIdForItem(draggedItem);
	std::string targetId = getTopicRefIdForItem(targetItem);

	// Get target parent
	wxTreeItemId targetParent = GetItemParent(targetItem);
	std::string targetParentId;

	if (targetParent.IsOk() && targetParent != m_rootItem)
	{
		targetParentId = getTopicRefIdForItem(targetParent);
	}

	// Calculate position: insert after target item
	int position = 0;
	if (targetParent.IsOk())
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId sibling = GetFirstChild(targetParent, cookie);
		while (sibling.IsOk())
		{
			if (sibling == targetItem)
				break;
			position++;
			sibling = GetNextChild(targetParent, cookie);
		}
		position++; // Insert after target
	}

	// Update model
	if (m_model->moveTopicRef(draggedId, targetParentId, position))
	{
		// Refresh tree to reflect changes
		refreshTree();
		return true;
	}

	return false;
}

void DitaMapTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	// Don't allow dragging root
	if (item == m_rootItem)
	{
		event.Veto();
		return;
	}

	// Store dragged item
	m_draggedItem = item;
	m_isDragging = true;

	// Allow drag to continue
	event.Allow();
}

void DitaMapTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
	wxTreeItemId targetItem = event.GetItem();

	if (!m_isDragging || !m_draggedItem.IsOk())
		return;

	m_isDragging = false;

	// Validate and perform drop
	if (targetItem.IsOk() && targetItem != m_rootItem)
	{
		if (!performDrop(m_draggedItem, targetItem))
		{
			wxMessageBox(
				wxT("Cannot move topicref to this location."),
				wxT("Invalid Move"),
				wxOK | wxICON_WARNING
			);
		}
	}

	m_draggedItem = wxTreeItemId();
}

void DitaMapTreeCtrl::OnRightClick(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk() || item == m_rootItem)
		return;

	// Select the item
	SelectItem(item);

	// TODO: Show context menu (TASK-028)
	// This will be implemented in a later task
}

void DitaMapTreeCtrl::OnItemActivated(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk() || item == m_rootItem)
		return;

	// Get topicref
	std::string refId = getTopicRefIdForItem(item);
	if (refId.empty())
		return;

	TopicRef ref = m_model->getTopicRefById(refId);

	// Open referenced topic in editor
	if (!ref.href.empty())
	{
		// Convert href to wxString
		wxString href = wxString::FromUTF8(ref.href.c_str());

		// Resolve relative path against map file location
		wxFileName topicPath(href);
		if (!topicPath.IsAbsolute() && !m_mapFilePath.IsEmpty())
		{
			// Get directory of map file
			wxFileName mapFile(m_mapFilePath);
			wxString mapDir = mapFile.GetPath();

			// Make the href path absolute
			topicPath.MakeAbsolute(mapDir);
		}

		// Get the full path
		wxString fullPath = topicPath.GetFullPath();

		// Check if file exists
		if (!wxFileExists(fullPath))
		{
			wxMessageBox(
				wxT("Topic file not found:\n") + fullPath,
				wxT("File Not Found"),
				wxOK | wxICON_WARNING
			);
			return;
		}

		// Create a command event to open the file
		// We'll send this to the top-level parent (main frame)
		wxWindow* topFrame = wxGetTopLevelParent(this);
		if (topFrame)
		{
			// Create a custom event or use wxID_OPEN
			wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_OPEN);
			openEvent.SetString(fullPath);
			topFrame->GetEventHandler()->AddPendingEvent(openEvent);
		}
	}
}
