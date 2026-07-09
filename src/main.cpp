#include "beep.h"

#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <string>
#include <glm/glm.hpp>

#include "rlImGui.h"
#include "imgui.h"
#ifndef IMGUI_API
#define IMGUI_API
#endif
#include "misc/cpp/imgui_stdlib.h"

#include "emulator.hpp"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4, "Floats are not 32-bit");
static_assert(sizeof(f64) == 8, "Doubles are not 64-bit");

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

using vec2i = glm::ivec2;
using vec3i = glm::ivec3;
using vec4i = glm::ivec4;

using mat4 = glm::mat4;

#include <cassert>

void setup_imgui_style() {
	// Excellency style by gonzaloivan121 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6f;
	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 6.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 6.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(8.0f, 6.0f);
	style.FrameRounding = 6.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(6.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 11.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;
	style.TabBorderSize = 1.0f;
	style.TabMinWidthBase = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5019608f, 0.5019608f, 0.5019608f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08235294f, 0.08235294f, 0.08235294f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.19607843f, 0.19607843f, 0.19607843f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.101960786f, 0.101960786f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.05882353f, 0.05882353f, 0.05882353f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.09019608f, 0.09019608f, 0.09019608f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.05882353f, 0.05882353f, 0.05882353f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08235294f, 0.08235294f, 0.08235294f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08235294f, 0.08235294f, 0.08235294f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15294118f, 0.15294118f, 0.15294118f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.019607844f, 0.019607844f, 0.019607844f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30980393f, 0.30980393f, 0.30980393f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4117647f, 0.4117647f, 0.4117647f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7529412f, 0.7529412f, 0.7529412f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 0.7f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65882355f, 0.65882355f, 0.65882355f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.21960784f, 0.21960784f, 0.21960784f, 0.784f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.27450982f, 0.27450982f, 0.27450982f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.21960784f, 0.21960784f, 0.21960784f, 0.588f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.18431373f, 0.18431373f, 0.18431373f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18431373f, 0.18431373f, 0.18431373f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.18431373f, 0.18431373f, 0.18431373f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.101960786f, 0.101960786f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.15294118f, 0.7254902f, 0.9490196f, 0.588f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.15294118f, 0.7254902f, 0.9490196f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9098039f, 0.9098039f, 0.9098039f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8117647f, 0.8117647f, 0.8117647f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.45882353f, 0.45882353f, 0.45882353f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08235294f, 0.08235294f, 0.08235294f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.88235295f, 0.5294118f, 0.118f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.88235295f, 0.5294118f, 0.235f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08235294f, 0.08235294f, 0.08235294f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.0f, 0.88235295f, 0.5294118f, 0.118f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6117647f, 0.6117647f, 0.6117647f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43137255f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9019608f, 0.7019608f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18431373f, 0.18431373f, 0.18431373f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.101960786f, 0.101960786f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15294118f, 0.7254902f, 0.9490196f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.15294118f, 0.7254902f, 0.9490196f, 0.8f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}

void raylib_spdlog_hook(int level, const char *text, va_list args) {
    spdlog::level::level_enum log_level = spdlog::level::trace;
    switch (level) {
        default: break;
        case LOG_DEBUG: log_level = spdlog::level::debug; break;
        case LOG_INFO: log_level = spdlog::level::info; break;
        case LOG_WARNING: log_level = spdlog::level::warn; break;
        case LOG_ERROR: log_level = spdlog::level::err; break;
        case LOG_FATAL: log_level = spdlog::level::critical; break;
    }

    char buf[4096] = {};
    std::vsnprintf(buf, 4096, text, args);

    spdlog::log(log_level, "{}", buf);
}

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    SetTraceLogCallback(raylib_spdlog_hook);
    SetTraceLogLevel(LOG_INFO);

    signal(SIGSEGV, [](int) -> void {
        spdlog::critical("segmentation fault");
        spdlog::default_logger()->flush();
        std::exit(100 + SIGSEGV);
    });

    std::string rom = "rom.bin";
    float tgt = 660;

    for (i32 i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--rom") == 0) {
            if (i + 1 < argc) {
                rom = argv[i + 1];
            } else {
                spdlog::error("--rom didn't get a value provided with it");
            }
        } else if (strcmp(argv[i], "--speed") == 0) {
            if (i + 1 < argc) {
                tgt = std::atoi(argv[i + 1]);
            } else {
                spdlog::error("--speed didn't get a value provided with it");
            }
        }
    }

    if (!std::filesystem::exists(rom)) {
        spdlog::critical("Path: {} does not exist!", rom);
        return 1;
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	spdlog::info("Window of {}x{} size (scale={})", fb_x * scale, fb_y * scale, scale);
    InitWindow(fb_x * scale, fb_y * scale, "gbemu");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    setup_imgui_style();

    emulator_t emulator;
    std::vector<uint8_t> data;
    {
        std::ifstream input(rom, std::ios::binary);

        input.seekg(0, std::ios::end);
        size_t sz = input.tellg();
        input.seekg(0, std::ios::beg);

        data.resize(sz);

        input.read(reinterpret_cast<char*>(data.data()), sz);
    }

    spdlog::info("ROM Loaded: {}", rom);

    // using palette_t = std::array<Color, 2>;

    InitAudioDevice();

    emulator.init(std::move(data));

    float acc = 0.0;
    float last = 0.0;

    bool start = true;

    RenderTexture2D rtex = LoadRenderTexture(fb_x * scale, fb_y * scale);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color(26, 26, 26));
        BeginTextureMode(rtex);
        ClearBackground(Color(26, 26, 26));
        {
        	DrawRectangle(0, 0, 140, 140, RED);
            // TODO: Draw Shit Here
        }
        EndTextureMode();

        if (start) {
            if (last == 0.0) {
                last = GetTime();
            }
            float now = GetTime();
            float dt = now - last;
            last = now;

            acc += dt;

            float step = 1. / tgt;

        	// Temporary step 1 only
        	// FIXME: Replcace
        	emulator.step(1024);

            // while (acc >= step) {
            //     emulator.step();
            //     acc -= step;
            // }
        } else {
            last = 0.0;
        }


        rlImGuiBegin();
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        // ImGui::Begin("Emulator", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {
            // ImGui::SetWindowSize({ (float) fb_x * scale, (float) fb_y * scale });
            // ImGui::Image(rtex.texture.id, { (float) rtex.texture.width + 1, (float) rtex.texture.height + 1 }, {0, 1}, {1, 0});
            // auto window_sz = ImGui::GetWindowSize();
            // if (window_sz.x > fb_x * scale && window_sz.y > fb_y * scale) {
            //     ++scale;
            //     UnloadRenderTexture(rtex);
            //     rtex = LoadRenderTexture(fb_x * scale, fb_y * scale);
            // } else if (window_sz.x < fb_x * scale && window_sz.y < fb_y * scale) {
            //     --scale;
            //     UnloadRenderTexture(rtex);
            //     rtex = LoadRenderTexture(fb_x * scale, fb_y * scale);
            // }
            //
            // ImGui::SetWindowSize({ (float) fb_x * scale, (float) fb_y * scale });
        }
        // ImGui::End();
        // ImGui::PopStyleVar();
        DrawFPS(10, 10);
        {
        }
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
}
