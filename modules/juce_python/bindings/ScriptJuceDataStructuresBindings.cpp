/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#if __has_include(<juce_data_structures/juce_data_structures.h>)

#include "ScriptJuceDataStructuresBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#undef JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceDataStructuresBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::UndoableAction

    struct PyUndoableAction : public UndoableAction
    {
        bool perform() override
        {
            PYBIND11_OVERRIDE_PURE(bool, UndoableAction, perform);
        }

        bool undo() override
        {
            PYBIND11_OVERRIDE_PURE(bool, UndoableAction, undo);
        }

        int getSizeInUnits() override
        {
            PYBIND11_OVERRIDE(int, UndoableAction, getSizeInUnits);
        }

        UndoableAction* createCoalescedAction (UndoableAction* nextAction) override
        {
            PYBIND11_OVERRIDE(UndoableAction*, UndoableAction, createCoalescedAction, nextAction);
        }
    };

    py::class_<UndoableAction, PyUndoableAction> classUndoableAction (m, "UndoableAction");

    classUndoableAction
        .def ("perform", &UndoableAction::perform)
        .def ("undo", &UndoableAction::undo)
        .def ("getSizeInUnits", &UndoableAction::getSizeInUnits)
        .def ("createCoalescedAction", &UndoableAction::createCoalescedAction)
    ;

    // ============================================================================================ juce::UndoManager

    py::class_<UndoManager> classUndoManager (m, "UndoManager");

    classUndoManager
        .def ("clearUndoHistory", &UndoManager::clearUndoHistory)
        .def ("getNumberOfUnitsTakenUpByStoredCommands", &UndoManager::getNumberOfUnitsTakenUpByStoredCommands)
        .def ("setMaxNumberOfStoredUnits", &UndoManager::setMaxNumberOfStoredUnits)
        .def ("perform", py::overload_cast<UndoableAction*> (&UndoManager::perform))
        .def ("perform", py::overload_cast<UndoableAction*, const String&> (&UndoManager::perform))
        .def ("beginNewTransaction", py::overload_cast<> (&UndoManager::beginNewTransaction))
        .def ("beginNewTransaction", py::overload_cast<const String&> (&UndoManager::beginNewTransaction))
        .def ("setCurrentTransactionName", &UndoManager::setCurrentTransactionName)
        .def ("getCurrentTransactionName", &UndoManager::getCurrentTransactionName)
        .def ("canUndo", &UndoManager::canUndo)
        .def ("undo", &UndoManager::undo)
        .def ("undoCurrentTransactionOnly", &UndoManager::undoCurrentTransactionOnly)
        .def ("getUndoDescription", &UndoManager::getUndoDescription)
        .def ("getUndoDescriptions", &UndoManager::getUndoDescriptions)
        .def ("getTimeOfUndoTransaction", &UndoManager::getTimeOfUndoTransaction)
        .def ("getActionsInCurrentTransaction", &UndoManager::getActionsInCurrentTransaction)
        .def ("getNumActionsInCurrentTransaction", &UndoManager::getNumActionsInCurrentTransaction)
        .def ("canRedo", &UndoManager::canRedo)
        .def ("redo", &UndoManager::redo)
        .def ("getRedoDescription", &UndoManager::getRedoDescription)
        .def ("getRedoDescriptions", &UndoManager::getRedoDescriptions)
        .def ("getTimeOfRedoTransaction", &UndoManager::getTimeOfRedoTransaction)
        .def ("isPerformingUndoRedo", &UndoManager::isPerformingUndoRedo)
    ;

    // ============================================================================================ juce::Value

    struct PyValueValueSource : public Value::ValueSource
    {
        using Value::ValueSource::ValueSource;
    
        var getValue () const override
        {
            PYBIND11_OVERRIDE_PURE(var, Value::ValueSource, getValue);
        }

        void setValue (const var& newValue) override
        {
            PYBIND11_OVERRIDE_PURE(void, Value::ValueSource, setValue, newValue);
        }
    };

    struct PyValueListener : public Value::Listener
    {
        using Value::Listener::Listener;
    
        void valueChanged(Value& value) override
        {
            PYBIND11_OVERRIDE_PURE(void, Value::Listener, valueChanged, value);
        }
    };

    py::class_<Value> classValue (m, "Value");
    py::class_<Value::ValueSource, PyValueValueSource> classValueValueSource (classValue, "ValueSource");
    py::class_<Value::Listener, PyValueListener> classValueListener (classValue, "Listener");

    classValue
        .def (py::init<>())
        .def (py::init<const var&>())
        .def (py::init<Value::ValueSource*>())
        .def ("getValue", &Value::getValue)
        .def ("toString", &Value::toString)
        .def ("setValue", &Value::setValue)
    //.def ("asVar", &Value::operator var)
        .def ("referTo", &Value::referTo)
        .def ("refersToSameSourceAs", &Value::refersToSameSourceAs)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("addListener", &Value::addListener)
        .def ("removeListener", &Value::removeListener)
        .def ("getValueSource", &Value::getValueSource, py::return_value_policy::reference)
    ;

    classValueValueSource
        .def (py::init<>())
        .def ("getValue", &Value::ValueSource::getValue)
        .def ("setValue", &Value::ValueSource::setValue)
        .def ("sendChangeMessage", &Value::ValueSource::sendChangeMessage)
    ;

    classValueListener
        .def (py::init<>())
        .def ("valueChanged", &Value::Listener::valueChanged)
    ;

    // ============================================================================================ juce::ValueTree

    struct PyValueTreeListener : public ValueTree::Listener
    {
        using ValueTree::Listener::Listener;
    
        void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreePropertyChanged, treeWhosePropertyHasChanged, property);
        }

        void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreeChildAdded, parentTree, childWhichHasBeenAdded);
        }

        void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreeChildRemoved, parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved);
        }

        void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreeChildOrderChanged, parentTreeWhoseChildrenHaveMoved, oldIndex, newIndex);
        }

        void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreeParentChanged, treeWhoseParentHasChanged);
        }

        void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override
        {
            PYBIND11_OVERRIDE(void, ValueTree::Listener, valueTreeRedirected, treeWhichHasBeenChanged);
        }
    };

    py::class_<ValueTree> classValueTree (m, "ValueTree");
    py::class_<ValueTree::Listener, PyValueTreeListener> classValueTreeListener (classValueTree, "Listener");

    classValueTree
        .def (py::init<>())
        .def (py::init<const Identifier&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isEquivalentTo", &ValueTree::isEquivalentTo)
        .def ("isValid", &ValueTree::isValid)
        .def ("createCopy", &ValueTree::createCopy)
        .def ("copyPropertiesFrom", &ValueTree::copyPropertiesFrom)
        .def ("copyPropertiesAndChildrenFrom", &ValueTree::copyPropertiesAndChildrenFrom)
        .def ("getType", &ValueTree::getType)
        .def ("hasType", &ValueTree::hasType)
        .def ("getProperty", py::overload_cast<const Identifier&> (&ValueTree::getProperty, py::const_))
        .def ("getProperty", py::overload_cast<const Identifier&, const var&> (&ValueTree::getProperty, py::const_))
        .def ("getPropertyPointer", &ValueTree::getPropertyPointer, py::return_value_policy::reference)
        .def ("setProperty", &ValueTree::setProperty)
        .def ("hasProperty", &ValueTree::hasProperty)
        .def ("removeProperty", &ValueTree::removeProperty)
        .def ("removeAllProperties", &ValueTree::removeAllProperties)
        .def ("getPropertyName", &ValueTree::getPropertyName)
        .def ("getPropertyAsValue", &ValueTree::getPropertyAsValue)
        .def ("getNumChildren", &ValueTree::getNumChildren)
        .def ("getChild", &ValueTree::getChild)
        .def ("getChildWithName", &ValueTree::getChildWithName)
        .def ("getOrCreateChildWithName", &ValueTree::getOrCreateChildWithName)
        .def ("getChildWithProperty", &ValueTree::getChildWithProperty)
        .def ("addChild", &ValueTree::addChild)
        .def ("appendChild", &ValueTree::appendChild)
        .def ("removeChild", py::overload_cast<const ValueTree&, UndoManager*> (&ValueTree::removeChild))
        .def ("removeChild", py::overload_cast<int, UndoManager*> (&ValueTree::removeChild))
        .def ("removeAllChildren", &ValueTree::removeAllChildren)
        .def ("moveChild", &ValueTree::moveChild)
        .def ("isAChildOf", &ValueTree::isAChildOf)
        .def ("indexOf", &ValueTree::indexOf)
        .def ("getParent", &ValueTree::getParent)
        .def ("getRoot", &ValueTree::getRoot)
        .def ("getSibling", &ValueTree::getSibling)
        .def ("createXml", &ValueTree::createXml)
        .def_static ("fromXml", static_cast<ValueTree (*)(const XmlElement&)> (&ValueTree::fromXml))
        .def_static ("fromXml", static_cast<ValueTree (*)(const String&)> (&ValueTree::fromXml))
        .def ("toXmlString", &ValueTree::toXmlString)
        .def ("writeToStream", &ValueTree::writeToStream)
        .def_static ("readFromStream", &ValueTree::readFromStream)
    //.def_static ("readFromData", &ValueTree::readFromData)
    //.def_static ("readFromGZIPData", &ValueTree::readFromGZIPData)
        .def ("addListener", &ValueTree::addListener)
        .def ("removeListener", &ValueTree::removeListener)
        .def ("setPropertyExcludingListener", &ValueTree::setPropertyExcludingListener)
        .def ("sendPropertyChangeMessage", &ValueTree::sendPropertyChangeMessage)
    //.def ("sort", &ValueTree::sort)
        .def ("getReferenceCount", &ValueTree::getReferenceCount)
    ;

#if 0

py::class_<ValueTreeSynchroniser, Listener> classValueTreeSynchroniser (m, "ValueTreeSynchroniser");
classValueTreeSynchroniser
    .def ("stateChanged", &ValueTreeSynchroniser::stateChanged)
    .def ("sendFullSyncCallback", &ValueTreeSynchroniser::sendFullSyncCallback)
    .def_static ("applyChange", &ValueTreeSynchroniser::applyChange)
    .def ("getRoot", &ValueTreeSynchroniser::getRoot)
;

py::class_<ValueTreePropertyWithDefault, Listener> classValueTreePropertyWithDefault (m, "ValueTreePropertyWithDefault");
classValueTreePropertyWithDefault
    .def ("get", &ValueTreePropertyWithDefault::get)
    .def ("getPropertyAsValue", &ValueTreePropertyWithDefault::getPropertyAsValue)
    .def ("getDefault", &ValueTreePropertyWithDefault::getDefault)
    .def ("setDefault", &ValueTreePropertyWithDefault::setDefault)
    .def ("isUsingDefault", &ValueTreePropertyWithDefault::isUsingDefault)
    .def ("resetToDefault", &ValueTreePropertyWithDefault::resetToDefault)
    // .def ("operator=", py::overload_cast<const int &>(&ValueTreePropertyWithDefault::operator=))
    // .def ("operator=", py::overload_cast<const ValueTreePropertyWithDefault &>(&ValueTreePropertyWithDefault::operator=))
    .def ("setValue", &ValueTreePropertyWithDefault::setValue)
    .def ("referTo", py::overload_cast<ValueTree, const int &, UndoManager *>(&ValueTreePropertyWithDefault::referTo))
    .def ("referTo", py::overload_cast<ValueTree, const int &, UndoManager *, int>(&ValueTreePropertyWithDefault::referTo))
    .def ("referTo", py::overload_cast<ValueTree, const int &, UndoManager *, int, int>(&ValueTreePropertyWithDefault::referTo))
    .def ("getValueTree", &ValueTreePropertyWithDefault::getValueTree)
    .def ("getPropertyID", &ValueTreePropertyWithDefault::getPropertyID)
    .def ("getUndoManager", &ValueTreePropertyWithDefault::getUndoManager)
;

py::class_<ValueTreePropertyWithDefault::SynchronousValueSource> classValueTreePropertyWithDefaultSynchronousValueSource (classValueTreePropertyWithDefault, "SynchronousValueSource");
classValueTreePropertyWithDefaultSynchronousValueSource
    .def ("getValue", &ValueTreePropertyWithDefault::SynchronousValueSource::getValue)
    .def ("setValue", &ValueTreePropertyWithDefault::SynchronousValueSource::setValue)
;


py::class_<PropertiesFile> classPropertiesFile (m, "PropertiesFile");
classPropertiesFile
    .def ("isValidFile", &PropertiesFile::isValidFile)
    .def ("saveIfNeeded", &PropertiesFile::saveIfNeeded)
    .def ("save", &PropertiesFile::save)
    .def ("needsToBeSaved", &PropertiesFile::needsToBeSaved)
    .def ("setNeedsToBeSaved", &PropertiesFile::setNeedsToBeSaved)
    .def ("reload", &PropertiesFile::reload)
    .def ("getFile", &PropertiesFile::getFile)
;


py::class_<PropertiesFile::Options> classPropertiesFileOptions (classPropertiesFile, "Options");
classPropertiesFileOptions
    .def ("getDefaultFile", &PropertiesFile::Options::getDefaultFile)
;

Options >  int   applicationName
Options >  int   filenameSuffix
Options >  int   folderName
Options >  int   osxLibrarySubFolder
Options >  bool   commonToAllUsers
Options >  bool   ignoreCaseOfKeyNames
Options >  bool   doNotSave
Options >  int   millisecondsBeforeSaving
Options >  StorageFormat   storageFormat
Options >  int *   processLock

py::class_<ApplicationProperties> classApplicationProperties (m, "ApplicationProperties");
classApplicationProperties
    .def ("setStorageParameters", &ApplicationProperties::setStorageParameters)
    .def ("getStorageParameters", &ApplicationProperties::getStorageParameters)
    .def ("getUserSettings", &ApplicationProperties::getUserSettings)
    .def ("getCommonSettings", &ApplicationProperties::getCommonSettings)
    .def ("saveIfNeeded", &ApplicationProperties::saveIfNeeded)
    .def ("closeFiles", &ApplicationProperties::closeFiles)
;

#endif
}

} // namespace popsicle::Bindings

#endif
