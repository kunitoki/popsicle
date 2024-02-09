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

#include "ScriptJuceAudioBasicsBindings.h"
#include "../utilities/ClassDemangling.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#include "../utilities/PyBind11Includes.h"

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace pybind11::literals;

// ============================================================================================

template <template <class> class Class, class... Types>
void registerAudioBuffer (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("AudioBuffer", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8(), py::buffer_protocol())
            .def (py::init<>())
            .def (py::init<int, int>(), "numChannels"_a, "numSamples"_a)
        //.def (py::init ([](py::buffer dataToReferTo, int numChannelsToUse, int startSample, int numSamples) {})
            .def ("getNumChannels", &T::getNumChannels)
            .def ("getNumSamples", &T::getNumSamples)
            .def ("getReadPointer", [](const T& self, int channelNumber)
            {
                return PyArrayView<const ValueType> (self.getReadPointer (channelNumber), static_cast<size_t> (self.getNumSamples()));
            }, "channelNumber"_a)
            .def ("getReadPointer", [](const T& self, int channelNumber, int sampleIndex)
            {
                return PyArrayView<const ValueType> (self.getReadPointer (channelNumber, sampleIndex), static_cast<size_t> (self.getNumSamples() - sampleIndex));
            }, "channelNumber"_a, "sampleIndex"_a)
            .def ("getWritePointer", [](T& self, int channelNumber)
            {
                return PyArrayView<ValueType> (self.getWritePointer (channelNumber), static_cast<size_t> (self.getNumSamples()));
            }, "channelNumber"_a)
            .def ("getWritePointer", [](T& self, int channelNumber, int sampleIndex)
            {
                return PyArrayView<ValueType> (self.getWritePointer (channelNumber, sampleIndex), static_cast<size_t> (self.getNumSamples() - sampleIndex));
            }, "channelNumber"_a, "sampleIndex"_a)
            .def ("getArrayOfReadPointers", [](const T& self)
            {
                py::list result (self.getNumChannels());

                auto pointersArray = self.getArrayOfReadPointers();
                for (size_t i = 0; i < static_cast<size_t> (self.getNumChannels()); ++i)
                    result[i] = PyArrayView<const ValueType> (pointersArray[i], static_cast<size_t> (self.getNumSamples()));

                return result;
            })
            .def ("getArrayOfWritePointers", [](T& self)
            {
                py::list result (self.getNumChannels());

                auto pointersArray = self.getArrayOfWritePointers();
                for (size_t i = 0; i < static_cast<size_t> (self.getNumChannels()); ++i)
                    result[i] = PyArrayView<ValueType> (pointersArray[i], static_cast<size_t> (self.getNumSamples()));

                return result;
            })
            .def ("setSize", &T::setSize,
                "newNumChannels"_a, "newNumSamples"_a, "keepExistingContent"_a = false, "clearExtraSpace"_a = false, "avoidReallocating"_a = false)
        //.def ("setDataToReferTo", py::overload_cast<ValueType* const*, int, int, int> (&T::setDataToReferTo))
        //.def ("setDataToReferTo", py::overload_cast<ValueType* const*, int, int> (&T::setDataToReferTo))
            .def ("makeCopyOf", &T::template makeCopyOf<ValueType>, "other"_a, "avoidReallocating"_a = false)
            .def ("clear", py::overload_cast<> (&T::clear))
            .def ("clear", py::overload_cast<int, int> (&T::clear), "startSample"_a, "numSamples"_a)
            .def ("clear", py::overload_cast<int, int, int> (&T::clear), "channel"_a, "startSample"_a, "numSamples"_a)
            .def ("hasBeenCleared", &T::hasBeenCleared)
            .def ("setNotClear", &T::setNotClear)
            .def ("getSample", &T::getSample, "channel"_a, "sampleIndex"_a)
            .def ("setSample", &T::setSample, "destChannel"_a, "destSample"_a, "newValue"_a)
            .def ("addSample", &T::addSample, "destChannel"_a, "destSample"_a, "valueToAdd"_a)
            .def ("applyGain", py::overload_cast<int, int, int, ValueType> (&T::applyGain), "channel"_a, "startSample"_a, "numSamples"_a, "gain"_a)
            .def ("applyGain", py::overload_cast<int, int, ValueType> (&T::applyGain), "startSample"_a, "numSamples"_a, "gain"_a)
            .def ("applyGain", py::overload_cast<ValueType> (&T::applyGain), "gain"_a)
            .def ("applyGainRamp", py::overload_cast<int, int, int, ValueType, ValueType> (&T::applyGainRamp), "channel"_a, "startSample"_a, "numSamples"_a, "startGain"_a, "endGain"_a)
            .def ("applyGainRamp", py::overload_cast<int, int, ValueType, ValueType> (&T::applyGainRamp), "startSample"_a, "numSamples"_a, "startGain"_a, "endGain"_a)
            .def ("addFrom", py::overload_cast<int, int, const T&, int, int, int, ValueType> (&T::addFrom),
                "destChannel"_a, "destStartSample"_a, "source"_a, "sourceChannel"_a, "sourceStartSample"_a, "numSamples"_a, "gainToApplyToSource"_a = ValueType (1))
        //.def ("addFrom", py::overload_cast<int, int, const ValueType*, int, ValueType> (&T::addFrom))
        //.def ("addFromWithRamp", &T::addFromWithRamp)
            .def ("copyFrom", py::overload_cast<int, int, const T&, int, int, int> (&T::copyFrom),
                "destChannel"_a, "destStartSample"_a, "source"_a, "sourceChannel"_a, "sourceStartSample"_a, "numSamples"_a)
        //.def ("copyFrom", py::overload_cast<int, int, const ValueType*, int> (&T::copyFrom))
        //.def ("copyFrom", py::overload_cast<int, int, const ValueType*, int, ValueType> (&T::copyFrom))
        //.def ("copyFromWithRamp", &T::copyFromWithRamp)
            .def ("findMinMax", &T::findMinMax, "channel"_a, "startSample"_a, "numSamples"_a)
            .def ("getMagnitude", py::overload_cast<int, int, int> (&T::getMagnitude, py::const_), "channel"_a, "startSample"_a, "numSamples"_a)
            .def ("getMagnitude", py::overload_cast<int, int> (&T::getMagnitude, py::const_), "startSample"_a, "numSamples"_a)
            .def ("getRMSLevel", &T::getRMSLevel, "channel"_a, "startSample"_a, "numSamples"_a)
            .def ("reverse", py::overload_cast<int, int, int> (&T::reverse, py::const_), "channel"_a, "startSample"_a, "numSamples"_a)
            .def ("reverse", py::overload_cast<int, int> (&T::reverse, py::const_), "startSample"_a, "numSamples"_a)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getNumChannels() << ", " << self.getNumSamples() << ")";
                return result;
            })
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("AudioBuffer", type);
}

// ============================================================================================

void registerJuceAudioBasicsBindings (py::module_& m)
{
    // ============================================================================================ juce::FloatArrayView

    py::class_<PyArrayView<const float>> classConstFloatArrayView (m, "ConstFloatArrayView", py::buffer_protocol());

    classConstFloatArrayView
        .def ("__getitem__", [](PyArrayView<const float>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__len__", &PyArrayView<const float>::size)
        .def ("__iter__", [](PyArrayView<const float>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<const float>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), true);
        })
    ;

    py::class_<PyArrayView<float>> classFloatArrayView (m, "FloatArrayView", py::buffer_protocol());

    classFloatArrayView
        .def ("__getitem__", [](PyArrayView<float>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__setitem__", [](PyArrayView<float>& self, size_t index, float value)
        {
            if (index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of channel data");

            *(self.data() + index) = value;
        })
        .def ("__len__", &PyArrayView<float>::size)
        .def ("__iter__", [](PyArrayView<float>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<float>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), false);
        })
    ;

    // ============================================================================================ juce::DoubleArrayView

    py::class_<PyArrayView<const double>> classConstDoubleArrayView (m, "ConstDoubleArrayView", py::buffer_protocol());

    classConstDoubleArrayView
        .def ("__getitem__", [](PyArrayView<const double>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__len__", &PyArrayView<const double>::size)
        .def ("__iter__", [](PyArrayView<const double>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<const double>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), true);
        })
    ;

    py::class_<PyArrayView<double>> classDoubleArrayView (m, "DoubleArrayView", py::buffer_protocol());

    classDoubleArrayView
        .def ("__getitem__", [](PyArrayView<double>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__setitem__", [](PyArrayView<double>& self, size_t index, double value)
        {
            if (index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of channel data");

            *(self.data() + index) = value;
        })
        .def ("__len__", &PyArrayView<double>::size)
        .def ("__iter__", [](PyArrayView<double>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<double>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), false);
        })
    ;

    // ============================================================================================ juce::IntArrayView

    py::class_<PyArrayView<const int>> classConstIntArrayView (m, "ConstIntArrayView", py::buffer_protocol());

    classConstIntArrayView
        .def ("__getitem__", [](PyArrayView<const int>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__len__", &PyArrayView<const int>::size)
        .def ("__iter__", [](PyArrayView<const int>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<const int>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), true);
        })
    ;

    py::class_<PyArrayView<int>> classIntArrayView (m, "IntArrayView", py::buffer_protocol());

    classIntArrayView
        .def ("__getitem__", [](PyArrayView<int>& self, size_t index)
        {
            if (self.data() == nullptr || index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of array data");

            return *(self.data() + index);
        })
        .def ("__setitem__", [](PyArrayView<int>& self, size_t index, int value)
        {
            if (index >= self.size())
                pybind11::pybind11_fail ("Out of bound access of channel data");

            *(self.data() + index) = value;
        })
        .def ("__len__", &PyArrayView<int>::size)
        .def ("__iter__", [](PyArrayView<int>& self)
        {
            if (self.data() == nullptr)
                pybind11::pybind11_fail ("Invalid empty array");

            return py::make_iterator (self.data(), self.data() + self.size());
        })
        .def_buffer ([](PyArrayView<int>& self) -> py::buffer_info
        {
            return py::buffer_info (self.data(), static_cast<ssize_t> (self.size()), false);
        })
    ;

    // ============================================================================================ juce::AudioBuffer

    registerAudioBuffer<AudioBuffer, float, double>(m);

    m.attr ("AudioSampleBuffer") = m.attr ("AudioBuffer")[py::type::of (py::cast (float{}))];

    // ============================================================================================ juce::AudioChannelSet

    py::class_<AudioChannelSet> classAudioChannelSet (m, "AudioChannelSet");

    py::enum_<AudioChannelSet::ChannelType> (classAudioChannelSet, "ChannelType")
        .value ("unknown", AudioChannelSet::ChannelType::unknown)
        .value ("left", AudioChannelSet::ChannelType::left)
        .value ("right", AudioChannelSet::ChannelType::right)
        .value ("centre", AudioChannelSet::ChannelType::centre)
        .value ("LFE", AudioChannelSet::ChannelType::LFE)
        .value ("leftSurround", AudioChannelSet::ChannelType::leftSurround)
        .value ("rightSurround", AudioChannelSet::ChannelType::rightSurround)
        .value ("leftCentre", AudioChannelSet::ChannelType::leftCentre)
        .value ("rightCentre", AudioChannelSet::ChannelType::rightCentre)
        .value ("centreSurround", AudioChannelSet::ChannelType::centreSurround)
        .value ("surround", AudioChannelSet::ChannelType::surround)
        .value ("leftSurroundSide", AudioChannelSet::ChannelType::leftSurroundSide)
        .value ("rightSurroundSide", AudioChannelSet::ChannelType::rightSurroundSide)
        .value ("topMiddle", AudioChannelSet::ChannelType::topMiddle)
        .value ("topFrontLeft", AudioChannelSet::ChannelType::topFrontLeft)
        .value ("topFrontCentre", AudioChannelSet::ChannelType::topFrontCentre)
        .value ("topFrontRight", AudioChannelSet::ChannelType::topFrontRight)
        .value ("topRearLeft", AudioChannelSet::ChannelType::topRearLeft)
        .value ("topRearCentre", AudioChannelSet::ChannelType::topRearCentre)
        .value ("topRearRight", AudioChannelSet::ChannelType::topRearRight)
        .value ("LFE2", AudioChannelSet::ChannelType::LFE2)
        .value ("leftSurroundRear", AudioChannelSet::ChannelType::leftSurroundRear)
        .value ("rightSurroundRear", AudioChannelSet::ChannelType::rightSurroundRear)
        .value ("wideLeft", AudioChannelSet::ChannelType::wideLeft)
        .value ("wideRight", AudioChannelSet::ChannelType::wideRight)
        .value ("topSideLeft", AudioChannelSet::ChannelType::topSideLeft)
        .value ("topSideRight", AudioChannelSet::ChannelType::topSideRight)
        .value ("ambisonicACN0", AudioChannelSet::ChannelType::ambisonicACN0)
        .value ("ambisonicACN1", AudioChannelSet::ChannelType::ambisonicACN1)
        .value ("ambisonicACN2", AudioChannelSet::ChannelType::ambisonicACN2)
        .value ("ambisonicACN3", AudioChannelSet::ChannelType::ambisonicACN3)
        .value ("ambisonicACN4", AudioChannelSet::ChannelType::ambisonicACN4)
        .value ("ambisonicACN5", AudioChannelSet::ChannelType::ambisonicACN5)
        .value ("ambisonicACN6", AudioChannelSet::ChannelType::ambisonicACN6)
        .value ("ambisonicACN7", AudioChannelSet::ChannelType::ambisonicACN7)
        .value ("ambisonicACN8", AudioChannelSet::ChannelType::ambisonicACN8)
        .value ("ambisonicACN9", AudioChannelSet::ChannelType::ambisonicACN9)
        .value ("ambisonicACN10", AudioChannelSet::ChannelType::ambisonicACN10)
        .value ("ambisonicACN11", AudioChannelSet::ChannelType::ambisonicACN11)
        .value ("ambisonicACN12", AudioChannelSet::ChannelType::ambisonicACN12)
        .value ("ambisonicACN13", AudioChannelSet::ChannelType::ambisonicACN13)
        .value ("ambisonicACN14", AudioChannelSet::ChannelType::ambisonicACN14)
        .value ("ambisonicACN15", AudioChannelSet::ChannelType::ambisonicACN15)
        .value ("ambisonicACN16", AudioChannelSet::ChannelType::ambisonicACN16)
        .value ("ambisonicACN17", AudioChannelSet::ChannelType::ambisonicACN17)
        .value ("ambisonicACN18", AudioChannelSet::ChannelType::ambisonicACN18)
        .value ("ambisonicACN19", AudioChannelSet::ChannelType::ambisonicACN19)
        .value ("ambisonicACN20", AudioChannelSet::ChannelType::ambisonicACN20)
        .value ("ambisonicACN21", AudioChannelSet::ChannelType::ambisonicACN21)
        .value ("ambisonicACN22", AudioChannelSet::ChannelType::ambisonicACN22)
        .value ("ambisonicACN23", AudioChannelSet::ChannelType::ambisonicACN23)
        .value ("ambisonicACN24", AudioChannelSet::ChannelType::ambisonicACN24)
        .value ("ambisonicACN25", AudioChannelSet::ChannelType::ambisonicACN25)
        .value ("ambisonicACN26", AudioChannelSet::ChannelType::ambisonicACN26)
        .value ("ambisonicACN27", AudioChannelSet::ChannelType::ambisonicACN27)
        .value ("ambisonicACN28", AudioChannelSet::ChannelType::ambisonicACN28)
        .value ("ambisonicACN29", AudioChannelSet::ChannelType::ambisonicACN29)
        .value ("ambisonicACN30", AudioChannelSet::ChannelType::ambisonicACN30)
        .value ("ambisonicACN31", AudioChannelSet::ChannelType::ambisonicACN31)
        .value ("ambisonicACN32", AudioChannelSet::ChannelType::ambisonicACN32)
        .value ("ambisonicACN33", AudioChannelSet::ChannelType::ambisonicACN33)
        .value ("ambisonicACN34", AudioChannelSet::ChannelType::ambisonicACN34)
        .value ("ambisonicACN35", AudioChannelSet::ChannelType::ambisonicACN35)
        .value ("ambisonicW", AudioChannelSet::ChannelType::ambisonicW)
        .value ("ambisonicX", AudioChannelSet::ChannelType::ambisonicX)
        .value ("ambisonicY", AudioChannelSet::ChannelType::ambisonicY)
        .value ("ambisonicZ", AudioChannelSet::ChannelType::ambisonicZ)
        .value ("bottomFrontLeft", AudioChannelSet::ChannelType::bottomFrontLeft)
        .value ("bottomFrontCentre", AudioChannelSet::ChannelType::bottomFrontCentre)
        .value ("bottomFrontRight", AudioChannelSet::ChannelType::bottomFrontRight)
        .value ("proximityLeft", AudioChannelSet::ChannelType::proximityLeft)
        .value ("proximityRight", AudioChannelSet::ChannelType::proximityRight)
        .value ("bottomSideLeft", AudioChannelSet::ChannelType::bottomSideLeft)
        .value ("bottomSideRight", AudioChannelSet::ChannelType::bottomSideRight)
        .value ("bottomRearLeft", AudioChannelSet::ChannelType::bottomRearLeft)
        .value ("bottomRearCentre", AudioChannelSet::ChannelType::bottomRearCentre)
        .value ("bottomRearRight", AudioChannelSet::ChannelType::bottomRearRight)
        .value ("ambisonicACN36", AudioChannelSet::ChannelType::ambisonicACN36)
        .value ("ambisonicACN37", AudioChannelSet::ChannelType::ambisonicACN37)
        .value ("ambisonicACN38", AudioChannelSet::ChannelType::ambisonicACN38)
        .value ("ambisonicACN39", AudioChannelSet::ChannelType::ambisonicACN39)
        .value ("ambisonicACN40", AudioChannelSet::ChannelType::ambisonicACN40)
        .value ("ambisonicACN41", AudioChannelSet::ChannelType::ambisonicACN41)
        .value ("ambisonicACN42", AudioChannelSet::ChannelType::ambisonicACN42)
        .value ("ambisonicACN43", AudioChannelSet::ChannelType::ambisonicACN43)
        .value ("ambisonicACN44", AudioChannelSet::ChannelType::ambisonicACN44)
        .value ("ambisonicACN45", AudioChannelSet::ChannelType::ambisonicACN45)
        .value ("ambisonicACN46", AudioChannelSet::ChannelType::ambisonicACN46)
        .value ("ambisonicACN47", AudioChannelSet::ChannelType::ambisonicACN47)
        .value ("ambisonicACN48", AudioChannelSet::ChannelType::ambisonicACN48)
        .value ("ambisonicACN49", AudioChannelSet::ChannelType::ambisonicACN49)
        .value ("ambisonicACN50", AudioChannelSet::ChannelType::ambisonicACN50)
        .value ("ambisonicACN51", AudioChannelSet::ChannelType::ambisonicACN51)
        .value ("ambisonicACN52", AudioChannelSet::ChannelType::ambisonicACN52)
        .value ("ambisonicACN53", AudioChannelSet::ChannelType::ambisonicACN53)
        .value ("ambisonicACN54", AudioChannelSet::ChannelType::ambisonicACN54)
        .value ("ambisonicACN55", AudioChannelSet::ChannelType::ambisonicACN55)
        .value ("ambisonicACN56", AudioChannelSet::ChannelType::ambisonicACN56)
        .value ("ambisonicACN57", AudioChannelSet::ChannelType::ambisonicACN57)
        .value ("ambisonicACN58", AudioChannelSet::ChannelType::ambisonicACN58)
        .value ("ambisonicACN59", AudioChannelSet::ChannelType::ambisonicACN59)
        .value ("ambisonicACN60", AudioChannelSet::ChannelType::ambisonicACN60)
        .value ("ambisonicACN61", AudioChannelSet::ChannelType::ambisonicACN61)
        .value ("ambisonicACN62", AudioChannelSet::ChannelType::ambisonicACN62)
        .value ("ambisonicACN63", AudioChannelSet::ChannelType::ambisonicACN63)
        .value ("discreteChannel0", AudioChannelSet::ChannelType::discreteChannel0)
        .export_values();

    classAudioChannelSet
        .def (py::init<>())
        .def_static ("disabled", &AudioChannelSet::disabled)
        .def_static ("mono", &AudioChannelSet::mono)
        .def_static ("stereo", &AudioChannelSet::stereo)
        .def_static ("createLCR", &AudioChannelSet::createLCR)
        .def_static ("createLRS", &AudioChannelSet::createLRS)
        .def_static ("createLCRS", &AudioChannelSet::createLCRS)
        .def_static ("create5point0", &AudioChannelSet::create5point0)
        .def_static ("create5point1", &AudioChannelSet::create5point1)
        .def_static ("create6point0", &AudioChannelSet::create6point0)
        .def_static ("create6point1", &AudioChannelSet::create6point1)
        .def_static ("create6point0Music", &AudioChannelSet::create6point0Music)
        .def_static ("create6point1Music", &AudioChannelSet::create6point1Music)
        .def_static ("create7point0", &AudioChannelSet::create7point0)
        .def_static ("create7point0SDDS", &AudioChannelSet::create7point0SDDS)
        .def_static ("create7point1", &AudioChannelSet::create7point1)
        .def_static ("create7point1SDDS", &AudioChannelSet::create7point1SDDS)
        .def_static ("create5point0point2", &AudioChannelSet::create5point0point2)
        .def_static ("create5point1point2", &AudioChannelSet::create5point1point2)
        .def_static ("create5point0point4", &AudioChannelSet::create5point0point4)
        .def_static ("create5point1point4", &AudioChannelSet::create5point1point4)
        .def_static ("create7point0point2", &AudioChannelSet::create7point0point2)
        .def_static ("create7point1point2", &AudioChannelSet::create7point1point2)
        .def_static ("create7point0point4", &AudioChannelSet::create7point0point4)
        .def_static ("create7point1point4", &AudioChannelSet::create7point1point4)
        .def_static ("create7point0point6", &AudioChannelSet::create7point0point6)
        .def_static ("create7point1point6", &AudioChannelSet::create7point1point6)
        .def_static ("create9point0point4", &AudioChannelSet::create9point0point4)
        .def_static ("create9point1point4", &AudioChannelSet::create9point1point4)
        .def_static ("create9point0point6", &AudioChannelSet::create9point0point6)
        .def_static ("create9point1point6", &AudioChannelSet::create9point1point6)
        .def_static ("quadraphonic", &AudioChannelSet::quadraphonic)
        .def_static ("pentagonal", &AudioChannelSet::pentagonal)
        .def_static ("hexagonal", &AudioChannelSet::hexagonal)
        .def_static ("octagonal", &AudioChannelSet::octagonal)
        .def_static ("ambisonic", &AudioChannelSet::ambisonic, "order"_a = 1)
        .def ("getAmbisonicOrder", &AudioChannelSet::getAmbisonicOrder)
        .def_static ("discreteChannels", &AudioChannelSet::discreteChannels)
        .def_static ("canonicalChannelSet", &AudioChannelSet::canonicalChannelSet)
        .def_static ("namedChannelSet", &AudioChannelSet::namedChannelSet)
        .def_static ("channelSetsWithNumberOfChannels", &AudioChannelSet::channelSetsWithNumberOfChannels)
        .def_static ("getChannelTypeName", &AudioChannelSet::getChannelTypeName)
        .def_static ("getAbbreviatedChannelTypeName", &AudioChannelSet::getAbbreviatedChannelTypeName)
        .def_static ("getChannelTypeFromAbbreviation", &AudioChannelSet::getChannelTypeFromAbbreviation)
        .def ("addChannel", &AudioChannelSet::addChannel)
        .def ("removeChannel", &AudioChannelSet::removeChannel)
        .def ("size", &AudioChannelSet::size)
        .def ("isDisabled", &AudioChannelSet::isDisabled)
        .def ("getChannelTypes", &AudioChannelSet::getChannelTypes)
        .def ("getTypeOfChannel", &AudioChannelSet::getTypeOfChannel)
        .def ("getChannelIndexForType", &AudioChannelSet::getChannelIndexForType)
        .def ("getSpeakerArrangementAsString", &AudioChannelSet::getSpeakerArrangementAsString)
        .def_static ("fromAbbreviatedString", &AudioChannelSet::fromAbbreviatedString)
        .def ("getDescription", &AudioChannelSet::getDescription)
        .def ("isDiscreteLayout", &AudioChannelSet::isDiscreteLayout)
        .def ("intersect", &AudioChannelSet::intersect)
        .def_static ("channelSetWithChannels", &AudioChannelSet::channelSetWithChannels)
        .def_static ("fromWaveChannelMask", &AudioChannelSet::fromWaveChannelMask)
        .def ("getWaveChannelMask", &AudioChannelSet::getWaveChannelMask)
        .def_static ("getAmbisonicOrderForNumChannels", &AudioChannelSet::getAmbisonicOrderForNumChannels)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
    ;

    classAudioChannelSet.attr ("maxChannelsOfNamedLayout") = py::int_ (static_cast<int> (AudioChannelSet::maxChannelsOfNamedLayout));

    // ============================================================================================ juce::AudioProcessLoadMeasurer

    py::class_<AudioProcessLoadMeasurer> classAudioProcessLoadMeasurer (m, "AudioProcessLoadMeasurer");

    py::class_<AudioProcessLoadMeasurer::ScopedTimer> classAudioProcessLoadMeasurerScopedTimer (classAudioProcessLoadMeasurer, "ScopedTimer");

    classAudioProcessLoadMeasurerScopedTimer
        .def (py::init<AudioProcessLoadMeasurer&>())
        .def (py::init<AudioProcessLoadMeasurer&, int>())
        // TODO __enter__ __exit__
    ;

    classAudioProcessLoadMeasurer
        .def (py::init<>())
        .def ("reset", py::overload_cast<> (&AudioProcessLoadMeasurer::reset))
        .def ("reset", py::overload_cast<double, int> (&AudioProcessLoadMeasurer::reset))
        .def ("getLoadAsProportion", &AudioProcessLoadMeasurer::getLoadAsProportion)
        .def ("getLoadAsPercentage", &AudioProcessLoadMeasurer::getLoadAsPercentage)
        .def ("getXRunCount", &AudioProcessLoadMeasurer::getXRunCount)
        .def ("registerBlockRenderTime", &AudioProcessLoadMeasurer::registerBlockRenderTime)
        .def ("registerRenderTime", &AudioProcessLoadMeasurer::registerRenderTime)
    ;

    // ============================================================================================ juce::AudioSourceChannelInfo

    py::class_<AudioSourceChannelInfo> classAudioSourceChannelInfo (m, "AudioSourceChannelInfo");

    classAudioSourceChannelInfo
        .def (py::init<>())
        .def (py::init<AudioBuffer<float>*, int, int>(), "bufferToUse"_a, "startSampleOffset"_a, "numSamplesToUse"_a)
        .def (py::init<AudioBuffer<float>&>(), "bufferToUse"_a)
        .def_readwrite ("buffer", &AudioSourceChannelInfo::buffer, py::return_value_policy::reference_internal)
        .def_readwrite ("startSample", &AudioSourceChannelInfo::startSample)
        .def_readwrite ("numSamples", &AudioSourceChannelInfo::numSamples)
        .def ("clearActiveBufferRegion", &AudioSourceChannelInfo::clearActiveBufferRegion)
    ;

    // ============================================================================================ juce::AudioSource

    py::class_<AudioSource, PyAudioSource<>> classAudioSource (m, "AudioSource");

    classAudioSource
        .def (py::init<>())
        .def ("prepareToPlay", &AudioSource::prepareToPlay)
        .def ("releaseResources", &AudioSource::releaseResources)
        .def ("getNextAudioBlock", &AudioSource::getNextAudioBlock)
    ;

    py::class_<PositionableAudioSource, AudioSource, PyPositionableAudioSource<>> classPositionableAudioSource (m, "PositionableAudioSource");

    classPositionableAudioSource
        .def (py::init<>())
        .def ("setNextReadPosition", &PositionableAudioSource::setNextReadPosition)
        .def ("getNextReadPosition", &PositionableAudioSource::getNextReadPosition)
        .def ("getTotalLength", &PositionableAudioSource::getTotalLength)
        .def ("isLooping", &PositionableAudioSource::isLooping)
        .def ("setLooping", &PositionableAudioSource::setLooping)
    ;

    py::class_<BufferingAudioSource, PositionableAudioSource, PyPositionableAudioSource<BufferingAudioSource>> classBufferingAudioSource (m, "BufferingAudioSource");

    classBufferingAudioSource
        .def (py::init<PositionableAudioSource*, TimeSliceThread&, bool, int, int, bool>(),
            "source"_a, "backgroundThread"_a, "deleteSourceWhenDeleted"_a, "numberOfSamplesToBuffer"_a, "numberOfChannels"_a = 2, "prefillBufferOnPrepareToPlay"_a = true)
        .def ("waitForNextAudioBlockReady", &BufferingAudioSource::waitForNextAudioBlockReady)
    ;

    py::class_<ChannelRemappingAudioSource, AudioSource, PyAudioSource<ChannelRemappingAudioSource>> classChannelRemappingAudioSource (m, "ChannelRemappingAudioSource");

    classChannelRemappingAudioSource
        .def (py::init<AudioSource*, bool>(), "source"_a, "deleteSourceWhenDeleted"_a = false)
        .def ("setNumberOfChannelsToProduce", &ChannelRemappingAudioSource::setNumberOfChannelsToProduce)
        .def ("clearAllMappings", &ChannelRemappingAudioSource::clearAllMappings)
        .def ("setInputChannelMapping", &ChannelRemappingAudioSource::setInputChannelMapping)
        .def ("setOutputChannelMapping", &ChannelRemappingAudioSource::setOutputChannelMapping)
        .def ("getRemappedInputChannel", &ChannelRemappingAudioSource::getRemappedInputChannel)
        .def ("getRemappedOutputChannel", &ChannelRemappingAudioSource::getRemappedOutputChannel)
        .def ("createXml", &ChannelRemappingAudioSource::createXml)
        .def ("restoreFromXml", &ChannelRemappingAudioSource::restoreFromXml)
    ;

    py::class_<IIRFilterAudioSource, AudioSource, PyAudioSource<IIRFilterAudioSource>> classIIRFilterAudioSource (m, "IIRFilterAudioSource");

    classIIRFilterAudioSource
        .def (py::init<AudioSource*, bool>(), "source"_a, "deleteSourceWhenDeleted"_a = false)
    //.def ("setCoefficients", &IIRFilterAudioSource::setCoefficients)
        .def ("makeInactive", &IIRFilterAudioSource::makeInactive)
    ;

    py::class_<MemoryAudioSource, PositionableAudioSource, PyPositionableAudioSource<MemoryAudioSource>> classMemoryAudioSource (m, "MemoryAudioSource");

    classMemoryAudioSource
        .def (py::init<AudioBuffer<float>&, bool, bool>(), "audioBuffer"_a, "copyMemory"_a, "shouldLoop"_a = false)
    ;

    py::class_<MixerAudioSource, AudioSource, PyAudioSource<MixerAudioSource>> classMixerAudioSource (m, "MixerAudioSource");

    classMixerAudioSource
        .def (py::init<>())
        .def ("addInputSource", &MixerAudioSource::addInputSource)
        .def ("removeInputSource", &MixerAudioSource::removeInputSource)
        .def ("removeAllInputs", &MixerAudioSource::removeAllInputs)
    ;

    py::class_<ResamplingAudioSource, AudioSource, PyAudioSource<ResamplingAudioSource>> classResamplingAudioSource (m, "ResamplingAudioSource");

    classResamplingAudioSource
        .def (py::init<AudioSource*, bool, int>(), "inputSource"_a, "deleteSourceWhenDeleted"_a = false, "numChannels"_a = 2)
        .def ("setResamplingRatio", &ResamplingAudioSource::setResamplingRatio)
        .def ("getResamplingRatio", &ResamplingAudioSource::getResamplingRatio)
        .def ("flushBuffers", &ResamplingAudioSource::flushBuffers)
    ;

    py::class_<ReverbAudioSource, AudioSource, PyAudioSource<ReverbAudioSource>> classReverbAudioSource (m, "ReverbAudioSource");

    classReverbAudioSource
        .def (py::init<AudioSource*, bool>(), "inputSource"_a, "deleteSourceWhenDeleted"_a = false)
    //.def ("getParameters", &ReverbAudioSource::getParameters)
    //.def ("setParameters", &ReverbAudioSource::setParameters)
        .def ("setBypassed", &ReverbAudioSource::setBypassed)
        .def ("isBypassed", &ReverbAudioSource::isBypassed)
    ;

    py::class_<ToneGeneratorAudioSource, AudioSource, PyAudioSource<ToneGeneratorAudioSource>> classToneGeneratorAudioSource (m, "ToneGeneratorAudioSource");

    classToneGeneratorAudioSource
        .def (py::init<>())
        .def ("setAmplitude", &ToneGeneratorAudioSource::setAmplitude)
        .def ("setFrequency", &ToneGeneratorAudioSource::setFrequency)
    ;

    // ============================================================================================ juce::AudioPlayHead

    py::class_<AudioPlayHead, PyAudioPlayHead> classAudioPlayHead (m, "AudioPlayHead");

    py::enum_<AudioPlayHead::FrameRateType> (classAudioPlayHead, "FrameRateType")
        .value ("fps23976", AudioPlayHead::FrameRateType::fps23976)
        .value ("fps24", AudioPlayHead::FrameRateType::fps24)
        .value ("fps25", AudioPlayHead::FrameRateType::fps25)
        .value ("fps2997", AudioPlayHead::FrameRateType::fps2997)
        .value ("fps30", AudioPlayHead::FrameRateType::fps30)
        .value ("fps2997drop", AudioPlayHead::FrameRateType::fps2997drop)
        .value ("fps30drop", AudioPlayHead::FrameRateType::fps30drop)
        .value ("fps60", AudioPlayHead::FrameRateType::fps60)
        .value ("fps60drop", AudioPlayHead::FrameRateType::fps60drop)
        .value ("fpsUnknown", AudioPlayHead::FrameRateType::fpsUnknown)
        .export_values();

    py::class_<AudioPlayHead::FrameRate> classAudioPlayHeadFrameRate (classAudioPlayHead, "FrameRate");

    classAudioPlayHeadFrameRate
        .def (py::init<>())
        .def (py::init<AudioPlayHead::FrameRateType>())
        .def ("getType", &AudioPlayHead::FrameRate::getType)
        .def ("getBaseRate", &AudioPlayHead::FrameRate::getBaseRate)
        .def ("isDrop", &AudioPlayHead::FrameRate::isDrop)
        .def ("isPullDown", &AudioPlayHead::FrameRate::isPullDown)
        .def ("getEffectiveRate", &AudioPlayHead::FrameRate::getEffectiveRate)
        .def ("withBaseRate", &AudioPlayHead::FrameRate::withBaseRate)
        .def ("withDrop", &AudioPlayHead::FrameRate::withDrop)
        .def ("withPullDown", &AudioPlayHead::FrameRate::withPullDown)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    py::class_<AudioPlayHead::TimeSignature> classAudioPlayHeadTimeSignature (classAudioPlayHead, "TimeSignature");

    classAudioPlayHeadTimeSignature
        .def (py::init<>())
        .def_readwrite ("numerator", &AudioPlayHead::TimeSignature::numerator)
        .def_readwrite ("denominator", &AudioPlayHead::TimeSignature::denominator)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    py::class_<AudioPlayHead::LoopPoints> classAudioPlayHeadLoopPoints (classAudioPlayHead, "LoopPoints");

    classAudioPlayHeadLoopPoints
        .def (py::init<>())
        .def_readwrite ("ppqStart", &AudioPlayHead::LoopPoints::ppqStart)
        .def_readwrite ("ppqEnd", &AudioPlayHead::LoopPoints::ppqEnd)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    py::class_<AudioPlayHead::CurrentPositionInfo> classAudioPlayHeadCurrentPositionInfo (classAudioPlayHead, "CurrentPositionInfo");

    classAudioPlayHeadCurrentPositionInfo
        .def (py::init<>())
        .def_readwrite ("bpm", &AudioPlayHead::CurrentPositionInfo::bpm)
        .def_readwrite ("timeSigNumerator", &AudioPlayHead::CurrentPositionInfo::timeSigNumerator)
        .def_readwrite ("timeSigDenominator", &AudioPlayHead::CurrentPositionInfo::timeSigDenominator)
        .def_readwrite ("timeInSamples", &AudioPlayHead::CurrentPositionInfo::timeInSamples)
        .def_readwrite ("timeInSeconds", &AudioPlayHead::CurrentPositionInfo::timeInSeconds)
        .def_readwrite ("editOriginTime", &AudioPlayHead::CurrentPositionInfo::editOriginTime)
        .def_readwrite ("ppqPosition", &AudioPlayHead::CurrentPositionInfo::ppqPosition)
        .def_readwrite ("ppqPositionOfLastBarStart", &AudioPlayHead::CurrentPositionInfo::ppqPositionOfLastBarStart)
        .def_readwrite ("frameRate", &AudioPlayHead::CurrentPositionInfo::frameRate)
        .def_readwrite ("isPlaying", &AudioPlayHead::CurrentPositionInfo::isPlaying)
        .def_readwrite ("isRecording", &AudioPlayHead::CurrentPositionInfo::isRecording)
        .def_readwrite ("ppqLoopStart", &AudioPlayHead::CurrentPositionInfo::ppqLoopStart)
        .def_readwrite ("ppqLoopEnd", &AudioPlayHead::CurrentPositionInfo::ppqLoopEnd)
        .def_readwrite ("isLooping", &AudioPlayHead::CurrentPositionInfo::isLooping)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("resetToDefault", &AudioPlayHead::CurrentPositionInfo::resetToDefault)
    ;

    py::class_<AudioPlayHead::PositionInfo> classAudioPlayHeadPositionInfo (classAudioPlayHead, "PositionInfo");

    classAudioPlayHeadPositionInfo
        .def (py::init<>())
        .def ("getTimeInSamples", &AudioPlayHead::PositionInfo::getTimeInSamples)
        .def ("setTimeInSamples", &AudioPlayHead::PositionInfo::setTimeInSamples)
        .def ("getTimeInSeconds", &AudioPlayHead::PositionInfo::getTimeInSeconds)
        .def ("setTimeInSeconds", &AudioPlayHead::PositionInfo::setTimeInSeconds)
        .def ("getBpm", &AudioPlayHead::PositionInfo::getBpm)
        .def ("setBpm", &AudioPlayHead::PositionInfo::setBpm)
        .def ("getTimeSignature", &AudioPlayHead::PositionInfo::getTimeSignature)
        .def ("setTimeSignature", &AudioPlayHead::PositionInfo::setTimeSignature)
        .def ("getLoopPoints", &AudioPlayHead::PositionInfo::getLoopPoints)
        .def ("setLoopPoints", &AudioPlayHead::PositionInfo::setLoopPoints)
        .def ("getBarCount", &AudioPlayHead::PositionInfo::getBarCount)
        .def ("setBarCount", &AudioPlayHead::PositionInfo::setBarCount)
        .def ("getPpqPositionOfLastBarStart", &AudioPlayHead::PositionInfo::getPpqPositionOfLastBarStart)
        .def ("setPpqPositionOfLastBarStart", &AudioPlayHead::PositionInfo::setPpqPositionOfLastBarStart)
        .def ("getFrameRate", &AudioPlayHead::PositionInfo::getFrameRate)
        .def ("setFrameRate", &AudioPlayHead::PositionInfo::setFrameRate)
        .def ("getPpqPosition", &AudioPlayHead::PositionInfo::getPpqPosition)
        .def ("setPpqPosition", &AudioPlayHead::PositionInfo::setPpqPosition)
        .def ("getEditOriginTime", &AudioPlayHead::PositionInfo::getEditOriginTime)
        .def ("setEditOriginTime", &AudioPlayHead::PositionInfo::setEditOriginTime)
        .def ("getHostTimeNs", &AudioPlayHead::PositionInfo::getHostTimeNs)
        .def ("setHostTimeNs", &AudioPlayHead::PositionInfo::setHostTimeNs)
        .def ("getIsPlaying", &AudioPlayHead::PositionInfo::getIsPlaying)
        .def ("setIsPlaying", &AudioPlayHead::PositionInfo::setIsPlaying)
        .def ("getIsRecording", &AudioPlayHead::PositionInfo::getIsRecording)
        .def ("setIsRecording", &AudioPlayHead::PositionInfo::setIsRecording)
        .def ("getIsLooping", &AudioPlayHead::PositionInfo::getIsLooping)
        .def ("setIsLooping", &AudioPlayHead::PositionInfo::setIsLooping)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    classAudioPlayHead
        .def (py::init<>())
    //.def ("getCurrentPosition", &AudioPlayHead::getCurrentPosition)
        .def ("getPosition", &AudioPlayHead::getPosition)
        .def ("canControlTransport", &AudioPlayHead::canControlTransport)
        .def ("transportPlay", &AudioPlayHead::transportPlay)
        .def ("transportRecord", &AudioPlayHead::transportRecord)
        .def ("transportRewind", &AudioPlayHead::transportRewind)
    ;

    // ============================================================================================ juce::Decibels

    py::class_<Decibels> classDecibels (m, "Decibels");

    classDecibels
        .def_static ("decibelsToGain", &Decibels::template decibelsToGain<float>, "decibels"_a, "minusInfinityDb"_a = -100.0f)
        .def_static ("gainToDecibels", &Decibels::template gainToDecibels<float>, "gain"_a, "minusInfinityDb"_a = -100.0f)
        .def_static ("gainWithLowerBound", &Decibels::template gainWithLowerBound<float>, "gain"_a, "lowerBoundDb"_a)
        .def_static ("toString", &Decibels::template toString<float>, "decibels"_a, "decimalPlaces"_a = 2, "minusInfinityDb"_a = -100.0f, "shouldIncludeSuffix"_a = true, "customMinusInfinityString"_a = String())
    ;
}

} // namespace popsicle::Bindings
