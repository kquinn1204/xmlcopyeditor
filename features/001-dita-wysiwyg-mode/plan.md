# Implementation Plan: DITA WYSIWYG Mode Integration

**Feature:** 001-dita-wysiwyg-mode
**Created:** 2025-11-14
**Status:** Draft

---

## 1. Architecture Overview

### 1.1 Current XML Copy Editor Architecture

Based on codebase analysis:

- **Editor Core:** `XmlCtrl` (extends `wxStyledTextCtrl` - Scintilla)
- **Document Model:** `XmlDoc` (extends `XmlCtrl`)
- **UI Framework:** wxWidgets 3.0+ with wxAUI for docking panels
- **Panels:** LocationPanel, InsertPanel, CommandPanel, FindReplacePanel
- **Validation:** ValidationThread (background), libxml2 integration
- **XML Parsing:** libxml2 via `wraplibxml.h`

### 1.2 Proposed DITA Extension Architecture

```
                    ┌─────────────────────────────┐
                    │   MyFrame (Main Window)     │
                    │   (xmlcopyeditor.h)         │
                    └──────────┬──────────────────┘
                               │
                ┌──────────────┴──────────────┐
                │                             │
        ┌───────▼────────┐           ┌───────▼────────┐
        │  XmlDoc        │           │  DitaDoc       │
        │  (Code View)   │           │  (extends      │
        │                │           │   XmlDoc)      │
        └────────────────┘           └───────┬────────┘
                                             │
                          ┌──────────────────┴─────────────────┐
                          │                                    │
                  ┌───────▼────────┐                  ┌────────▼────────┐
                  │ DitaWysiwygCtrl│                  │ DitaMapTreeCtrl │
                  │ (Topic editing)│                  │ (Map structure) │
                  └───────┬────────┘                  └────────┬────────┘
                          │                                    │
                  ┌───────▼────────┐                  ┌────────▼────────┐
                  │ DitaTopicModel │                  │  DitaMapModel   │
                  │ (Topic logic)  │                  │  (Map logic)    │
                  └────────────────┘                  └─────────────────┘
```

### 1.3 Component Responsibilities

| Component | Responsibility | Dependencies |
|-----------|---------------|--------------|
| `DitaDoc` | Orchestrate views, detect DITA files | `XmlDoc`, `DitaTopicModel`, `DitaMapModel` |
| `DitaWysiwygCtrl` | WYSIWYG rendering and editing | `wxRichTextCtrl` or custom control |
| `DitaMapTreeCtrl` | Hierarchical map display | `wxTreeCtrl` with drag-drop |
| `DitaTopicModel` | Topic DOM manipulation | libxml2 |
| `DitaMapModel` | Map DOM manipulation | libxml2 |
| `DitaValidator` | DITA-specific validation | DTD files in `src/dtd/dita/` |
| `DitaToolbar` | WYSIWYG formatting toolbar | wxToolBar |

---

## 2. Data Models (CQ4 Compliance)

### 2.1 DitaTopicModel

**File:** `src/ditatopicmodel.h`, `src/ditatopicmodel.cpp`

**Purpose:** Encapsulate all DITA Topic document manipulation logic.

**Key Methods:**
```cpp
class DitaTopicModel {
public:
    // Document lifecycle
    bool loadFromXml(const std::string& xml);
    std::string serializeToXml();

    // Structure queries
    bool isValidTopic();
    std::string getTitle();
    void setTitle(const std::string& title);

    // Content manipulation
    bool insertParagraph(int position, const std::string& text);
    bool insertList(int position, bool ordered);
    bool insertListItem(const std::string& listId, const std::string& text);
    bool transformElement(const std::string& elementId, const std::string& newType);
    bool deleteElement(const std::string& elementId);

    // Validation
    bool canInsertElement(const std::string& parentId, const std::string& elementType);
    std::vector<std::string> getValidChildElements(const std::string& parentId);

private:
    xmlDocPtr doc;
    xmlNodePtr rootNode;
    std::map<std::string, xmlNodePtr> elementIdMap;
};
```

**Dependencies:** libxml2, DITA DTD

### 2.2 DitaMapModel

**File:** `src/ditamapmodel.h`, `src/ditamapmodel.cpp`

**Purpose:** Encapsulate all DITA Map document manipulation logic.

**Key Methods:**
```cpp
class DitaMapModel {
public:
    // Document lifecycle
    bool loadFromXml(const std::string& xml);
    std::string serializeToXml();

    // Structure queries
    std::vector<TopicRef> getTopicRefs();
    TopicRef getTopicRefById(const std::string& id);
    std::vector<TopicRef> getChildren(const std::string& parentId);

    // Manipulation
    bool addTopicRef(const std::string& parentId, const TopicRef& ref);
    bool removeTopicRef(const std::string& id);
    bool moveTopicRef(const std::string& id, const std::string& newParentId, int position);
    bool updateTopicRef(const std::string& id, const TopicRef& updatedRef);

    // Validation
    bool canMoveTopicRef(const std::string& id, const std::string& targetParentId);

private:
    xmlDocPtr doc;
    xmlNodePtr mapNode;
    struct TopicRef {
        std::string id;
        std::string href;
        std::string navtitle;
        std::string format;
        std::vector<std::string> childIds;
    };
};
```

**Dependencies:** libxml2, DITA DTD

---

## 3. UI Components

### 3.1 DitaDoc (Document Orchestrator)

**File:** `src/ditadoc.h`, `src/ditadoc.cpp`

**Purpose:** Extend `XmlDoc` to provide DITA-specific capabilities.

**Key Features:**
- Auto-detect DITA Topic vs. DITAMAP files
- Manage view switching (Code ↔ WYSIWYG ↔ Map)
- Synchronize model changes with UI
- Maintain cursor position across view switches

**Implementation Strategy:**
```cpp
class DitaDoc : public XmlDoc {
public:
    DitaDoc(wxWindow* parent, /* ... */);

    // DITA detection
    bool isDitaTopic();
    bool isDitaMap();

    // View management
    void switchToCodeView();
    void switchToWysiwygView();  // Topics only
    void switchToMapView();       // Maps only
    ViewMode getCurrentView();

    // Model sync
    void syncModelToView();
    void syncViewToModel();

private:
    ViewMode currentView;
    DitaTopicModel* topicModel;
    DitaMapModel* mapModel;
    DitaWysiwygCtrl* wysiwygCtrl;
    DitaMapTreeCtrl* mapTreeCtrl;
};
```

### 3.2 DitaWysiwygCtrl (WYSIWYG Editor)

**File:** `src/ditawysiwygctrl.h`, `src/ditawysiwygctrl.cpp`

**Purpose:** Provide visual DITA Topic editing.

**Base Class Options:**
1. **wxRichTextCtrl** (Recommended)
   - Pros: Built-in rich text support, styling, undo/redo
   - Cons: May need custom rendering for DITA semantics

2. **wxHtmlWindow**
   - Pros: Simple HTML rendering
   - Cons: Limited editing capabilities

3. **Custom wxScrolledWindow**
   - Pros: Full control
   - Cons: Significant implementation effort

**Recommended:** wxRichTextCtrl with custom XML mapping

**Key Features:**
```cpp
class DitaWysiwygCtrl : public wxRichTextCtrl {
public:
    DitaWysiwygCtrl(wxWindow* parent, DitaTopicModel* model);

    // Rendering
    void renderFromModel();
    void updateModelFromContent();

    // Element manipulation
    void insertParagraph();
    void insertList(bool ordered);
    void transformCurrentElement(const std::string& newType);

    // Selection handling
    std::string getCurrentElementId();
    std::vector<std::string> getAvailableTransformations();

private:
    DitaTopicModel* model;
    std::map<long, std::string> positionToElementId;
};
```

### 3.3 DitaMapTreeCtrl (Map Tree View)

**File:** `src/ditamaptreectrl.h`, `src/ditamaptreectrl.cpp`

**Purpose:** Display and edit DITAMAP hierarchy.

**Key Features:**
```cpp
class DitaMapTreeCtrl : public wxTreeCtrl {
public:
    DitaMapTreeCtrl(wxWindow* parent, DitaMapModel* model);

    // Rendering
    void buildTreeFromModel();
    void refreshTree();

    // Interaction
    void onBeginDrag(wxTreeEvent& event);
    void onEndDrag(wxTreeEvent& event);
    void onItemActivated(wxTreeEvent& event);
    void onRightClick(wxTreeEvent& event);

    // Editing
    void addTopicRef(const wxTreeItemId& parent);
    void removeTopicRef(const wxTreeItemId& item);
    void editTopicRef(const wxTreeItemId& item);

private:
    DitaMapModel* model;
    std::map<wxTreeItemId, std::string> treeItemToRefId;
    wxTreeItemId draggedItem;
};
```

### 3.4 DitaToolbar (Contextual Toolbar)

**File:** `src/ditatoolbar.h`, `src/ditatoolbar.cpp`

**Purpose:** Provide WYSIWYG formatting controls.

**Implementation:**
- Add to existing toolbar infrastructure in `xmlcopyeditor.h`
- Show/hide based on current view mode
- Enable/disable buttons based on current element context

**Buttons:**
- Paragraph (transform to `<p>`)
- Ordered List (transform to `<ol>`)
- Unordered List (transform to `<ul>`)
- List Item (transform to `<li>`)
- Bold (wrap in `<b>`)
- Italic (wrap in `<i>`)

---

## 4. Integration Points

### 4.1 File Type Detection

**Location:** `src/xmldoc.cpp` or `src/ditadoc.cpp`

**Strategy:**
```cpp
bool DitaDoc::detectDitaType() {
    // 1. Check file extension
    if (fileName.EndsWith(".dita")) return DITA_TOPIC;
    if (fileName.EndsWith(".ditamap")) return DITA_MAP;

    // 2. Check DOCTYPE
    std::string doctype = extractDoctype();
    if (doctype.find("topic.dtd") != std::string::npos) return DITA_TOPIC;
    if (doctype.find("map.dtd") != std::string::npos) return DITA_MAP;

    // 3. Check root element
    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root && xmlStrcmp(root->name, BAD_CAST "topic") == 0) return DITA_TOPIC;
    if (root && xmlStrcmp(root->name, BAD_CAST "map") == 0) return DITA_MAP;

    return NOT_DITA;
}
```

### 4.2 View Switching UI

**Location:** `src/xmlcopyeditor.cpp` (main frame)

**Approach:**
- Add toggle button to main toolbar (similar to existing buttons)
- Use wxAUI notebook to manage multiple views of same document
- Or use wxPanel with wxSizer to stack views

**Recommended:** wxAUI notebook tabs
- Tab 1: "Code" (existing XmlCtrl)
- Tab 2: "WYSIWYG" (DitaWysiwygCtrl) - Topics only
- Tab 2 alternative: "Map" (DitaMapTreeCtrl) - Maps only

### 4.3 Validation Integration

**Location:** `src/ditavalidator.h`, `src/ditavalidator.cpp`

**Strategy:**
- Extend existing `ValidationThread` approach
- Use DITA DTD files from `src/dtd/dita/`
- Provide DITA-specific error messages

**Real-time Validation:**
- In WYSIWYG mode: prevent invalid operations before they occur
- In Code mode: use existing validation thread

### 4.4 Spell Check Integration

**Location:** `src/ditawysiwygctrl.cpp`

**Strategy:**
- wxRichTextCtrl supports spell checking via `wxTextCtrl` base
- Hook into existing spell check infrastructure (`wrapaspell.h`)
- Ensure text extraction preserves DITA structure

---

## 5. Implementation Phases

### Phase 1: Foundation (Week 1-2)

**Goal:** Set up basic DITA infrastructure without UI.

**Tasks:**
1. Create `DitaTopicModel` with basic load/save
2. Create `DitaMapModel` with basic load/save
3. Add DITA file detection logic
4. Unit tests for models

**Deliverables:**
- `src/ditatopicmodel.h/cpp`
- `src/ditamapmodel.h/cpp`
- `src/ditadetector.h/cpp`
- `tests/test_dita_models.cpp`

**Success Criteria:**
- Can parse DITA Topic XML into model
- Can serialize model back to valid XML
- Can detect DITA files reliably

### Phase 2: Topic WYSIWYG View (Week 3-4)

**Goal:** Basic WYSIWYG editing for DITA Topics.

**Tasks:**
1. Create `DitaWysiwygCtrl` based on wxRichTextCtrl
2. Implement model-to-view rendering
3. Implement view-to-model synchronization
4. Add view toggle button
5. Basic paragraph and list editing

**Deliverables:**
- `src/ditawysiwygctrl.h/cpp`
- `src/ditadoc.h/cpp`
- Modified `src/xmlcopyeditor.cpp` for view switching

**Success Criteria:**
- Can open DITA Topic and see formatted view
- Can edit title, paragraphs
- Can switch back to code view without data loss

### Phase 3: WYSIWYG Toolbar & Validation (Week 5)

**Goal:** Add formatting toolbar and prevent structural errors.

**Tasks:**
1. Create `DitaToolbar` with element transformation buttons
2. Implement `DitaValidator` for structural rules
3. Add contextual button enabling/disabling
4. Integrate with existing validation framework

**Deliverables:**
- `src/ditatoolbar.h/cpp`
- `src/ditavalidator.h/cpp`
- Enhanced `DitaWysiwygCtrl` with validation

**Success Criteria:**
- Can transform paragraph to list item
- Invalid operations are prevented
- Clear user feedback for validation

### Phase 4: DITAMAP Tree View (Week 6-7)

**Goal:** Implement Map View with drag-and-drop.

**Tasks:**
1. Create `DitaMapTreeCtrl` with tree rendering
2. Implement drag-and-drop reordering
3. Add context menu for add/edit/remove
4. Synchronize with code view

**Deliverables:**
- `src/ditamaptreectrl.h/cpp`
- Modified `DitaDoc` for Map view

**Success Criteria:**
- Can view DITAMAP structure as tree
- Can reorder topicrefs via drag-and-drop
- Changes reflect in XML immediately

### Phase 5: Polish & Integration (Week 8)

**Goal:** Performance optimization and final integration.

**Tasks:**
1. Performance profiling and optimization
2. Memory leak testing
3. Integration with spell check
4. Documentation and help updates
5. User acceptance testing

**Deliverables:**
- Performance benchmarks
- Updated user documentation
- Final bug fixes

**Success Criteria:**
- View switching < 200ms (PR1 requirement)
- No memory leaks
- All user stories satisfied

---

## 6. Technical Decisions

### 6.1 WYSIWYG Control Choice

**Decision:** Use `wxRichTextCtrl`

**Rationale:**
- Built-in rich text support reduces implementation effort
- Good performance on Linux
- Undo/redo support built-in
- Existing spell check integration points

**Alternatives Considered:**
- Custom wxScrolledWindow: Too much effort for MVP
- wxHtmlWindow: Insufficient editing capabilities

### 6.2 Model-View Synchronization

**Decision:** Bi-directional sync on view switch and save

**Rationale:**
- Simplifies implementation (no real-time sync complexity)
- Meets performance requirements
- Clear user mental model

**Synchronization Points:**
- On view switch (Code → WYSIWYG or vice versa)
- On save operation
- On validation request

### 6.3 Element Identification

**Decision:** Use XPath or generate unique IDs for elements

**Rationale:**
- Needed to map UI selections to DOM nodes
- XPath: Standard but potentially slow
- Unique IDs: Fast but requires DOM modification

**Recommended:** Generate transient IDs during model load, not persisted to XML

### 6.4 Drag-and-Drop Implementation

**Decision:** Use wxWidgets built-in tree drag-and-drop

**Rationale:**
- wxTreeCtrl has native drag-and-drop support
- Cross-platform compatibility
- Well-tested implementation

**Validation:** Perform validation in `onEndDrag` before accepting drop

---

## 7. File Structure

### New Files

```
src/
├── ditadoc.h                 # DITA document orchestrator
├── ditadoc.cpp
├── ditatopicmodel.h          # DITA Topic model (CQ4)
├── ditatopicmodel.cpp
├── ditamapmodel.h            # DITA Map model (CQ4)
├── ditamapmodel.cpp
├── ditawysiwygctrl.h         # WYSIWYG editor control
├── ditawysiwygctrl.cpp
├── ditamaptreectrl.h         # Map tree view control
├── ditamaptreectrl.cpp
├── ditatoolbar.h             # Contextual toolbar
├── ditatoolbar.cpp
├── ditavalidator.h           # DITA-specific validation
├── ditavalidator.cpp
└── ditadetector.h            # File type detection
    ditadetector.cpp

tests/
├── test_dita_models.cpp      # Unit tests for models
├── test_dita_validator.cpp   # Validation tests
└── test_data/                # Sample DITA files
    ├── simple_topic.dita
    ├── complex_topic.dita
    └── sample_map.ditamap
```

### Modified Files

```
src/
├── xmlcopyeditor.h           # Add DITA menu items, toolbar buttons
├── xmlcopyeditor.cpp         # Integrate DITA views
├── Makefile.am               # Add new source files
└── xmldoc.h                  # Possibly add DITA detection hooks
```

---

## 8. Dependencies & Build

### External Dependencies

- **wxWidgets 3.0+:** Already present
- **libxml2:** Already present
- **DITA DTDs:** Already present in `src/dtd/dita/`

**No new external dependencies required.**

### Build Integration

**Makefile.am modifications:**
```makefile
xmlcopyeditor_SOURCES = \
    # ... existing sources ...
    src/ditadoc.cpp \
    src/ditatopicmodel.cpp \
    src/ditamapmodel.cpp \
    src/ditawysiwygctrl.cpp \
    src/ditamaptreectrl.cpp \
    src/ditatoolbar.cpp \
    src/ditavalidator.cpp \
    src/ditadetector.cpp
```

### Platform-Specific Considerations

**Fedora Linux (primary target):**
- wxWidgets available via `dnf install wxGTK-devel`
- libxml2 available via `dnf install libxml2-devel`
- Test on Fedora 42 (kernel 6.17.6-200.fc42.x86_64)

**Cross-platform:**
- Code should remain cross-platform (Windows, macOS)
- Use wxWidgets abstractions, avoid platform-specific calls

---

## 9. Testing Strategy

### Unit Tests

**Framework:** Existing test infrastructure or add Catch2/Google Test

**Coverage:**
- `DitaTopicModel`: Load, save, manipulation, validation
- `DitaMapModel`: Load, save, reordering, validation
- `DitaValidator`: Structural rule enforcement

### Integration Tests

**Scenarios:**
1. Open DITA Topic → switch to WYSIWYG → edit → switch to Code → verify XML
2. Open DITAMAP → switch to Map View → reorder → verify XML
3. Transform paragraph to list item → verify DOM changes
4. Attempt invalid operation → verify prevention

### Performance Tests

**Benchmarks:**
- View switching time (target: < 200ms)
- Large document rendering (1000 elements, target: < 1s)
- Drag-and-drop response (target: < 100ms)

### User Acceptance Testing

**Test Users:** DITA documentation authors

**Scenarios:**
- Create simple DITA Topic from scratch
- Edit existing DITA Topic
- Reorganize DITAMAP structure
- Usability feedback on toolbar and controls

---

## 10. Risks & Mitigations

### Risk 1: wxRichTextCtrl Limitations

**Risk:** wxRichTextCtrl may not support all needed DITA rendering.

**Mitigation:**
- Prototype early (Phase 2, Week 3)
- Have fallback plan: simplify WYSIWYG to basic formatting
- Focus on most common elements (p, ul, li, b, i)

### Risk 2: Performance Degradation

**Risk:** View switching or large document rendering may be slow.

**Mitigation:**
- Profile early and often
- Implement lazy loading for tree view
- Use wxWidgets' virtual tree control for large maps
- Cache rendered content where possible

### Risk 3: Model-View Sync Complexity

**Risk:** Keeping Code and WYSIWYG views in sync may introduce bugs.

**Mitigation:**
- Comprehensive unit tests for sync logic
- Clear ownership: model is source of truth
- Validate after every sync operation

### Risk 4: Regression in Existing Features

**Risk:** Changes to core classes may break existing functionality.

**Mitigation:**
- Extend via inheritance (DitaDoc extends XmlDoc)
- Avoid modifying core XmlCtrl/XmlDoc logic
- Regression testing before each milestone
- Feature flag to disable DITA features if needed

---

## 11. Future Enhancements (Out of Scope for v1)

The following enhancements are explicitly deferred to future iterations:

1. **Advanced DITA Features:**
   - Conref support
   - Conditional processing (ditaval)
   - Key references
   - Content reuse mechanisms

2. **Publishing Integration:**
   - DITA Open Toolkit integration
   - PDF/HTML output generation
   - Direct publishing workflows

3. **Collaborative Features:**
   - Real-time collaborative editing
   - Change tracking and review
   - Version control integration

4. **Advanced WYSIWYG:**
   - Table editing in WYSIWYG
   - Image preview and management
   - Cross-reference visualization
   - Inline attribute editing

5. **Specialization Support:**
   - Custom DITA specializations
   - Specialized topic types (task, concept, reference)
   - Domain specializations

---

## 12. Success Metrics

This implementation will be considered successful when:

1. **Functional Completeness:**
   - All 6 user stories (US-001 through US-006) are implemented
   - All acceptance criteria are met

2. **Performance:**
   - View switching < 200ms (PR1)
   - No perceivable lag during normal editing

3. **Quality:**
   - Zero P0/P1 bugs at release
   - Code coverage > 70% for new components
   - No regressions in existing features

4. **User Satisfaction:**
   - Positive feedback from at least 3 DITA authors
   - Time to create simple topic reduced by 50% vs. pure code editing

5. **Maintainability:**
   - Code adheres to CQ4 (separate models)
   - Documentation complete for all public APIs
   - Architecture allows for future enhancements

---

## 13. Timeline Summary

| Phase | Duration | Key Deliverables |
|-------|----------|------------------|
| Phase 1: Foundation | 2 weeks | Topic/Map models, file detection |
| Phase 2: WYSIWYG View | 2 weeks | Basic WYSIWYG editing |
| Phase 3: Toolbar & Validation | 1 week | Formatting controls, validation |
| Phase 4: Map Tree View | 2 weeks | Hierarchical map editing |
| Phase 5: Polish | 1 week | Performance, integration, testing |
| **Total** | **8 weeks** | **Feature complete** |

---

## 14. Next Steps

1. **Review this plan** with stakeholders
2. **Break down into tasks** using `/tasks` command
3. **Set up development environment** (Fedora Linux, build tools)
4. **Begin Phase 1 implementation**
5. **Schedule weekly progress reviews**

---

**Plan Status:** Ready for task breakdown
**Next Command:** `/tasks` to generate actionable task list
