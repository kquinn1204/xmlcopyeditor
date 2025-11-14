/*
 * Standalone test program for DITA models
 * Compiles and runs independently of Catch framework
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include "../src/ditatopicmodel.h"
#include "../src/ditamapmodel.h"
#include "../src/ditadetector.h"

// Test counter
int tests_passed = 0;
int tests_failed = 0;

// Test macros
#define TEST_ASSERT(condition, message) \
    if (condition) { \
        tests_passed++; \
        std::cout << "  ✓ " << message << std::endl; \
    } else { \
        tests_failed++; \
        std::cerr << "  ✗ FAILED: " << message << std::endl; \
    }

#define TEST_SECTION(name) \
    std::cout << "\n" << name << ":" << std::endl;

// Helper function to read file contents
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void testDitaTopicModel() {
    TEST_SECTION("DitaTopicModel Tests");

    // Test 1: Load valid topic XML
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaTopicModel model;
        bool loaded = model.loadFromXml(xml);
        TEST_ASSERT(loaded, "Loads valid topic XML");
        TEST_ASSERT(model.isValidTopic(), "Validates topic structure");
    }

    // Test 2: Extract title
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaTopicModel model;
        model.loadFromXml(xml);
        std::string title = model.getTitle();
        TEST_ASSERT(title == "Simple Test Topic", "Extracts title correctly");
    }

    // Test 3: Set title
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaTopicModel model;
        model.loadFromXml(xml);
        model.setTitle("Updated Title");
        std::string newTitle = model.getTitle();
        TEST_ASSERT(newTitle == "Updated Title", "Sets title correctly");
    }

    // Test 4: Serialize to XML
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaTopicModel model;
        model.loadFromXml(xml);
        std::string serialized = model.serializeToXml();
        TEST_ASSERT(!serialized.empty(), "Serializes to XML");
        TEST_ASSERT(serialized.find("<?xml") != std::string::npos, "XML contains declaration");
        TEST_ASSERT(serialized.find("<topic") != std::string::npos, "XML contains topic element");
        TEST_ASSERT(serialized.find("</topic>") != std::string::npos, "XML is well-formed");
    }

    // Test 5: Round-trip preserves content
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaTopicModel model1;
        model1.loadFromXml(xml);
        std::string title1 = model1.getTitle();

        std::string serialized = model1.serializeToXml();

        DitaTopicModel model2;
        model2.loadFromXml(serialized);
        std::string title2 = model2.getTitle();

        TEST_ASSERT(title1 == title2, "Round-trip preserves title");
    }

    // Test 6: Handle complex topic
    {
        std::string xml = readFile("test_data/complex_topic.dita");
        DitaTopicModel model;
        bool loaded = model.loadFromXml(xml);
        TEST_ASSERT(loaded, "Loads complex topic XML");
        TEST_ASSERT(model.getTitle() == "Complex Test Topic", "Extracts title from complex topic");
    }

    // Test 7: Reject invalid XML
    {
        std::string invalidXml = "This is not XML";
        DitaTopicModel model;
        bool loaded = model.loadFromXml(invalidXml);
        TEST_ASSERT(!loaded, "Rejects invalid XML");
    }

    // Test 8: Reject non-topic XML
    {
        std::string nonTopicXml = "<?xml version=\"1.0\"?><root><child>test</child></root>";
        DitaTopicModel model;
        bool loaded = model.loadFromXml(nonTopicXml);
        TEST_ASSERT(!loaded, "Rejects non-topic XML");
    }
}

void testDitaMapModel() {
    TEST_SECTION("DitaMapModel Tests");

    // Test 1: Load valid map XML
    {
        std::string xml = readFile("test_data/sample_map.ditamap");
        DitaMapModel model;
        bool loaded = model.loadFromXml(xml);
        TEST_ASSERT(loaded, "Loads valid map XML");
    }

    // Test 2: Extract topicref list
    {
        std::string xml = readFile("test_data/sample_map.ditamap");
        DitaMapModel model;
        model.loadFromXml(xml);
        auto topicrefs = model.getTopicRefs();
        TEST_ASSERT(topicrefs.size() > 0, "Extracts topicref list");
        TEST_ASSERT(topicrefs.size() == 4, "Extracts correct number of top-level topicrefs");
    }

    // Test 3: Serialize to XML
    {
        std::string xml = readFile("test_data/sample_map.ditamap");
        DitaMapModel model;
        model.loadFromXml(xml);
        std::string serialized = model.serializeToXml();
        TEST_ASSERT(!serialized.empty(), "Serializes to XML");
        TEST_ASSERT(serialized.find("<?xml") != std::string::npos, "XML contains declaration");
        TEST_ASSERT(serialized.find("<map") != std::string::npos, "XML contains map element");
        TEST_ASSERT(serialized.find("topicref") != std::string::npos, "XML contains topicref elements");
    }

    // Test 4: Round-trip preserves structure
    {
        std::string xml = readFile("test_data/sample_map.ditamap");
        DitaMapModel model1;
        model1.loadFromXml(xml);
        size_t count1 = model1.getTopicRefs().size();

        std::string serialized = model1.serializeToXml();

        DitaMapModel model2;
        model2.loadFromXml(serialized);
        size_t count2 = model2.getTopicRefs().size();

        TEST_ASSERT(count1 == count2, "Round-trip preserves topicref count");
    }
}

void testDitaDetector() {
    TEST_SECTION("DitaDetector Tests");

    // Test 1: Identify topic by extension
    {
        DitaFileType type = detectDitaType(wxString("test.dita"));
        TEST_ASSERT(type == DITA_TYPE_TOPIC, "Identifies topic file by .dita extension");
    }

    // Test 2: Identify map by extension
    {
        DitaFileType type = detectDitaType(wxString("test.ditamap"));
        TEST_ASSERT(type == DITA_TYPE_MAP, "Identifies map file by .ditamap extension");
    }

    // Test 3: Identify topic from content
    {
        std::string xml = readFile("test_data/simple_topic.dita");
        DitaFileType type = detectDitaTypeFromContent(xml);
        TEST_ASSERT(type == DITA_TYPE_TOPIC, "Identifies topic from XML content");
    }

    // Test 4: Identify map from content
    {
        std::string xml = readFile("test_data/sample_map.ditamap");
        DitaFileType type = detectDitaTypeFromContent(xml);
        TEST_ASSERT(type == DITA_TYPE_MAP, "Identifies map from XML content");
    }

    // Test 5: Return NONE for non-DITA content
    {
        std::string nonDitaXml = "<?xml version=\"1.0\"?><root><child>test</child></root>";
        DitaFileType type = detectDitaTypeFromContent(nonDitaXml);
        TEST_ASSERT(type == DITA_TYPE_NONE, "Returns NONE for non-DITA content");
    }

    // Test 6: Return NONE for invalid XML
    {
        std::string invalidXml = "This is not XML at all";
        DitaFileType type = detectDitaTypeFromContent(invalidXml);
        TEST_ASSERT(type == DITA_TYPE_NONE, "Returns NONE for invalid XML");
    }

    // Test 7: Handle specialized topic types
    {
        std::string conceptXml = "<?xml version=\"1.0\"?><concept id=\"test\"><title>Test</title></concept>";
        DitaFileType type = detectDitaTypeFromContent(conceptXml);
        TEST_ASSERT(type == DITA_TYPE_TOPIC, "Recognizes concept as DITA topic");
    }
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "DITA Models Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    // Run all test suites
    testDitaTopicModel();
    testDitaMapModel();
    testDitaDetector();

    // Print summary
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "  Passed: " << tests_passed << std::endl;
    std::cout << "  Failed: " << tests_failed << std::endl;
    std::cout << "  Total:  " << (tests_passed + tests_failed) << std::endl;
    std::cout << "========================================" << std::endl;

    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\n✗ Some tests failed!" << std::endl;
        return 1;
    }
}
