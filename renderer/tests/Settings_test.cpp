// SettingsTest.cpp

#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include "Settings.hpp"
#include "Parameters.hpp"

// Helper function to create a Settings object with custom values
Settings createCustomSettings() {
    return Settings(
        1280,
        720,
        500,
        false,
        32,
        512,
        16,
        0.5f,
        '\\',
        256.0f,
        0.1f,
        2048.0f,
        UIPage::WorldMenuOpen,
        "TestWorld",
        std::make_shared<Parameters>(Parameters(true)),
        10.0f,
        1000.0f,
        0.8f,
        glm::vec3(1.0f, 0.5f, 0.25f),
        true
    );
}

// --- Tests ---

TEST(SettingsTest, DefaultConstructorTest) {
    Settings settings;

    EXPECT_EQ(settings.getWindowWidth(), 1920);
    EXPECT_EQ(settings.getWindowHeight(), 1080);
    EXPECT_EQ(settings.getUIWidth(), 700);
    EXPECT_TRUE(settings.getFullscreen());
    EXPECT_EQ(settings.getRenderDistance(), 16);
    EXPECT_EQ(settings.getChunkSize(), 1024);
    EXPECT_EQ(settings.getSubChunkSize(), 32);
    EXPECT_FLOAT_EQ(settings.getSubChunkResolution(), 1.0f);
    EXPECT_EQ(settings.getFilePathDelimitter(), '/');
    EXPECT_FLOAT_EQ(settings.getMaximumHeight(), 192.0f);
    EXPECT_FLOAT_EQ(settings.getSeaLevel(), 0.2f);
    EXPECT_FLOAT_EQ(settings.getRequestDistance(), 1024.0f);
    EXPECT_EQ(settings.getCurrentPage(), UIPage::Home);
    EXPECT_EQ(settings.getCurrentWorld(), "");
    EXPECT_NE(settings.getParameters(), nullptr);
    EXPECT_FLOAT_EQ(settings.getFogStart(), 0.0f);
    EXPECT_FLOAT_EQ(settings.getFogEnd(), 512.0f);
    EXPECT_FLOAT_EQ(settings.getFogDensity(), 1.0f);
    EXPECT_FLOAT_EQ(settings.getFogColor().x, 0.5f);
    EXPECT_FLOAT_EQ(settings.getFogColor().y, 0.5f);
    EXPECT_FLOAT_EQ(settings.getFogColor().z, 0.5f);
    EXPECT_TRUE(settings.getUse1kTextures());
}

TEST(SettingsTest, ParameterizedConstructorTest) {
    Settings settings = createCustomSettings();

    EXPECT_EQ(settings.getWindowWidth(), 1280);
    EXPECT_EQ(settings.getWindowHeight(), 720);
    EXPECT_EQ(settings.getUIWidth(), 500);
    EXPECT_FALSE(settings.getFullscreen());
    EXPECT_EQ(settings.getRenderDistance(), 32);
    EXPECT_EQ(settings.getChunkSize(), 512);
    EXPECT_EQ(settings.getSubChunkSize(), 16);
    EXPECT_FLOAT_EQ(settings.getSubChunkResolution(), 0.5f);
    EXPECT_EQ(settings.getFilePathDelimitter(), '\\');
    EXPECT_FLOAT_EQ(settings.getMaximumHeight(), 256.0f);
    EXPECT_FLOAT_EQ(settings.getSeaLevel(), 0.1f);
    EXPECT_FLOAT_EQ(settings.getRequestDistance(), 2048.0f);
    EXPECT_EQ(settings.getCurrentPage(), UIPage::WorldMenuOpen);
    EXPECT_EQ(settings.getCurrentWorld(), "TestWorld");
    EXPECT_NE(settings.getParameters(), nullptr);
    EXPECT_FLOAT_EQ(settings.getFogStart(), 10.0f);
    EXPECT_FLOAT_EQ(settings.getFogEnd(), 1000.0f);
    EXPECT_FLOAT_EQ(settings.getFogDensity(), 0.8f);
    EXPECT_FLOAT_EQ(settings.getFogColor().x, 1.0f);
    EXPECT_FLOAT_EQ(settings.getFogColor().y, 0.5f);
    EXPECT_FLOAT_EQ(settings.getFogColor().z, 0.25f);
    EXPECT_TRUE(settings.getUse1kTextures());
}

TEST(SettingsTest, UpdateSettingsTest) {
    Settings settings;

    settings.updateSettings(
        1600, 900, 600, false, 24, 2048, 64, 2.0f, '\\',
        300.0f, 1.0f, 1500.0f, UIPage::WorldMenuClosed, "UpdatedWorld",
        std::make_shared<Parameters>(Parameters(true)),
        5.0f, 800.0f, 0.5f, glm::vec3(0.1f, 0.2f, 0.3f),
        false
    );

    EXPECT_EQ(settings.getWindowWidth(), 1600);
    EXPECT_EQ(settings.getWindowHeight(), 900);
    EXPECT_EQ(settings.getUIWidth(), 600);
    EXPECT_FALSE(settings.getFullscreen());
    EXPECT_EQ(settings.getRenderDistance(), 24);
    EXPECT_EQ(settings.getChunkSize(), 2048);
    EXPECT_EQ(settings.getSubChunkSize(), 64);
    EXPECT_FLOAT_EQ(settings.getSubChunkResolution(), 2.0f);
    EXPECT_EQ(settings.getFilePathDelimitter(), '\\');
    EXPECT_FLOAT_EQ(settings.getMaximumHeight(), 300.0f);
    EXPECT_FLOAT_EQ(settings.getSeaLevel(), 1.0f);
    EXPECT_FLOAT_EQ(settings.getRequestDistance(), 1500.0f);
    EXPECT_EQ(settings.getCurrentPage(), UIPage::WorldMenuClosed);
    EXPECT_EQ(settings.getCurrentWorld(), "UpdatedWorld");
    EXPECT_NE(settings.getParameters(), nullptr);
    EXPECT_FLOAT_EQ(settings.getFogStart(), 5.0f);
    EXPECT_FLOAT_EQ(settings.getFogEnd(), 800.0f);
    EXPECT_FLOAT_EQ(settings.getFogDensity(), 0.5f);
    EXPECT_FLOAT_EQ(settings.getFogColor().x, 0.1f);
    EXPECT_FLOAT_EQ(settings.getFogColor().y, 0.2f);
    EXPECT_FLOAT_EQ(settings.getFogColor().z, 0.3f);
    EXPECT_FALSE(settings.getUse1kTextures());
}

TEST(SettingsTest, SettersTest) {
    Settings settings;

    settings.setUIWidth(800);
    settings.setCurrentPage(UIPage::Loading);
    settings.setCurrentWorld("NewWorld");
    auto params = std::make_shared<Parameters>(Parameters(true));
    settings.setParameters(params);

    EXPECT_EQ(settings.getUIWidth(), 800);
    EXPECT_EQ(settings.getCurrentPage(), UIPage::Loading);
    EXPECT_EQ(settings.getCurrentWorld(), "NewWorld");
    EXPECT_EQ(settings.getParameters(), params);
}

// TEST(SettingsTest, OutputOperatorTest) {
//     Settings settings = createCustomSettings();
//     std::ostringstream output;

//     settings.operator<<(output);

//     std::string outputStr = output.str();
//     EXPECT_NE(outputStr.find("Window Width: 1280"), std::string::npos);
//     EXPECT_NE(outputStr.find("Window Height: 720"), std::string::npos);
//     EXPECT_NE(outputStr.find("Current World: TestWorld"), std::string::npos);
//     EXPECT_NE(outputStr.find("Fog Color: 1, 0.5, 0.25"), std::string::npos);
// }

// Optional: You could test the getenv-based path functions separately
TEST(SettingsTest, ProjectPathsTest) {
    Settings settings;

    // Only test if the environment variables are set, otherwise skip
    if (getenv("PROJECT_ROOT")) {
        EXPECT_FALSE(settings.getProjectRoot().empty());
    }
    if (getenv("PRISM_ROOT")) {
        EXPECT_FALSE(settings.getRenderPath().empty());
    }
    if (getenv("SHADER_ROOT")) {
        EXPECT_FALSE(settings.getShaderPath().empty());
    }
    if (getenv("TEXTURE_ROOT")) {
        EXPECT_FALSE(settings.getTexturePath().empty());
    }
    if (getenv("DATA_ROOT")) {
        EXPECT_FALSE(settings.getDataPath().empty());
    }
}