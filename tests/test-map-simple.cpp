/*
 * Simple test program for DITA Map Model functionality
 * Tests DitaMapModel operations without GUI dependencies
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "../src/ditamapmodel.h"

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

void printTopicRefTree(DitaMapModel& model, const std::string& parentId, int indent = 0) {
    std::vector<TopicRef> children;

    if (parentId.empty()) {
        children = model.getTopicRefs();
    } else {
        children = model.getChildren(parentId);
    }

    for (size_t i = 0; i < children.size(); ++i) {
        const TopicRef& ref = children[i];

        // Print indentation
        for (int j = 0; j < indent; ++j) {
            std::cout << "  ";
        }

        std::cout << "- " << ref.navtitle;
        if (!ref.href.empty()) {
            std::cout << " (" << ref.href << ")";
        }
        std::cout << " [ID: " << ref.id << "]" << std::endl;

        // Recursively print children
        if (!ref.childIds.empty()) {
            printTopicRefTree(model, ref.id, indent + 1);
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << " DITA Map Model Test" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Determine which map file to use
    std::string mapFile;
    if (argc > 1) {
        mapFile = argv[1];
    } else {
        mapFile = "../test_map_view.ditamap";
    }

    std::cout << "\n📄 Using map file: " << mapFile << std::endl;

    // Read map file
    std::string mapContent = readFile(mapFile);
    if (mapContent.empty()) {
        std::cerr << "❌ Error: Could not read map file" << std::endl;
        return 1;
    }

    std::cout << "   Size: " << mapContent.length() << " bytes" << std::endl;

    // Test 1: Load map
    std::cout << "\n[TEST 1] Loading DITA map..." << std::endl;
    DitaMapModel model;
    if (!model.loadFromXml(mapContent)) {
        std::cout << "❌ Failed to load map" << std::endl;
        return 1;
    }
    std::cout << "✓ Map loaded successfully" << std::endl;

    // Test 2: Display structure
    std::cout << "\n[TEST 2] Map structure:" << std::endl;
    printTopicRefTree(model, "");

    // Test 3: Serialize
    std::cout << "\n[TEST 3] Serializing map..." << std::endl;
    std::string serialized = model.serializeToXml();
    if (serialized.empty()) {
        std::cout << "❌ Serialization failed" << std::endl;
        return 1;
    }
    std::cout << "✓ Serialized (" << serialized.length() << " bytes)" << std::endl;

    // Test 4: Round-trip
    std::cout << "\n[TEST 4] Testing round-trip..." << std::endl;
    DitaMapModel model2;
    if (!model2.loadFromXml(serialized)) {
        std::cout << "❌ Failed to reload serialized map" << std::endl;
        return 1;
    }
    std::cout << "✓ Round-trip successful" << std::endl;

    // Test 5: Move operation (simulate drag-and-drop)
    std::cout << "\n[TEST 5] Testing move operation..." << std::endl;
    std::vector<TopicRef> refs = model.getTopicRefs();

    if (refs.size() < 2) {
        std::cout << "⚠ Not enough topicrefs to test move" << std::endl;
    } else {
        std::string firstId = refs[0].id;
        std::string firstNavtitle = refs[0].navtitle;

        std::cout << "   Moving '" << firstNavtitle << "' to position 2..." << std::endl;

        // Check if move is valid
        if (!model.canMoveTopicRef(firstId, "")) {
            std::cout << "❌ canMoveTopicRef returned false" << std::endl;
            return 1;
        }

        // Perform move
        if (!model.moveTopicRef(firstId, "", 2)) {
            std::cout << "❌ moveTopicRef failed" << std::endl;
            return 1;
        }

        std::cout << "✓ Move successful" << std::endl;
        std::cout << "   New structure:" << std::endl;
        printTopicRefTree(model, "");
    }

    // Test 6: Cycle prevention
    std::cout << "\n[TEST 6] Testing cycle prevention..." << std::endl;
    refs = model.getTopicRefs();

    // Find a topicref with children
    bool foundParentWithChild = false;
    for (size_t i = 0; i < refs.size(); ++i) {
        if (!refs[i].childIds.empty()) {
            std::string parentId = refs[i].id;
            std::string childId = refs[i].childIds[0];

            std::cout << "   Attempting to move parent into its own child..." << std::endl;

            if (model.canMoveTopicRef(parentId, childId)) {
                std::cout << "❌ Cycle prevention failed - should not allow this move" << std::endl;
                return 1;
            }

            std::cout << "✓ Cycle prevention working correctly" << std::endl;
            foundParentWithChild = true;
            break;
        }
    }

    if (!foundParentWithChild) {
        std::cout << "⚠ No nested topicrefs found to test cycle prevention" << std::endl;
    }

    // Test 7: Save modified map
    std::cout << "\n[TEST 7] Saving modified map..." << std::endl;
    std::string finalXml = model.serializeToXml();

    std::ofstream outFile("test_output.ditamap");
    if (outFile.is_open()) {
        outFile << finalXml;
        outFile.close();
        std::cout << "✓ Modified map saved to test_output.ditamap" << std::endl;
    } else {
        std::cout << "⚠ Could not save output file" << std::endl;
    }

    // Summary
    std::cout << "\n==========================================" << std::endl;
    std::cout << "✅ All tests passed!" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
