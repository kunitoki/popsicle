/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
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

#include "ScriptJuceDataStructuresBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// =================================================================================================

template <template <class> class Class, class... Types>
void registerCachedValue (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("CachedValue", typeid (Types).name());

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

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("CachedValue", type);
}

void registerJuceDataStructuresBindings (py::module_& m)
{
    // ============================================================================================ juce::UndoableAction

    py::class_<UndoableAction, PyUndoableAction> classUndoableAction (m, "UndoableAction");

    classUndoableAction
        .def (py::init<>())
        .def ("perform", &UndoableAction::perform)
        .def ("undo", &UndoableAction::undo)
        .def ("getSizeInUnits", &UndoableAction::getSizeInUnits)
        .def ("createCoalescedAction", &UndoableAction::createCoalescedAction)
    ;

    // ============================================================================================ juce::UndoManager

    py::class_<UndoManager> classUndoManager (m, "UndoManager");

    classUndoManager
        .def (py::init<int, int>(), "maxNumberOfUnitsToKeep"_a = 30000, "minimumTransactionsToKeep"_a = 30)
        .def ("clearUndoHistory", &UndoManager::clearUndoHistory)
        .def ("getNumberOfUnitsTakenUpByStoredCommands", &UndoManager::getNumberOfUnitsTakenUpByStoredCommands)
        .def ("setMaxNumberOfStoredUnits", &UndoManager::setMaxNumberOfStoredUnits)
        .def ("perform", [](UndoManager& self, py::object action)
        {
            auto pyActionPtr = action.cast<PyUndoableAction*>();
            if (pyActionPtr->isOwnershipTaken())
                py::pybind11_fail ("Ownership of the action has already been taken by an UndoManager");

            auto actionPtr = action.cast<UndoableAction*>();
            pyActionPtr->markOwnershipTaken();
            action.release();

            return self.perform (actionPtr);
        })
        .def ("perform", [](UndoManager& self, py::object action, const String& transactionName)
        {
            auto pyActionPtr = action.cast<PyUndoableAction*>();
            if (pyActionPtr->isOwnershipTaken())
                py::pybind11_fail ("Ownership of the action has already been taken by an UndoManager");

            auto actionPtr = action.cast<UndoableAction*>();
            pyActionPtr->markOwnershipTaken();
            action.release();

            return self.perform (actionPtr, transactionName);
        })
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
        .def ("getActionsInCurrentTransaction", [](const UndoManager& self)
        {
            Array<const UndoableAction*> actions;
            self.getActionsInCurrentTransaction (actions);

            py::list list;
            for (const auto* action : actions)
                list.append (action);

            return list;
        }, py::return_value_policy::reference_internal)
        .def ("getNumActionsInCurrentTransaction", &UndoManager::getNumActionsInCurrentTransaction)
        .def ("canRedo", &UndoManager::canRedo)
        .def ("redo", &UndoManager::redo)
        .def ("getRedoDescription", &UndoManager::getRedoDescription)
        .def ("getRedoDescriptions", &UndoManager::getRedoDescriptions)
        .def ("getTimeOfRedoTransaction", &UndoManager::getTimeOfRedoTransaction)
        .def ("isPerformingUndoRedo", &UndoManager::isPerformingUndoRedo)
    ;

    // ============================================================================================ juce::Value

    py::class_<Value> classValue (m, "Value");
    py::class_<Value::ValueSource, ReferenceCountedObjectPtr<Value::ValueSource>, PyValueValueSource> classValueValueSource (classValue, "ValueSource");
    py::class_<Value::Listener, PyValueListener> classValueListener (classValue, "Listener");

    classValue
        .def (py::init<>())
        .def (py::init<Value::ValueSource*>())
        .def (py::init<const var&>())
        .def (py::init<const Value&>())
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
        .def (py::init([] { return ReferenceCountedObjectPtr<Value::ValueSource> (new PyValueValueSource()); }))
        .def ("getValue", &Value::ValueSource::getValue)
        .def ("setValue", &Value::ValueSource::setValue)
        .def ("sendChangeMessage", &Value::ValueSource::sendChangeMessage)
    ;

    classValueListener
        .def (py::init<>())
        .def ("valueChanged", &Value::Listener::valueChanged)
    ;

    // ============================================================================================ juce::ValueTree

    py::class_<ValueTree> classValueTree (m, "ValueTree");
    py::class_<ValueTree::Listener, PyValueTreeListener> classValueTreeListener (classValueTree, "Listener");
    py::class_<PyValueTreeComparator> classValueTreeComparator (classValueTree, "Comparator");

    classValueTreeListener.def (py::init<>());

    classValueTreeComparator
        .def (py::init<>())
        .def ("compareElements", &PyValueTreeComparator::compareElements)
    ;

    classValueTree
        .def (py::init<>())
        .def (py::init<const Identifier&>(), "type"_a)
        .def (py::init ([](const String& type) { return ValueTree (type); }), "type"_a)
        .def (py::init ([](py::str type, py::dict dict)
        {
            auto result = ValueTree (Identifier (static_cast<std::string> (type)));

            for (auto item : dict)
            {
                py::detail::make_caster<Identifier> convKey;
                py::detail::make_caster<var> convValue;

                if (! convKey.load (item.first, true))
                    py::pybind11_fail("Invalid property type of a \"ValueTree\", needs to be \"str\" or \"Identifier\"");

                if (! convValue.load (item.second, true))
                    py::pybind11_fail("Invalid property type of a \"ValueTree\", needs to be a \"var\" convertible");

                result.setProperty (
                    py::detail::cast_op<juce::Identifier&&> (std::move (convKey)),
                    py::detail::cast_op<juce::var&&> (std::move (convValue)),
                    nullptr);
            }

            return result;
        }), "type"_a, "dictionary"_a)
        .def (py::init<const ValueTree&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isEquivalentTo", &ValueTree::isEquivalentTo)
        .def ("isValid", &ValueTree::isValid)
        .def ("createCopy", &ValueTree::createCopy)
        .def ("copyPropertiesFrom", &ValueTree::copyPropertiesFrom, "source"_a, "undoManager"_a)
        .def ("copyPropertiesAndChildrenFrom", &ValueTree::copyPropertiesAndChildrenFrom, "source"_a, "undoManager"_a)
        .def ("getType", &ValueTree::getType)
        .def ("hasType", &ValueTree::hasType, "typeName"_a)
        .def ("hasType", [](const ValueTree& self, const String& type) { return self.hasType (type); }, "typeName"_a)
        .def ("getProperty", py::overload_cast<const Identifier&> (&ValueTree::getProperty, py::const_), "name"_a, py::return_value_policy::reference)
        .def ("getProperty", [](const ValueTree& self, const String& name) { return self.getProperty (name); }, "name"_a, py::return_value_policy::reference)
        .def ("getProperty", py::overload_cast<const Identifier&, const var&> (&ValueTree::getProperty, py::const_), "name"_a, "defaultReturnValue"_a, py::return_value_policy::reference)
        .def ("getProperty", [](const ValueTree& self, const String& name, const var& defaultReturnValue) { return self.getProperty (name, defaultReturnValue); }, "name"_a, "defaultReturnValue"_a, py::return_value_policy::reference)
        .def ("__getitem__", [](const ValueTree& self, const Identifier& name) { return self[name]; })
        .def ("__getitem__", [](const ValueTree& self, const String& name) { return self[name]; })
        .def ("setProperty", &ValueTree::setProperty, "name"_a, "newValue"_a, "undoManager"_a, py::return_value_policy::reference)
        .def ("setProperty", [](ValueTree& self, const String& name, const var& newValue, UndoManager* undoManager) { return self.setProperty (name, newValue, undoManager); }, "name"_a, "newValue"_a, "undoManager"_a, py::return_value_policy::reference)
        .def ("hasProperty", &ValueTree::hasProperty, "name"_a)
        .def ("hasProperty", [](const ValueTree& self, const String& name) { return self.hasProperty (name); }, "name"_a)
        .def ("removeProperty", &ValueTree::removeProperty, "name"_a, "undoManager"_a)
        .def ("removeProperty", [](ValueTree& self, const String& name, UndoManager* undoManager) { self.removeProperty (name, undoManager); }, "name"_a, "undoManager"_a)
        .def ("removeAllProperties", &ValueTree::removeAllProperties, "undoManager"_a)
        .def ("getNumProperties", &ValueTree::getNumProperties)
        .def ("getPropertyName", &ValueTree::getPropertyName, "index"_a)
        .def ("getPropertyAsValue", &ValueTree::getPropertyAsValue, "name"_a, "undoManager"_a, "shouldUpdateSynchronously"_a = false)
        .def ("getNumChildren", &ValueTree::getNumChildren)
        .def ("getChild", &ValueTree::getChild, "index"_a)
        .def ("getChildWithName", &ValueTree::getChildWithName, "type"_a)
        .def ("getChildWithName", [](const ValueTree& self, const String& name) { return self.getChildWithName (name); }, "type"_a)
        .def ("getOrCreateChildWithName", &ValueTree::getOrCreateChildWithName, "type"_a, "undoManager"_a)
        .def ("getOrCreateChildWithName", [](ValueTree& self, const String& name, UndoManager* undoManager) { return self.getOrCreateChildWithName (name, undoManager); }, "type"_a, "undoManager"_a)
        .def ("getChildWithProperty", &ValueTree::getChildWithProperty, "propertyName"_a, "propertyValue"_a)
        .def ("getChildWithProperty", [](const ValueTree& self, const String& propertyName, const var& propertyValue) { return self.getChildWithProperty (propertyName, propertyValue); }, "propertyName"_a, "propertyValue"_a)
        .def ("addChild", &ValueTree::addChild, "child"_a, "index"_a, "undoManager"_a)
        .def ("appendChild", &ValueTree::appendChild, "child"_a, "undoManager"_a)
        .def ("removeChild", py::overload_cast<const ValueTree&, UndoManager*> (&ValueTree::removeChild), "child"_a, "undoManager"_a)
        .def ("removeChild", py::overload_cast<int, UndoManager*> (&ValueTree::removeChild), "childIndex"_a, "undoManager"_a)
        .def ("removeAllChildren", &ValueTree::removeAllChildren, "undoManager"_a)
        .def ("moveChild", &ValueTree::moveChild, "currentIndex"_a, "newIndex"_a, "undoManager"_a)
        .def ("isAChildOf", &ValueTree::isAChildOf, "possibleParent"_a)
        .def ("indexOf", &ValueTree::indexOf, "child"_a)
        .def ("getParent", &ValueTree::getParent)
        .def ("getRoot", &ValueTree::getRoot)
        .def ("getSibling", &ValueTree::getSibling, "delta"_a)
        .def ("__iter__", [](const ValueTree& self) { return py::make_iterator (self.begin(), self.end()); }, py::keep_alive<0, 1>())
        .def ("createXml", &ValueTree::createXml)
        .def_static ("fromXml", static_cast<ValueTree (*)(const XmlElement&)> (&ValueTree::fromXml), "xml"_a)
        .def_static ("fromXml", static_cast<ValueTree (*)(const String&)> (&ValueTree::fromXml), "xmlText"_a)
        .def ("toXmlString", &ValueTree::toXmlString, "format"_a = XmlElement::TextFormat())
        .def ("writeToStream", &ValueTree::writeToStream, "output"_a)
        .def_static ("readFromStream", &ValueTree::readFromStream, "input"_a)
        .def_static ("readFromData", [](py::buffer data)
        {
            auto info = data.request();
            return ValueTree::readFromData (info.ptr, static_cast<size_t> (info.size));
        }, "data"_a)
        .def_static ("readFromGZIPData", [](py::buffer data)
        {
            auto info = data.request();
            return ValueTree::readFromGZIPData (info.ptr, static_cast<size_t> (info.size));
        }, "data"_a)
        .def ("addListener", &ValueTree::addListener, "listener"_a)
        .def ("removeListener", &ValueTree::removeListener, "listener"_a)
        .def ("setPropertyExcludingListener", &ValueTree::setPropertyExcludingListener,
            "listenerToExclude"_a, "name"_a, "newValue"_a, "undoManager"_a, py::return_value_policy::reference)
        .def ("sendPropertyChangeMessage", &ValueTree::sendPropertyChangeMessage, "property"_a)
        .def ("sendPropertyChangeMessage", [](ValueTree& self, const String& property) { self.sendPropertyChangeMessage (property); }, "property"_a)
        .def ("sort", &ValueTree::template sort<PyValueTreeComparator>,
            "comparator"_a, "undoManager"_a, "retainOrderOfEquivalentItems"_a)
        .def ("getReferenceCount", &ValueTree::getReferenceCount)
    ;

    // ============================================================================================ juce::CachedValue

    registerCachedValue<CachedValue, bool, int, float, String> (m);

    // ============================================================================================ juce::ValueTreeSynchroniser

    py::class_<ValueTreeSynchroniser, PyValueTreeSynchroniser> classValueTreeSynchroniser (m, "ValueTreeSynchroniser");

    classValueTreeSynchroniser
        .def (py::init<const ValueTree&>())
        .def ("stateChanged", popsicle::Helpers::makeVoidPointerAndSizeCallable<ValueTreeSynchroniser> (&ValueTreeSynchroniser::stateChanged))
        .def ("sendFullSyncCallback", &ValueTreeSynchroniser::sendFullSyncCallback)
        .def_static ("applyChange", [](ValueTree& root, py::buffer data, UndoManager* undoManager)
        {
            auto info = data.request();
            return ValueTreeSynchroniser::applyChange (root, info.ptr, static_cast<size_t> (info.size), undoManager);
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
