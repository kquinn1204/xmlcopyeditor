# XML Copy Editor

Fast, free, validating XML editor with enhanced DITA support.

## Overview

XML Copy Editor is a fast, free, validating XML editor for Linux, Windows, and Mac. It features syntax highlighting, tag completion, validation against DTDs and XML Schemas, XSLT transformation, and spell checking.

### Enhanced DITA Features

This fork includes significant enhancements for DITA (Darwin Information Typing Architecture) document editing:

- **WYSIWYG Preview Mode** - Toggle between code view and formatted preview for DITA topics
- **Complete DITA Task Support** - Proper rendering of task-specific elements including:
  - Steps and substeps with hierarchical numbering
  - Command elements (cmd)
  - Step examples (stepxmp)
  - Code blocks with proper formatting and indentation
  - Context, info, and other DITA specialized elements
- **DITA Type Auto-Detection** - Automatically detects DITA topic types (task, concept, reference, troubleshooting, glossentry)
- **DITA Map Tree View** - Interactive tree-based navigation and editing of DITA maps with:
  - Visual hierarchy display of topicref elements
  - Drag-and-drop reorganization of map structure
  - Double-click to open referenced topics
  - Automatic cycle prevention to maintain valid map structure
  - Seamless switching between tree view and code view

## Features

- Fast editing of large XML documents
- Syntax highlighting with configurable color schemes
- Tag and attribute auto-completion
- Well-formedness and DTD/XML Schema validation
- XSLT 1.0 transformation and XPath evaluation
- Pretty-print and formatting
- Tag locking to prevent accidental changes
- Spell checking (requires Aspell)
- Built-in templates for common document types
- Cross-platform support (Linux, Windows, macOS)

## Installation

### Prerequisites

Before building XML Copy Editor, ensure you have the following dependencies installed:

#### Core Dependencies
- **wxWidgets** 2.8.0 or higher (with Unicode support)
- **libxml2** and **libxml2-dev**
- **libxslt** and **libxslt-dev**
- **Xerces-C** and **xerces-c-devel**
- **Boost** libraries and headers (specifically boost/shared_ptr.hpp)
- **PCRE2** (libpcre2-8 and pcre2-devel)
- **Expat** and **expat-devel**
- **Aspell** and **aspell-devel** (for spell checking)

#### Build Tools
- **autoconf** and **automake**
- **libtool**
- **pkg-config**
- **intltool** (version 0.40.0 or higher)
- **g++** or compatible C++ compiler with C++11 support

### Installing Dependencies

#### Fedora/RHEL
```bash
sudo dnf install wxGTK3-devel libxml2-devel libxslt-devel xerces-c-devel \
                 boost-devel pcre2-devel expat-devel aspell-devel \
                 autoconf automake libtool pkg-config intltool
```

#### Ubuntu/Debian
```bash
sudo apt-get install libwxgtk3.0-gtk3-dev libxml2-dev libxslt1-dev \
                     libxerces-c-dev libboost-dev libpcre2-dev \
                     libexpat1-dev libaspell-dev autoconf automake \
                     libtool pkg-config intltool
```

#### macOS (using Homebrew)
```bash
brew install wxwidgets libxml2 libxslt xerces-c boost pcre2 aspell
```

### Building from Source

1. **Clone or extract the source code**
   ```bash
   git clone https://github.com/kquinn1204/xmlcopyeditor.git
   cd xmlcopyeditor/xmlcopyeditor-1.3.1.0
   ```

2. **Generate the configure script** (if building from git)
   ```bash
   autoreconf -i
   ```

3. **Configure the build**
   ```bash
   ./configure
   ```

   Common configuration options:
   - `--enable-debug` - Enable debug build with symbols
   - `--prefix=/custom/path` - Install to custom location (default: /usr/local)
   - `--with-wx-config=/path/to/wx-config` - Specify custom wxWidgets configuration

4. **Compile the application**
   ```bash
   make -j$(nproc)
   ```

5. **Install** (optional)
   ```bash
   sudo make install
   ```

6. **Run without installing**
   ```bash
   ./src/xmlcopyeditor
   ```

### Post-Installation

At first startup, the application locates its data files in:
1. `/usr/local/share/xmlcopyeditor` (default installation)
2. `/usr/share/xmlcopyeditor` (alternative)

If icons or the About page are missing, the application couldn't find the data directory. You can:
- Correct it in **Tools > Options > General Pane**
- Manually edit `~/.xmlcopyeditor` configuration file

## Usage

### Basic XML Editing

```bash
# Open a file
xmlcopyeditor document.xml

# Validate against DTD
# Use Tools > Validate

# Apply XSLT transformation
# Use XML > XSLT
```

### DITA Editing

1. **Open a DITA file** (topic or map)
   - The editor automatically detects DITA document type

2. **Toggle WYSIWYG Preview** (for DITA topics)
   - Click the "Preview" button in the toolbar (magnifying glass icon)
   - Or use the View menu
   - Available for all DITA topic types (task, concept, reference, etc.)

3. **Work with DITA Maps** (for .ditamap files)
   - **Open a DITA map** - The editor loads the map structure
   - **Toggle Map Tree View** - Click the "Map View" button in the toolbar (list icon)
   - **Navigate the hierarchy** - Expand/collapse topics to see the map structure
   - **Open referenced topics** - Double-click any topic in the tree to open it in a new tab
   - **Reorganize map structure** - Drag topics and drop them on other topics to reorganize
   - **Switch views** - Toggle between Tree View and Code View to see/edit XML directly
   - **Invalid moves prevented** - The editor blocks operations that would create circular references

### Keyboard Shortcuts

- **Ctrl+F** - Find
- **Ctrl+H** - Find and Replace
- **Ctrl+G** - Go to Line
- **Ctrl+T** - Validate
- **F5** - Reload document

## Development

### Project Structure

```
xmlcopyeditor-1.3.1.0/
├── src/                    # C++ source files
│   ├── xmlcopyeditor.cpp   # Main application
│   ├── xmldoc.cpp          # XML document handling
│   ├── ditadoc.cpp         # DITA document handling
│   ├── ditatopicmodel.cpp  # DITA topic model
│   ├── ditamapmodel.cpp    # DITA map model
│   ├── ditamaptreectrl.cpp # DITA map tree view control
│   ├── ditawysiwygctrl.cpp # WYSIWYG rendering control
│   ├── ditadetector.cpp    # DITA type detection
│   └── ...
├── catalog/                # XML catalogs
├── templates/              # Document templates
├── tests/                  # Test files and programs
├── po/                     # Internationalization files
└── pixmaps/                # Application icons
```

### Building for Development

Enable debug mode for development:

```bash
./configure --enable-debug
make -j$(nproc)
./src/xmlcopyeditor
```

Debug builds include:
- GDB debugging symbols (`-ggdb`)
- No optimization (`-O0`)
- Additional debug output

### Running Tests

After making changes, rebuild and test:

```bash
make clean
make -j$(nproc)
./src/xmlcopyeditor /path/to/test/file.xml
```

For DITA features, test with sample DITA files:
- Task topics (.dita with `<task>` root)
- Concept topics (.dita with `<concept>` root)
- DITA maps (.ditamap)

#### Testing Map View Functionality

A comprehensive test map is included:

```bash
# Open the test map
./src/xmlcopyeditor test_map_view.ditamap

# Test the features:
# 1. Click "Map View" toolbar button to see tree structure
# 2. Expand/collapse topics to navigate hierarchy
# 3. Double-click topics to open referenced files
# 4. Drag topics and drop on other topics to reorganize
# 5. Toggle back to Code View to see XML changes
```

Sample topics are provided in `sample_topics/` directory for testing file opening functionality.

#### Backend Tests

Compile and run the standalone map model test:

```bash
cd tests
g++ -o test-map-simple test-map-simple.cpp \
    ../src/ditamapmodel.cpp ../src/ditadetector.cpp \
    $(pkg-config --cflags --libs libxml-2.0) -std=c++11
./test-map-simple
```

## Contributing

Contributions are welcome! When submitting changes:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/enhanced-xml-editing`)
3. Make your changes
4. Test thoroughly
5. Commit with descriptive messages
6. Push to your fork
7. Open a Pull Request

### Commit Message Format

Follow the pattern:
```
TASK-XXX: Brief description

Detailed explanation of changes and rationale.
```

## Troubleshooting

### Configure fails with "wxWidgets is required"

Ensure wxWidgets is installed and `wx-config` is in your PATH:
```bash
wx-config --version
```

If wx-config is in a non-standard location:
```bash
./configure --with-wx-config=/path/to/wx-config
```

### Missing WYSIWYG Preview or Map View buttons

The view toggle buttons have specific requirements:

**Preview button** (for DITA topics):
- Requires DITA topic detected (task, concept, reference, etc.)
- Requires wxWidgets with richtext support
- Not available for DITA maps

**Map View button** (for DITA maps):
- Requires DITA map detected (.ditamap file)
- Uses wxTreeCtrl for display
- Not available for DITA topics

Verify wxWidgets was compiled with required components:
```bash
wx-config --libs | grep richtext
wx-config --libs | grep core
```

### Application can't find data files

Check or set the application directory:
1. Open **Tools > Options**
2. Go to **General** tab
3. Set **Application Directory** to the installation path

Or edit `~/.xmlcopyeditor`:
```ini
applicationDir=/usr/local/share/xmlcopyeditor
```

### Build errors with boost headers

Ensure boost development headers are installed:
```bash
# Fedora/RHEL
sudo dnf install boost-devel

# Ubuntu/Debian
sudo apt-get install libboost-dev
```

## License

XML Copy Editor is free software; you can redistribute it and/or modify it under the terms of the **GNU General Public License** as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

See the LICENSE file for details.

## Credits

- **Original Author**: Gerald Schmidt
- **Contributors**: 2025 Contributors (DITA enhancements and features)

## Contact

For questions, issues, or feature requests:
- Open an issue on GitHub: https://github.com/kquinn1204/xmlcopyeditor/issues
- Original contact: gnschmidt@users.sourceforge.net

## Version

Current version: **1.3.1.0** (with enhanced DITA support)
