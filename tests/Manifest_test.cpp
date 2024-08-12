#include <QTest>
#include <QTimer>

#include <iostream>

#include "Manifest.h"

class ManifestTest : public QObject {
    Q_OBJECT

   private slots:
    void test_emptyManifest()
    {
        std::istringstream iss;
        const auto& manifest = new Manifest(iss);
        QVERIFY(manifest->getMainAttributes().empty());
        QVERIFY(manifest->getEntries().empty());
    }
    void test_parseManifest()
    {
        std::string manifest_text{ R"(Manifest-Version: 1.0
Created-By: 1.8.0 (Oracle Inc.)

Name: common/class1.class
SHA-256-Digest: D7fzW7bq0W+7YRCfxfZ4LY5LlCy+PXisjRgMCIiebS4=

Name: common/class2.class
SHA-256-Digest: TwUa/b/a2EQRsHWKupdFAR7S/BTeL52xTBvaB8C78Kc=
)" };

        std::istringstream iss{ manifest_text };
        const auto& manifest = new Manifest(iss);

        QVERIFY(manifest->getEntries().size() == 2);

        auto& main_attributes = manifest->getMainAttributes();
        QVERIFY(main_attributes.size() == 2);
        QVERIFY(main_attributes["Manifest-Version"] == "1.0");
        QVERIFY(main_attributes["Created-By"] == "1.8.0 (Oracle Inc.)");

        auto& class1_attributes = manifest->getAttributes("common/class1.class");
        QVERIFY(class1_attributes.size() == 1);
        QVERIFY(class1_attributes["SHA-256-Digest"] == "D7fzW7bq0W+7YRCfxfZ4LY5LlCy+PXisjRgMCIiebS4=");

        auto& class2_attributes = manifest->getAttributes("common/class2.class");
        QVERIFY(class2_attributes.size() == 1);
        QVERIFY(class2_attributes["SHA-256-Digest"] == "TwUa/b/a2EQRsHWKupdFAR7S/BTeL52xTBvaB8C78Kc=");

        // Manifest should parse even without the trailing newline
        std::string manifest_text_no_newline{ manifest_text };
        manifest_text_no_newline.pop_back();

        std::istringstream iss_no_newline{ manifest_text_no_newline };
        const auto& manifest_no_newline = new Manifest(iss_no_newline);

        QVERIFY(*manifest_no_newline == *manifest);
    }
    void test_invalidName()
    {
        std::istringstream iss{ R"(Manifest-Version: 1.0

A-Name-That-Is-Way-Too-Loooooooooooooooooooooooooooooooooooooooooooooooonoooooooooong: 1
)" };
        bool caught = false;
        try {
            new Manifest(iss);
        } catch (const std::runtime_error&) {
            caught = true;
        }
        QVERIFY(caught);
    }
    void test_lineTooLong()
    {
        std::string manifest_text{ "Manifest-Version: " };
        manifest_text.append(std::string(MANIFEST_MAX_LINE_LENGTH, '1'));
        std::istringstream iss{ manifest_text };
        bool caught = false;
        try {
            new Manifest(iss);
        } catch (const std::length_error&) {
            caught = true;
        }
        QVERIFY(caught);
    }
    void test_misplacedContinuation()
    {
        std::istringstream iss{ " Manifest-Version: 1.0" };
        bool caught = false;
        try {
            new Manifest(iss);
        } catch (const std::runtime_error&) {
            caught = true;
        }
        QVERIFY(caught);
    }
    void test_misingColon()
    {
        std::istringstream iss{ "Manifest-Version 1.0" };
        bool caught = false;
        try {
            new Manifest(iss);
        } catch (const std::runtime_error&) {
            caught = true;
        }
        QVERIFY(caught);
    }
    void test_misingSpace()
    {
        std::istringstream iss{ "Manifest-Version:1.0" };
        bool caught = false;
        try {
            new Manifest(iss);
        } catch (const std::runtime_error&) {
            caught = true;
        }
        QVERIFY(caught);
    }
};

QTEST_GUILESS_MAIN(ManifestTest)

#include "Manifest_test.moc"
