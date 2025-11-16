# DITA Map View - Testing Guide

## Phase 4 Implementation Complete ✅

All Map View functionality has been implemented and backend tests pass successfully.

## What Was Tested

### Backend Tests (All Passing ✓)
- **Map Loading**: 1,489 byte DITA map with 18 topicrefs
- **Structure Parsing**: 3 levels of nesting, all preserved
- **XML Serialization**: Round-trip with no data loss
- **Drag-and-Drop**: Successfully moved topicrefs
- **Cycle Prevention**: Correctly blocks invalid moves
- **Output Validation**: Modified XML is well-formed

## GUI Testing Instructions

### 1. Build and Run the Application

```bash
cd /home/kquinn/xml-copy-editor/xmlcopyeditor-1.3.1.0

# If not already built
make -j4

# Run the application
./src/xmlcopyeditor
```

### 2. Open a DITA Map File

Use one of the test files:

- **Simple Map**: `tests/test_data/sample_map.ditamap`
- **Complex Map**: `test_map_view.ditamap` (created for testing)

**Steps:**
1. File → Open
2. Navigate to test file
3. Open the `.ditamap` file

### 3. Verify Map Detection

When a DITA Map is opened:
- The **Map View** button in the toolbar should become **enabled**
- It appears next to the "Preview" (WYSIWYG) button
- Icon: Tree/List view icon

### 4. Test Map View Toggle

**Click the Map View button** to switch views:

**Expected behavior:**
- Code view hides
- Tree control appears showing map structure
- Tree displays:
  - Root: "DITA Map"
  - Children: All `<topicref>` elements
  - Each item shows: `navtitle (href)`
  - Nested topicrefs are indented

### 5. Test Drag-and-Drop

**To reorganize topicrefs:**

1. Click and hold on a topicref item
2. Drag to a new position
3. Release mouse button

**Valid moves:**
- Reorder siblings
- Move to different parent
- Move to root level

**Invalid moves (will show error):**
- Move parent into its own descendant
- Drop on root item

### 6. Verify Changes in Code View

After moving items:

1. Click **Map View** button again to return to Code view
2. Verify the XML structure has been updated
3. The moved `<topicref>` should be in its new position
4. All children and attributes preserved

### 7. Test View Switching

**Workflow:**
1. Open map in Code view
2. Switch to Map view → see tree
3. Make changes via drag-and-drop
4. Switch to Code view → see updated XML
5. Switch back to Map view → tree reflects XML changes

## Expected Visual Layout

```
┌─────────────────────────────────────────────────────┐
│ File  Edit  View  XML  Tools  Help        [Toolbar] │
├─────────────────────────────────────────────────────┤
│  ⚙ 💾 ✂ 📋 ↩ ↪  |  🔍 🌲 ← Map View & Preview    │
├─────────────────────────────────────────────────────┤
│ CODE VIEW                │ MAP VIEW                 │
│ (Shown by default)       │ (Click Map View button)  │
│                          │                          │
│ <?xml version="1.0"?>    │ ▼ DITA Map              │
│ <map>                    │   ▼ Getting Started     │
│   <topicref ...>         │     • Installation      │
│   ...                    │     • Quick Start       │
│ </map>                   │   ▼ Core Concepts       │
│                          │     • Architecture      │
│                          │     ▼ Data Model        │
│                          │       • Entities        │
│                          │       • Relationships   │
└──────────────────────────┴──────────────────────────┘
```

## Sample Test Scenarios

### Scenario 1: Simple Reorganization

1. Open `test_map_view.ditamap`
2. Switch to Map View
3. Drag "Troubleshooting" before "FAQ"
4. Switch to Code view
5. Verify `<topicref href="troubleshooting.dita"/>` moved

### Scenario 2: Nested Structure Change

1. Open map with nested topicrefs
2. Drag a top-level item into another section
3. Verify it becomes a child of the drop target
4. Check XML shows proper nesting

### Scenario 3: Cycle Prevention

1. Try to drag a parent item onto its own child
2. Should see error: "Cannot move topicref to this location"
3. Move is blocked, tree unchanged

## Features Implemented

✅ **DitaMapTreeCtrl** - Tree control for map display  
✅ **Drag-and-drop** - Interactive reorganization  
✅ **Move validation** - Prevents cycles and invalid moves  
✅ **View switching** - Seamless Code ↔ Map transitions  
✅ **Content preservation** - All attributes and nesting maintained  
✅ **Toolbar integration** - Map View button with proper enable/disable  
✅ **Model synchronization** - XML stays in sync with tree

## Known Limitations (By Design)

- **Map View is for DITA Maps only** - Button disabled for topic files
- **Read-write mode** - Changes made via drag-and-drop only
- **Context menu not implemented** - Add/Edit/Remove operations planned for future

## Troubleshooting

### Map View button is disabled
- Verify the file is a DITA Map (root element `<map>`)
- Check file extension is `.ditamap`
- Try reopening the file

### Tree doesn't appear
- Check console for errors
- Verify DitaMapModel loaded successfully
- Try a known-good test file

### Drag-and-drop doesn't work
- Ensure you're clicking on a topicref (not root)
- Verify target is not a descendant of dragged item
- Check for error messages

## Test Files Included

1. **test_map_view.ditamap** - Comprehensive test with 3 levels of nesting
2. **tests/test_data/sample_map.ditamap** - Simple 2-level structure
3. **tests/test_output.ditamap** - Example of post-move structure

## Backend Test Program

A standalone test validates all model operations:

```bash
cd tests
./test-map-simple ../test_map_view.ditamap
```

This runs automated tests without GUI dependencies.

---

**Status**: Phase 4 Complete  
**Next**: Phase 5 (Polish & Integration) or manual GUI testing
