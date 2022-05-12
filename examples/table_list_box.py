import sys
sys.path.insert(0, "../")

import cppyy

from popsicle import juce_gui_basics, juce_gui_extra
from popsicle import juce, juce_multi, START_JUCE_COMPONENT


def forEachXmlChildElement(parentXmlElement):
    childElementVariableName = parentXmlElement.getFirstChildElement()
    while childElementVariableName:
        yield childElementVariableName
        childElementVariableName = childElementVariableName.getNextElement()


class EditableTextCustomComponent(juce.Label):
    def __init__(self, td):
        super().__init__()

        self.owner = td
        self.row = 0
        self.columnId = 0

        self.setEditable(False, True, False)

    def mouseDown(self, event):
        self.owner.table.selectRowsBasedOnModifierKeys(self.row, event.mods, False)

        super().mouseDown(event)

    def textWasEdited(self):
        self.owner.setText(self.columnId, self.row, self.getText())

    def setRowAndColumn(self, newRow, newColumn):
        self.row = newRow
        self.columnId = newColumn
        self.setText(self.owner.getText(self.columnId, self.row), juce.dontSendNotification)


class SelectionColumnCustomComponent(juce.Component):
    def __init__(self, td):
        super().__init__()

        self.owner = td
        self.row = 0
        self.columnId = 0

        self.toggleButton = juce.ToggleButton()
        self.toggleButton.onClick = lambda: self.owner.setSelection(self.row, self.toggleButton.getToggleState())
        self.addAndMakeVisible(self.toggleButton)

    def resized(self):
        #self.toggleButton.setBoundsInset(juce.BorderSize[int](2))
        #self.toggleButton.setBounds(0, 0, self.getWidth(), self.getHeight())
        pass

    def setRowAndColumn(self, newRow, newColumn):
        self.row = newRow
        self.columnId = newColumn

        self.toggleButton.setToggleState(self.owner.getSelection(self.row), juce.dontSendNotification)


class TutorialDataSorter(object):
    def __init__(self, attributeToSortBy, forwards):
        self.attributeToSort = attributeToSortBy
        self.direction = 1 if forwards else -1

    def compareElements(self, first, second):
        sortWith = second.getStringAttribute(self.attributeToSort)
        result = first.getStringAttribute(self.attributeToSort).compareNatural(sortWith)

        if result == 0:
            result = first.getStringAttribute("ID").compareNatural(second.getStringAttribute("ID"))

        return self.direction * result


class TableTutorialComponent(juce_multi(juce.Component, juce.TableListBoxModel)):
    font = juce.Font(14.0)
    numRows = 0
    columnList = None
    dataList = None

    def __init__(self):
        super().__init__((), ())

        self.loadData()

        self.table = juce.TableListBox("Table", self)
        self.addAndMakeVisible(self.table)

        header = self.table.getHeader()

        if self.columnList:
            for columnXml in forEachXmlChildElement(self.columnList):
                header.addColumn(
                    columnXml.getStringAttribute("name"),
                    columnXml.getIntAttribute("columnId"),
                    columnXml.getIntAttribute("width"),
                    50,
                    400,
                    juce.TableHeaderComponent.defaultFlags)

        header.setSortColumnId(1, True)

        self.table.setMultipleSelectionEnabled(True)
        self.table.setColour(juce.ListBox.outlineColourId, juce.Colours.grey)
        self.table.setOutlineThickness(1)

    def getNumRows(self):
        return self.numRows

    def paintRowBackground(self, g, rowNumber, width, height, rowIsSelected):
        alternateColour = self.getLookAndFeel().findColour(juce.ListBox.backgroundColourId) \
            .interpolatedWith(self.getLookAndFeel().findColour(juce.ListBox.textColourId), 0.03)

        if rowIsSelected:
            g.fillAll(juce.Colours.lightblue)
        elif rowNumber % 2:
            g.fillAll(alternateColour)

    def paintCell(self, g, rowNumber, columnId, width, height, rowIsSelected):
        g.setColour(juce.Colours.darkblue if rowIsSelected else self.getLookAndFeel().findColour(juce.ListBox.textColourId))
        g.setFont(self.font)

        rowElement = self.dataList.getChildElement(rowNumber)
        if rowElement:
            text = rowElement.getStringAttribute(self.getAttributeNameForColumnId(columnId))

            g.drawText(text, 2, 0, width - 4, height, juce.Justification.centredLeft, True)

        g.setColour(self.getLookAndFeel().findColour(juce.ListBox.backgroundColourId))
        g.fillRect(width - 1, 0, 1, height)

    def sortOrderChanged(self, newSortColumnId, isForwards):
        if newSortColumnId != 0 and self.dataList:
            sorter = TutorialDataSorter(self.getAttributeNameForColumnId(newSortColumnId), isForwards)

            self.dataList.sortChildElements(lambda lhs, rhs: sorter.compareElements(lhs, rhs))

            self.table.updateContent()

    def refreshComponentForCell(self, rowNumber, columnId, isRowSelected, existingComponentToUpdate):
        if columnId == 9:
            if existingComponentToUpdate:
                selectionBox = existingComponentToUpdate
            else:
                selectionBox = SelectionColumnCustomComponent(self)

            selectionBox.setRowAndColumn(rowNumber, columnId)
            selectionBox.__python_owns__ = False
            return selectionBox

        if columnId == 8:
            if existingComponentToUpdate:
                textLabel = existingComponentToUpdate
            else:
                textLabel = EditableTextCustomComponent(self)

            textLabel.setRowAndColumn(rowNumber, columnId)
            textLabel.__python_owns__ = False
            return textLabel

        assert(not existingComponentToUpdate)
        return cppyy.nullptr

    def getColumnAutoSizeWidth(self, columnId):
        if columnId == 9:
            return 50

        widest = 32

        for i in range(self.getNumRows(), 0, -1):
            rowElement = self.dataList.getChildElement(i)
            if rowElement:
                text = rowElement.getStringAttribute(self.getAttributeNameForColumnId(columnId))

                widest = juce.jmax(widest, self.font.getStringWidth(text))

        return widest + 8

    def getSelection(self, rowNumber):
        return self.dataList.getChildElement(rowNumber).getIntAttribute("Select")

    def setSelection(self, rowNumber, newSelection):
        self.dataList.getChildElement(rowNumber).setAttribute("Select", newSelection)

    def getText(self, columnNumber, rowNumber):
        return self.dataList.getChildElement(rowNumber).getStringAttribute(self.getAttributeNameForColumnId(columnNumber))

    def setText(self, columnNumber, rowNumber, newText):
        columnName = self.table.getHeader().getColumnName(columnNumber)
        self.dataList.getChildElement(rowNumber).setAttribute(columnName, newText)

    def resized(self):
        self.table.setBoundsInset(juce.BorderSize[int](8))

    def loadData(self):
        tableFile = juce.File.getCurrentWorkingDirectory().getChildFile("table_list_box.xml")

        if tableFile.exists():
            self.tutorialData = juce.XmlDocument.parse(tableFile)

            self.dataList = self.tutorialData.getChildByName("DATA")
            self.columnList = self.tutorialData.getChildByName("HEADERS")

            self.numRows = self.dataList.getNumChildElements()

    def getAttributeNameForColumnId(self, columnId):
        for columnXml in forEachXmlChildElement(self.columnList):
            if columnXml.getIntAttribute("columnId") == columnId:
                return columnXml.getStringAttribute("name")

        return ""


class MainContentComponent(juce.Component):
    def __init__(self):
        super().__init__()

        self.table = TableTutorialComponent()
        self.addAndMakeVisible(self.table)

        self.setSize(1200, 600)

    def paint(self, g):
        g.fillAll(self.getLookAndFeel().findColour(juce.ResizableWindow.backgroundColourId))

    def resized(self):
        self.table.setBounds(self.getLocalBounds())


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Animated Component")
