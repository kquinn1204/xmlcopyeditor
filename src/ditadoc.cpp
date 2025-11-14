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
	mViewMode(DITA_VIEW_CODE)
{
	// Auto-detect DITA type from content if buffer provided
	if (buffer && bufferLen > 0)
	{
		initializeDitaModels();
	}
}

DitaDoc::~DitaDoc()
{
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

void DitaDoc::setViewMode(DitaViewMode mode)
{
	mViewMode = mode;
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
