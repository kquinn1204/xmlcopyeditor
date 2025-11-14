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

#include "ditadoc.h"
#include <wx/wx.h>

DitaDoc::DitaDoc(
	wxWindow *parent,
	XmlCtrlProperties properties,
	bool *protectTags,
	int visibilityState,
	int type,
	wxWindowID id,
	const char *buffer,
	size_t bufferLen,
	const wxString& basePath,
	const wxString& auxPath,
	const wxPoint& position,
	const wxSize& size,
	long style
) : XmlDoc(
		parent,
		properties,
		protectTags,
		visibilityState,
		type,
		id,
		buffer,
		bufferLen,
		basePath,
		auxPath,
		position,
		size,
		style
	),
	mDitaType(DITA_TYPE_NONE),
	mViewMode(DITA_VIEW_CODE),
	mWysiwygCtrl(NULL)
{
	// Auto-detect DITA type from content if buffer provided
	if (buffer && bufferLen > 0)
	{
		initializeDitaModels();
	}
}

DitaDoc::~DitaDoc()
{
	// Delete WYSIWYG control if created
	if (mWysiwygCtrl)
	{
		mWysiwygCtrl->Destroy();
		mWysiwygCtrl = NULL;
	}
	// boost::scoped_ptr will automatically clean up models
}

DitaFileType DitaDoc::getDitaType() const
{
	return mDitaType;
}

DitaViewMode DitaDoc::getViewMode() const
{
	return mViewMode;
}

bool DitaDoc::isDitaDocument() const
{
	return (mDitaType == DITA_TYPE_TOPIC || mDitaType == DITA_TYPE_MAP);
}

DitaTopicModel* DitaDoc::getTopicModel()
{
	return mTopicModel.get();
}

DitaMapModel* DitaDoc::getMapModel()
{
	return mMapModel.get();
}

DitaWysiwygCtrl* DitaDoc::getWysiwygCtrl()
{
	return mWysiwygCtrl;
}

void DitaDoc::setViewMode(DitaViewMode mode)
{
	// If mode is same, do nothing
	if (mode == mViewMode)
		return;

	// Only support WYSIWYG for topics
	if (mode == DITA_VIEW_WYSIWYG && mDitaType != DITA_TYPE_TOPIC)
		return;

	// Perform the switch
	if (mode == DITA_VIEW_WYSIWYG)
	{
		switchToWysiwygView();
	}
	else if (mode == DITA_VIEW_CODE)
	{
		switchToCodeView();
	}

	mViewMode = mode;
}

void DitaDoc::switchToCodeView()
{
	if (!mWysiwygCtrl || !mTopicModel)
		return;

	// NOTE: Skipping syncWysiwygToModel() for now because buildXmlFromContent()
	// creates a simplified topic structure and loses task-specific elements
	// (steps, substeps, codeblocks, etc.). WYSIWYG view is currently read-only.
	// TODO: Implement proper two-way sync that preserves all DITA elements

	// Render model to code (original content preserved)
	renderModelToCode();

	// Hide WYSIWYG, show code view
	mWysiwygCtrl->Hide();
	this->Show();
	this->SetFocus();
}

void DitaDoc::switchToWysiwygView()
{
	if (!mTopicModel)
		return;

	// Create WYSIWYG control if not yet created
	if (!mWysiwygCtrl)
	{
		// Get parent window (should be the notebook)
		wxWindow *parent = GetParent();
		if (!parent)
			return;

		// Create WYSIWYG control with same parent
		mWysiwygCtrl = new DitaWysiwygCtrl(
			parent,
			mTopicModel.get(),
			wxID_ANY,
			wxEmptyString,
			GetPosition(),
			GetSize()
		);

		// Position it to overlap this control
		mWysiwygCtrl->SetPosition(GetPosition());
		mWysiwygCtrl->SetSize(GetSize());
	}

	// Sync code to model
	syncCodeToModel();

	// Render model to WYSIWYG
	renderModelToWysiwyg();

	// Hide code view, show WYSIWYG
	this->Hide();
	mWysiwygCtrl->Show();
	mWysiwygCtrl->SetFocus();
}

bool DitaDoc::initializeDitaModels()
{
	// Get document content as std::string
	wxString wxContent = GetText();
	std::string content(wxContent.mb_str(wxConvUTF8));

	// Detect DITA type from content
	mDitaType = detectDitaTypeFromContent();

	// Initialize appropriate model based on type
	if (mDitaType == DITA_TYPE_TOPIC)
	{
		mTopicModel.reset(new DitaTopicModel());
		if (!mTopicModel->loadFromXml(content))
		{
			// Failed to load - not a valid DITA topic
			mDitaType = DITA_TYPE_NONE;
			mTopicModel.reset();
			return false;
		}
		return true;
	}
	else if (mDitaType == DITA_TYPE_MAP)
	{
		mMapModel.reset(new DitaMapModel());
		if (!mMapModel->loadFromXml(content))
		{
			// Failed to load - not a valid DITA map
			mDitaType = DITA_TYPE_NONE;
			mMapModel.reset();
			return false;
		}
		return true;
	}

	// Not a DITA document
	return false;
}

DitaFileType DitaDoc::detectDitaTypeFromContent()
{
	// Get document content as std::string
	wxString wxContent = GetText();
	std::string content(wxContent.mb_str(wxConvUTF8));

	// Use ditadetector utility to detect type (global function)
	return ::detectDitaTypeFromContent(content);
}

void DitaDoc::syncCodeToModel()
{
	if (!mTopicModel)
		return;

	// Get XML content from code view
	wxString wxContent = GetText();
	std::string content(wxContent.mb_str(wxConvUTF8));

	// Load into model
	mTopicModel->loadFromXml(content);
}

void DitaDoc::syncWysiwygToModel()
{
	if (!mWysiwygCtrl || !mTopicModel)
		return;

	// Sync WYSIWYG changes to model
	mWysiwygCtrl->updateModelFromContent();
}

void DitaDoc::renderModelToCode()
{
	if (!mTopicModel)
		return;

	// Serialize model to XML
	std::string xml = mTopicModel->serializeToXml();

	// Set text in code view
	wxString wxXml = wxString::FromUTF8(xml.c_str());
	SetText(wxXml);
}

void DitaDoc::renderModelToWysiwyg()
{
	if (!mWysiwygCtrl || !mTopicModel)
		return;

	// Render model in WYSIWYG view
	mWysiwygCtrl->renderFromModel();
}
