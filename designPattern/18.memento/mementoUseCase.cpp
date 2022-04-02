#include "mementoUseCase.h"
#include <iostream>

namespace memento {

void Snapshot::restore() {
    editor->setText(text);
    editor->setCursor(curX, curY);
    editor->setSelectionWidth(selectionWidth);
}

void demo() {
    
}
} // namespace memento