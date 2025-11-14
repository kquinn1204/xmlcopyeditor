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

#ifndef DITA_WYSIWYG_CTRL_H
#define DITA_WYSIWYG_CTRL_H

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include "ditatopicmodel.h"

// Forward declaration for libxml2
struct _xmlNode;
typedef struct _xmlNode *xmlNodePtr;

/**
 * DitaWysiwygCtrl - WYSIWYG editing control for DITA Topics
 *
 * This class extends wxRichTextCtrl to provide WYSIWYG editing
 * capabilities for DITA Topic documents.
 *
 * Key responsibilities:
 * - Render DITA Topic model as formatted rich text
 * - Capture user edits and sync back to model
 * - Maintain bi-directional synchronization
 * - Provide formatting toolbar integration
 *
 * Design pattern:
 * - View layer in Model-View architecture
 * - DitaTopicModel is the single source of truth
 * - Syncs on demand (not real-time)
 */
class DitaWysiwygCtrl : public wxRichTextCtrl
{
public:
	/**
	 * Constructor
	 *
	 * @param parent Parent window
	 * @param model Pointer to DitaTopicModel (must remain valid)
	 * @param id Window ID
	 * @param value Initial text value
	 * @param pos Window position
	 * @param size Window size
	 * @param style Window style flags
	 */
	DitaWysiwygCtrl(
		wxWindow *parent,
		DitaTopicModel *model,
		wxWindowID id = wxID_ANY,
		const wxString& value = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxRE_MULTILINE | wxWANTS_CHARS
	);

	/**
	 * Destructor
	 */
	virtual ~DitaWysiwygCtrl();

	/**
	 * Render DITA Topic model content into WYSIWYG view
	 *
	 * Reads from the model and populates the rich text control
	 * with formatted content.
	 *
	 * Called when:
	 * - Switching from Code view to WYSIWYG view
	 * - Model content changes externally
	 */
	void renderFromModel();

	/**
	 * Update DITA Topic model from WYSIWYG content
	 *
	 * Reads the rich text content and updates the model.
	 *
	 * Called when:
	 * - Switching from WYSIWYG view to Code view
	 * - Saving document
	 */
	void updateModelFromContent();

	/**
	 * Get the associated DITA Topic model
	 *
	 * @return Pointer to model
	 */
	DitaTopicModel* getModel();

protected:
	/**
	 * Initialize default styles and formatting
	 */
	void initializeStyles();

	/**
	 * Set up basic fonts and paragraph attributes
	 */
	void setupDefaultFormatting();

	/**
	 * Render a single XML node and its children
	 *
	 * @param node libxml2 node pointer
	 */
	void renderNode(xmlNodePtr node);

	/**
	 * Render text content from a node
	 *
	 * @param node libxml2 node pointer
	 */
	void renderTextContent(xmlNodePtr node);

	/**
	 * Build DITA XML from current rich text content
	 *
	 * @return DITA Topic XML string
	 */
	std::string buildXmlFromContent();

	/**
	 * Build list XML from rich text buffer
	 *
	 * @param buffer wxRichTextBuffer reference
	 * @return List XML fragment
	 */
	std::string buildListsXml(wxRichTextBuffer& buffer);

	/**
	 * Escape special XML characters
	 *
	 * @param text Text to escape
	 * @return Escaped XML text
	 */
	std::string escapeXml(const std::string& text);

private:
	DitaTopicModel *mModel;  // Pointer to the DITA Topic model (not owned)

	// Declare event table
	DECLARE_EVENT_TABLE()
};

#endif // DITA_WYSIWYG_CTRL_H
