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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sstream>

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
	if (!mModel)
	{
		return;
	}

	// Clear existing content
	Clear();

	// Get XML from model
	std::string xmlContent = mModel->serializeToXml();
	if (xmlContent.empty())
	{
		return;
	}

	// Parse XML with libxml2
	xmlDocPtr doc = xmlParseMemory(xmlContent.c_str(), xmlContent.length());
	if (!doc)
	{
		return;
	}

	// Get root element
	xmlNodePtr root = xmlDocGetRootElement(doc);
	if (!root)
	{
		xmlFreeDoc(doc);
		return;
	}

	// Start rendering
	BeginSuppressUndo();

	// Render the topic content
	renderNode(root);

	EndSuppressUndo();

	// Clean up
	xmlFreeDoc(doc);

	// Move cursor to start
	SetInsertionPoint(0);
}

void DitaWysiwygCtrl::renderNode(xmlNodePtr node)
{
	if (!node)
	{
		return;
	}

	// Get node name
	const char* nodeName = (const char*)node->name;

	// Handle different DITA elements
	if (strcmp(nodeName, "title") == 0)
	{
		// Render title - large, bold
		wxRichTextAttr titleAttr;
		titleAttr.SetFontSize(16);
		titleAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
		titleAttr.SetParagraphSpacingAfter(20);

		BeginStyle(titleAttr);
		renderTextContent(node);
		EndStyle();
		Newline();
	}
	else if (strcmp(nodeName, "p") == 0)
	{
		// Render paragraph - normal text
		wxRichTextAttr paraAttr;
		paraAttr.SetFontSize(10);
		paraAttr.SetParagraphSpacingAfter(10);

		BeginStyle(paraAttr);
		renderTextContent(node);
		EndStyle();
		Newline();
	}
	else if (strcmp(nodeName, "ul") == 0)
	{
		// Render unordered list
		BeginListStyle(wxT("BulletList"));

		// Render each list item
		for (xmlNodePtr child = node->children; child; child = child->next)
		{
			if (child->type == XML_ELEMENT_NODE && strcmp((const char*)child->name, "li") == 0)
			{
				renderNode(child);
			}
		}

		EndListStyle();
		Newline();
	}
	else if (strcmp(nodeName, "ol") == 0)
	{
		// Render ordered list
		BeginNumberedBullet(1, 100, 60);

		// Render each list item
		int itemNumber = 1;
		for (xmlNodePtr child = node->children; child; child = child->next)
		{
			if (child->type == XML_ELEMENT_NODE && strcmp((const char*)child->name, "li") == 0)
			{
				BeginNumberedBullet(itemNumber++, 100, 60);
				renderTextContent(child);
				EndNumberedBullet();
				Newline();
			}
		}

		EndNumberedBullet();
	}
	else if (strcmp(nodeName, "li") == 0)
	{
		// Render list item (for unordered lists)
		BeginStandardBullet(wxT("standard/circle"), 100, 60);
		renderTextContent(node);
		EndStandardBullet();
		Newline();
	}
	else if (strcmp(nodeName, "b") == 0 || strcmp(nodeName, "bold") == 0)
	{
		// Render bold inline
		BeginBold();
		renderTextContent(node);
		EndBold();
	}
	else if (strcmp(nodeName, "i") == 0 || strcmp(nodeName, "italic") == 0)
	{
		// Render italic inline
		BeginItalic();
		renderTextContent(node);
		EndItalic();
	}
	else if (strcmp(nodeName, "body") == 0 || strcmp(nodeName, "topic") == 0 ||
	         strcmp(nodeName, "task") == 0 || strcmp(nodeName, "concept") == 0 ||
	         strcmp(nodeName, "reference") == 0 || strcmp(nodeName, "taskbody") == 0 ||
	         strcmp(nodeName, "context") == 0)
	{
		// Container elements - render children
		for (xmlNodePtr child = node->children; child; child = child->next)
		{
			if (child->type == XML_ELEMENT_NODE)
			{
				renderNode(child);
			}
		}
	}
	else if (strcmp(nodeName, "steps") == 0)
	{
		// Render steps as numbered list
		Newline();
		int stepNumber = 1;
		for (xmlNodePtr child = node->children; child; child = child->next)
		{
			if (child->type == XML_ELEMENT_NODE && strcmp((const char*)child->name, "step") == 0)
			{
				renderStep(child, stepNumber++);
			}
		}
		Newline();
	}
	else if (strcmp(nodeName, "codeblock") == 0)
	{
		// Render code block with monospace font
		wxRichTextAttr codeAttr;
		codeAttr.SetFontFamily(wxFONTFAMILY_TELETYPE);
		codeAttr.SetFontSize(9);
		codeAttr.SetBackgroundColour(wxColour(240, 240, 240));
		codeAttr.SetLeftIndent(200);
		codeAttr.SetParagraphSpacingBefore(10);
		codeAttr.SetParagraphSpacingAfter(10);

		BeginStyle(codeAttr);

		// Get all text content preserving whitespace
		xmlChar* content = xmlNodeGetContent(node);
		if (content)
		{
			wxString text = wxString::FromUTF8((const char*)content);
			WriteText(text);
			xmlFree(content);
		}

		EndStyle();
		Newline();
	}
	else
	{
		// Unknown element - render children anyway
		for (xmlNodePtr child = node->children; child; child = child->next)
		{
			if (child->type == XML_ELEMENT_NODE)
			{
				renderNode(child);
			}
		}
	}
}

void DitaWysiwygCtrl::renderTextContent(xmlNodePtr node)
{
	if (!node)
	{
		return;
	}

	// Render direct text content
	for (xmlNodePtr child = node->children; child; child = child->next)
	{
		if (child->type == XML_TEXT_NODE)
		{
			xmlChar* content = child->content;
			if (content)
			{
				wxString text = wxString::FromUTF8((const char*)content);
				WriteText(text);
			}
		}
		else if (child->type == XML_ELEMENT_NODE)
		{
			// Handle inline elements
			const char* childName = (const char*)child->name;
			if (strcmp(childName, "b") == 0 || strcmp(childName, "bold") == 0)
			{
				BeginBold();
				renderTextContent(child);
				EndBold();
			}
			else if (strcmp(childName, "i") == 0 || strcmp(childName, "italic") == 0)
			{
				BeginItalic();
				renderTextContent(child);
				EndItalic();
			}
			else
			{
				// Other inline elements - just render text
				renderTextContent(child);
			}
		}
	}
}

void DitaWysiwygCtrl::renderStep(xmlNodePtr node, int stepNumber)
{
	if (!node)
	{
		return;
	}

	// Start the numbered list item for this step
	BeginNumberedBullet(stepNumber, 100, 60);

	// Process children
	for (xmlNodePtr child = node->children; child; child = child->next)
	{
		if (child->type != XML_ELEMENT_NODE)
		{
			continue;
		}

		const char* childName = (const char*)child->name;

		if (strcmp(childName, "cmd") == 0)
		{
			// Render the command/instruction
			renderTextContent(child);
		}
		else if (strcmp(childName, "substeps") == 0)
		{
			// Render substeps as nested numbered list
			Newline();
			int substepNumber = 1;
			for (xmlNodePtr substep = child->children; substep; substep = substep->next)
			{
				if (substep->type == XML_ELEMENT_NODE && strcmp((const char*)substep->name, "substep") == 0)
				{
					// Render substep
					BeginNumberedBullet(substepNumber++, 150, 60);

					// Look for cmd in substep
					for (xmlNodePtr substepChild = substep->children; substepChild; substepChild = substepChild->next)
					{
						if (substepChild->type == XML_ELEMENT_NODE)
						{
							const char* substepChildName = (const char*)substepChild->name;
							if (strcmp(substepChildName, "cmd") == 0)
							{
								renderTextContent(substepChild);
							}
							else if (strcmp(substepChildName, "stepxmp") == 0)
							{
								Newline();
								// Render step example content
								for (xmlNodePtr exChild = substepChild->children; exChild; exChild = exChild->next)
								{
									if (exChild->type == XML_ELEMENT_NODE)
									{
										renderNode(exChild);
									}
								}
							}
						}
					}

					EndNumberedBullet();
					Newline();
				}
			}
		}
		else if (strcmp(childName, "stepxmp") == 0)
		{
			// Render step example
			Newline();
			for (xmlNodePtr exChild = child->children; exChild; exChild = exChild->next)
			{
				if (exChild->type == XML_ELEMENT_NODE)
				{
					renderNode(exChild);
				}
			}
		}
		else if (strcmp(childName, "info") == 0)
		{
			// Render additional info
			Newline();
			renderTextContent(child);
		}
	}

	EndNumberedBullet();
	Newline();
}

void DitaWysiwygCtrl::updateModelFromContent()
{
	if (!mModel)
	{
		return;
	}

	// Build XML string from rich text content
	std::string xmlContent = buildXmlFromContent();

	// Load the XML back into the model
	if (!xmlContent.empty())
	{
		mModel->loadFromXml(xmlContent);
	}
}

std::string DitaWysiwygCtrl::buildXmlFromContent()
{
	// Start building DITA Topic XML
	std::ostringstream xml;

	xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	xml << "<!DOCTYPE topic PUBLIC \"-//OASIS//DTD DITA Topic//EN\" \"topic.dtd\">\n";
	xml << "<topic id=\"topic\">\n";

	// Get the rich text buffer
	wxRichTextBuffer& buffer = GetBuffer();

	// Track if we've seen the title
	bool foundTitle = false;
	bool inBody = false;

	// Iterate through paragraphs
	for (size_t i = 0; i < buffer.GetChildCount(); i++)
	{
		wxRichTextParagraph* para = wxDynamicCast(buffer.GetChild(i), wxRichTextParagraph);
		if (!para)
		{
			continue;
		}

		// Get paragraph text
		wxString paraText = para->GetTextForRange(para->GetRange());
		paraText.Trim(true).Trim(false);

		if (paraText.IsEmpty())
		{
			continue;
		}

		// Get paragraph attributes
		wxRichTextAttr attr = para->GetAttributes();

		// Check if this is the title (first non-empty paragraph with larger font)
		if (!foundTitle)
		{
			// Assume first paragraph is title
			xml << "  <title>" << escapeXml(paraText.ToStdString()) << "</title>\n";
			foundTitle = true;
			continue;
		}

		// Start body if not already
		if (!inBody)
		{
			xml << "  <body>\n";
			inBody = true;
		}

		// Check if this is a list item
		if (attr.HasBulletStyle())
		{
			// This is a list item - handle in buildListXml
			continue; // Will be processed separately
		}

		// Regular paragraph
		xml << "    <p>" << escapeXml(paraText.ToStdString()) << "</p>\n";
	}

	// Process lists
	std::string listXml = buildListsXml(buffer);
	if (!listXml.empty())
	{
		if (!inBody)
		{
			xml << "  <body>\n";
			inBody = true;
		}
		xml << listXml;
	}

	// Close body if opened
	if (inBody)
	{
		xml << "  </body>\n";
	}

	xml << "</topic>\n";

	return xml.str();
}

std::string DitaWysiwygCtrl::buildListsXml(wxRichTextBuffer& buffer)
{
	std::ostringstream xml;
	bool inList = false;

	for (size_t i = 0; i < buffer.GetChildCount(); i++)
	{
		wxRichTextParagraph* para = wxDynamicCast(buffer.GetChild(i), wxRichTextParagraph);
		if (!para)
		{
			continue;
		}

		wxRichTextAttr attr = para->GetAttributes();

		if (attr.HasBulletStyle())
		{
			wxString paraText = para->GetTextForRange(para->GetRange());
			paraText.Trim(true).Trim(false);

			if (!inList)
			{
				// Determine list type
				if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC)
				{
					xml << "    <ol>\n";
				}
				else
				{
					xml << "    <ul>\n";
				}
				inList = true;
			}

			xml << "      <li>" << escapeXml(paraText.ToStdString()) << "</li>\n";
		}
		else if (inList)
		{
			// End of list
			if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC)
			{
				xml << "    </ol>\n";
			}
			else
			{
				xml << "    </ul>\n";
			}
			inList = false;
		}
	}

	// Close any open list
	if (inList)
	{
		xml << "    </ul>\n";
	}

	return xml.str();
}

std::string DitaWysiwygCtrl::escapeXml(const std::string& text)
{
	std::string escaped;
	escaped.reserve(text.length());

	for (char c : text)
	{
		switch (c)
		{
			case '&':  escaped += "&amp;"; break;
			case '<':  escaped += "&lt;"; break;
			case '>':  escaped += "&gt;"; break;
			case '"':  escaped += "&quot;"; break;
			case '\'': escaped += "&apos;"; break;
			default:   escaped += c; break;
		}
	}

	return escaped;
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
