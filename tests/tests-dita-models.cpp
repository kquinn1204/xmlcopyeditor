#include "catch.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include "../src/ditatopicmodel.h"
#include "../src/ditamapmodel.h"
#include "../src/ditadetector.h"

// Helper function to read file contents
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST_CASE("DitaTopicModel loads valid topic XML", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(xml) == true);
    REQUIRE(model.isValidTopic() == true);
}

TEST_CASE("DitaTopicModel extracts title", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(xml));
    REQUIRE(model.getTitle() == "Simple Test Topic");
}

TEST_CASE("DitaTopicModel sets title", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(xml));

    model.setTitle("Updated Title");
    REQUIRE(model.getTitle() == "Updated Title");
}

TEST_CASE("DitaTopicModel serializes to XML", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(xml));

    std::string serialized = model.serializeToXml();
    REQUIRE(!serialized.empty());
    REQUIRE(serialized.find("<?xml") != std::string::npos);
    REQUIRE(serialized.find("<topic") != std::string::npos);
    REQUIRE(serialized.find("</topic>") != std::string::npos);
}

TEST_CASE("DitaTopicModel round-trip preserves content", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model1;
    REQUIRE(model1.loadFromXml(xml));

    std::string title1 = model1.getTitle();
    std::string serialized = model1.serializeToXml();

    DitaTopicModel model2;
    REQUIRE(model2.loadFromXml(serialized));

    std::string title2 = model2.getTitle();
    REQUIRE(title1 == title2);
}

TEST_CASE("DitaTopicModel handles complex topic", "[ditatopicmodel]") {
    std::string xml = readFile("test_data/complex_topic.dita");
    REQUIRE(!xml.empty());

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(xml));
    REQUIRE(model.isValidTopic() == true);
    REQUIRE(model.getTitle() == "Complex Test Topic");
}

TEST_CASE("DitaTopicModel rejects invalid XML", "[ditatopicmodel]") {
    std::string invalidXml = "This is not XML";

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(invalidXml) == false);
}

TEST_CASE("DitaTopicModel rejects non-topic XML", "[ditatopicmodel]") {
    std::string nonTopicXml = "<?xml version=\"1.0\"?><root><child>test</child></root>";

    DitaTopicModel model;
    REQUIRE(model.loadFromXml(nonTopicXml) == false);
}

TEST_CASE("DitaMapModel loads valid map XML", "[ditamapmodel]") {
    std::string xml = readFile("test_data/sample_map.ditamap");
    REQUIRE(!xml.empty());

    DitaMapModel model;
    REQUIRE(model.loadFromXml(xml) == true);
}

TEST_CASE("DitaMapModel extracts topicref list", "[ditamapmodel]") {
    std::string xml = readFile("test_data/sample_map.ditamap");
    REQUIRE(!xml.empty());

    DitaMapModel model;
    REQUIRE(model.loadFromXml(xml));

    auto topicrefs = model.getTopicRefs();
    REQUIRE(topicrefs.size() > 0);
}

TEST_CASE("DitaMapModel serializes to XML", "[ditamapmodel]") {
    std::string xml = readFile("test_data/sample_map.ditamap");
    REQUIRE(!xml.empty());

    DitaMapModel model;
    REQUIRE(model.loadFromXml(xml));

    std::string serialized = model.serializeToXml();
    REQUIRE(!serialized.empty());
    REQUIRE(serialized.find("<?xml") != std::string::npos);
    REQUIRE(serialized.find("<map") != std::string::npos);
    REQUIRE(serialized.find("</map>") != std::string::npos);
    REQUIRE(serialized.find("topicref") != std::string::npos);
}

TEST_CASE("DitaMapModel round-trip preserves structure", "[ditamapmodel]") {
    std::string xml = readFile("test_data/sample_map.ditamap");
    REQUIRE(!xml.empty());

    DitaMapModel model1;
    REQUIRE(model1.loadFromXml(xml));

    size_t count1 = model1.getTopicRefs().size();
    std::string serialized = model1.serializeToXml();

    DitaMapModel model2;
    REQUIRE(model2.loadFromXml(serialized));

    size_t count2 = model2.getTopicRefs().size();
    REQUIRE(count1 == count2);
}

TEST_CASE("DitaDetector identifies topic file by extension", "[ditadetector]") {
    DitaFileType type = detectDitaType(wxString("test.dita"));
    REQUIRE(type == DITA_TYPE_TOPIC);
}

TEST_CASE("DitaDetector identifies map file by extension", "[ditadetector]") {
    DitaFileType type = detectDitaType(wxString("test.ditamap"));
    REQUIRE(type == DITA_TYPE_MAP);
}

TEST_CASE("DitaDetector identifies topic from content", "[ditadetector]") {
    std::string xml = readFile("test_data/simple_topic.dita");
    REQUIRE(!xml.empty());

    DitaFileType type = detectDitaTypeFromContent(xml);
    REQUIRE(type == DITA_TYPE_TOPIC);
}

TEST_CASE("DitaDetector identifies map from content", "[ditadetector]") {
    std::string xml = readFile("test_data/sample_map.ditamap");
    REQUIRE(!xml.empty());

    DitaFileType type = detectDitaTypeFromContent(xml);
    REQUIRE(type == DITA_TYPE_MAP);
}

TEST_CASE("DitaDetector returns NONE for non-DITA content", "[ditadetector]") {
    std::string nonDitaXml = "<?xml version=\"1.0\"?><root><child>test</child></root>";

    DitaFileType type = detectDitaTypeFromContent(nonDitaXml);
    REQUIRE(type == DITA_TYPE_NONE);
}

TEST_CASE("DitaDetector returns NONE for invalid XML", "[ditadetector]") {
    std::string invalidXml = "This is not XML at all";

    DitaFileType type = detectDitaTypeFromContent(invalidXml);
    REQUIRE(type == DITA_TYPE_NONE);
}
