/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2022 - kunitoki <kunitoki@gmail.com>
 *
 * popsicle is an open source library subject to commercial or open-source licensing.
 *
 * By using popsicle, you agree to the terms of the popsicle License Agreement, which can
 * be found at https://raw.githubusercontent.com/kunitoki/popsicle/master/LICENSE
 *
 * Or: You may also use this code under the terms of the GPL v3 (see www.gnu.org/licenses).
 *
 * POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER EXPRESSED
 * OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE DISCLAIMED.
 */

#if __has_include(<juce_data_structures/juce_data_structures.h>)

#include "ScriptJuceDataStructuresBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

namespace popsicle::Bindings {

//=================================================================================================

template <template <class> class Class, class... Types>
void registerCachedValue (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "CachedValue[" << popsicle::Helpers::pythonizeClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueTree&, const Identifier&, UndoManager*>())
            .def (py::init<ValueTree&, const Identifier&, UndoManager*, const ValueType&>())
            .def ("get", &T::get)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("getPropertyAsValue", &T::getPropertyAsValue)
            .def ("isUsingDefault", &T::isUsingDefault)
            .def ("getDefault", &T::getDefault)
            .def ("setValue", &T::setValue)
            .def ("resetToDefault", py::overload_cast<> (&T::resetToDefault))
            .def ("resetToDefault", py::overload_cast<UndoManager*> (&T::resetToDefault))
            .def ("setDefault", &T::setDefault)
            .def ("referTo", py::overload_cast<ValueTree&, const Identifier&, UndoManager*> (&T::referTo))
            .def ("referTo", py::overload_cast<ValueTree&, const Identifier&, UndoManager*, const ValueType&> (&T::referTo))
            .def ("forceUpdateOfCachedValue", &T::forceUpdateOfCachedValue)
            .def ("getValueTree", &T::getValueTree, py::return_value_policy::reference)
            .def ("getPropertyID", &T::getPropertyID, py::return_value_policy::reference)
            .def ("getUndoManager", &T::getUndoManager, py::return_value_policy::reference)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("CachedValue", type);
}

void registerJuceDataStructuresBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::UndoableAction

    struct PyUndoableAction : public UndoableAction
    {
        bool perform() override
        {
            PYBIND11_OVERRIDE_PURE (bool, UndoableAction, perform);
        }

        bool undo() override
        {
            PYBIND11_OVERRIDE_PURE (bool, UndoableAction, undo);
        }

        int getSizeInUnits() override
        {
            PYBIND11_OVERRIDE (int, UndoableAction, getSizeInUnits);
        }

        UndoableAction* createCoalescedAction (UndoableAction* nextAction) override
        {
            PYBIND11_OVERRIDE (UndoableAction*, UndoableAction, createCoalescedAction, nextAction);
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
            PYBIND11_OVERRIDE_PURE (var, Value::ValueSource, getValue);
        }

        void setValue (const var& newValue) override
        {
            PYBIND11_OVERRIDE_PURE (void, Value::ValueSource, setValue, newValue);
        }
    };

    struct PyValueListener : public Value::Listener
    {
        using Value::Listener::Listener;

        void valueChanged (Value& value) override
        {
            PYBIND11_OVERRIDE_PURE (void, Value::Listener, valueChanged, value);
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
        .def ("referTo", &Value::referTo)
        .def ("refersToSameSourceAs", &Value::refersToSameSourceAs)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("addListener", &Value::addListener)
        .def ("removeListener", &Value::removeListener)
        .def ("getValueSource", &Value::getValueSource, py::return_value_policy::reference)
        // Custom
        .def ("asVar", [](const Value& self) { return static_cast<var> (self); })
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

        void valueTreePropertyChanged (ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreePropertyChanged, treeWhosePropertyHasChanged, property);
        }

        void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreeChildAdded, parentTree, childWhichHasBeenAdded);
        }

        void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreeChildRemoved, parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved);
        }

        void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreeChildOrderChanged, parentTreeWhoseChildrenHaveMoved, oldIndex, newIndex);
        }

        void valueTreeParentChanged (ValueTree& treeWhoseParentHasChanged) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreeParentChanged, treeWhoseParentHasChanged);
        }

        void valueTreeRedirected (ValueTree& treeWhichHasBeenChanged) override
        {
            PYBIND11_OVERRIDE (void, ValueTree::Listener, valueTreeRedirected, treeWhichHasBeenChanged);
        }
    };

    struct PyValueTreeComparator
    {
        PyValueTreeComparator() = default;

        int compareElements (const ValueTree& first, const ValueTree& second)
        {
            py::gil_scoped_acquire gil;

            if (py::function override_ = py::get_override (static_cast<PyValueTreeComparator*> (this), "compareElements"); override_)
            {
                auto result = override_ (first, second);

                return result.cast<int>();
            }

            py::pybind11_fail("Tried to call pure virtual function \"ValueTree::Comparator::compareElements\"");
        }
    };

    py::class_<ValueTree> classValueTree (m, "ValueTree");
    py::class_<ValueTree::Listener, PyValueTreeListener> classValueTreeListener (classValueTree, "Listener");
    py::class_<PyValueTreeComparator> classValueTreeComparator (classValueTree, "Comparator");

    classValueTreeListener.def (py::init<>());

    classValueTreeComparator.def (py::init<>());

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
        .def ("getProperty", py::overload_cast<const Identifier&> (&ValueTree::getProperty, py::const_), py::return_value_policy::reference)
        .def ("getProperty", py::overload_cast<const Identifier&, const var&> (&ValueTree::getProperty, py::const_), py::return_value_policy::reference)
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
        .def_static ("readFromData", [](const py::buffer& data)
        {
            py::buffer_info info = data.request();
            return ValueTree::readFromData (info.ptr, static_cast<size_t> (info.size));
        })
        .def_static ("readFromGZIPData", [](const py::buffer& data)
        {
            py::buffer_info info = data.request();
            return ValueTree::readFromGZIPData (info.ptr, static_cast<size_t> (info.size));
        })
        .def ("addListener", &ValueTree::addListener)
        .def ("removeListener", &ValueTree::removeListener)
        .def ("setPropertyExcludingListener", &ValueTree::setPropertyExcludingListener)
        .def ("sendPropertyChangeMessage", &ValueTree::sendPropertyChangeMessage)
        .def ("sort", &ValueTree::template sort<PyValueTreeComparator>)
        .def ("getReferenceCount", &ValueTree::getReferenceCount)
    ;

    // ============================================================================================ juce::CachedValue

    registerCachedValue<CachedValue, bool, int, float, String> (m);

    // ============================================================================================ juce::ValueTreeSynchroniser

    struct PyValueTreeSynchroniser : public ValueTreeSynchroniser
    {
        using ValueTreeSynchroniser::ValueTreeSynchroniser;

        void stateChanged (const void* encodedChange, size_t encodedChangeSize) override
        {
            py::gil_scoped_acquire gil;

            if (py::function override_ = py::get_override (static_cast<ValueTreeSynchroniser*> (this), "stateChanged"); override_)
            {
                auto change = py::memoryview::from_memory (encodedChange, static_cast<ssize_t> (encodedChangeSize));

                override_ (change, encodedChangeSize);

                return;
            }

            py::pybind11_fail("Tried to call pure virtual function \"ValueTreeSynchroniser::stateChanged\"");
        }
    };

    py::class_<ValueTreeSynchroniser, PyValueTreeSynchroniser> classValueTreeSynchroniser (m, "ValueTreeSynchroniser");

    classValueTreeSynchroniser
        .def (py::init<const ValueTree&>())
        .def ("stateChanged", &ValueTreeSynchroniser::stateChanged)
        .def ("sendFullSyncCallback", &ValueTreeSynchroniser::sendFullSyncCallback)
        .def_static ("applyChange", [](ValueTree* root, const py::buffer& data, UndoManager* undoManager)
        {
            jassert (root != nullptr); // TODO

            py::buffer_info info = data.request();

            return ValueTreeSynchroniser::applyChange (*root, info.ptr, static_cast<size_t> (info.size), undoManager);
        })
        .def ("getRoot", &ValueTreeSynchroniser::getRoot, py::return_value_policy::reference)
    ;

    // ============================================================================================ juce::ValueTreeSynchroniser

    py::class_<ValueTreePropertyWithDefault> classValueTreePropertyWithDefault (m, "ValueTreePropertyWithDefault");

    classValueTreePropertyWithDefault
        .def (py::init<>())
        .def (py::init<ValueTree&, const Identifier&, UndoManager*>())
        .def (py::init<ValueTree&, const Identifier&, UndoManager*, var>())
        .def (py::init<ValueTree&, const Identifier&, UndoManager*, var, StringRef>())
        .def ("get", &ValueTreePropertyWithDefault::get)
        .def ("getPropertyAsValue", &ValueTreePropertyWithDefault::getPropertyAsValue)
        .def ("getDefault", &ValueTreePropertyWithDefault::getDefault)
        .def ("setDefault", &ValueTreePropertyWithDefault::setDefault)
        .def ("isUsingDefault", &ValueTreePropertyWithDefault::isUsingDefault)
        .def ("resetToDefault", &ValueTreePropertyWithDefault::resetToDefault)
        .def_readwrite ("onDefaultChange", &ValueTreePropertyWithDefault::onDefaultChange)
        .def ("setValue", &ValueTreePropertyWithDefault::setValue)
        .def ("referTo", py::overload_cast<ValueTree, const Identifier&, UndoManager*> (&ValueTreePropertyWithDefault::referTo))
        .def ("referTo", py::overload_cast<ValueTree, const Identifier&, UndoManager*, var> (&ValueTreePropertyWithDefault::referTo))
        .def ("referTo", py::overload_cast<ValueTree, const Identifier&, UndoManager*, var, StringRef> (&ValueTreePropertyWithDefault::referTo))
        .def ("getValueTree", &ValueTreePropertyWithDefault::getValueTree, py::return_value_policy::reference)
        .def ("getPropertyID", &ValueTreePropertyWithDefault::getPropertyID, py::return_value_policy::reference)
        .def ("getUndoManager", &ValueTreePropertyWithDefault::getUndoManager, py::return_value_policy::reference)
    ;

    // ============================================================================================ juce::PropertiesFile

    py::class_<PropertiesFile> classPropertiesFile (m, "PropertiesFile");

    py::enum_<PropertiesFile::StorageFormat> (classPropertiesFile, "StorageFormat")
        .value ("storeAsBinary", PropertiesFile::StorageFormat::storeAsBinary)
        .value ("storeAsCompressedBinary", PropertiesFile::StorageFormat::storeAsCompressedBinary)
        .value ("storeAsXML", PropertiesFile::StorageFormat::storeAsXML)
        .export_values();

    py::class_<PropertiesFile::Options> classPropertiesFileOptions (classPropertiesFile, "Options");

    classPropertiesFileOptions
        .def (py::init<>())
        .def_readwrite ("applicationName", &PropertiesFile::Options::applicationName)
        .def_readwrite ("filenameSuffix", &PropertiesFile::Options::filenameSuffix)
        .def_readwrite ("folderName", &PropertiesFile::Options::folderName)
        .def_readwrite ("osxLibrarySubFolder", &PropertiesFile::Options::osxLibrarySubFolder)
        .def_readwrite ("commonToAllUsers", &PropertiesFile::Options::commonToAllUsers)
        .def_readwrite ("ignoreCaseOfKeyNames", &PropertiesFile::Options::ignoreCaseOfKeyNames)
        .def_readwrite ("doNotSave", &PropertiesFile::Options::doNotSave)
        .def_readwrite ("millisecondsBeforeSaving", &PropertiesFile::Options::millisecondsBeforeSaving)
        .def_readwrite ("storageFormat", &PropertiesFile::Options::storageFormat)
        .def_readwrite ("processLock", &PropertiesFile::Options::processLock)
        .def ("getDefaultFile", &PropertiesFile::Options::getDefaultFile)
    ;

    classPropertiesFile
        .def (py::init<const PropertiesFile::Options&>())
        .def (py::init<const File&, const PropertiesFile::Options&>())
        .def ("isValidFile", &PropertiesFile::isValidFile)
        .def ("saveIfNeeded", &PropertiesFile::saveIfNeeded)
        .def ("save", &PropertiesFile::save)
        .def ("needsToBeSaved", &PropertiesFile::needsToBeSaved)
        .def ("setNeedsToBeSaved", &PropertiesFile::setNeedsToBeSaved)
        .def ("reload", &PropertiesFile::reload)
        .def ("getFile", &PropertiesFile::getFile)
    ;

    // ============================================================================================ juce::ApplicationProperties

    py::class_<ApplicationProperties> classApplicationProperties (m, "ApplicationProperties");

    classApplicationProperties
        .def (py::init<>())
        .def ("setStorageParameters", &ApplicationProperties::setStorageParameters)
        .def ("getStorageParameters", &ApplicationProperties::getStorageParameters)
        .def ("getUserSettings", &ApplicationProperties::getUserSettings, py::return_value_policy::reference)
        .def ("getCommonSettings", &ApplicationProperties::getCommonSettings, py::return_value_policy::reference)
        .def ("saveIfNeeded", &ApplicationProperties::saveIfNeeded)
        .def ("closeFiles", &ApplicationProperties::closeFiles)
    ;
}

} // namespace popsicle::Bindings

#endif
