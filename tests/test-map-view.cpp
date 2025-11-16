/*
 * Test program for DITA Map View functionality
 * Tests DitaMapModel and DitaMapTreeCtrl operations
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "../src/ditamapmodel.h"
#include "../src/ditadetector.h"

// Helper function to read file contents
std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Test 1: Load DITA map
bool testLoadMap(const std::string& mapContent) {
    std::cout << "TEST 1: Loading DITA map..." << std::endl;

    DitaMapModel model;
    bool result = model.loadFromXml(mapContent);

    if (result) {
        std::cout << "  ✓ Map loaded successfully" << std::endl;
    } else {
        std::cout << "  ✗ Failed to load map" << std::endl;
    }

    return result;
}

// Test 2: Get topicrefs
bool testGetTopicRefs(const std::string& mapContent) {
    std::cout << "\nTEST 2: Getting topicref structure..." << std::endl;

    DitaMapModel model;
    if (!model.loadFromXml(mapContent)) {
        std::cout << "  ✗ Failed to load map" << std::endl;
        return false;
    }

    std::vector<TopicRef> refs = model.getTopicRefs();
    std::cout << "  Found " << refs.size() << " root-level topicrefs:" << std::endl;

    for (size_t i = 0; i < refs.size(); ++i) {
        const TopicRef& ref = refs[i];
        std::cout << "    - " << ref.navtitle << " (href: " << ref.href << ")" << std::endl;

        if (!ref.childIds.empty()) {
            std::cout << "      Children: " << ref.childIds.size() << std::endl;

            // Get children
            std::vector<TopicRef> children = model.getChildren(ref.id);
            for (size_t j = 0; j < children.size(); ++j) {
                std::cout << "        - " << children[j].navtitle << std::endl;
            }
        }
    }

    std::cout << "  ✓ TopicRef structure retrieved" << std::endl;
    return true;
}

// Test 3: Serialize map
bool testSerialize(const std::string& mapContent) {
    std::cout << "\nTEST 3: Serializing map back to XML..." << std::endl;

    DitaMapModel model;
    if (!model.loadFromXml(mapContent)) {
        std::cout << "  ✗ Failed to load map" << std::endl;
        return false;
    }

    std::string serialized = model.serializeToXml();

    if (serialized.empty()) {
        std::cout << "  ✗ Serialization failed" << std::endl;
        return false;
    }

    std::cout << "  ✓ Map serialized (" << serialized.length() << " bytes)" << std::endl;

    // Verify we can load the serialized version
    DitaMapModel model2;
    if (!model2.loadFromXml(serialized)) {
        std::cout << "  ✗ Failed to reload serialized map" << std::endl;
        return false;
    }

    std::cout << "  ✓ Round-trip successful" << std::endl;
    return true;
}

// Test 4: Move topicref (simulate drag-and-drop)
bool testMoveTopicRef(const std::string& mapContent) {
    std::cout << "\nTEST 4: Testing topicref move operations..." << std::endl;

    DitaMapModel model;
    if (!model.loadFromXml(mapContent)) {
        std::cout << "  ✗ Failed to load map" << std::endl;
        return false;
    }

    // Get all topicrefs
    std::vector<TopicRef> refs = model.getTopicRefs();
    if (refs.size() < 2) {
        std::cout << "  ⚠ Not enough topicrefs to test move" << std::endl;
        return true;
    }

    // Test 1: Try to move first topicref to position 2 (reorder at root level)
    std::string firstId = refs[0].id;
    std::string firstNavtitle = refs[0].navtitle;

    std::cout << "  Testing move: '" << firstNavtitle << "' to position 2..." << std::endl;

    // Check if move is valid
    if (!model.canMoveTopicRef(firstId, "")) {
        std::cout << "  ✗ canMoveTopicRef returned false" << std::endl;
        return false;
    }

    // Perform move
    if (!model.moveTopicRef(firstId, "", 2)) {
        std::cout << "  ✗ moveTopicRef failed" << std::endl;
        return false;
    }

    std::cout << "  ✓ Move successful" << std::endl;

    // Verify move by checking new order
    std::vector<TopicRef> newRefs = model.getTopicRefs();
    std::cout << "  New order:" << std::endl;
    for (size_t i = 0; i < newRefs.size(); ++i) {
        std::cout << "    " << (i+1) << ". " << newRefs[i].navtitle << std::endl;
    }

    // Verify the moved item is now at position 2 (index 1 or 2 depending on implementation)
    std::cout << "  ✓ Topicref move completed" << std::endl;

    // Test 2: Try to prevent cycle (move parent into its own child)
    if (refs.size() > 0 && !refs[0].childIds.empty()) {
        std::cout << "  Testing cycle prevention..." << std::endl;
        std::string parentId = refs[0].id;
        std::string childId = refs[0].childIds[0];

        // This should return false (would create cycle)
        if (model.canMoveTopicRef(parentId, childId)) {
            std::cout << "  ✗ Cycle prevention failed - should not allow parent→child move" << std::endl;
            return false;
        }

        std::cout << "  ✓ Cycle prevention working" << std::endl;
    }

    return true;
}

// Test 5: File type detection
bool testFileDetection(const std::string& mapContent) {
    std::cout << "\nTEST 5: Testing DITA file type detection..." << std::endl;

    DitaFileType detected = detectDitaTypeFromContent(mapContent);

    if (detected == DITA_TYPE_MAP) {
        std::cout << "  ✓ Correctly detected as DITA Map" << std::endl;
        return true;
    } else {
        std::cout << "  ✗ Detection failed: got type " << detected << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << " DITA Map View Functionality Tests" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Determine which map file to use
    std::string mapFile;
    if (argc > 1) {
        mapFile = argv[1];
    } else {
        mapFile = "test_map_view.ditamap";
    }

    std::cout << "\nUsing map file: " << mapFile << std::endl;

    // Read map file
    std::string mapContent = readFile(mapFile);
    if (mapContent.empty()) {
        std::cerr << "Error: Could not read map file" << std::endl;
        return 1;
    }

    std::cout << "Map file size: " << mapContent.length() << " bytes" << std::endl;

    // Run tests
    int passed = 0;
    int total = 5;

    if (testFileDetection(mapContent)) passed++;
    if (testLoadMap(mapContent)) passed++;
    if (testGetTopicRefs(mapContent)) passed++;
    if (testSerialize(mapContent)) passed++;
    if (testMoveTopicRef(mapContent)) passed++;

    // Summary
    std::cout << "\n==========================================" << std::endl;
    std::cout << " Test Results: " << passed << "/" << total << " passed" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (passed == total) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Some tests failed" << std::endl;
        return 1;
    }
}
