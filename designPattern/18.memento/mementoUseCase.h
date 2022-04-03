#pragma once
#include <string>
#include <stack>
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
    void setSelectionWidth(int width) { selectionWidth = width; }
    Snapshot createSnapshot() {
        return Snapshot(this, text, curX, curY, selectionWidth);
    }
    void printInternal() const;
private:
    std::string text;
    int curX;
    int curY;
    int selectionWidth;
};

class Command {
public:
    Command(Editor& editor) : editor(editor) {}
    void makeBackup() {
        backups.push(std::move(editor.createSnapshot()));
    }
    void undo() {
        if (!backups.empty()) {
            backups.top().restore();
            backups.pop();
        }
    }
private:
    std::stack<Snapshot> backups;
    Editor& editor;
};
void demo();
} // namespace memento