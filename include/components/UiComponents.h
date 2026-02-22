#ifndef ECHORIFT_UICOMPONENTS_H
#define ECHORIFT_UICOMPONENTS_H
#include <raylib.h>


namespace UI {

    //Text Components
    struct Label {
        const char* text;
        int fontSize;
        Color color;

        void draw(const int screenWidth, const int yPos)const {
            const int textWidth = MeasureText(text, fontSize);
            DrawText(text, (screenWidth / 2) - (textWidth / 2), yPos, fontSize, color);
        }
    };

    // interactive components
    struct Button {
        Rectangle rect;
        const char* text;
        Color baseColor;
        Color hoverColor;

        Button(const Rectangle r, const char* t, const Color base = DARKGRAY, const Color hover = GRAY)
            : rect(r), text(t), baseColor(base), hoverColor(hover){}


        [[nodiscard]] bool isHovered() const {
            return CheckCollisionPointRec(GetMousePosition(), rect);
        }

        [[nodiscard]] bool isClicked() const {

            return isHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        }

        void draw() const {
            const Color currentWeight = isHovered() ? hoverColor : baseColor;
            DrawRectangleRec(rect, currentWeight);
            DrawRectangleLinesEx(rect, 2, GOLD);

            constexpr int fontSize = 20;
            const int textWidth = MeasureText(text, fontSize);
            DrawText(text,
                    rect.x + (rect.width / 2) - (textWidth / 2),
                    rect.y + (rect.height / 2) - (fontSize / 2),
                    fontSize, RAYWHITE);
        }
    };
}


#endif //ECHORIFT_UICOMPONENTS_H