# Feature 001: DITA WYSIWYG Mode Integration

**Short Name:** dita-wysiwyg-mode
**Status:** Draft
**Created:** 2025-11-14
**Platform:** Fedora Linux (primary target)

---

## Overview

This feature integrates DITA (Darwin Information Typing Architecture) Topic and DITAMAP editing capabilities into the existing XML Copy Editor application framework. It provides a WYSIWYG (What You See Is What You Get) editing mode for DITA Topic documents and a hierarchical tree-based interface for DITAMAP files, while maintaining the existing code view capabilities.

## User Stories

### Topic WYSIWYG Mode

**US-001: View Mode Toggle**
As a DITA author, when a DITA Topic file is open, I want a "WYSIWYG View" toggle button visible in the interface so I can switch from the current code view to an editable, visually structured view.

**Acceptance Criteria:**
- Toggle button is visible when a `.dita` or `.xml` file with DITA Topic schema is open
- Button clearly indicates current mode (Code/WYSIWYG)
- Switching modes preserves all document content
- Mode preference is remembered per document type

**US-002: Basic Topic Editing**
In WYSIWYG View, I must be able to create and edit basic DITA Topic structures including the required `<title>` element, paragraphs (`<p>`), and unordered lists (`<ul>`/`<li>`).

**Acceptance Criteria:**
- User can load an empty `<topic>` template
- Title field is prominently displayed and editable
- Paragraph text can be entered and edited inline
- Lists can be created and nested appropriately
- Visual formatting distinguishes different element types

**US-003: Contextual Formatting Toolbar**
I need a contextual toolbar that appears only in WYSIWYG mode, providing buttons to transform the current element between common types like `<p>` and `<li>`.

**Acceptance Criteria:**
- Toolbar appears only when WYSIWYG mode is active
- Buttons clearly indicate element type transformation options
- Current element type is visually indicated
- Transformations maintain content while changing structure
- Invalid transformations are disabled/grayed out

**US-004: Structural Validation**
Structural errors (e.g., list item outside a list) must be prevented by the editor's logic before they occur, building on the existing Validation feature of XML Copy Editor.

**Acceptance Criteria:**
- Invalid element placements are prevented in real-time
- User receives clear feedback when attempting invalid operations
- Validation rules align with DITA DTD specifications
- Existing XML Copy Editor validation integration is maintained

### DITAMAP Tree Interface

**US-005: Map View Display**
As a DITA architect, when a DITAMAP file is open, I want a "Map View" tab or panel to display the hierarchical structure of `<topicref>` elements clearly.

**Acceptance Criteria:**
- Map View appears as a tab or docked panel (not a separate window)
- Tree structure clearly shows nesting levels
- Each `<topicref>` displays its `href` and `navtitle` attributes
- Expand/collapse controls for nested structures work smoothly

**US-006: Drag-and-Drop Reordering**
In Map View, I must be able to drag-and-drop `<topicref>` elements to reorder them or change their nesting level, with DITA Map schema rules enforced.

**Acceptance Criteria:**
- Users can drag `<topicref>` elements within the tree
- Drop targets are visually indicated during drag operations
- Invalid drop locations are prevented or clearly indicated
- XML source is updated immediately after successful reorder
- Undo/redo support for reordering operations

## Functional Requirements

### FR-001: DITA Document Detection
The application must automatically detect when a DITA Topic or DITAMAP file is opened based on:
- File extension (`.dita`, `.ditamap`)
- DOCTYPE declaration
- Root element analysis

### FR-002: View Switching
Users can toggle between Code View (existing) and WYSIWYG/Map View:
- Switching preserves all content and formatting
- Cursor position is maintained where semantically possible
- Changes in either view are immediately reflected
- No data loss during view transitions

### FR-003: WYSIWYG Rendering
WYSIWYG View renders DITA elements with appropriate visual styling:
- Headings (title, section titles) are prominently displayed
- Paragraphs have readable spacing
- Lists are visually indented
- Tables maintain structural clarity
- Inline elements (bold, italic) are rendered appropriately

### FR-004: Element Manipulation
Users can manipulate DITA elements through visual interface:
- Insert new paragraphs, lists, and sections
- Transform element types via toolbar
- Delete elements with proper cleanup
- Cut/copy/paste operations respect DITA structure

### FR-005: Map Structure Editing
DITAMAP editing supports hierarchical manipulation:
- Add new `<topicref>` elements
- Edit `href` and `navtitle` attributes inline
- Reorder elements via drag-and-drop
- Change nesting levels
- Remove references

### FR-006: Real-time Validation
Structural validation prevents invalid DITA constructs:
- Elements must appear in valid parent contexts
- Required attributes are enforced
- Content models are validated
- User receives immediate feedback on violations

## Non-Functional Requirements

### NFR-001: Performance (PR1 - Rendering Threshold)
- View switching completes in under 200ms for documents up to 1000 elements
- WYSIWYG rendering maintains 60fps scrolling on target Fedora Linux platform
- Map tree operations complete in under 100ms

### NFR-002: Code Quality (CQ4 - Separate Map and Topic Models)
- Topic manipulation logic is isolated in dedicated model classes
- Map manipulation logic is isolated in separate model classes
- Models are independent and reusable
- Clear separation between UI and business logic

### NFR-003: Integration
- WYSIWYG view integrates into existing tabbed document interface
- Map View appears as dockable panel or tab (not separate window)
- Existing spell check and style check features work in WYSIWYG mode
- All existing XML Copy Editor features remain functional

### NFR-004: User Experience (UX Principles)
- **UX1:** Invalid operations are prevented proactively, not just flagged
- **UX2:** Toggle controls are clearly labeled and easy to locate
- **UX4:** Hierarchical structures are visually intuitive and navigable
- Interface follows existing XML Copy Editor design patterns

## Technical Constraints

### Existing Architecture
- Built with wxWidgets 3.0+ (per version 1.3.1.0)
- Uses libxml2 for XML parsing
- Scintilla-based text editing component
- Must maintain compatibility with existing validation framework

### Platform Requirements
- Primary target: Fedora Linux (current: 6.17.6-200.fc42.x86_64)
- Must compile with standard development tools
- Dependencies should be available in Fedora repositories

### DITA Specifications
- Support DITA 1.3 specification as baseline
- Honor existing DITA DTD files in `src/dtd/dita/` directory
- Align with DITA content models and specialization rules

## Out of Scope

The following are explicitly excluded from this feature:
- Full DITA specialization support (focus on base topic types)
- Advanced DITA features (conref, conditional processing, keys)
- Publishing/transformation capabilities
- Collaborative editing or version control integration
- DITA 2.0 specific features
- Image/media asset management beyond basic `href` references

## Success Criteria

This feature will be considered successful when:

1. **Usability:** Users can create and edit simple DITA topics without touching XML code
2. **Performance:** View transitions are instantaneous (subjectively smooth)
3. **Stability:** No regressions in existing XML Copy Editor functionality
4. **Validation:** Structural errors are prevented, not just detected
5. **Integration:** Features fit naturally into existing UI without feeling bolted-on

## Dependencies

- Existing DITA DTD files in `src/dtd/dita/` directory
- wxWidgets 3.0+ GUI framework
- libxml2 for XML parsing and validation
- Existing validation infrastructure in XML Copy Editor

## Assumptions

- Users opening DITA files have basic familiarity with DITA concepts
- DITA files conform to DITA 1.3 specification
- Primary use case is documentation authoring, not complex content reuse
- Single-user editing (no concurrent multi-user scenarios)

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Performance degradation on large DITA maps | High | Implement lazy loading and virtual tree rendering |
| WYSIWYG rendering complexity for nested structures | Medium | Start with basic elements, iterate based on usage |
| Validation logic conflicts with existing framework | High | Thorough integration testing, separate DITA validation layer |
| User confusion with mode switching | Medium | Clear visual indicators, comprehensive tooltips |

## Open Questions

1. Should WYSIWYG mode support inline editing of DITA attributes?
2. How should conrefs be displayed in WYSIWYG view (if at all)?
3. Should Map View support filtering or searching of topicrefs?
4. What level of undo granularity is expected (per-keystroke vs. per-operation)?

---

**Next Steps:**
1. Review specification with stakeholders
2. Create technical implementation plan (`/plan`)
3. Break down into actionable tasks (`/tasks`)
4. Begin implementation (`/implement`)
