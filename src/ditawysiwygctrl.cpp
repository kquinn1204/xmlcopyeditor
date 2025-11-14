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

#include "ditawysiwygctrl.h"
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>

// Event table implementation
BEGIN_EVENT_TABLE(DitaWysiwygCtrl, wxRichTextCtrl)
	// Future: Add event handlers here
END_EVENT_TABLE()

DitaWysiwygCtrl::DitaWysiwygCtrl(
	wxWindow *parent,
	DitaTopicModel *model,
	wxWindowID id,
	const wxString& value,
	const wxPoint& pos,
	const wxSize& size,
	long style
) : wxRichTextCtrl(parent, id, value, pos, size, style),
	mModel(model)
{
	// Initialize default formatting and styles
	initializeStyles();
	setupDefaultFormatting();
}

DitaWysiwygCtrl::~DitaWysiwygCtrl()
{
	// Model is not owned, so don't delete it
}

void DitaWysiwygCtrl::renderFromModel()
{
	// Placeholder for TASK-011
	// This will be implemented in the next task

	if (!mModel)
	{
		return;
	}

	// Clear existing content
	Clear();

	// TODO (TASK-011): Implement rendering logic
	// For now, just display a placeholder message
	wxRichTextAttr attr;
	attr.SetFontSize(12);
	attr.SetTextColour(*wxBLACK);

	BeginSuppressUndo();
	BeginParagraphSpacing(0, 20);

	WriteText(wxT("WYSIWYG View - Rendering from model will be implemented in TASK-011"));

	EndParagraphSpacing();
	EndSuppressUndo();
}

void DitaWysiwygCtrl::updateModelFromContent()
{
	// Placeholder for TASK-012
	// This will be implemented later

	if (!mModel)
	{
		return;
	}

	// TODO (TASK-012): Implement sync logic from view to model
}

DitaTopicModel* DitaWysiwygCtrl::getModel()
{
	return mModel;
}

void DitaWysiwygCtrl::initializeStyles()
{
	// Set up basic styles for DITA elements
	// These will be used for rendering in TASK-011

	// Get the style sheet
	wxRichTextStyleSheet* styleSheet = GetStyleSheet();
	if (!styleSheet)
	{
		styleSheet = new wxRichTextStyleSheet;
		SetStyleSheet(styleSheet);
	}

	// Define paragraph style for title
	wxRichTextParagraphStyleDefinition* titleStyle = new wxRichTextParagraphStyleDefinition(wxT("Title"));
	wxRichTextAttr titleAttr;
	titleAttr.SetFontSize(16);
	titleAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
	titleAttr.SetParagraphSpacingAfter(20);
	titleStyle->SetStyle(titleAttr);
	styleSheet->AddParagraphStyle(titleStyle);

	// Define paragraph style for normal paragraphs
	wxRichTextParagraphStyleDefinition* paraStyle = new wxRichTextParagraphStyleDefinition(wxT("Paragraph"));
	wxRichTextAttr paraAttr;
	paraAttr.SetFontSize(10);
	paraAttr.SetParagraphSpacingAfter(10);
	paraStyle->SetStyle(paraAttr);
	styleSheet->AddParagraphStyle(paraStyle);

	// Define list style for bullet lists
	wxRichTextListStyleDefinition* listStyle = new wxRichTextListStyleDefinition(wxT("BulletList"));
	wxRichTextAttr listAttr;
	listAttr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
	listAttr.SetLeftIndent(100, 60);  // Left indent and sub-indent
	listStyle->SetLevelAttributes(0, listAttr);
	styleSheet->AddListStyle(listStyle);
}

void DitaWysiwygCtrl::setupDefaultFormatting()
{
	// Set up default font and formatting for the control

	wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(defaultFont);

	// Set basic attributes
	wxRichTextAttr attr;
	attr.SetFont(defaultFont);
	attr.SetTextColour(*wxBLACK);
	attr.SetBackgroundColour(*wxWHITE);
	attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

	SetBasicStyle(attr);

	// Enable some useful features
	SetEditable(true);
	ShowPosition(0);
}
