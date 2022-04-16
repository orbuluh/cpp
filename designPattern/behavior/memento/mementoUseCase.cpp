#include "mementoUseCase.h"
#include <iostream>

namespace memento {

void Snapshot::restore() {
    editor->setText(text);
    editor->setCursor(curX, curY);
    editor->setSelectionWidth(selectionWidth);
}

void Editor::printInternal() const {
    std::cout << "[" << curX << ", "
        << curY << "] selectionWidth=" << selectionWidth
        << " text=" << text
        << '\n';
}

void demo() {
    Editor editor;
    Command command(editor);
    editor.setText("abc");
    editor.setCursor(3, 0);
    editor.setSelectionWidth(3);
    command.makeBackup();
    editor.printInternal();
    editor.setText("abcdef");
    editor.setCursor(6, 0);
    editor.setSelectionWidth(6);
    editor.printInternal();
    command.undo();
    editor.printInternal();
}
} // namespace memento