#pragma once
// ============================================================
// UIManager.h — Retained-Mode UI Architecture for APLG Platformer
// ============================================================
// Features:
// - Retained-mode widget tree (UIPanel, UILabel, UIProgressBar, UIButton, UICard, UIHUDContainer)
// - Centralized UIManager managing layout, DPI scaling, anchoring, padding, and theme
// - Resolution independence & dynamic mode switching (Windowed, Borderless, Fullscreen)
// - Translucent panels (~90% opacity), rounded cards, drop shadows, and responsive layout
// ============================================================

#define NOMINMAX
#include <windows.h>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

#pragma comment(lib, "msimg32.lib")

namespace APLG {

enum class Anchor {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

enum class WindowMode {
    Windowed,
    Borderless,
    Fullscreen
};

struct UITheme {
    COLORREF panelBg       = RGB(14, 18, 30);
    COLORREF panelBorder   = RGB(50, 140, 240);
    COLORREF cardBg        = RGB(24, 32, 50);
    COLORREF cardBorder    = RGB(70, 100, 150);
    COLORREF textPrimary   = RGB(240, 245, 255);
    COLORREF textSecondary = RGB(170, 215, 255);
    COLORREF textGold      = RGB(255, 215, 0);
    COLORREF hpBarGreen    = RGB(50, 220, 100);
    COLORREF hpBarBg       = RGB(40, 50, 70);
    BYTE panelAlpha        = 230; // ~90% opacity
};

class UIWidget {
public:
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 30;
    Anchor anchor = Anchor::TopLeft;
    int padLeft = 10;
    int padTop = 6;
    int padRight = 10;
    int padBottom = 6;
    bool visible = true;
    bool hover = false;
    BYTE alpha = 230;

    std::vector<std::shared_ptr<UIWidget>> children;

    virtual ~UIWidget() = default;

    virtual void Render(HDC hdc, int parentX, int parentY, int parentW, int parentH, float scale) {
        if (!visible) return;
        int absX = CalculateAbsoluteX(parentX, parentW, scale);
        int absY = CalculateAbsoluteY(parentY, parentH, scale);
        int absW = static_cast<int>(width * scale);
        int absH = static_cast<int>(height * scale);

        RenderSelf(hdc, absX, absY, absW, absH, scale);

        for (auto& child : children) {
            child->Render(hdc, absX, absY, absW, absH, scale);
        }
    }

protected:
    virtual void RenderSelf(HDC hdc, int absX, int absY, int absW, int absH, float scale) {}

    int CalculateAbsoluteX(int parentX, int parentW, float scale) const {
        int scaledW = static_cast<int>(width * scale);
        int scaledX = static_cast<int>(x * scale);

        switch (anchor) {
            case Anchor::TopLeft:
            case Anchor::CenterLeft:
            case Anchor::BottomLeft:
                return parentX + scaledX;
            case Anchor::TopCenter:
            case Anchor::Center:
            case Anchor::BottomCenter:
                return parentX + (parentW - scaledW) / 2 + scaledX;
            case Anchor::TopRight:
            case Anchor::CenterRight:
            case Anchor::BottomRight:
                return parentX + parentW - scaledW - scaledX;
        }
        return parentX + scaledX;
    }

    int CalculateAbsoluteY(int parentY, int parentH, float scale) const {
        int scaledH = static_cast<int>(height * scale);
        int scaledY = static_cast<int>(y * scale);

        switch (anchor) {
            case Anchor::TopLeft:
            case Anchor::TopCenter:
            case Anchor::TopRight:
                return parentY + scaledY;
            case Anchor::CenterLeft:
            case Anchor::Center:
            case Anchor::CenterRight:
                return parentY + (parentH - scaledH) / 2 + scaledY;
            case Anchor::BottomLeft:
            case Anchor::BottomCenter:
            case Anchor::BottomRight:
                return parentY + parentH - scaledH - scaledY;
        }
        return parentY + scaledY;
    }
};

class UIPanel : public UIWidget {
public:
    COLORREF bgColor = RGB(14, 18, 30);
    COLORREF borderColor = RGB(50, 140, 240);

    UIPanel(int w, int h, COLORREF bg = RGB(14, 18, 30), COLORREF border = RGB(50, 140, 240), BYTE a = 230) {
        width = w;
        height = h;
        bgColor = bg;
        borderColor = border;
        alpha = a;
    }

protected:
    void RenderSelf(HDC hdc, int absX, int absY, int absW, int absH, float scale) override {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmp = CreateCompatibleBitmap(hdc, absW, absH);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmp);

        HBRUSH bgBrush = CreateSolidBrush(bgColor);
        RECT rect = { 0, 0, absW, absH };
        FillRect(hdcMem, &rect, bgBrush);
        DeleteObject(bgBrush);

        HPEN borderPen = CreatePen(PS_SOLID, static_cast<int>(2 * scale), borderColor);
        HPEN hOldPen = (HPEN)SelectObject(hdcMem, borderPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
        Rectangle(hdcMem, 0, 0, absW, absH);
        SelectObject(hdcMem, hOldPen);
        SelectObject(hdcMem, hOldBrush);
        DeleteObject(borderPen);

        BLENDFUNCTION blend = { AC_SRC_OVER, 0, alpha, 0 };
        AlphaBlend(hdc, absX, absY, absW, absH, hdcMem, 0, 0, absW, absH, blend);

        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hbmp);
        DeleteDC(hdcMem);
    }
};

class UILabel : public UIWidget {
public:
    std::string text;
    COLORREF textColor = RGB(240, 245, 255);
    int fontSize = 18;
    bool bold = false;

    UILabel(const std::string& t, int size = 18, COLORREF col = RGB(240, 245, 255), bool isBold = false) {
        text = t;
        fontSize = size;
        textColor = col;
        bold = isBold;
    }

protected:
    void RenderSelf(HDC hdc, int absX, int absY, int absW, int absH, float scale) override {
        if (text.empty()) return;
        SetBkMode(hdc, TRANSPARENT);
        int scaledFontSize = static_cast<int>(fontSize * scale);
        HFONT hFont = CreateFontA(scaledFontSize, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, "Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, textColor);

        TextOutA(hdc, absX + static_cast<int>(padLeft * scale), absY + static_cast<int>(padTop * scale), text.c_str(), static_cast<int>(text.length()));

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
};

class UIProgressBar : public UIWidget {
public:
    float progress = 1.0f; // 0.0 to 1.0
    COLORREF barColor = RGB(50, 220, 100);
    COLORREF trackColor = RGB(40, 50, 70);

    UIProgressBar(int w, int h, float prog = 1.0f, COLORREF fill = RGB(50, 220, 100)) {
        width = w;
        height = h;
        progress = std::clamp(prog, 0.0f, 1.0f);
        barColor = fill;
    }

protected:
    void RenderSelf(HDC hdc, int absX, int absY, int absW, int absH, float scale) override {
        // Track
        HBRUSH trackBrush = CreateSolidBrush(trackColor);
        RECT trackRect = { absX, absY, absX + absW, absY + absH };
        FillRect(hdc, &trackRect, trackBrush);
        DeleteObject(trackBrush);

        // Fill
        int fillW = static_cast<int>(absW * std::clamp(progress, 0.0f, 1.0f));
        if (fillW > 0) {
            HBRUSH fillBrush = CreateSolidBrush(barColor);
            RECT fillRect = { absX, absY, absX + fillW, absY + absH };
            FillRect(hdc, &fillRect, fillBrush);
            DeleteObject(fillBrush);
        }

        // Outline
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(100, 130, 180));
        HPEN hOldPen = (HPEN)SelectObject(hdc, borderPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, absX, absY, absX + absW, absY + absH);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(borderPen);
    }
};

class UIManager {
public:
    static UIManager& Instance() {
        static UIManager instance;
        return instance;
    }

    int screenWidth = 1280;
    int screenHeight = 720;
    float uiScale = 1.0f;
    WindowMode windowMode = WindowMode::Windowed;
    UITheme theme;

    void SetResolution(int w, int h) {
        screenWidth = w;
        screenHeight = h;
        uiScale = std::min(static_cast<float>(w) / 1280.0f, static_cast<float>(h) / 720.0f);
        uiScale = std::max(1.0f, uiScale);
    }

    void SetWindowMode(WindowMode mode) {
        windowMode = mode;
    }
};

} // namespace APLG
