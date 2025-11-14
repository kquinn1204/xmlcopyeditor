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

#ifndef DITA_DOC_H
#define DITA_DOC_H

#include <wx/wx.h>
#include <boost/scoped_ptr.hpp>
#include "xmldoc.h"
#include "ditadetector.h"
#include "ditatopicmodel.h"
#include "ditamapmodel.h"

/**
 * View modes for DITA documents
 */
enum DitaViewMode
{
	DITA_VIEW_CODE,      // Standard XML code view
	DITA_VIEW_WYSIWYG,   // WYSIWYG editing for topics
	DITA_VIEW_MAP        // Tree view for maps
};

/**
 * DitaDoc extends XmlDoc to provide DITA-specific functionality
 *
 * This class adds:
 * - Automatic DITA file type detection
 * - DITA-specific model management (Topic or Map)
 * - View mode switching (Code, WYSIWYG, Map)
 *
 * Design pattern: Composition over modification
 * - Does not modify XmlDoc base class
 * - Uses models as member variables
 * - Follows existing document lifecycle patterns
 */
class DitaDoc : public XmlDoc
{
public:
	/**
	 * Constructor - same signature as XmlDoc for compatibility
	 *
	 * @param parent Parent window
	 * @param properties XML control properties
	 * @param protectTags Pointer to tag protection flag
	 * @param visibilityState Initial visibility state
	 * @param type File type constant
	 * @param id Window ID
	 * @param buffer Initial buffer content
	 * @param bufferLen Buffer length
	 * @param basePath Base path for relative files
	 * @param auxPath Auxiliary path
	 * @param position Window position
	 * @param size Window size
	 * @param style Window style flags
	 */
	DitaDoc(
		wxWindow *parent,
		XmlCtrlProperties properties,
		bool *protectTags,
		int visibilityState = SHOW_TAGS,
		int type = FILE_TYPE_XML,
		wxWindowID id = wxID_ANY,
		const char *buffer = NULL,
		size_t bufferLen = 0,
		const wxString& basePath = wxEmptyString,
		const wxString& auxPath = wxEmptyString,
		const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0
	);

	/**
	 * Destructor - cleans up models
	 */
	virtual ~DitaDoc();

	/**
	 * Get the detected DITA file type
	 *
	 * @return DitaFileType enumeration (TOPIC, MAP, or NONE)
	 */
	DitaFileType getDitaType() const;

	/**
	 * Get the current view mode
	 *
	 * @return DitaViewMode enumeration
	 */
	DitaViewMode getViewMode() const;

	/**
	 * Check if this is a DITA document
	 *
	 * @return true if DITA Topic or Map, false otherwise
	 */
	bool isDitaDocument() const;

	/**
	 * Get the DITA Topic model (if this is a topic)
	 *
	 * @return Pointer to DitaTopicModel or NULL if not a topic
	 */
	DitaTopicModel* getTopicModel();

	/**
	 * Get the DITA Map model (if this is a map)
	 *
	 * @return Pointer to DitaMapModel or NULL if not a map
	 */
	DitaMapModel* getMapModel();

	/**
	 * Set the view mode (called by view switching logic)
	 *
	 * @param mode New view mode to set
	 */
	void setViewMode(DitaViewMode mode);

	/**
	 * Initialize DITA models from current document content
	 * Called after document is loaded or content changes
	 *
	 * @return true if models initialized successfully
	 */
	bool initializeDitaModels();

protected:
	/**
	 * Detect DITA file type from document content
	 *
	 * @return Detected DitaFileType
	 */
	DitaFileType detectDitaTypeFromContent();

private:
	DitaFileType mDitaType;                        // Detected DITA file type
	DitaViewMode mViewMode;                        // Current view mode
	boost::scoped_ptr<DitaTopicModel> mTopicModel; // Topic model (if applicable)
	boost::scoped_ptr<DitaMapModel> mMapModel;     // Map model (if applicable)
};

#endif // DITA_DOC_H
