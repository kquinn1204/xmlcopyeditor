# Task Breakdown: DITA WYSIWYG Mode Integration

**Feature:** 001-dita-wysiwyg-mode
**Created:** 2025-11-14
**Status:** Ready for Implementation

---

## Task Summary

| Phase | Tasks | Estimated Effort |
|-------|-------|------------------|
| Phase 1: Foundation | 8 tasks | 2 weeks |
| Phase 2: WYSIWYG View | 7 tasks | 2 weeks |
| Phase 3: Toolbar & Validation | 6 tasks | 1 week |
| Phase 4: Map Tree View | 7 tasks | 2 weeks |
| Phase 5: Polish & Integration | 6 tasks | 1 week |
| **Total** | **34 tasks** | **8 weeks** |

---

## Phase 1: Foundation (Week 1-2)

**Goal:** Set up basic DITA infrastructure without UI.

### TASK-001: Create DitaTopicModel Header and Skeleton
**Priority:** P0 (Blocking)
**Effort:** 2 hours
**Dependencies:** None

**Description:**
Create the `DitaTopicModel` class header file with all method signatures defined.

**Files to Create:**
- `src/ditatopicmodel.h`
- `src/ditatopicmodel.cpp` (skeleton only)

**Acceptance Criteria:**
- [ ] Header file compiles without errors
- [ ] All public methods declared with documentation comments
- [ ] Private member variables defined (xmlDocPtr, xmlNodePtr, maps)
- [ ] Includes necessary libxml2 headers
- [ ] Copyright and license headers present

**Implementation Notes:**
- Follow existing XML Copy Editor code style
- Use boost::scoped_ptr for memory management consistency
- Reference plan.md section 2.1 for method signatures

---

### TASK-002: Implement DitaTopicModel Basic Load/Parse
**Priority:** P0 (Blocking)
**Effort:** 4 hours
**Dependencies:** TASK-001

**Description:**
Implement XML loading and parsing into the DitaTopicModel DOM structure.

**Methods to Implement:**
- `bool loadFromXml(const std::string& xml)`
- `bool isValidTopic()`
- Private helper methods for DOM initialization

**Acceptance Criteria:**
- [ ] Can parse valid DITA Topic XML string
- [ ] Returns false for invalid XML
- [ ] Properly initializes internal DOM structures
- [ ] No memory leaks (verify with valgrind)
- [ ] Root element validation (must be `<topic>`)

**Test Cases:**
- Load minimal valid topic: `<topic id="test"><title>Test</title></topic>`
- Load invalid XML (malformed)
- Load valid XML but not DITA topic

---

### TASK-003: Implement DitaTopicModel Serialization
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-002

**Description:**
Implement serialization from internal DOM back to XML string.

**Methods to Implement:**
- `std::string serializeToXml()`

**Acceptance Criteria:**
- [ ] Serialized XML is well-formed
- [ ] Preserves all elements and attributes
- [ ] Maintains proper DITA structure
- [ ] Round-trip test passes (load → serialize → load produces identical DOM)
- [ ] Proper XML declaration and encoding

**Test Cases:**
- Load sample DITA file, serialize, compare
- Round-trip test with various topic structures

---

### TASK-004: Implement DitaTopicModel Content Queries
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-002

**Description:**
Implement methods to query topic structure and content.

**Methods to Implement:**
- `std::string getTitle()`
- `void setTitle(const std::string& title)`
- Helper methods for element traversal

**Acceptance Criteria:**
- [ ] Can extract title text from topic
- [ ] Can set/update title
- [ ] Handles missing title element gracefully
- [ ] Handles nested title content (text nodes only for MVP)

**Test Cases:**
- Get title from valid topic
- Get title from topic without title
- Set title on new topic
- Update existing title

---

### TASK-005: Create DitaMapModel with Load/Save
**Priority:** P0 (Blocking)
**Effort:** 5 hours
**Dependencies:** TASK-003 (similar pattern)

**Description:**
Create `DitaMapModel` class with basic load, parse, and serialize functionality.

**Files to Create:**
- `src/ditamapmodel.h`
- `src/ditamapmodel.cpp`

**Methods to Implement:**
- `bool loadFromXml(const std::string& xml)`
- `std::string serializeToXml()`
- `std::vector<TopicRef> getTopicRefs()`
- Internal TopicRef structure parsing

**Acceptance Criteria:**
- [ ] Can parse DITAMAP XML
- [ ] Extracts topicref elements into structures
- [ ] Preserves href, navtitle, and nesting
- [ ] Round-trip serialization works
- [ ] No memory leaks

**Test Cases:**
- Load flat DITAMAP (no nesting)
- Load nested DITAMAP (2-3 levels)
- Serialize and compare

---

### TASK-006: Implement DITA File Detection
**Priority:** P1
**Effort:** 3 hours
**Dependencies:** None

**Description:**
Create utility functions to detect DITA file types (Topic vs. Map vs. Other).

**Files to Create:**
- `src/ditadetector.h`
- `src/ditadetector.cpp`

**Functions to Implement:**
```cpp
enum DitaFileType { DITA_TOPIC, DITA_MAP, NOT_DITA };
DitaFileType detectDitaType(const wxString& filePath);
DitaFileType detectDitaTypeFromContent(const std::string& xmlContent);
```

**Detection Strategy:**
1. Check file extension (.dita, .ditamap)
2. Check DOCTYPE declaration
3. Check root element name

**Acceptance Criteria:**
- [ ] Correctly identifies DITA Topic files
- [ ] Correctly identifies DITAMAP files
- [ ] Returns NOT_DITA for other XML files
- [ ] Works with both file path and content string
- [ ] Handles edge cases (no DOCTYPE, generic .xml extension)

**Test Cases:**
- test_data/simple_topic.dita → DITA_TOPIC
- test_data/sample_map.ditamap → DITA_MAP
- Regular XML file → NOT_DITA

---

### TASK-007: Add Build System Integration
**Priority:** P1
**Effort:** 2 hours
**Dependencies:** TASK-001, TASK-005, TASK-006

**Description:**
Update build system to compile new DITA source files.

**Files to Modify:**
- `src/Makefile.am`

**Changes:**
- Add new .cpp files to `xmlcopyeditor_SOURCES`
- Verify compilation on Fedora Linux

**Acceptance Criteria:**
- [ ] `./configure && make` completes successfully
- [ ] All new source files compile without warnings
- [ ] No linker errors
- [ ] Application still runs (smoke test)

**Verification:**
```bash
./configure
make clean
make
./src/xmlcopyeditor
```

---

### TASK-008: Create Unit Tests for Models
**Priority:** P1
**Effort:** 4 hours
**Dependencies:** TASK-004, TASK-005

**Description:**
Create comprehensive unit tests for DitaTopicModel and DitaMapModel.

**Files to Create:**
- `tests/test_dita_models.cpp`
- `tests/test_data/simple_topic.dita`
- `tests/test_data/complex_topic.dita`
- `tests/test_data/sample_map.ditamap`

**Test Coverage:**
- Load valid/invalid XML
- Serialize and round-trip
- Get/set title
- Extract topicref structures
- Error handling

**Acceptance Criteria:**
- [ ] All tests pass
- [ ] Code coverage > 70% for model classes
- [ ] Tests are automated and can run in CI
- [ ] Test data files are valid DITA

**Test Framework:** Use existing test infrastructure or add minimal test harness

---

## Phase 2: WYSIWYG View (Week 3-4)

**Goal:** Basic WYSIWYG editing for DITA Topics.

### TASK-009: Create DitaDoc Class Structure
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-004

**Description:**
Create `DitaDoc` class that extends `XmlDoc` to add DITA-specific capabilities.

**Files to Create:**
- `src/ditadoc.h`
- `src/ditadoc.cpp`

**Key Features:**
- Detect DITA file type on construction
- Maintain reference to DitaTopicModel or DitaMapModel
- Track current view mode (Code, WYSIWYG, Map)

**Acceptance Criteria:**
- [ ] Extends XmlDoc properly
- [ ] Constructor accepts same parameters as XmlDoc
- [ ] Auto-detects DITA type using ditadetector
- [ ] Initializes appropriate model (Topic or Map)
- [ ] Compiles and links successfully

**Implementation Notes:**
- Don't modify XmlDoc base class
- Use composition for models (member variables)
- Follow existing document lifecycle patterns

---

### TASK-010: Create DitaWysiwygCtrl Skeleton
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-009

**Description:**
Create the WYSIWYG control class based on wxRichTextCtrl.

**Files to Create:**
- `src/ditawysiwygctrl.h`
- `src/ditawysiwygctrl.cpp`

**Key Features:**
- Extend wxRichTextCtrl
- Accept DitaTopicModel pointer in constructor
- Placeholder methods for rendering and syncing

**Acceptance Criteria:**
- [ ] Class compiles successfully
- [ ] Can be instantiated with a model pointer
- [ ] Shows empty editable area
- [ ] Basic wxRichTextCtrl features work (typing, selection)

**Implementation Notes:**
- Reference wxRichTextCtrl documentation
- Set up basic styling attributes
- Initialize default font and paragraph settings

---

### TASK-011: Implement Model-to-View Rendering (Basic)
**Priority:** P0 (Blocking)
**Effort:** 6 hours
**Dependencies:** TASK-010

**Description:**
Implement rendering of DITA Topic model into WYSIWYG view.

**Method to Implement:**
- `void DitaWysiwygCtrl::renderFromModel()`

**Elements to Support (MVP):**
- `<title>` → Large bold heading
- `<p>` → Normal paragraph
- `<ul>` / `<li>` → Bullet list
- Basic text content

**Acceptance Criteria:**
- [ ] Title renders prominently (larger font, bold)
- [ ] Paragraphs have proper spacing
- [ ] Lists are indented and bulleted
- [ ] Multiple paragraphs render in sequence
- [ ] Nested lists render with proper indentation

**Test Cases:**
- Render simple_topic.dita
- Render topic with nested lists
- Visual inspection: formatting looks reasonable

---

### TASK-012: Implement View-to-Model Synchronization (Basic)
**Priority:** P0 (Blocking)
**Effort:** 6 hours
**Dependencies:** TASK-011

**Description:**
Implement synchronization from WYSIWYG view changes back to model.

**Method to Implement:**
- `void DitaWysiwygCtrl::updateModelFromContent()`

**Synchronization Strategy:**
- Parse rich text structure
- Map rich text styles to DITA elements
- Update model DOM

**Acceptance Criteria:**
- [ ] Title text changes sync to model
- [ ] Paragraph text changes sync to model
- [ ] List structure changes sync to model
- [ ] Round-trip works: render → edit → sync → render produces same result
- [ ] Model serializes to valid DITA XML after sync

**Test Cases:**
- Edit title, sync, verify model
- Add paragraph, sync, serialize, validate XML
- Modify list, sync, verify structure

---

### TASK-013: Add View Toggle Button to Main Toolbar
**Priority:** P1
**Effort:** 3 hours
**Dependencies:** TASK-009

**Description:**
Add a toggle button to main toolbar to switch between Code and WYSIWYG views.

**Files to Modify:**
- `src/xmlcopyeditor.h` (add enum/IDs)
- `src/xmlcopyeditor.cpp` (add button, handler)

**Button Design:**
- Label: "WYSIWYG" or icon
- Enabled only when DITA Topic is open
- Toggle state: Code ↔ WYSIWYG

**Acceptance Criteria:**
- [ ] Button appears in main toolbar
- [ ] Button is disabled for non-DITA files
- [ ] Button is enabled for DITA Topic files
- [ ] Clicking button triggers view switch handler
- [ ] Button state reflects current view

**Implementation Notes:**
- Use existing toolbar infrastructure
- Follow pattern of existing toggle buttons (e.g., ID_PROTECT_TAGS)

---

### TASK-014: Implement View Switching Logic in DitaDoc
**Priority:** P0 (Blocking)
**Effort:** 5 hours
**Dependencies:** TASK-012, TASK-013

**Description:**
Implement actual view switching between Code and WYSIWYG modes.

**Methods to Implement:**
- `void DitaDoc::switchToCodeView()`
- `void DitaDoc::switchToWysiwygView()`
- `ViewMode DitaDoc::getCurrentView()`

**Switching Logic:**
1. Sync current view to model
2. Hide current view widget
3. Show target view widget
4. Render model in target view
5. Update button state

**Acceptance Criteria:**
- [ ] Switching from Code to WYSIWYG works
- [ ] Switching from WYSIWYG to Code works
- [ ] Content is preserved during switch
- [ ] Cursor position is maintained where possible
- [ ] No memory leaks or crashes
- [ ] Performance < 200ms (PR1 requirement)

**Test Cases:**
- Open topic in Code → switch to WYSIWYG → verify content
- Edit in WYSIWYG → switch to Code → verify XML
- Rapid switching (stress test)

---

### TASK-015: Integrate DitaWysiwygCtrl into Document Layout
**Priority:** P1
**Effort:** 4 hours
**Dependencies:** TASK-010, TASK-014

**Description:**
Integrate WYSIWYG control into the main document area using wxAUI or sizers.

**Integration Approach:**
- Option A: wxAUI notebook with Code/WYSIWYG tabs
- Option B: Stack views with show/hide logic

**Recommended:** Option B (simpler, better UX)

**Acceptance Criteria:**
- [ ] WYSIWYG control occupies same area as Code view
- [ ] Only one view visible at a time
- [ ] Resize behavior works correctly
- [ ] Scroll behavior works correctly
- [ ] Focus management works (can type in WYSIWYG)

**Files to Modify:**
- `src/ditadoc.cpp`
- Possibly `src/xmlcopyeditor.cpp` for layout

---

## Phase 3: Toolbar & Validation (Week 5)

**Goal:** Add formatting toolbar and prevent structural errors.

### TASK-016: Create DitaToolbar Class
**Priority:** P1
**Effort:** 3 hours
**Dependencies:** TASK-014

**Description:**
Create contextual toolbar for WYSIWYG formatting operations.

**Files to Create:**
- `src/ditatoolbar.h`
- `src/ditatoolbar.cpp`

**Toolbar Buttons:**
- Paragraph (transform to `<p>`)
- Unordered List (transform to `<ul>`)
- List Item (transform to `<li>`)
- Bold (wrap in `<b>`)
- Italic (wrap in `<i>`)

**Acceptance Criteria:**
- [ ] Toolbar class compiles and displays
- [ ] Buttons have appropriate icons or labels
- [ ] Toolbar can be shown/hidden programmatically
- [ ] Button click events are routed to handlers

**Implementation Notes:**
- Use wxToolBar
- Follow existing toolbar patterns in xmlcopyeditor.cpp
- Consider using stock wxWidgets icons

---

### TASK-017: Implement Element Transformation Logic
**Priority:** P0 (Blocking)
**Effort:** 5 hours
**Dependencies:** TASK-016

**Description:**
Implement logic to transform selected element to different types.

**Methods to Implement (DitaTopicModel):**
- `bool transformElement(const std::string& elementId, const std::string& newType)`

**Supported Transformations:**
- Paragraph → List Item (if parent is list)
- List Item → Paragraph (extract from list)
- Paragraph → Unordered List (convert to list with paragraph as first item)

**Acceptance Criteria:**
- [ ] Paragraph can be transformed to list item
- [ ] List item can be transformed to paragraph
- [ ] Invalid transformations return false
- [ ] Content is preserved during transformation
- [ ] DOM structure remains valid after transformation

**Test Cases:**
- Transform paragraph to list item
- Transform list item to paragraph
- Attempt invalid transformation
- Verify XML structure after transformation

---

### TASK-018: Wire Toolbar Buttons to Transformations
**Priority:** P1
**Effort:** 3 hours
**Dependencies:** TASK-017

**Description:**
Connect toolbar button clicks to element transformation logic.

**Flow:**
1. User selects text in WYSIWYG view
2. User clicks toolbar button (e.g., "List Item")
3. DitaWysiwygCtrl identifies current element
4. Calls DitaTopicModel::transformElement()
5. Re-renders updated content

**Acceptance Criteria:**
- [ ] Clicking "Paragraph" button transforms current element to `<p>`
- [ ] Clicking "List Item" button transforms to `<li>` (if valid)
- [ ] Invalid transformations are prevented
- [ ] UI updates immediately after transformation
- [ ] Undo/redo works (wxRichTextCtrl provides this)

**Implementation Notes:**
- Use wxRichTextCtrl selection APIs
- Maintain element ID mapping (position → element ID)

---

### TASK-019: Create DitaValidator Class
**Priority:** P1
**Effort:** 4 hours
**Dependencies:** TASK-017

**Description:**
Create DITA-specific validation logic to prevent structural errors.

**Files to Create:**
- `src/ditavalidator.h`
- `src/ditavalidator.cpp`

**Validation Rules (MVP):**
- `<li>` must be child of `<ul>` or `<ol>`
- `<title>` must be direct child of `<topic>`
- `<p>` can be child of `<topic>`, `<li>`, or sections
- No nested lists beyond 3 levels (UX guideline)

**Methods to Implement:**
- `bool canInsertElement(const std::string& parentId, const std::string& elementType)`
- `std::vector<std::string> getValidChildElements(const std::string& parentId)`

**Acceptance Criteria:**
- [ ] Returns false for invalid element placements
- [ ] Returns list of valid child elements for any parent
- [ ] Validation aligns with DITA DTD rules
- [ ] Performance is fast (< 10ms per check)

**Test Cases:**
- Check if `<li>` can be inserted under `<topic>` (false)
- Check if `<p>` can be inserted under `<li>` (true)
- Get valid children of `<topic>`

---

### TASK-020: Integrate Validation into WYSIWYG Operations
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-019

**Description:**
Prevent invalid operations in WYSIWYG view using DitaValidator.

**Integration Points:**
- Before element transformation
- Before element insertion
- Before delete operations (ensure required elements remain)

**User Feedback:**
- Disable toolbar buttons when operation is invalid
- Show tooltip explaining why button is disabled
- Optional: Show validation message for complex cases

**Acceptance Criteria:**
- [ ] Invalid transformations are prevented
- [ ] Toolbar buttons are disabled when transformation is invalid
- [ ] User receives clear feedback about why operation is blocked
- [ ] Valid operations work smoothly
- [ ] No structural violations can be created via WYSIWYG

**Test Cases:**
- Try to transform paragraph to list item (when not in list)
- Try to delete required title element
- Verify button states update based on selection

---

### TASK-021: Add Contextual Button Enabling/Disabling
**Priority:** P2
**Effort:** 2 hours
**Dependencies:** TASK-020

**Description:**
Update toolbar button states based on current selection and validation rules.

**Logic:**
- On selection change event
- Query current element from cursor position
- Ask DitaValidator what transformations are valid
- Enable/disable buttons accordingly

**Acceptance Criteria:**
- [ ] Buttons update when selection changes
- [ ] Only valid operations have enabled buttons
- [ ] Button states are accurate (no false positives/negatives)
- [ ] Performance is acceptable (updates feel instant)

**Implementation Notes:**
- Hook into wxRichTextCtrl selection changed event
- Cache validation results to avoid repeated checks

---

## Phase 4: Map Tree View (Week 6-7)

**Goal:** Implement Map View with drag-and-drop.

### TASK-022: Create DitaMapTreeCtrl Class
**Priority:** P0 (Blocking)
**Effort:** 3 hours
**Dependencies:** TASK-005

**Description:**
Create tree control for displaying DITAMAP structure.

**Files to Create:**
- `src/ditamaptreectrl.h`
- `src/ditamaptreectrl.cpp`

**Key Features:**
- Extend wxTreeCtrl
- Accept DitaMapModel pointer
- Basic tree item display

**Acceptance Criteria:**
- [ ] Class compiles and displays
- [ ] Can be instantiated with model
- [ ] Shows empty tree initially
- [ ] Standard tree operations work (expand/collapse)

**Implementation Notes:**
- Use wxTreeCtrl with custom data
- Set up tree image list (optional icons)

---

### TASK-023: Implement Tree Rendering from Model
**Priority:** P0 (Blocking)
**Effort:** 5 hours
**Dependencies:** TASK-022

**Description:**
Populate tree from DitaMapModel structure.

**Method to Implement:**
- `void DitaMapTreeCtrl::buildTreeFromModel()`

**Rendering Logic:**
- Recursively traverse topicref structure
- Create tree items with navtitle as label
- Show href as secondary text or tooltip
- Maintain mapping: wxTreeItemId → topicref ID

**Acceptance Criteria:**
- [ ] Flat DITAMAP renders as list of items
- [ ] Nested DITAMAP renders with proper hierarchy
- [ ] Each item displays navtitle and href
- [ ] Tree structure matches XML nesting
- [ ] Expand/collapse works for nested items

**Test Cases:**
- Render test_data/sample_map.ditamap
- Render deeply nested map (3+ levels)
- Visual inspection of tree structure

---

### TASK-024: Implement Drag-and-Drop Support
**Priority:** P1
**Effort:** 6 hours
**Dependencies:** TASK-023

**Description:**
Add drag-and-drop reordering of topicref elements.

**wxTreeCtrl Drag-and-Drop Events:**
- `EVT_TREE_BEGIN_DRAG` - Start drag operation
- `EVT_TREE_END_DRAG` - Complete drag operation

**Implementation Logic:**
1. On begin drag: store dragged item ID
2. During drag: highlight valid drop targets
3. On end drag: validate drop location
4. If valid: update model, refresh tree

**Acceptance Criteria:**
- [ ] Can drag tree items
- [ ] Drop target is visually indicated
- [ ] Invalid drop targets are prevented
- [ ] Successful drop updates model
- [ ] Tree refreshes to show new structure
- [ ] Drag-and-drop feels smooth (< 100ms response)

**Test Cases:**
- Reorder sibling topicrefs
- Move topicref to different parent
- Try invalid drop (verify prevention)

---

### TASK-025: Implement Model Update from Drag-and-Drop
**Priority:** P0 (Blocking)
**Effort:** 4 hours
**Dependencies:** TASK-024

**Description:**
Update DitaMapModel when tree items are reordered.

**Method to Implement (DitaMapModel):**
- `bool moveTopicRef(const std::string& id, const std::string& newParentId, int position)`

**Update Logic:**
- Remove topicref from current parent
- Insert into new parent at specified position
- Update DOM structure
- Maintain all attributes and child elements

**Acceptance Criteria:**
- [ ] Model updates correctly after drag-and-drop
- [ ] Serialized XML reflects new structure
- [ ] Attributes and nested topicrefs are preserved
- [ ] Invalid moves return false
- [ ] No DOM corruption

**Test Cases:**
- Move topicref within parent (reorder)
- Move topicref to different parent
- Verify XML structure after move

---

### TASK-026: Add Map View Toggle in UI
**Priority:** P1
**Effort:** 3 hours
**Dependencies:** TASK-023

**Description:**
Add UI controls to switch to Map View for DITAMAP files.

**Integration Approach:**
- Add "Map View" toggle button to toolbar
- Enable only when DITAMAP file is open
- Use similar pattern to WYSIWYG toggle

**Acceptance Criteria:**
- [ ] Toggle button appears in toolbar
- [ ] Enabled only for DITAMAP files
- [ ] Clicking switches to Map View
- [ ] Button state reflects current view

**Files to Modify:**
- `src/xmlcopyeditor.h/cpp`
- `src/ditadoc.cpp`

---

### TASK-027: Implement Map View Switching Logic
**Priority:** P0 (Blocking)
**Effort:** 4 hours
**Dependencies:** TASK-026

**Description:**
Implement view switching for DITAMAP files (Code ↔ Map).

**Methods to Implement (DitaDoc):**
- `void switchToMapView()`
- Update `switchToCodeView()` to support maps

**Switching Logic:**
- Sync Code view to model
- Build/refresh tree from model
- Show tree control, hide code editor
- Update button states

**Acceptance Criteria:**
- [ ] Can switch from Code to Map View
- [ ] Can switch from Map to Code View
- [ ] Content is preserved during switch
- [ ] Performance < 200ms
- [ ] Tree reflects latest XML changes

**Test Cases:**
- Open DITAMAP → switch to Map View
- Reorder in Map View → switch to Code → verify XML
- Edit XML in Code → switch to Map → verify tree

---

### TASK-028: Add Context Menu for Map Operations
**Priority:** P2
**Effort:** 3 hours
**Dependencies:** TASK-023

**Description:**
Add right-click context menu for common map operations.

**Context Menu Items:**
- Add Child Topicref
- Add Sibling Topicref
- Edit Topicref (href, navtitle)
- Remove Topicref

**Acceptance Criteria:**
- [ ] Right-click shows context menu
- [ ] Menu items are contextual (valid for selected item)
- [ ] "Add Child" only on items that can have children
- [ ] Operations update model and refresh tree

**Implementation Notes:**
- Use wxMenu and EVT_TREE_ITEM_MENU event
- Show dialogs for edit operations (wxTextEntryDialog)

---

## Phase 5: Polish & Integration (Week 8)

**Goal:** Performance optimization and final integration.

### TASK-029: Performance Profiling and Optimization
**Priority:** P1
**Effort:** 5 hours
**Dependencies:** TASK-014, TASK-027

**Description:**
Profile view switching and rendering performance, optimize as needed.

**Profiling Tools:**
- gprof (GNU profiler)
- valgrind --tool=callgrind
- Custom timing instrumentation

**Performance Targets:**
- View switching: < 200ms (PR1)
- Large document (1000 elements): < 1s rendering
- Drag-and-drop: < 100ms response

**Optimization Areas:**
- Model parsing (lazy loading)
- Tree building (virtual tree for large maps)
- Rendering (cache formatted content)
- Memory allocation (object pooling)

**Acceptance Criteria:**
- [ ] All performance targets met
- [ ] Profiling data collected and analyzed
- [ ] Identified bottlenecks optimized
- [ ] Performance tests automated

**Test Cases:**
- Large DITA Topic (1000 paragraphs)
- Large DITAMAP (500 topicrefs)
- Rapid view switching (10 times)

---

### TASK-030: Memory Leak Testing and Fixes
**Priority:** P0 (Blocking)
**Effort:** 4 hours
**Dependencies:** All implementation tasks

**Description:**
Test for memory leaks and fix any found.

**Testing Tools:**
- valgrind --leak-check=full
- AddressSanitizer (clang/gcc)

**Test Scenarios:**
- Open/close DITA files repeatedly
- Switch views repeatedly
- Drag-and-drop operations
- Long-running editing session

**Acceptance Criteria:**
- [ ] Zero memory leaks reported by valgrind
- [ ] No use-after-free errors
- [ ] Memory usage is stable over time
- [ ] Proper cleanup in destructors

**Common Leak Sources:**
- libxml2 docs not freed
- wxWidgets controls not destroyed
- Event handlers not disconnected

---

### TASK-031: Spell Check Integration Testing
**Priority:** P2
**Effort:** 3 hours
**Dependencies:** TASK-012

**Description:**
Verify spell checking works in WYSIWYG mode.

**Integration Points:**
- wxRichTextCtrl built-in spell check (if available)
- Or hook into existing wrapaspell infrastructure

**Acceptance Criteria:**
- [ ] Spell check underlines misspelled words in WYSIWYG
- [ ] Right-click shows spell check suggestions
- [ ] Spell check language setting is respected
- [ ] Performance is acceptable (no lag during typing)

**Test Cases:**
- Type misspelled word, verify underline
- Right-click, verify suggestions
- Add word to dictionary

**Note:** May require minimal implementation if wxRichTextCtrl provides built-in support.

---

### TASK-032: User Documentation Updates
**Priority:** P2
**Effort:** 4 hours
**Dependencies:** All user-facing features

**Description:**
Update user documentation to cover DITA WYSIWYG features.

**Documentation Files:**
- `src/help/html/dita_editing.html` (new)
- Update `src/help/html/features.html`
- Update `src/help/Table_of_Contents.hhc`

**Content to Document:**
- How to enable WYSIWYG view
- Using the DITA toolbar
- Map View usage
- Drag-and-drop in maps
- Known limitations

**Acceptance Criteria:**
- [ ] New DITA help page created
- [ ] Screenshots included
- [ ] Help menu links to new page
- [ ] Content is clear and accurate

**Format:** HTML matching existing help style

---

### TASK-033: Regression Testing
**Priority:** P0 (Blocking)
**Effort:** 5 hours
**Dependencies:** All tasks

**Description:**
Comprehensive testing to ensure no regressions in existing XML Copy Editor features.

**Test Coverage:**
- Opening/editing non-DITA XML files
- Validation features
- Find/replace
- Spell checking
- Printing
- All existing menu items and toolbar buttons

**Acceptance Criteria:**
- [ ] All existing features work as before
- [ ] No crashes or errors
- [ ] UI layout not broken
- [ ] Performance not degraded

**Test Approach:**
- Manual testing of key workflows
- Automated regression test suite (if exists)
- Test on fresh Fedora Linux install

---

### TASK-034: Final Bug Fixes and Release Preparation
**Priority:** P0 (Blocking)
**Effort:** 8 hours
**Dependencies:** TASK-029 through TASK-033

**Description:**
Address any remaining bugs and prepare for release/merge.

**Activities:**
- Fix all P0/P1 bugs found during testing
- Code review and cleanup
- Update ChangeLog
- Create release notes
- Tag feature branch

**Acceptance Criteria:**
- [ ] Zero P0 bugs remaining
- [ ] All P1 bugs fixed or documented
- [ ] Code passes review
- [ ] ChangeLog updated
- [ ] Ready to merge to main branch

**Release Notes Content:**
- Features added
- Known limitations
- Upgrade notes (if any)

---

## Task Dependencies Diagram

```
Phase 1: Foundation
001 → 002 → 003 → 004
          ↓
001 → 005
001,005,006 → 007
004,005 → 008

Phase 2: WYSIWYG View
004 → 009 → 010 → 011 → 012
009 → 013
012,013 → 014
010,014 → 015

Phase 3: Toolbar & Validation
014 → 016 → 017 → 018
017 → 019 → 020 → 021

Phase 4: Map Tree View
005 → 022 → 023 → 024 → 025
023 → 026 → 027
023 → 028

Phase 5: Polish
014,027 → 029
All → 030, 031, 032, 033
029-033 → 034
```

---

## Priority Legend

- **P0 (Blocking):** Must complete for feature to work
- **P1 (High):** Critical for MVP but has workarounds
- **P2 (Medium):** Important but can be deferred
- **P3 (Low):** Nice to have

---

## Effort Estimates

- **2 hours:** Small, focused task
- **3-4 hours:** Moderate task, single component
- **5-6 hours:** Large task, multiple components or complex logic
- **8+ hours:** Very large task, may need breakdown

**Total Estimated Effort:** ~140 hours = 3.5 weeks at 40 hrs/week

**Scheduled Timeline:** 8 weeks allows for:
- Learning curve
- Testing and iteration
- Unexpected issues
- Code review and polish

---

## Next Steps

1. **Review task breakdown** with team
2. **Begin Phase 1: Foundation** (TASK-001 through TASK-008)
3. **Set up development environment** on Fedora Linux
4. **Create feature branch checkpoint** before coding

**Ready to start implementation!**

Use `/implement` to begin guided implementation of tasks.
