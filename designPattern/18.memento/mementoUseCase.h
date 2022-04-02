#pragma once
#include <string>
#include <memory>
namespace memento {

class Editor;

class Snapshot { // the memento
public:
    Snapshot(Editor* editor, std::string_view t, int x, int y, int sw)
        : editor(editor), text(t), curX(x), curY(y), selectionWidth(sw) {}
    void restore();
private:
    Editor* editor;
    std::string text;
    int curX;
    int curY;
    int selectionWidth;
};
class Editor {
public:
    void setText(std::string_view s) { text = s; }
    void setCursor(int x, int y) { curX = x; curY = y; }
    void selectionWidth(int width) { selectionWidth = width; }
    std::unique_ptr<Snapshot> createSnapshot() {
        return std::make_unique<Snapshot>(this, text, curX, curY, selectionWidth);
    }
private:
    std::string text;
    int curX;
    int curY;
    int selectionWidth;
};

class Command {
    Command(Editor& editor) : editor(editor) {}
    void makeBackup() {
        backup = std::move(editor.createSnapshot());
    }
    void undo() {
        if (backup) {
            backup->restore();
        }
    }
private:
    std::unique_ptr<Snapshot> backup;
    Editor& editor;
};
void demo();
} // namespace memento