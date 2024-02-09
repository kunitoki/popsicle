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

#pragma once

#if !JUCE_MODULE_AVAILABLE_juce_data_structures
 #error This binding file requires adding the juce_data_structures module in the project
#else
 #include <juce_data_structures/juce_data_structures.h>
#endif

#include "../utilities/PyBind11Includes.h"

#include <cstddef>

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceDataStructuresBindings (pybind11::module_& m);

// =================================================================================================

struct PyUndoableAction : public juce::UndoableAction
{
    using juce::UndoableAction::UndoableAction;

    bool perform() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::UndoableAction, perform);
    }

    bool undo() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::UndoableAction, undo);
    }

    int getSizeInUnits() override
    {
        PYBIND11_OVERRIDE (int, juce::UndoableAction, getSizeInUnits);
    }

    juce::UndoableAction* createCoalescedAction (juce::UndoableAction* nextAction) override
    {
        PYBIND11_OVERRIDE (juce::UndoableAction*, juce::UndoableAction, createCoalescedAction, nextAction);
    }

    bool isOwnershipTaken() const noexcept
    {
        return takenOwnership;
    }

    void markOwnershipTaken() noexcept
    {
        takenOwnership = true;
    }

private:
    bool takenOwnership = false;
};

// =================================================================================================

struct PyValueValueSource : public juce::Value::ValueSource
{
    using juce::Value::ValueSource::ValueSource;

    juce::var getValue () const override
    {
        PYBIND11_OVERRIDE_PURE (juce::var, juce::Value::ValueSource, getValue);
    }

    void setValue (const juce::var& newValue) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Value::ValueSource, setValue, newValue);
    }
};

// =================================================================================================

struct PyValueListener : public juce::Value::Listener
{
    using juce::Value::Listener::Listener;

    void valueChanged (juce::Value& value) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Value::Listener, valueChanged, value);
    }
};

// =================================================================================================

struct PyValueTreeListener : public juce::ValueTree::Listener
{
    using juce::ValueTree::Listener::Listener;

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreePropertyChanged, treeWhosePropertyHasChanged, property);
    }

    void valueTreeChildAdded (juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreeChildAdded, parentTree, childWhichHasBeenAdded);
    }

    void valueTreeChildRemoved (juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreeChildRemoved, parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved);
    }

    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreeChildOrderChanged, parentTreeWhoseChildrenHaveMoved, oldIndex, newIndex);
    }

    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreeParentChanged, treeWhoseParentHasChanged);
    }

    void valueTreeRedirected (juce::ValueTree& treeWhichHasBeenChanged) override
    {
        PYBIND11_OVERRIDE (void, juce::ValueTree::Listener, valueTreeRedirected, treeWhichHasBeenChanged);
    }
};

// =================================================================================================

struct PyValueTreeComparator
{
    PyValueTreeComparator() = default;

    int compareElements (const juce::ValueTree& first, const juce::ValueTree& second)
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<PyValueTreeComparator*> (this), "compareElements"); override_)
        {
            auto result = override_ (first, second);

            return result.cast<int>();
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"ValueTree::Comparator::compareElements\"");
    }
};

// =================================================================================================

struct PyValueTreeSynchroniser : public juce::ValueTreeSynchroniser
{
    using juce::ValueTreeSynchroniser::ValueTreeSynchroniser;

    void stateChanged (const void* encodedChange, size_t encodedChangeSize) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<juce::ValueTreeSynchroniser*> (this), "stateChanged"); override_)
        {
            auto change = pybind11::memoryview::from_memory (encodedChange, static_cast<Py_ssize_t> (encodedChangeSize));

            override_ (change, encodedChangeSize);

            return;
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"ValueTreeSynchroniser::stateChanged\"");
    }
};

} // namespace popsicle::Bindings
