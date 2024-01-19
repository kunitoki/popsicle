import copy
import pytest

import popsicle as juce


class CustomAction(juce.UndoableAction):
    value: list = []
    oldValue: list = []

    def __init__(self, value):
        super().__init__()
        self.value = value

    def perform(self):
        self.oldValue = copy.copy(self.value)
        self.value.clear()
        return True

    def undo(self):
        self.value = copy.copy(self.oldValue)
        self.oldValue.clear()
        return True


def test_undoable_action():
    um = juce.UndoManager()
    assert not um.canRedo()
    assert not um.canUndo()

    um.beginNewTransaction("abc")
    assert um.getCurrentTransactionName() == "abc"

    um.setCurrentTransactionName("def")
    assert um.getCurrentTransactionName() == "def"

    assert um.getNumActionsInCurrentTransaction() == 0

    action = CustomAction([1, 2, 3, 4])

    um.perform(action)
    assert action.value == []
    assert not um.canRedo()
    assert um.canUndo()
    assert um.getNumActionsInCurrentTransaction() == 1
    assert um.getActionsInCurrentTransaction()[0] == action
    assert um.getUndoDescription() == "def"
    assert um.getUndoDescriptions() == ["def"]

    um.undo()
    assert action.value == [1, 2, 3, 4]
    assert um.canRedo()
    assert not um.canUndo()
    assert um.getNumActionsInCurrentTransaction() == 0
    assert um.getActionsInCurrentTransaction() == []
    assert um.getUndoDescription() == ""
    assert um.getUndoDescriptions() == []

    um.redo()
    assert action.value == []
    assert not um.canRedo()
    assert um.canUndo()
    assert um.getNumActionsInCurrentTransaction() == 0
    assert um.getActionsInCurrentTransaction() == []
    assert um.getUndoDescription() == "def"
    assert um.getUndoDescriptions() == ["def"]

    um.clearUndoHistory()
    assert not um.canRedo()
    assert not um.canUndo()
    assert um.getNumActionsInCurrentTransaction() == 0
    assert um.getActionsInCurrentTransaction() == []
    assert um.getUndoDescription() == ""
    assert um.getUndoDescriptions() == []


def test_undoable_action_ownership():
    um = juce.UndoManager()

    action = CustomAction([1, 2, 3, 4])
    um.perform(action)

    with pytest.raises(Exception):
        um.perform(action)

