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

#include "ScriptJuceCoreBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../utilities/CrashHandling.h"

#include <optional>
#include <string_view>

namespace PYBIND11_NAMESPACE {
namespace detail {

// =================================================================================================

bool type_caster<juce::StringRef>::load (handle src, bool convert)
{
    juce::ignoreUnused (convert);

    if (! src)
        return false;

    if (! PyUnicode_Check (src.ptr()))
        return load_raw(src);

    Py_ssize_t size = -1;
    const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (src.ptr(), &size));
    if (buffer == nullptr)
        return false;

    value = buffer;

    loader_life_support::add_patient (src);

    return true;
}

handle type_caster<juce::StringRef>::cast (const juce::StringRef& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (static_cast<const char*> (src.text), static_cast<Py_ssize_t> (src.length()));
}

bool type_caster<juce::StringRef>::load_raw (handle src)
{
    if (PYBIND11_BYTES_CHECK (src.ptr()))
    {
        const char* bytes = PYBIND11_BYTES_AS_STRING (src.ptr());
        if (! bytes)
            pybind11_fail ("Unexpected PYBIND11_BYTES_AS_STRING() failure.");

        value = bytes;
        return true;
    }

    if (PyByteArray_Check (src.ptr()))
    {
        const char* bytearray = PyByteArray_AsString (src.ptr());
        if (! bytearray)
            pybind11_fail ("Unexpected PyByteArray_AsString() failure.");

        value = bytearray;
        return true;
    }

    return false;
}

// =================================================================================================

bool type_caster<juce::String>::load (handle src, bool convert)
{
    juce::ignoreUnused (convert);

    if (! src)
        return false;

    if (! PyUnicode_Check (src.ptr()))
        return load_raw(src);

    Py_ssize_t size = -1;
    const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (src.ptr(), &size));
    if (buffer == nullptr)
        return false;

    value = juce::String::fromUTF8 (buffer, static_cast<int> (size));
    return true;
}

handle type_caster<juce::String>::cast (const juce::String& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return PyUnicode_FromStringAndSize (src.toRawUTF8(), static_cast<Py_ssize_t> (src.getNumBytesAsUTF8()));
}

bool type_caster<juce::String>::load_raw (handle src)
{
    if (PYBIND11_BYTES_CHECK (src.ptr()))
    {
        const char* bytes = PYBIND11_BYTES_AS_STRING (src.ptr());
        if (! bytes)
            pybind11_fail ("Unexpected PYBIND11_BYTES_AS_STRING() failure.");

        value = juce::String::fromUTF8 (bytes, static_cast<int> (PYBIND11_BYTES_SIZE (src.ptr())));
        return true;
    }

    if (PyByteArray_Check (src.ptr()))
    {
        const char* bytearray = PyByteArray_AsString (src.ptr());
        if (! bytearray)
            pybind11_fail ("Unexpected PyByteArray_AsString() failure.");

        value = juce::String::fromUTF8 (bytearray, static_cast<int> (PyByteArray_Size (src.ptr())));
        return true;
    }

    return false;
}

// =================================================================================================

bool type_caster<juce::Identifier>::load (handle src, bool convert)
{
    if (! src)
        return false;

    if (base_type::load (src, convert))
    {
        value = *reinterpret_cast<juce::Identifier*> (base_type::value);
        return true;
    }

    if (! PyUnicode_Check (src.ptr()))
        return load_raw(src);

    Py_ssize_t size = -1;
    const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (src.ptr(), &size));
    if (buffer == nullptr)
        return false;

    value = juce::Identifier (juce::String::fromUTF8 (buffer, static_cast<int> (size)));
    return true;
}

handle type_caster<juce::Identifier>::cast (const juce::Identifier& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    if (auto result = base_type::cast (src, policy, parent))
        return result;

    return make_caster<juce::String>::cast (src.toString(), policy, parent);
}

bool type_caster<juce::Identifier>::load_raw (handle src)
{
    if (PYBIND11_BYTES_CHECK (src.ptr()))
    {
        const char* bytes = PYBIND11_BYTES_AS_STRING (src.ptr());
        if (! bytes)
            pybind11_fail ("Unexpected PYBIND11_BYTES_AS_STRING() failure.");

        value = juce::Identifier (juce::String::fromUTF8 (bytes, static_cast<int> (PYBIND11_BYTES_SIZE (src.ptr()))));
        return true;
    }

    if (PyByteArray_Check (src.ptr()))
    {
        const char* bytearray = PyByteArray_AsString (src.ptr());
        if (! bytearray)
            pybind11_fail ("Unexpected PyByteArray_AsString() failure.");

        value = juce::Identifier (juce::String::fromUTF8 (bytearray, static_cast<int> (PyByteArray_Size (src.ptr()))));
        return true;
    }

    return false;
}

// =================================================================================================

bool type_caster<juce::var>::load (handle src, bool convert)
{
    PyObject* source = src.ptr();

    if (PyNone_Check(source))
        value = juce::var::undefined();

    else if (PyBool_Check(source))
        value = PyObject_IsTrue (source) ? true : false;

    else if (PyLong_Check (source))
        value = static_cast<int> (PyLong_AsLong (source));

    else if (PyFloat_Check (source))
        value = PyFloat_AsDouble (source);

    else if (PyUnicode_Check (source))
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (src.ptr(), &size));
        if (buffer == nullptr)
            return false;

        value = juce::String::fromUTF8 (buffer, static_cast<int> (size));
    }

    else if (PYBIND11_BYTES_CHECK (source))
    {
        const char* bytes = PYBIND11_BYTES_AS_STRING (source);
        if (! bytes)
            return false;

        value = juce::var (bytes, static_cast<size_t> (PYBIND11_BYTES_SIZE (source)));
    }

    else if (PyByteArray_Check (source))
    {
        const char* bytearray = PyByteArray_AsString (source);
        if (! bytearray)
            return false;

        value = juce::var (bytearray, static_cast<size_t> (PyByteArray_Size (source)));
    }

    else if (PyTuple_Check (source))
    {
        value = juce::var();

        const auto tupleSize = PyTuple_Size(source);
        for (Py_ssize_t i = 0; i < tupleSize; ++i)
        {
            make_caster<juce::var> conv;

            if (! conv.load (PyTuple_GetItem(source, i), convert))
                return false;

            value.append (cast_op<juce::var&&> (std::move (conv)));
        }
    }

    else if (PyList_Check (source))
    {
        value = juce::var();

        const auto tupleSize = PyList_Size(source);
        for (Py_ssize_t i = 0; i < tupleSize; ++i)
        {
            make_caster<juce::var> conv;

            if (! conv.load (PyList_GetItem(source, i), convert))
                return false;

            value.append (cast_op<juce::var&&> (std::move (conv)));
        }
    }

    else if (PyDict_Check (source))
    {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject;

        value = juce::var (obj.get());

        PyObject* key;
        PyObject* val;
        Py_ssize_t pos = 0;

        while (PyDict_Next (source, &pos, &key, &val))
        {
            make_caster<juce::Identifier> convKey;
            make_caster<juce::var> convValue;

            if (! convKey.load (key, convert) || ! convValue.load (val, convert))
                return false;

            obj->setProperty (
                cast_op<juce::Identifier&&> (std::move (convKey)),
                cast_op<juce::var&&> (std::move (convValue)));
        }
    }

    else if (isinstance<juce::MemoryBlock&> (src))
    {
        value = juce::var (src.cast<juce::MemoryBlock&>());
    }

    else
    {
        value = juce::var::undefined();
    }

    // TODO - raise

    return !PyErr_Occurred();
}

handle type_caster<juce::var>::cast (const juce::var& src, return_value_policy policy, handle parent)
{
    if (src.isVoid() || src.isUndefined())
        return Py_None;

    if (src.isBool())
        return PyBool_FromLong (static_cast<bool> (src));

    if (src.isInt())
        return PyLong_FromLong (static_cast<int> (src));

    if (src.isInt64())
        return PyLong_FromLongLong (static_cast<juce::int64> (src));

    if (src.isDouble())
        return PyFloat_FromDouble (static_cast<double> (src));

    if (src.isString())
        return make_caster<juce::String>::cast (src, policy, parent);

    if (src.isArray())
    {
        list list;

        if (auto array = src.getArray())
        {
            for (const auto& value : *array)
                list.append (value);
        }

        return list.release();
    }

    auto dynamicObject = src.getDynamicObject();
    if (src.isObject() && dynamicObject != nullptr)
    {
        dict result;

        for (const auto& props : dynamicObject->getProperties())
            result [make_caster<juce::String>::cast (props.name.toString(), policy, parent)] = props.value;

        return result.release();
    }

    if (src.isBinaryData())
    {
        if (auto data = src.getBinaryData())
            return bytes (static_cast<const char*> (data->getData()), static_cast<Py_ssize_t> (data->getSize())).release();
    }

    if (src.isMethod())
    {
        return cpp_function ([src]
        {
            juce::var::NativeFunctionArgs args (juce::var(), nullptr, 0);
            return src.getNativeFunction() (args);
        }).release();
    }

    return Py_None;
}

}} // namespace PYBIND11_NAMESPACE::detail

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

template <template <class> class Class, class... Types>
void registerMathConstants (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("MathConstants", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def_readonly_static ("pi", &T::pi)
            .def_readonly_static ("twoPi", &T::twoPi)
            .def_readonly_static ("halfPi", &T::halfPi)
            .def_readonly_static ("euler", &T::euler)
            .def_readonly_static ("sqrt2", &T::sqrt2)
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("MathConstants", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRange (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = underlying_type_t<Types>;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Range", typeid (ValueType).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def_static ("between", &T::between)
            .def_static ("withStartAndLength", &T::withStartAndLength)
            .def_static ("emptyRange", &T::emptyRange)
            .def ("getStart", &T::getStart)
            .def ("getLength", &T::getLength)
            .def ("getEnd", &T::getEnd)
            .def ("isEmpty", &T::isEmpty)
            .def ("setStart", &T::setStart)
            .def ("withStart", &T::withStart)
            .def ("movedToStartAt", &T::movedToStartAt)
            .def ("setEnd", &T::setEnd)
            .def ("withEnd", &T::withEnd)
            .def ("movedToEndAt", &T::movedToEndAt)
            .def ("setLength", &T::setLength)
            .def ("expanded", &T::expanded)
            .def (py::self += ValueType())
            .def (py::self -= ValueType())
            .def (py::self + ValueType())
            .def (py::self - ValueType())
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("contains", py::overload_cast<const ValueType> (&T::contains, py::const_))
            .def ("clipValue", &T::clipValue)
            .def ("contains", py::overload_cast<T> (&T::contains, py::const_))
            .def ("intersects", &T::intersects)
            .def ("getIntersectionWith", &T::getIntersectionWith)
            .def ("getUnionWith", py::overload_cast<T> (&T::getUnionWith, py::const_))
            .def ("getUnionWith", py::overload_cast<const ValueType> (&T::getUnionWith, py::const_))
            .def ("constrainRange", &T::constrainRange)
        //.def_static ("findMinAndMax", [](const T& self, py::buffer values, int numValues)
        //{
        //  auto info = values.request();
        //  return self.findMinAndMax (reinterpret_cast<ValueType*> (info.ptr), numValues);
        //})
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getStart() << ", " << self.getEnd() << ")";
                return result;
            })
        ;

        if constexpr (! std::is_same_v<ValueType, Types>)
            class_.def (py::init ([](Types start, Types end) { return T (static_cast<ValueType> (start), static_cast<ValueType> (end)); }));

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Range", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerAtomic (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = underlying_type_t<Types>;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Atomic", typeid (ValueType).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType>())
            .def (py::init<const T&>())
            .def ("get", &T::get)
            .def ("set", &T::set)
            .def ("exchange", &T::exchange)
            .def ("compareAndSetBool", &T::compareAndSetBool)
            .def ("memoryBarrier", &T::memoryBarrier)
        ;

        if constexpr (! std::is_same_v<ValueType, Types>)
            class_.def (py::init ([](Types value) { return T (static_cast<ValueType> (value)); }));

        if constexpr (!std::is_same_v<ValueType, bool> && !std::is_floating_point_v<ValueType>)
        {
            class_
                .def ("__iadd__", &T::operator+=)
                .def ("__isub__", &T::operator-=)
            ;
        }

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Atomic", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerSparseSet (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;
    using namespace py::literals;

    auto type = py::hasattr (m, "SparseSet") ? m.attr ("SparseSet").cast<py::dict>() : py::dict{};

    ([&]
    {
        using ValueType = underlying_type_t<Types>;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("SparseSet", typeid (ValueType).name());

        py::class_<T> class_ (m, className.toRawUTF8());

        class_
            .def (py::init<>())
            .def (py::init<const T&>())
            .def ("clear", &T::clear)
            .def ("isEmpty", &T::isEmpty)
            .def ("size", &T::size)
            .def ("__getitem__", &T::operator[])
            .def ("contains", &T::contains)
            .def ("getNumRanges", &T::getNumRanges)
            .def ("getRange", &T::getRange)
            .def ("getTotalRange", &T::getTotalRange)
            .def ("addRange", &T::addRange)
            .def ("removeRange", &T::removeRange)
            .def ("invertRange", &T::invertRange)
            .def ("overlapsRange", &T::overlapsRange)
            .def ("containsRange", &T::containsRange)
            .def ("__len__", &T::size)
            .def ("__repr__", [className](T& self)
            {
                String result;
                result
                    << "<" << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (T).name(), 1)
                    << " object at " << String::formatted ("%p", std::addressof (self)) << ">";
                return result;
            })
        ;

        if constexpr (isEqualityComparable<ValueType>::value)
        {
            class_
                .def (py::self == py::self)
                .def (py::self != py::self)
            ;
        }

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.attr ("SparseSet") = type;
}

void registerJuceCoreBindings (py::module_& m)
{
#if !JUCE_PYTHON_EMBEDDED_INTERPRETER
    juce::SystemStats::setApplicationCrashHandler (Helpers::applicationCrashHandler);
#endif

    // ============================================================================================ GenericInteger<T>

    py::class_<GenericInteger<int8>> (m, "int8")
        .def (py::init<int8>())
        .def ("get", &GenericInteger<int8>::get);

    py::class_<GenericInteger<uint8>> (m, "uint8")
        .def (py::init<uint8>())
        .def ("get", &GenericInteger<uint8>::get);

    py::class_<GenericInteger<int16>> (m, "int16")
        .def (py::init<int16>())
        .def ("get", &GenericInteger<int16>::get);

    py::class_<GenericInteger<uint16>> (m, "uint16")
        .def (py::init<uint16>())
        .def ("get", &GenericInteger<uint16>::get);

    py::class_<GenericInteger<int32>> (m, "int32")
        .def (py::init<int32>())
        .def ("get", &GenericInteger<int32>::get);

    py::class_<GenericInteger<uint32>> (m, "uint32")
        .def (py::init<uint32>())
        .def ("get", &GenericInteger<uint32>::get);

    py::class_<GenericInteger<int64>> (m, "int64")
        .def (py::init<int64>())
        .def ("get", &GenericInteger<int64>::get);

    py::class_<GenericInteger<uint64>> (m, "uint64")
        .def (py::init<uint64>())
        .def ("get", &GenericInteger<uint64>::get);

    // ============================================================================================ juce::Math

    m.def ("juce_hypot", &juce_hypot<float>);
    m.def ("juce_hypot", &juce_hypot<double>);
    m.def ("degreesToRadians", &degreesToRadians<float>);
    m.def ("degreesToRadians", &degreesToRadians<double>);
    m.def ("radiansToDegrees", &radiansToDegrees<float>);
    m.def ("radiansToDegrees", &radiansToDegrees<double>);
    m.def ("juce_isfinite", &juce_isfinite<float>);
    m.def ("juce_isfinite", &juce_isfinite<double>);
    m.def ("exactlyEqual", &exactlyEqual<float>);
    m.def ("exactlyEqual", &exactlyEqual<double>);
    m.def ("absoluteTolerance", &absoluteTolerance<float>);
    m.def ("absoluteTolerance", &absoluteTolerance<double>);
    m.def ("relativeTolerance", &relativeTolerance<float>);
    m.def ("relativeTolerance", &relativeTolerance<double>);
    m.def ("approximatelyEqual", [](int a, int b) { return approximatelyEqual (a, b); });
    m.def ("approximatelyEqual", [](int64 a, int64 b) { return approximatelyEqual (a, b); });
    m.def ("approximatelyEqual", [](float a, float b) { return approximatelyEqual (a, b); });
    m.def ("approximatelyEqual", [](double a, double b) { return approximatelyEqual (a, b); });
    m.def ("nextFloatUp", &nextFloatUp<float>);
    m.def ("nextFloatUp", &nextFloatUp<double>);
    m.def ("nextFloatDown", &nextFloatDown<float>);
    m.def ("nextFloatDown", &nextFloatDown<double>);
    m.def ("mapToLog10", &mapToLog10<float>);
    m.def ("mapToLog10", &mapToLog10<double>);
    m.def ("mapFromLog10", &mapFromLog10<float>);
    m.def ("mapFromLog10", &mapFromLog10<double>);
    //m.def ("findMinimum", &findMinimum<float, int>);
    //m.def ("findMaximum", &findMaximum<float, int>);
    //m.def ("findMinAndMax", &findMaximum<float>);
    //m.def ("jlimit", &jlimit<?>);
    //m.def ("isPositiveAndBelow", &isPositiveAndBelow<?>);
    //m.def ("isPositiveAndNotGreaterThan", &isPositiveAndNotGreaterThan<?>);
    //m.def ("isWithin", &isWithin<?>);
    m.def ("roundToInt", &roundToInt<int>);
    m.def ("roundToInt", &roundToInt<float>);
    m.def ("roundToInt", &roundToInt<double>);
    m.def ("roundToIntAccurate", &roundToIntAccurate);
    m.def ("truncatePositiveToUnsignedInt", &truncatePositiveToUnsignedInt<float>);
    m.def ("truncatePositiveToUnsignedInt", &truncatePositiveToUnsignedInt<double>);
    m.def ("isPowerOfTwo", &isPowerOfTwo<int>);
    m.def ("isPowerOfTwo", &isPowerOfTwo<int64>);
    m.def ("nextPowerOfTwo", &nextPowerOfTwo);
    m.def ("nextPowerOfTwo", &nextPowerOfTwo);
    m.def ("findHighestSetBit", &findHighestSetBit);
    m.def ("countNumberOfBits", static_cast<int (*)(uint32) noexcept> (&countNumberOfBits));
    m.def ("countNumberOfBits", static_cast<int (*)(uint64) noexcept> (&countNumberOfBits));
    m.def ("negativeAwareModulo", &negativeAwareModulo<char>);
    m.def ("negativeAwareModulo", &negativeAwareModulo<uint8>);
    m.def ("negativeAwareModulo", &negativeAwareModulo<short>);
    m.def ("negativeAwareModulo", &negativeAwareModulo<int>);
    m.def ("negativeAwareModulo", &negativeAwareModulo<int64>);
    m.def ("negativeAwareModulo", &negativeAwareModulo<int64>);
    //m.def ("square", &square<?>);
    m.def ("writeLittleEndianBitsInBuffer", [](py::buffer target, uint32 startBit, uint32 numBits, uint32 value)
    {
        auto info = target.request (true);
        if ((startBit + numBits) >= static_cast<uint32> (info.size) * 8)
             py::pybind11_fail ("Insufficient bytes to write into provided buffer");
        writeLittleEndianBitsInBuffer (info.ptr, startBit, numBits, value);
    });
    m.def ("readLittleEndianBitsInBuffer", [](py::buffer target, uint32 startBit, uint32 numBits)
    {
        auto info = target.request();
        if ((startBit + numBits) >= static_cast<uint32> (info.size) * 8)
             py::pybind11_fail ("Insufficient bytes to write into provided buffer");
        return readLittleEndianBitsInBuffer (info.ptr, startBit, numBits);
    });

    // ============================================================================================ juce::MathConstants

    registerMathConstants<MathConstants, float, double> (m);

    // ============================================================================================ juce::Atomic

    registerAtomic<Atomic, bool, int, float> (m);

    // ============================================================================================ juce::Identifier

    py::class_<Identifier> classIdentifier (m, "Identifier");

    classIdentifier
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const Identifier&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self > py::self)
        .def (py::self >= py::self)
        .def (py::self < py::self)
        .def (py::self <= py::self)
        .def ("toString", &Identifier::toString)
        .def ("isValid", &Identifier::isValid)
        .def ("isNull", &Identifier::isNull)
        .def_static ("isValidIdentifier", &Identifier::isValidIdentifier)
        .def ("__repr__", Helpers::makeRepr<Identifier> (&Identifier::toString))
        .def ("__str__", &Identifier::toString)
    ;

    // ============================================================================================ juce::ByteOrder

    py::class_<ByteOrder> classByteOrder (m, "ByteOrder");

    classByteOrder
        .def_static ("swap", static_cast<uint16 (*)(uint16)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<int16 (*)(int16)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<uint32 (*)(uint32)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<int32 (*)(int32)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<uint64 (*)(uint64)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<int64 (*)(int64)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<float (*)(float)> (&ByteOrder::swap))
        .def_static ("swap", static_cast<double (*)(double)> (&ByteOrder::swap))
        .def_static ("littleEndianInt", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (int))
                py::pybind11_fail ("Insufficient bytes to construct an 32bit integer");
            return ByteOrder::littleEndianInt (info.ptr);
        })
        .def_static ("littleEndianInt64", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (int64))
                py::pybind11_fail ("Insufficient bytes to construct an 64bit integer");
            return ByteOrder::littleEndianInt64 (info.ptr);
        })
        .def_static ("littleEndianShort", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (short))
                py::pybind11_fail ("Insufficient bytes to construct an 16bit integer");
            return ByteOrder::littleEndianShort (info.ptr);
        })
        .def_static ("littleEndian24Bit", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (int8) * 3)
                py::pybind11_fail ("Insufficient bytes to construct an 24bit integer");
            return ByteOrder::littleEndian24Bit (info.ptr);
        })
    //.def_static ("littleEndian24BitToChars", &ByteOrder::littleEndian24BitToChars)
        .def_static ("bigEndianInt", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (int))
                py::pybind11_fail ("Insufficient bytes to construct an 32bit integer");
            return ByteOrder::bigEndianInt (info.ptr);
        })
        .def_static ("bigEndianInt64", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (int64))
                py::pybind11_fail ("Insufficient bytes to construct an 64bit integer");
            return ByteOrder::bigEndianInt64 (info.ptr);
        })
        .def_static ("bigEndianShort", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (short))
                py::pybind11_fail ("Insufficient bytes to construct an 16bit integer");
            return ByteOrder::bigEndianShort (info.ptr);
        })
        .def_static ("bigEndian24Bit", [](py::buffer data)
        {
            auto info = data.request();
            if (static_cast <size_t> (info.size) < sizeof (char) * 3)
                py::pybind11_fail ("Insufficient bytes to construct an 24bit integer");
            return ByteOrder::bigEndian24Bit (info.ptr);
        })
    //.def_static ("bigEndian24BitToChars", [](py::buffer data) { auto info = data.request(); return ByteOrder::bigEndian24BitToChars (info.ptr); })
        .def_static ("makeInt", static_cast<uint16 (*)(uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("makeInt", static_cast<uint32 (*)(uint8, uint8, uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("makeInt", static_cast<uint64 (*)(uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("isBigEndian", &ByteOrder::isBigEndian)
    ;

    // ============================================================================================ juce::StringArray

    py::class_<StringArray> classStringArray (m, "StringArray");

    classStringArray
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const StringArray&>())
        .def (py::init ([](const String& firstValue, py::args values)
        {
            auto result = StringArray();
            result.add (firstValue);

            for (auto value : values)
                result.add (static_cast<std::string> (value.cast<py::str>()).c_str());

            return result;
        }))
        .def (py::init ([](py::list values)
        {
            auto result = StringArray();

            for (auto value : values)
                result.add (static_cast<std::string> (value.cast<py::str>()).c_str());

            return result;
        }))
        .def ("swapWith", &StringArray::swapWith)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("__getitem__", [](const StringArray& self, int index) { return self[index]; })
        .def ("__setitem__", [](StringArray& self, int index, const String& value) { return self.set (index, value); })
        .def("__iter__", [](const StringArray& self)
        {
            return py::make_iterator (self.begin(), self.end());
        }, py::keep_alive<0, 1>())
        .def ("__len__", &StringArray::size)
        .def ("size", &StringArray::size)
        .def ("isEmpty", &StringArray::isEmpty)
        .def ("getReference", py::overload_cast<int> (&StringArray::getReference), py::return_value_policy::reference)
        .def ("contains", &StringArray::contains, "stringToLookFor"_a, "ignoreCase"_a = false)
        .def ("indexOf", &StringArray::indexOf, "stringToLookFor"_a, "ignoreCase"_a = false, "startIndex"_a = 0)
        .def ("add", &StringArray::add)
        .def ("insert", &StringArray::insert)
        .def ("addIfNotAlreadyThere", &StringArray::addIfNotAlreadyThere, "stringToAdd"_a, "ignoreCase"_a = false)
        .def ("set", &StringArray::set)
        .def ("addArray", [](StringArray& self, const StringArray& other, int startIndex, int numElementsToAdd)
        {
            self.addArray (other, startIndex, numElementsToAdd);
        }, "other"_a, "startIndex"_a = 0, "numElementsToAdd"_a = -1)
        .def ("addArray", [](StringArray& self, py::list other, int startIndex, int numElementsToAdd)
        {
            numElementsToAdd = numElementsToAdd < 0 ? static_cast<int> (other.size()) : (startIndex + numElementsToAdd);
            for (int i = startIndex; i < numElementsToAdd; ++i)
                self.add (static_cast<std::string> (other[static_cast<size_t> (i)].cast<py::str>()).c_str());
        }, "other"_a, "startIndex"_a = 0, "numElementsToAdd"_a = -1)
        .def ("mergeArray", &StringArray::mergeArray, "other"_a, "ignoreCase"_a = false)
        .def ("addTokens", py::overload_cast<StringRef, bool> (&StringArray::addTokens),
            "stringToTokenise"_a, "preserveQuotedStrings"_a)
        .def ("addTokens", py::overload_cast<StringRef, StringRef, StringRef> (&StringArray::addTokens),
            "stringToTokenise"_a, "breakCharacters"_a, "quoteCharacters"_a)
        .def ("addLines", &StringArray::addLines)
        .def_static ("fromTokens", static_cast<StringArray (*)(StringRef, bool)> (&StringArray::fromTokens),
            "stringToTokenise"_a, "preserveQuotedStrings"_a)
        .def_static ("fromTokens", static_cast<StringArray (*)(StringRef, StringRef, StringRef)> (&StringArray::fromTokens),
            "stringToTokenise"_a, "breakCharacters"_a, "quoteCharacters"_a)
        .def_static ("fromLines", &StringArray::fromLines)
        .def ("clear", &StringArray::clear)
        .def ("clearQuick", &StringArray::clearQuick)
        .def ("remove", &StringArray::remove)
        .def ("removeString", &StringArray::removeString, "stringToRemove"_a, "ignoreCase"_a = false)
        .def ("removeRange", &StringArray::removeRange)
        .def ("removeDuplicates", &StringArray::removeDuplicates)
        .def ("removeEmptyStrings", &StringArray::removeEmptyStrings, "removeWhitespaceStrings"_a = true)
        .def ("move", &StringArray::move)
        .def ("trim", &StringArray::trim)
        .def ("appendNumbersToDuplicates", [](StringArray& self, bool ignoreCaseWhenComparing, bool appendNumberToFirstInstance, const String* preNumberString, const String* postNumberString)
        {
            self.appendNumbersToDuplicates (ignoreCaseWhenComparing, appendNumberToFirstInstance,
                preNumberString != nullptr ? preNumberString->toUTF8() : CharPointer_UTF8(nullptr),
                postNumberString != nullptr ? postNumberString->toUTF8() : CharPointer_UTF8(nullptr));
        }, "ignoreCaseWhenComparing"_a, "appendNumberToFirstInstance"_a, "preNumberString"_a = static_cast<const String*> (nullptr), "postNumberString"_a = static_cast<const String*> (nullptr))
        .def ("joinIntoString", &StringArray::joinIntoString, "separatorString"_a, "startIndex"_a = 0, "numberOfElements"_a = -1)
        .def ("sort", &StringArray::sort, "ignoreCase"_a)
        .def ("sortNatural", &StringArray::sortNatural)
        .def ("ensureStorageAllocated", &StringArray::ensureStorageAllocated)
        .def ("minimiseStorageOverheads", &StringArray::minimiseStorageOverheads)
        .def_readwrite ("strings", &StringArray::strings)
    ;

    // ============================================================================================ juce::StringPairArray

    py::class_<StringPairArray> classStringPairArray (m, "StringPairArray");

    classStringPairArray
        .def (py::init<bool>(), "ignoreCaseWhenComparingKeys"_a = true)
        .def (py::init<const StringPairArray&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("__len__", &StringPairArray::size)
        .def ("__getitem__", [](const StringPairArray& self, StringRef key) { return self[ key]; }, "key"_a)
        .def ("getValue", &StringPairArray::getValue, "key"_a, "defaultReturnValue"_a)
        .def ("containsKey", &StringPairArray::containsKey)
        .def ("getAllKeys", &StringPairArray::getAllKeys)
        .def ("getAllValues", &StringPairArray::getAllValues)
        .def ("size", &StringPairArray::size)
        .def ("set", &StringPairArray::set, "key"_a, "value"_a)
        .def ("addArray", &StringPairArray::addArray, "other"_a)
        .def ("clear", &StringPairArray::clear)
        .def ("remove", py::overload_cast<StringRef> (&StringPairArray::remove), "key"_a)
        .def ("remove", py::overload_cast<int> (&StringPairArray::remove), "index"_a)
        .def ("setIgnoresCase", &StringPairArray::setIgnoresCase, "shouldIgnoreCase"_a)
        .def ("getIgnoresCase", &StringPairArray::getIgnoresCase)
        .def ("getDescription", &StringPairArray::getDescription)
        .def ("minimiseStorageOverheads", &StringPairArray::minimiseStorageOverheads)
        .def ("addMap", &StringPairArray::addMap)
        .def ("addUnorderedMap", &StringPairArray::addUnorderedMap)
    ;

    // ============================================================================================ juce::NamedValueSet

    py::class_<NamedValueSet> classNamedValueSet (m, "NamedValueSet");

    py::class_<NamedValueSet::NamedValue> classNamedValueSetNamedValue (classNamedValueSet, "NamedValue");

    classNamedValueSetNamedValue
        .def (py::init<>())
        .def (py::init<const Identifier&, const var&>())
        .def (py::init<const NamedValueSet::NamedValue&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def_readwrite ("name", &NamedValueSet::NamedValue::name)
        .def_readwrite ("value", &NamedValueSet::NamedValue::value)
    ;

    classNamedValueSet
        .def (py::init<>())
        .def (py::init<const NamedValueSet&>())
        .def (py::init ([](py::list list)
        {
            auto result = NamedValueSet();

            for (auto item : list)
            {
                py::detail::make_caster<NamedValueSet::NamedValue> conv;

                if (! conv.load (item, true))
                    py::pybind11_fail("Invalid property type of a \"NamedValueSet\", needs to be \"NamedValueSet::NamedValue\"");

                auto namedValue = py::detail::cast_op<NamedValueSet::NamedValue&&> (std::move (conv));

                result.set(namedValue.name, namedValue.value);
            }

            return result;
        }))
        .def (py::init ([](py::dict dict)
        {
            auto result = NamedValueSet();

            for (auto item : dict)
            {
                py::detail::make_caster<Identifier> convKey;
                py::detail::make_caster<var> convValue;

                if (! convKey.load (item.first, true))
                    py::pybind11_fail("Invalid property type of a \"NamedValueSet\", needs to be \"str\" or \"Identifier\"");

                if (! convValue.load (item.second, true))
                    py::pybind11_fail("Invalid property type of a \"NamedValueSet\", needs to be a \"var\" convertible");

                result.set(
                    py::detail::cast_op<juce::Identifier&&> (std::move (convKey)),
                    py::detail::cast_op<juce::var&&> (std::move (convValue)));
            }

            return result;
        }))
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def("__iter__", [](const NamedValueSet& self)
        {
            return py::make_iterator (self.begin(), self.end());
        }, py::keep_alive<0, 1>())
        .def ("size", &NamedValueSet::size)
        .def ("isEmpty", &NamedValueSet::isEmpty)
        .def ("__getitem__", [](const NamedValueSet& self, const Identifier& name) { return self[name]; }, py::return_value_policy::reference)
        .def ("__setitem__", [](NamedValueSet& self, const Identifier& name, juce::var value) { return self.set (name, std::move (value)); })
        .def ("getWithDefault", &NamedValueSet::getWithDefault)
        .def ("set", py::overload_cast<const Identifier&, const var&> (&NamedValueSet::set))
        .def ("contains", &NamedValueSet::contains)
        .def ("remove", &NamedValueSet::remove)
        .def ("getName", &NamedValueSet::getName)
        .def ("getVarPointer", py::overload_cast<const Identifier&> (&NamedValueSet::getVarPointer), py::return_value_policy::reference)
        .def ("getValueAt", &NamedValueSet::getValueAt, py::return_value_policy::reference)
        .def ("getVarPointerAt", py::overload_cast<int> (&NamedValueSet::getVarPointerAt), py::return_value_policy::reference)
        .def ("indexOf", &NamedValueSet::indexOf)
        .def ("clear", &NamedValueSet::clear)
        .def ("setFromXmlAttributes", &NamedValueSet::setFromXmlAttributes)
        .def ("copyToXmlAttributes", &NamedValueSet::copyToXmlAttributes)
    ;

    // ============================================================================================ juce::BigInteger

    py::class_<BigInteger> classBigInteger (m, "BigInteger");

    classBigInteger
        .def (py::init<>())
        .def (py::init<int32>())
        .def (py::init<int64>())
        .def (py::init<const BigInteger&>())
        .def ("swapWith", &BigInteger::swapWith)
        .def ("isZero", &BigInteger::isZero)
        .def ("isOne", &BigInteger::isOne)
        .def ("toInteger", &BigInteger::toInteger)
        .def ("toInt64", &BigInteger::toInt64)
        .def ("clear", &BigInteger::clear, py::return_value_policy::reference)
        .def ("clearBit", &BigInteger::clearBit, py::return_value_policy::reference)
        .def ("setBit", py::overload_cast<int> (&BigInteger::setBit), py::return_value_policy::reference)
        .def ("setBit", py::overload_cast<int, bool> (&BigInteger::setBit), py::return_value_policy::reference)
        .def ("setRange", &BigInteger::setRange, py::return_value_policy::reference)
        .def ("insertBit", &BigInteger::insertBit, py::return_value_policy::reference)
        .def ("getBitRange", &BigInteger::getBitRange, py::return_value_policy::reference)
        .def ("getBitRangeAsInt", &BigInteger::getBitRangeAsInt, py::return_value_policy::reference)
        .def ("setBitRangeAsInt", &BigInteger::setBitRangeAsInt, py::return_value_policy::reference)
        .def ("shiftBits", &BigInteger::shiftBits, py::return_value_policy::reference)
        .def ("countNumberOfSetBits", &BigInteger::countNumberOfSetBits)
        .def ("findNextSetBit", &BigInteger::findNextSetBit)
        .def ("findNextClearBit", &BigInteger::findNextClearBit)
        .def ("getHighestBit", &BigInteger::getHighestBit)
        .def ("isNegative", &BigInteger::isNegative)
        .def ("setNegative", &BigInteger::setNegative)
        .def ("negate", &BigInteger::negate)
        .def (py::self += py::self)
        .def (py::self -= py::self)
        .def (py::self *= py::self)
        .def (py::self /= py::self)
        .def (py::self |= py::self)
        .def (py::self &= py::self)
        .def (py::self ^= py::self)
        .def (py::self %= py::self)
        .def (py::self <<= int())
        .def (py::self >>= int())
        .def (-py::self)
        .def (py::self + py::self)
        .def (py::self - py::self)
        .def (py::self * py::self)
        .def (py::self / py::self)
        .def (py::self | py::self)
        .def (py::self & py::self)
        .def (py::self ^ py::self)
        .def (py::self % py::self)
        .def (py::self << int())
        .def (py::self >> int())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self <= py::self)
        .def (py::self > py::self)
        .def (py::self >= py::self)
        .def (py::self == int())
        .def (py::self != int())
        .def (py::self < int())
        .def (py::self <= int())
        .def (py::self > int())
        .def (py::self >= int())
        .def ("compare", &BigInteger::compare)
        .def ("compareAbsolute", &BigInteger::compareAbsolute)
        .def ("divideBy", &BigInteger::divideBy)
        .def ("findGreatestCommonDivisor", &BigInteger::findGreatestCommonDivisor)
        .def ("exponentModulo", &BigInteger::exponentModulo)
        .def ("inverseModulo", &BigInteger::inverseModulo)
        .def ("montgomeryMultiplication", &BigInteger::montgomeryMultiplication)
        .def ("extendedEuclidean", &BigInteger::extendedEuclidean)
        .def ("toString", &BigInteger::toString)
        .def ("parseString", &BigInteger::parseString)
        .def ("toMemoryBlock", &BigInteger::toMemoryBlock)
        .def ("loadFromMemoryBlock", &BigInteger::loadFromMemoryBlock)
        .def ("__repr__", [](const BigInteger& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.toString (16) << "')";
            return result;
        })
        .def ("__str__", &BigInteger::toString)
    ;

    // ============================================================================================ juce::Base64

    py::class_<Base64> classBase64 (m, "Base64");

    classBase64
        .def_static ("convertToBase64", [](OutputStream& base64Result, py::buffer data)
        {
            auto info = data.request();
            return Base64::convertToBase64 (base64Result, info.ptr, static_cast<size_t> (info.size));
        })
        .def_static ("convertFromBase64", &Base64::convertFromBase64)
        .def_static ("toBase64", [](py::buffer data)
        {
            auto info = data.request();
            return Base64::toBase64 (info.ptr, static_cast<size_t> (info.size));
        })
        .def_static ("toBase64", static_cast<String (*)(const String &)> (&Base64::toBase64))
    ;

    // ============================================================================================ juce::Result

    py::class_<Result> classResult (m, "Result");

    classResult
        .def (py::init<const Result&>())
        .def_static ("ok", &Result::ok)
        .def_static ("fail", &Result::fail)
        .def ("wasOk", &Result::wasOk)
        .def ("failed", &Result::failed)
        .def ("getErrorMessage", &Result::getErrorMessage)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (!py::self)
    ;

    // ============================================================================================ juce::Uuid

    py::class_<Uuid> classUuid (m, "Uuid");

    classUuid
        .def (py::init<>())
        .def (py::init<const Uuid&>())
        .def (py::init ([](py::buffer data)
        {
            auto info = data.request();

            if (info.size != 16)
                py::pybind11_fail ("Invalid length of bytes to construct a Uuid class, needs to be 16");

            return Uuid (static_cast<const uint8*> (info.ptr));
        }))
        .def (py::init<const String&>())
        .def (py::init ([](py::object obj)
        {
            auto uuid = py::module_::import ("uuid").attr ("UUID");
            if (! py::isinstance (obj, uuid))
                py::pybind11_fail ("Invalid object to construct a Uuid class, only uuid.UUID is supported");

            auto buffer = obj.attr ("bytes").cast<py::bytes>();
            return Uuid (reinterpret_cast<const uint8*> (static_cast<std::string_view> (buffer).data()));
        }))
        .def ("isNull", &Uuid::isNull)
        .def_static ("null", &Uuid::null)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self == String())
        .def (py::self != String())
        .def (py::self < py::self)
        .def (py::self > py::self)
        .def (py::self <= py::self)
        .def (py::self >= py::self)
        .def ("toString", &Uuid::toString)
        .def ("toDashedString", &Uuid::toDashedString)
        .def ("getTimeLow", &Uuid::getTimeLow)
        .def ("getTimeMid", &Uuid::getTimeMid)
        .def ("getTimeHighAndVersion", &Uuid::getTimeHighAndVersion)
        .def ("getClockSeqAndReserved", &Uuid::getClockSeqAndReserved)
        .def ("getClockSeqLow", &Uuid::getClockSeqLow)
        .def ("getNode", &Uuid::getNode)
        .def ("hash", &Uuid::hash)
        .def ("getRawData", [](const Uuid& self)
        {
            return py::memoryview::from_memory (self.getRawData(), 16);
        }, py::return_value_policy::reference_internal)
        .def ("__repr__", [](const Uuid& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('{" << self.toDashedString () << "}')";
            return result;
        })
        .def ("__str__", &Uuid::toDashedString)
    ;

    // ============================================================================================ juce::RelativeTime

    py::class_<RelativeTime> classRelativeTime (m, "RelativeTime");

    classRelativeTime
        .def (py::init<double>(), "seconds"_a = 0.0)
        .def (py::init<const RelativeTime&>())
        .def_static ("milliseconds", static_cast<RelativeTime (*)(int64)> (&RelativeTime::milliseconds))
        .def_static ("seconds", &RelativeTime::seconds)
        .def_static ("minutes", &RelativeTime::minutes)
        .def_static ("hours", &RelativeTime::hours)
        .def_static ("days", &RelativeTime::days)
        .def_static ("weeks", &RelativeTime::weeks)
        .def ("inMilliseconds", &RelativeTime::inMilliseconds)
        .def ("inSeconds", &RelativeTime::inSeconds)
        .def ("inMinutes", &RelativeTime::inMinutes)
        .def ("inHours", &RelativeTime::inHours)
        .def ("inDays", &RelativeTime::inDays)
        .def ("inWeeks", &RelativeTime::inWeeks)
        .def ("getDescription", &RelativeTime::getDescription, "returnValueForZeroTime"_a = "0")
        .def ("getApproximateDescription", &RelativeTime::getApproximateDescription)
        .def (py::self + py::self)
        .def (py::self - py::self)
        .def (py::self += py::self)
        .def (py::self -= py::self)
        .def (py::self += double())
        .def (py::self -= double())
        .def ("__repr__", [](const RelativeTime& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.getDescription() << "')";
            return result;
        })
        .def ("__str__", [](const RelativeTime& self) { return self.getDescription(); })
    ;

    // ============================================================================================ juce::Time

    py::class_<Time> classTime (m, "Time");

    classTime
        .def (py::init<>())
        .def (py::init<int64>())
        .def (py::init<const Time&>())
        .def (py::init<int, int, int, int, int, int, int, bool>(),
            "year"_a, "month"_a, "day"_a, "hours"_a, "minutes"_a, "seconds"_a = 0, "milliseconds"_a = 0, "useLocalTime"_a = true)
        .def_static ("getCurrentTime", &Time::getCurrentTime)
        .def ("toMilliseconds", &Time::toMilliseconds)
        .def ("getYear", &Time::getYear)
        .def ("getMonth", &Time::getMonth)
        .def ("getMonthName", py::overload_cast<bool> (&Time::getMonthName, py::const_))
    //.def_static ("getMonthName", static_cast<String (*)(int, bool)> (&Time::getMonthName))
        .def ("getDayOfMonth", &Time::getDayOfMonth)
        .def ("getDayOfWeek", &Time::getDayOfWeek)
        .def ("getDayOfYear", &Time::getDayOfYear)
        .def ("getWeekdayName", py::overload_cast<bool> (&Time::getWeekdayName, py::const_))
    //.def_static ("getWeekdayName", static_cast<String (*)(int, bool)> (&Time::getWeekdayName))
        .def ("getHours", &Time::getHours)
        .def ("isAfternoon", &Time::isAfternoon)
        .def ("getHoursInAmPmFormat", &Time::getHoursInAmPmFormat)
        .def ("getMinutes", &Time::getMinutes)
        .def ("getSeconds", &Time::getSeconds)
        .def ("getMilliseconds", &Time::getMilliseconds)
        .def ("isDaylightSavingTime", &Time::isDaylightSavingTime)
        .def ("getTimeZone", &Time::getTimeZone)
        .def ("getUTCOffsetSeconds", &Time::getUTCOffsetSeconds)
        .def ("getUTCOffsetString", &Time::getUTCOffsetString)
        .def ("toString", &Time::toString, "includeDate"_a, "includeTime"_a, "includeSeconds"_a = true, "use24HourClock"_a = false)
        .def ("formatted", &Time::formatted)
        .def ("toISO8601", &Time::toISO8601, "includeDividerCharacters"_a)
        .def_static ("fromISO8601", &Time::fromISO8601)
        .def (py::self + RelativeTime())
        .def (py::self - RelativeTime())
        .def (py::self += RelativeTime())
        .def (py::self -= RelativeTime())
        .def ("setSystemTimeToThisTime", &Time::setSystemTimeToThisTime)
        .def_static ("currentTimeMillis", &Time::currentTimeMillis)
        .def_static ("getMillisecondCounter", &Time::getMillisecondCounter)
        .def_static ("getMillisecondCounterHiRes", &Time::getMillisecondCounterHiRes)
        .def_static ("waitForMillisecondCounter", &Time::waitForMillisecondCounter)
        .def_static ("getApproximateMillisecondCounter", &Time::getApproximateMillisecondCounter)
        .def_static ("getHighResolutionTicks", &Time::getHighResolutionTicks)
        .def_static ("getHighResolutionTicksPerSecond", &Time::getHighResolutionTicksPerSecond)
        .def_static ("highResolutionTicksToSeconds", &Time::highResolutionTicksToSeconds)
        .def_static ("secondsToHighResolutionTicks", &Time::secondsToHighResolutionTicks)
        .def_static ("getCompilationDate", &Time::getCompilationDate)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self <= py::self)
        .def (py::self > py::self)
        .def (py::self >= py::self)
        .def ("__repr__", [](const Time& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.toISO8601 (false) << "')";
            return result;
        })
        .def ("__str__", [](const Time& self) { return self.toISO8601 (false); })
    ;

    // ============================================================================================ juce::Range<>

    registerRange<Range, int, GenericInteger<int64>, float> (m);

    // ============================================================================================ juce::MemoryBlock

    py::class_<MemoryBlock> classMemoryBlock (m, "MemoryBlock");

    classMemoryBlock
        .def (py::init<>())
        .def (py::init<const size_t, bool>(), "initialSize"_a, "initialiseToZero"_a = false)
        .def (py::init ([](py::list list)
        {
            auto mb = MemoryBlock (list.size());

            if (list.size() > 0)
            {
                char* data = reinterpret_cast<char*> (mb.getData());

                if (py::isinstance<py::int_> (list[0]))
                    for (const auto& item : list)
                        *data++ = static_cast<char> (item.cast<int>());
                else
                    for (const auto& item : list)
                        *data++ = item.cast<char>();
            }

            return mb;
        }))
        .def (py::init ([](py::buffer data)
        {
            auto info = data.request();
            return MemoryBlock (info.ptr, static_cast<size_t> (info.size));
        }))
        .def (py::init<const MemoryBlock&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("matches", Helpers::makeVoidPointerAndSizeCallable<MemoryBlock> (&MemoryBlock::matches))
        .def ("getData", [](MemoryBlock* self)
        {
            return py::memoryview::from_memory (self->getData(), static_cast<Py_ssize_t> (self->getSize()));
        }, py::return_value_policy::reference_internal)
        .def ("getData", [](const MemoryBlock* self)
        {
            return py::memoryview::from_memory (self->getData(), static_cast<Py_ssize_t> (self->getSize()));
        }, py::return_value_policy::reference_internal)
        .def ("__getitem__", [](const MemoryBlock& self, int index) { return self[index]; })
        .def ("__setitem__", [](MemoryBlock* self, int index, char value) { self->operator[] (index) = value; })
        .def ("__setitem__", [](MemoryBlock* self, int index, int value) { self->operator[] (index) = static_cast<char> (value); })
        .def ("isEmpty", &MemoryBlock::isEmpty)
        .def ("getSize", &MemoryBlock::getSize)
        .def ("setSize", &MemoryBlock::setSize, "newSize"_a, "initialiseNewSpaceToZero"_a = false)
        .def ("ensureSize", &MemoryBlock::ensureSize, "newSize"_a, "initialiseNewSpaceToZero"_a = false)
        .def ("reset", &MemoryBlock::reset)
        .def ("fillWith", &MemoryBlock::fillWith)
        .def ("append", Helpers::makeVoidPointerAndSizeCallable<MemoryBlock> (&MemoryBlock::append))
        .def ("replaceAll", Helpers::makeVoidPointerAndSizeCallable<MemoryBlock> (&MemoryBlock::replaceAll))
        .def ("insert", [](MemoryBlock* self, py::buffer data, size_t insertPosition)
        {
            auto info = data.request();
            self->insert (info.ptr, static_cast<size_t> (info.size), insertPosition);
        })
        .def ("removeSection", &MemoryBlock::removeSection)
        .def ("copyFrom", [](MemoryBlock* self, py::buffer data, int destinationOffset)
        {
            auto info = data.request();
            self->copyFrom (info.ptr, destinationOffset, static_cast<size_t> (info.size));
        })
        .def ("copyTo", [](const MemoryBlock* self, py::buffer data, int sourceOffset)
        {
            auto info = data.request (true);
            self->copyTo (info.ptr, sourceOffset, static_cast<size_t> (info.size));
        })
        .def ("swapWith", &MemoryBlock::swapWith)
        .def ("toString", &MemoryBlock::toString)
        .def ("loadFromHexString", &MemoryBlock::loadFromHexString)
        .def ("setBitRange", &MemoryBlock::setBitRange)
        .def ("getBitRange", &MemoryBlock::getBitRange)
        .def ("toBase64Encoding", &MemoryBlock::toBase64Encoding)
        .def ("fromBase64Encoding", &MemoryBlock::fromBase64Encoding)
        .def ("__repr__", [](const MemoryBlock& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "(b'";

            for (size_t index = 0; index < jmin (size_t (8), self.getSize()); ++index)
                result << "\\x" << String::toHexString (self[index]).paddedLeft(L'0', 2);

            if (self.getSize() > 8)
                result << "...";

            result << "')";
            return result;
        })
        .def ("__str__", &MemoryBlock::toString)
    ;

    // ============================================================================================ juce::InputStream

    py::class_<InputStream, PyInputStream<>> classInputStream (m, "InputStream");

    classInputStream
        .def (py::init<>())
        .def ("getTotalLength", &InputStream::getTotalLength)
        .def ("getNumBytesRemaining", &InputStream::getNumBytesRemaining)
        .def ("isExhausted", &InputStream::isExhausted)
        .def ("read", [](InputStream& self, py::buffer data)
        {
            auto info = data.request(false);
            return self.read (info.ptr, static_cast<size_t> (info.size));
        }, "buffer"_a)
        .def ("readByte", &InputStream::readByte)
        .def ("readBool", &InputStream::readBool)
        .def ("readShort", &InputStream::readShort)
        .def ("readShortBigEndian", &InputStream::readShortBigEndian)
        .def ("readInt", &InputStream::readInt)
        .def ("readIntBigEndian", &InputStream::readIntBigEndian)
        .def ("readInt64", &InputStream::readInt64)
        .def ("readInt64BigEndian", &InputStream::readInt64BigEndian)
        .def ("readFloat", &InputStream::readFloat)
        .def ("readFloatBigEndian", &InputStream::readFloatBigEndian)
        .def ("readDouble", &InputStream::readDouble)
        .def ("readDoubleBigEndian", &InputStream::readDoubleBigEndian)
        .def ("readCompressedInt", &InputStream::readCompressedInt)
        .def ("readNextLine", &InputStream::readNextLine)
        .def ("readString", &InputStream::readString)
        .def ("readEntireStreamAsString", &InputStream::readEntireStreamAsString)
        .def ("readIntoMemoryBlock", &InputStream::readIntoMemoryBlock, "destBlock"_a, "maxNumBytesToRead"_a = -1)
        .def ("getPosition", &InputStream::getPosition)
        .def ("setPosition", &InputStream::setPosition, "pos"_a)
        .def ("skipNextBytes", &InputStream::skipNextBytes, "numBytesToSkip"_a)
    ;

    py::class_<BufferedInputStream, InputStream, PyInputStream<BufferedInputStream>> classBufferedInputStream (m, "BufferedInputStream");

    classBufferedInputStream
        .def (py::init<InputStream&, int>(), "sourceStream"_a, "bufferSize"_a)
        .def ("peekByte", &BufferedInputStream::peekByte)
    ;

    py::class_<MemoryInputStream, InputStream, PyInputStream<MemoryInputStream>> classMemoryInputStream (m, "MemoryInputStream");

    classMemoryInputStream
        .def (py::init<const MemoryBlock&, bool>(), "data"_a, "keepInternalCopyOfData"_a)
        .def (py::init ([](py::buffer data, bool keepInternalCopyOfData)
        {
            auto info = data.request();
            return new MemoryInputStream (info.ptr, static_cast<size_t> (info.size), keepInternalCopyOfData);
        }), "data"_a, "keepInternalCopyOfData"_a)
        .def ("getData", [](const MemoryInputStream& self)
        {
            return py::memoryview::from_memory (self.getData(), static_cast<Py_ssize_t> (self.getDataSize()));
        }, py::return_value_policy::reference_internal)
        .def ("getDataSize", &MemoryInputStream::getDataSize)
    ;

    py::class_<SubregionStream, InputStream, PyInputStream<SubregionStream>> classSubregionStream (m, "SubregionStream");

    classSubregionStream
        .def (py::init<InputStream*, int64, int64, bool>(),
            "sourceStream"_a, "startPositionInSourceStream"_a, "lengthOfSourceStream"_a, "deleteSourceWhenDestroyed"_a)
    ;

    py::class_<GZIPDecompressorInputStream, InputStream, PyInputStream<GZIPDecompressorInputStream>> classGZIPDecompressorInputStream (m, "GZIPDecompressorInputStream");

    py::enum_<GZIPDecompressorInputStream::Format> (classGZIPDecompressorInputStream, "Format")
        .value ("zlibFormat", GZIPDecompressorInputStream::Format::zlibFormat)
        .value ("deflateFormat", GZIPDecompressorInputStream::Format::deflateFormat)
        .value ("gzipFormat", GZIPDecompressorInputStream::Format::gzipFormat)
        .export_values();

    classGZIPDecompressorInputStream
        .def (py::init<InputStream*, bool, GZIPDecompressorInputStream::Format, int64>(),
            "sourceStream"_a, "deleteSourceWhenDestroyed"_a = false, "sourceFormat"_a = GZIPDecompressorInputStream::zlibFormat, "uncompressedStreamLength"_a = -1)
        .def (py::init<InputStream&>(), "sourceStream"_a)
    ;

    // ============================================================================================ juce::InputSource

    py::class_<InputSource, PyInputSource<>> classInputSource (m, "InputSource");

    classInputSource
        .def (py::init<>())
        .def ("createInputStream", &InputSource::createInputStream)
        .def ("createInputStreamFor", &InputSource::createInputStreamFor)
        .def ("hashCode", &InputSource::hashCode)
    ;

    // ============================================================================================ juce::OutputStream

    py::class_<OutputStream, PyOutputStream<>> classOutputStream (m, "OutputStream");

    classOutputStream
        .def (py::init<>())
        .def ("flush", &OutputStream::flush)
        .def ("setPosition", &OutputStream::setPosition, "pos"_a)
        .def ("getPosition", &OutputStream::getPosition)
        .def ("write", popsicle::Helpers::makeVoidPointerAndSizeCallable<OutputStream> (&OutputStream::write))
        .def ("writeByte", &OutputStream::writeByte)
        .def ("writeBool", &OutputStream::writeBool)
        .def ("writeShort", &OutputStream::writeShort)
        .def ("writeShortBigEndian", &OutputStream::writeShortBigEndian)
        .def ("writeInt", &OutputStream::writeInt)
        .def ("writeIntBigEndian", &OutputStream::writeIntBigEndian)
        .def ("writeInt64", &OutputStream::writeInt64)
        .def ("writeInt64BigEndian", &OutputStream::writeInt64BigEndian)
        .def ("writeFloat", &OutputStream::writeFloat)
        .def ("writeFloatBigEndian", &OutputStream::writeFloatBigEndian)
        .def ("writeDouble", &OutputStream::writeDouble)
        .def ("writeDoubleBigEndian", &OutputStream::writeDoubleBigEndian)
        .def ("writeRepeatedByte", &OutputStream::writeRepeatedByte)
        .def ("writeCompressedInt", &OutputStream::writeCompressedInt)
        .def ("writeString", &OutputStream::writeString)
        .def ("writeText", &OutputStream::writeText)
        .def ("writeFromInputStream", &OutputStream::writeFromInputStream)
        .def ("setNewLineString", &OutputStream::setNewLineString)
        .def ("getNewLineString", &OutputStream::getNewLineString, py::return_value_policy::reference_internal)
    ;

    py::class_<MemoryOutputStream, OutputStream, PyOutputStream<MemoryOutputStream>> classMemoryOutputStream (m, "MemoryOutputStream");

    classMemoryOutputStream
        .def (py::init<size_t>(), "initialSize"_a = 256)
        .def (py::init<MemoryBlock&, bool>(), "memoryBlockToWriteTo"_a, "appendToExistingBlockContent"_a)
        .def (py::init ([](py::buffer data)
        {
            auto info = data.request();
            return new MemoryOutputStream (info.ptr, static_cast<size_t> (info.size));
        }), "destBuffer"_a)
        .def ("getData", [](const MemoryOutputStream* self)
        {
            return py::memoryview::from_memory (self->getData(), static_cast<Py_ssize_t> (self->getDataSize()));
        }, py::return_value_policy::reference_internal)
        .def ("getDataSize", &MemoryOutputStream::getDataSize)
        .def ("reset", &MemoryOutputStream::reset)
        .def ("preallocate", &MemoryOutputStream::preallocate, "bytesToPreallocate"_a)
        .def ("appendUTF8Char", &MemoryOutputStream::appendUTF8Char, "c"_a)
        .def ("toUTF8", &MemoryOutputStream::toUTF8)
        .def ("toString", &MemoryOutputStream::toString)
        .def ("getMemoryBlock", &MemoryOutputStream::getMemoryBlock)
        .def ("flush", &MemoryOutputStream::flush)
        .def ("__str__", &MemoryOutputStream::toString)
    ;

    py::class_<GZIPCompressorOutputStream, OutputStream, PyOutputStream<GZIPCompressorOutputStream>> classGZIPCompressorOutputStream (m, "GZIPCompressorOutputStream");

    py::enum_<GZIPCompressorOutputStream::WindowBitsValues> (classGZIPCompressorOutputStream, "WindowBitsValues")
        .value ("windowBitsRaw", GZIPCompressorOutputStream::WindowBitsValues::windowBitsRaw)
        .value ("windowBitsGZIP", GZIPCompressorOutputStream::WindowBitsValues::windowBitsGZIP)
        .export_values();

    classGZIPCompressorOutputStream
        .def (py::init<OutputStream&, int, int>(), "destStream"_a, "compressionLevel"_a = -1, "windowBits"_a = 0)
        .def (py::init<OutputStream*, int, bool, int>(), "destStream"_a, "compressionLevel"_a = -1, "deleteDestStreamWhenDestroyed"_a = false, "windowBits"_a = 0)
    ;

    // ============================================================================================ juce::Random

    py::class_<Random> classRandom (m, "Random");

    classRandom
        .def (py::init<>())
        .def (py::init<int64>(), "seedValue"_a)
        .def ("nextInt", py::overload_cast<> (&Random::nextInt))
        .def ("nextInt", py::overload_cast<int> (&Random::nextInt), "maxValue"_a)
        .def ("nextInt", py::overload_cast<Range<int>> (&Random::nextInt), "range"_a)
        .def ("nextInt64", &Random::nextInt64)
        .def ("nextFloat", &Random::nextFloat)
        .def ("nextDouble", &Random::nextDouble)
        .def ("nextBool", &Random::nextBool)
        .def ("nextLargeNumber", &Random::nextLargeNumber, "maximumValue"_a)
        .def ("fillBitsRandomly", [](Random& self, py::buffer data)
        {
            auto info = data.request (true);
            self.fillBitsRandomly (info.ptr, static_cast<size_t> (info.size));
        }, "bufferToFill"_a)
        .def ("fillBitsRandomly", py::overload_cast<BigInteger&, int, int> (&Random::fillBitsRandomly), "arrayToChange"_a, "startBit"_a, "numBits"_a)
        .def ("setSeed", &Random::setSeed, "newSeed"_a)
        .def ("getSeed", &Random::getSeed)
        .def ("combineSeed", &Random::combineSeed)
        .def ("setSeedRandomly", &Random::setSeedRandomly)
        .def_static ("getSystemRandom", &Random::getSystemRandom, py::return_value_policy::reference)
    ;

    // ============================================================================================ juce::File

    py::class_<File> classFile (m, "File");

    py::enum_<File::SpecialLocationType> (classFile, "SpecialLocationType")
        .value ("commonApplicationDataDirectory", File::SpecialLocationType::commonApplicationDataDirectory)
        .value ("commonDocumentsDirectory", File::SpecialLocationType::commonDocumentsDirectory)
        .value ("currentApplicationFile", File::SpecialLocationType::currentApplicationFile)
        .value ("currentExecutableFile", File::SpecialLocationType::currentExecutableFile)
        .value ("globalApplicationsDirectory", File::SpecialLocationType::globalApplicationsDirectory)
        .value ("hostApplicationPath", File::SpecialLocationType::hostApplicationPath)
        .value ("invokedExecutableFile", File::SpecialLocationType::invokedExecutableFile)
        .value ("tempDirectory", File::SpecialLocationType::tempDirectory)
        .value ("userApplicationDataDirectory", File::SpecialLocationType::userApplicationDataDirectory)
        .value ("userDesktopDirectory", File::SpecialLocationType::userDesktopDirectory)
        .value ("userDocumentsDirectory", File::SpecialLocationType::userDocumentsDirectory)
        .value ("userHomeDirectory", File::SpecialLocationType::userHomeDirectory)
        .value ("userMoviesDirectory", File::SpecialLocationType::userMoviesDirectory)
        .value ("userMusicDirectory", File::SpecialLocationType::userMusicDirectory)
        .value ("userPicturesDirectory", File::SpecialLocationType::userPicturesDirectory)
        .export_values();

    py::enum_<File::TypesOfFileToFind> (classFile, "TypesOfFileToFind")
        .value ("findDirectories", File::TypesOfFileToFind::findDirectories)
        .value ("findFiles", File::TypesOfFileToFind::findFiles)
        .value ("findFilesAndDirectories", File::TypesOfFileToFind::findFilesAndDirectories)
        .value ("ignoreHiddenFiles", File::TypesOfFileToFind::ignoreHiddenFiles)
        .export_values();

    py::enum_<File::FollowSymlinks> (classFile, "FollowSymlinks")
        .value ("no", File::FollowSymlinks::no)
        .value ("noCycles", File::FollowSymlinks::noCycles)
        .value ("yes", File::FollowSymlinks::yes);

    classFile
        .def (py::init<>())
        .def (py::init<const String&>(), "absolutePath"_a)
        .def (py::init<const File&>(), "other"_a)
        .def ("exists", &File::exists)
        .def ("existsAsFile", &File::existsAsFile)
        .def ("isDirectory", &File::isDirectory)
        .def ("isRoot", &File::isRoot)
        .def ("getSize", &File::getSize)
        .def_static ("descriptionOfSizeInBytes", &File::descriptionOfSizeInBytes, "bytes"_a)
        .def ("getFullPathName", &File::getFullPathName)
        .def ("getFileName", &File::getFileName)
        .def ("getRelativePathFrom", &File::getRelativePathFrom, "directoryToBeRelativeTo"_a)
        .def ("getFileExtension", &File::getFileExtension)
        .def ("hasFileExtension", &File::hasFileExtension, "extensionToTest"_a)
        .def ("withFileExtension", &File::withFileExtension, "newExtension"_a)
        .def ("getFileNameWithoutExtension", &File::getFileNameWithoutExtension)
        .def ("hashCode", &File::hashCode)
        .def ("hashCode64", &File::hashCode64)
        .def ("getChildFile", &File::getChildFile, "relativeOrAbsolutePath"_a)
        .def ("getSiblingFile", &File::getSiblingFile, "siblingFileName"_a)
        .def ("getParentDirectory", &File::getParentDirectory)
        .def ("isAChildOf", &File::isAChildOf, "potentialParentDirectory"_a)
        .def ("getNonexistentChildFile", &File::getNonexistentChildFile, "prefix"_a, "suffix"_a, "putNumbersInBrackets"_a = true)
        .def ("getNonexistentSibling", &File::getNonexistentSibling, "putNumbersInBrackets"_a = true)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self > py::self)
        .def ("hasWriteAccess", &File::hasWriteAccess)
        .def ("hasReadAccess", &File::hasReadAccess)
        .def ("setReadOnly", &File::setReadOnly, "shouldBeReadOnly"_a, "applyRecursively"_a = false)
        .def ("setExecutePermission", &File::setExecutePermission, "shouldBeExecutable"_a)
        .def ("isHidden", &File::isHidden)
        .def ("getFileIdentifier", &File::getFileIdentifier)
        .def ("getLastModificationTime", &File::getLastModificationTime)
        .def ("getLastAccessTime", &File::getLastAccessTime)
        .def ("getCreationTime", &File::getCreationTime)
        .def ("setLastModificationTime", &File::setLastModificationTime, "newTime"_a)
        .def ("setLastAccessTime", &File::setLastAccessTime, "newTime"_a)
        .def ("setCreationTime", &File::setCreationTime, "newTime"_a)
        .def ("getVersion", &File::getVersion)
        .def ("create", &File::create)
        .def ("createDirectory", &File::createDirectory)
        .def ("deleteFile", &File::deleteFile)
        .def ("deleteRecursively", &File::deleteRecursively, "followSymlinks"_a = false)
        .def ("moveToTrash", &File::moveToTrash)
        .def ("moveFileTo", &File::moveFileTo, "targetLocation"_a)
        .def ("copyFileTo", &File::copyFileTo, "targetLocation"_a)
        .def ("replaceFileIn", &File::replaceFileIn, "targetLocation"_a)
        .def ("copyDirectoryTo", &File::copyDirectoryTo, "newDirectory"_a)
        .def ("findChildFiles", py::overload_cast<int, bool, const String &, File::FollowSymlinks> (&File::findChildFiles, py::const_),
            "whatToLookFor"_a, "searchRecursively"_a, "wildCardPattern"_a = "*", "followSymlinks"_a = File::FollowSymlinks::yes)
        .def ("findChildFiles", py::overload_cast<Array<File>&, int, bool, const String &, File::FollowSymlinks> (&File::findChildFiles, py::const_),
             "results"_a, "whatToLookFor"_a, "searchRecursively"_a, "wildCardPattern"_a = "*", "followSymlinks"_a = File::FollowSymlinks::yes)
        .def ("getNumberOfChildFiles", &File::getNumberOfChildFiles, "whatToLookFor"_a, "wildCardPattern"_a = "*")
        .def ("containsSubDirectories", &File::containsSubDirectories)
        .def ("createInputStream", &File::createInputStream)
        .def ("createOutputStream", &File::createOutputStream, "bufferSize"_a = 0x8000)
        .def ("loadFileAsData", &File::loadFileAsData, "result"_a)
        .def ("loadFileAsString", &File::loadFileAsString)
        .def ("readLines", &File::readLines, "destLines"_a)
        .def ("appendData", [](const File& self, py::buffer data)
        {
            auto info = data.request();
            return self.appendData (info.ptr, static_cast<size_t> (info.size));
        }, "dataToAppend"_a)
        .def ("replaceWithData", [](const File& self, py::buffer data)
        {
            auto info = data.request();
            return self.replaceWithData (info.ptr, static_cast<size_t> (info.size));
        }, "dataToWrite"_a)
        .def ("appendText", &File::appendText,
            "textToAppend"_a, "asUnicode"_a = false, "writeUnicodeHeaderBytes"_a = false, "lineEndings"_a = "\r\n")
        .def ("replaceWithText", &File::replaceWithText,
            "textToWrite"_a, "asUnicode"_a = false, "writeUnicodeHeaderBytes"_a = false, "lineEndings"_a = "\r\n")
        .def ("hasIdenticalContentTo", &File::hasIdenticalContentTo, "other"_a)
        .def_static ("findFileSystemRoots", &File::findFileSystemRoots)
        .def ("getVolumeLabel", &File::getVolumeLabel)
        .def ("getVolumeSerialNumber", &File::getVolumeSerialNumber)
        .def ("getBytesFreeOnVolume", &File::getBytesFreeOnVolume)
        .def ("getVolumeTotalSize", &File::getVolumeTotalSize)
        .def ("isOnCDRomDrive", &File::isOnCDRomDrive)
        .def ("isOnHardDisk", &File::isOnHardDisk)
        .def ("isOnRemovableDrive", &File::isOnRemovableDrive)
        .def ("startAsProcess", &File::startAsProcess, "parameters"_a = String())
        .def ("revealToUser", &File::revealToUser)
        .def_static ("getSpecialLocation", &File::getSpecialLocation, "type"_a)
        .def_static ("createTempFile", &File::createTempFile, "fileNameEnding"_a)
        .def_static ("getCurrentWorkingDirectory", &File::getCurrentWorkingDirectory)
        .def ("setAsCurrentWorkingDirectory", &File::setAsCurrentWorkingDirectory)
        .def_static ("getSeparatorChar", [] { return static_cast<uint32> (File::getSeparatorChar()); })
        .def_static ("getSeparatorString", &File::getSeparatorString)
        .def_static ("createLegalFileName", &File::createLegalFileName, "fileNameToFix"_a)
        .def_static ("createLegalPathName", &File::createLegalPathName, "pathNameToFix"_a)
        .def_static ("areFileNamesCaseSensitive", &File::areFileNamesCaseSensitive)
        .def_static ("isAbsolutePath", &File::isAbsolutePath, "path"_a)
        .def_static ("createFileWithoutCheckingPath", &File::createFileWithoutCheckingPath, "absolutePath"_a)
        .def_static ("addTrailingSeparator", &File::addTrailingSeparator, "path"_a)
        .def ("createSymbolicLink", py::overload_cast<const File&, bool> (&File::createSymbolicLink, py::const_), "linkFileToCreate"_a, "overwriteExisting"_a)
    //.def_static ("createSymbolicLink", [](const File& linkFileToCreate, const String& nativePathOfTarget, bool overwriteExisting) {
    //    return File::createSymbolicLink (linkFileToCreate, nativePathOfTarget, overwriteExisting);
    //}, "linkFileToCreate"_a, "nativePathOfTarget"_a, "overwriteExisting"_a)
        .def ("isSymbolicLink", &File::isSymbolicLink)
        .def ("getLinkedTarget", &File::getLinkedTarget)
        .def ("getNativeLinkedTarget", &File::getNativeLinkedTarget)
#if JUCE_WINDOWS
        .def ("createShortcut", &File::createShortcut, "description"_a, "linkFileToCreate"_a)
        .def ("isShortcut", &File::isShortcut)
#elif JUCE_MAC
        .def ("getMacOSType", &File::getMacOSType)
        .def ("isBundle", &File::isBundle)
        .def ("addToDock", &File::addToDock)
        .def_static ("getContainerForSecurityApplicationGroupIdentifier", &File::getContainerForSecurityApplicationGroupIdentifier, "appGroup"_a)
#endif
        .def ("__repr__", [](const File& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.getFullPathName() << "')";
            return result;
        })
    ;

    // ============================================================================================ juce::File*Stream

    py::class_<FileInputStream, InputStream, PyInputStream<FileInputStream>> classFileInputStream (m, "FileInputStream");

    classFileInputStream
        .def (py::init<const File&>(), "fileToRead"_a)
        .def ("getFile", &FileInputStream::getFile, py::return_value_policy::reference_internal)
        .def ("getStatus", &FileInputStream::getStatus, py::return_value_policy::reference_internal)
        .def ("failedToOpen", &FileInputStream::failedToOpen)
        .def ("openedOk", &FileInputStream::openedOk)
    ;

    py::class_<FileInputSource, InputSource> classFileInputSource (m, "FileInputSource");

    classFileInputSource
        .def (py::init<const File&, bool>(), "file"_a, "useFileTimeInHashGeneration"_a = false)
    ;

    py::class_<FileOutputStream, OutputStream, PyOutputStream<FileOutputStream>> classFileOutputStream (m, "FileOutputStream");

    classFileOutputStream
        .def (py::init<const File&, size_t>(), "fileToWriteTo"_a, "bufferSizeToUse"_a = 16384)
        .def ("getFile", &FileOutputStream::getFile, py::return_value_policy::reference_internal)
        .def ("getStatus", &FileOutputStream::getStatus, py::return_value_policy::reference_internal)
        .def ("failedToOpen", &FileOutputStream::failedToOpen)
        .def ("openedOk", &FileOutputStream::openedOk)
        .def ("truncate", &FileOutputStream::truncate)
    ;

    // ============================================================================================ juce::MemoryMappedFile

    py::class_<MemoryMappedFile> classMemoryMappedFile (m, "MemoryMappedFile");

    py::enum_<MemoryMappedFile::AccessMode> (classMemoryMappedFile, "AccessMode")
        .value ("readOnly", MemoryMappedFile::AccessMode::readOnly)
        .value ("readWrite", MemoryMappedFile::AccessMode::readWrite)
        .export_values();

    classMemoryMappedFile
        .def (py::init<const File&, MemoryMappedFile::AccessMode, bool>(), "file"_a, "mode"_a, "exclusive"_a = false)
        .def (py::init<const File&, const Range<int64>&, MemoryMappedFile::AccessMode, bool>(), "file"_a, "fileRange"_a, "mode"_a, "exclusive"_a = false)
        .def ("getData", [](const MemoryMappedFile& self) -> std::optional<py::memoryview>
        {
            if (self.getData() == nullptr)
                return std::nullopt;

            return py::memoryview::from_memory (self.getData(), static_cast<Py_ssize_t> (self.getSize()));
        }, py::return_value_policy::reference_internal)
        .def ("getSize", &MemoryMappedFile::getSize)
        .def ("getRange", &MemoryMappedFile::getRange)
    ;

    // ============================================================================================ juce::FileFilter

    py::class_<FileSearchPath> classFileSearchPath (m, "FileSearchPath");

    classFileSearchPath
        .def (py::init<>())
        .def (py::init<const String&>())
        .def ("getNumPaths", &FileSearchPath::getNumPaths)
        .def ("__getitem__", [](const FileSearchPath& self, int index) { return self[index]; })
        .def ("getRawString", &FileSearchPath::getRawString)
        .def ("toString", &FileSearchPath::toString)
        .def ("toStringWithSeparator", &FileSearchPath::toStringWithSeparator)
        .def ("add", &FileSearchPath::add)
        .def ("addIfNotAlreadyThere", &FileSearchPath::addIfNotAlreadyThere)
        .def ("remove", &FileSearchPath::remove)
        .def ("addPath", &FileSearchPath::addPath)
        .def ("removeRedundantPaths", &FileSearchPath::removeRedundantPaths)
        .def ("removeNonExistentPaths", &FileSearchPath::removeNonExistentPaths)
        .def ("findChildFiles", py::overload_cast<int, bool, const String&> (&FileSearchPath::findChildFiles, py::const_),
            "whatToLookFor"_a, "searchRecursively"_a, "wildCardPattern"_a = "*")
        //.def ("findChildFiles", [](const FileSearchPath& self, int whatToLookFor, bool searchRecursively, const String& wildCardPattern)
        //{
        //}, "whatToLookFor"_a, "searchRecursively"_a, "wildCardPattern"_a = "*")
        .def ("isFileInPath", &FileSearchPath::isFileInPath)
    ;

    // ============================================================================================ juce::FileFilter

    py::class_<FileFilter, PyFileFilter<>> classFileFilter (m, "FileFilter");

    classFileFilter
        .def (py::init<const String&>())
        .def ("getDescription", &FileFilter::getDescription, py::return_value_policy::reference_internal)
        .def ("isFileSuitable", &FileFilter::isFileSuitable)
        .def ("isDirectorySuitable", &FileFilter::isDirectorySuitable)
    ;

    py::class_<WildcardFileFilter, FileFilter, PyFileFilter<WildcardFileFilter>> classWildcardFileFilter (m, "WildcardFileFilter");

    classWildcardFileFilter
        .def (py::init<const String&, const String&, const String&>())
    ;

    // ============================================================================================ juce::TemporaryFile

    py::class_<TemporaryFile> classTemporaryFile (m, "TemporaryFile");

    py::enum_<TemporaryFile::OptionFlags> (classTemporaryFile, "OptionFlags", py::arithmetic())
        .value ("useHiddenFile", TemporaryFile::OptionFlags::useHiddenFile)
        .value ("putNumbersInBrackets", TemporaryFile::OptionFlags::putNumbersInBrackets)
        .export_values();

    classTemporaryFile
        .def (py::init<const String&, int>(), "suffix"_a = String(), "optionFlags"_a = 0)
        .def (py::init<const File&, int>(), "targetFile"_a, "optionFlags"_a = 0)
        .def (py::init<const File&, const File&>(), "targetFile"_a, "temporaryFile"_a)
        .def ("getFile", &TemporaryFile::getFile)
        .def ("getTargetFile", &TemporaryFile::getTargetFile)
        .def ("overwriteTargetFileWithTemporary", &TemporaryFile::overwriteTargetFileWithTemporary)
        .def ("deleteTemporaryFile", &TemporaryFile::deleteTemporaryFile)
        .def ("__enter__", [](TemporaryFile& self)
        {
            return std::addressof(self);
        }, py::return_value_policy::reference)
        .def ("__exit__", [](TemporaryFile& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.overwriteTargetFileWithTemporary();
        })
    ;

    // ============================================================================================ juce::URL

    py::class_<DirectoryEntry> classDirectoryEntry (m, "DirectoryEntry");

    classDirectoryEntry
        .def (py::init<>())
        .def ("getFile", &DirectoryEntry::getFile)
        .def ("getModificationTime", &DirectoryEntry::getModificationTime)
        .def ("getCreationTime", &DirectoryEntry::getCreationTime)
        .def ("getFileSize", &DirectoryEntry::getFileSize)
        .def ("isDirectory", &DirectoryEntry::isDirectory)
        .def ("isHidden", &DirectoryEntry::isHidden)
        .def ("isReadOnly", &DirectoryEntry::isReadOnly)
        .def ("getEstimatedProgress", &DirectoryEntry::getEstimatedProgress)
    ;

    py::class_<RangedDirectoryIterator> classRangedDirectoryIterator (m, "RangedDirectoryIterator");

    classRangedDirectoryIterator
        .def (py::init<>())
        .def (py::init<const File&, bool, const String&, int, File::FollowSymlinks>(),
            "directory"_a, "isRecursive"_a, "wildCard"_a = "*", "whatToLookFor"_a = File::findFiles, "followSymlinks"_a = File::FollowSymlinks::yes)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("__iter__", [](const RangedDirectoryIterator& self)
        {
            return py::make_iterator (begin (self), end (self));
        }, py::keep_alive<0, 1>())
    ;

    // ============================================================================================ juce::URL

    py::class_<URL> classURL (m, "URL");

    py::enum_<URL::ParameterHandling> (classURL, "ParameterHandling")
        .value ("inAddress", URL::ParameterHandling::inAddress)
        .value ("inPostData", URL::ParameterHandling::inPostData);

    classURL
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<File>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("toString", &URL::toString, "includeGetParameters"_a)
        .def ("isEmpty", &URL::isEmpty)
        .def ("isWellFormed", &URL::isWellFormed)
        .def ("getDomain", &URL::getDomain)
        .def ("getSubPath", &URL::getSubPath)
        .def ("getQueryString", &URL::getQueryString)
        .def ("getAnchorString", &URL::getAnchorString)
        .def ("getScheme", &URL::getScheme)
        .def ("isLocalFile", &URL::isLocalFile)
        .def ("getLocalFile", &URL::getLocalFile)
        .def ("getFileName", &URL::getFileName)
        .def ("getPort", &URL::getPort)
        .def ("withNewDomainAndPath", &URL::withNewDomainAndPath)
        .def ("withNewSubPath", &URL::withNewSubPath)
        .def ("getParentURL", &URL::getParentURL)
        .def ("getChildURL", &URL::getChildURL)
        .def ("withParameter", &URL::withParameter)
        .def ("withParameters", &URL::withParameters)
        .def ("withAnchor", &URL::withAnchor)
        .def ("withFileToUpload", &URL::withFileToUpload)
        .def ("withDataToUpload", &URL::withDataToUpload)
        .def ("getParameterNames", &URL::getParameterNames)
        .def ("getParameterValues", &URL::getParameterValues)
        .def ("withPOSTData", py::overload_cast<const String &> (&URL::withPOSTData, py::const_))
        .def ("withPOSTData", py::overload_cast<const MemoryBlock &> (&URL::withPOSTData, py::const_))
        .def ("getPostData", &URL::getPostData)
        .def ("getPostDataAsMemoryBlock", &URL::getPostDataAsMemoryBlock)
        .def ("launchInDefaultBrowser", &URL::launchInDefaultBrowser)
        .def_static ("isProbablyAWebsiteURL", &URL::isProbablyAWebsiteURL)
        .def_static ("isProbablyAnEmailAddress", &URL::isProbablyAnEmailAddress)
        .def ("createInputStream", py::overload_cast<const URL::InputStreamOptions &> (&URL::createInputStream, py::const_))
        .def ("createOutputStream", &URL::createOutputStream)
        .def ("downloadToFile", py::overload_cast<const File &, const URL::DownloadTaskOptions &> (&URL::downloadToFile))
        .def ("readEntireBinaryStream", &URL::readEntireBinaryStream)
        .def ("readEntireTextStream", &URL::readEntireTextStream)
        .def ("readEntireXmlStream", &URL::readEntireXmlStream)
        .def_static ("addEscapeChars", &URL::addEscapeChars)
        .def_static ("removeEscapeChars", &URL::removeEscapeChars)
        .def_static ("createWithoutParsing", &URL::createWithoutParsing)
        .def ("__repr__", [](const URL& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.toString (true) << "')";
            return result;
        })
        .def ("__str__", [](const URL& self) { return self.toString (true); })
    ;

    py::class_<URL::InputStreamOptions> classURLInputStreamOptions (classURL, "InputStreamOptions");

    classURLInputStreamOptions
        .def (py::init<URL::ParameterHandling>())
        .def ("withProgressCallback", &URL::InputStreamOptions::withProgressCallback)
        .def ("withExtraHeaders", &URL::InputStreamOptions::withExtraHeaders)
        .def ("withConnectionTimeoutMs", &URL::InputStreamOptions::withConnectionTimeoutMs)
        .def ("withResponseHeaders", &URL::InputStreamOptions::withResponseHeaders)
        .def ("withStatusCode", &URL::InputStreamOptions::withStatusCode)
        .def ("withNumRedirectsToFollow", &URL::InputStreamOptions::withNumRedirectsToFollow)
        .def ("withHttpRequestCmd", &URL::InputStreamOptions::withHttpRequestCmd)
        .def ("getParameterHandling", &URL::InputStreamOptions::getParameterHandling)
        .def ("getProgressCallback", &URL::InputStreamOptions::getProgressCallback)
        .def ("getExtraHeaders", &URL::InputStreamOptions::getExtraHeaders)
        .def ("getConnectionTimeoutMs", &URL::InputStreamOptions::getConnectionTimeoutMs)
        .def ("getResponseHeaders", &URL::InputStreamOptions::getResponseHeaders)
        .def ("getStatusCode", &URL::InputStreamOptions::getStatusCode)
        .def ("getNumRedirectsToFollow", &URL::InputStreamOptions::getNumRedirectsToFollow)
        .def ("getHttpRequestCmd", &URL::InputStreamOptions::getHttpRequestCmd)
    ;

    py::class_<URL::DownloadTask> classURLDownloadTask (classURL, "DownloadTask");
    py::class_<URL::DownloadTaskListener, PyURLDownloadTaskListener> classURLDownloadTaskListener (classURL, "DownloadTaskListener");

    classURLDownloadTaskListener
        .def (py::init<>())
        .def ("finished", &URL::DownloadTaskListener::finished)
        .def ("progress", &URL::DownloadTaskListener::progress)
    ;

    classURLDownloadTask
        .def ("getTotalLength", &URL::DownloadTask::getTotalLength)
        .def ("getLengthDownloaded", &URL::DownloadTask::getLengthDownloaded)
        .def ("isFinished", &URL::DownloadTask::isFinished)
        .def ("statusCode", &URL::DownloadTask::statusCode)
        .def ("hadError", &URL::DownloadTask::hadError)
        .def ("getTargetLocation", &URL::DownloadTask::getTargetLocation)
        .def_property_readonly("Listener", [classURLDownloadTaskListener](const URL::DownloadTask&) { return classURLDownloadTaskListener; })
    ;

    py::class_<URL::DownloadTaskOptions> classURLDownloadTaskOptions (classURL, "DownloadTaskOptions");

    classURLDownloadTaskOptions
        .def (py::init<>())
        .def ("withExtraHeaders", &URL::DownloadTaskOptions::withExtraHeaders)
        .def ("withSharedContainer", &URL::DownloadTaskOptions::withSharedContainer)
        .def ("withListener", &URL::DownloadTaskOptions::withListener)
        .def ("withUsePost", &URL::DownloadTaskOptions::withUsePost)
        .def_readwrite ("extraHeaders", &URL::DownloadTaskOptions::extraHeaders)
        .def_readwrite ("sharedContainer", &URL::DownloadTaskOptions::sharedContainer)
        .def_readwrite ("listener", &URL::DownloadTaskOptions::listener)
        .def_readwrite ("usePost", &URL::DownloadTaskOptions::usePost)
    ;

    // ============================================================================================ juce::URLInputSource

    py::class_<URLInputSource, InputSource, PyInputSource<URLInputSource>> classURLInputSource (m, "URLInputSource");

    classURLInputSource
        .def (py::init<const URL&>())
    ;

    // ============================================================================================ juce::PerformanceCounter

    py::class_<PerformanceCounter> classPerformanceCounter (m, "PerformanceCounter");
    py::class_<PerformanceCounter::Statistics> classPerformanceCounterStatistics (classPerformanceCounter, "Statistics");

    classPerformanceCounterStatistics
        .def (py::init<>())
        .def ("clear", &PerformanceCounter::Statistics::clear)
        .def ("toString", &PerformanceCounter::Statistics::toString)
        .def ("addResult", &PerformanceCounter::Statistics::addResult)
        .def_readwrite ("name", &PerformanceCounter::Statistics::name)
        .def_readwrite ("averageSeconds", &PerformanceCounter::Statistics::averageSeconds)
        .def_readwrite ("maximumSeconds", &PerformanceCounter::Statistics::maximumSeconds)
        .def_readwrite ("minimumSeconds", &PerformanceCounter::Statistics::minimumSeconds)
        .def_readwrite ("totalSeconds", &PerformanceCounter::Statistics::totalSeconds)
        .def_readwrite ("numRuns", &PerformanceCounter::Statistics::numRuns)
        .def ("__str__", &PerformanceCounter::Statistics::toString)
    ;

    classPerformanceCounter
        .def (py::init<const String&, int, const File&>(), "counterName"_a, "runsPerPrintout"_a = 100, "loggingFile"_a = File())
        .def ("start", &PerformanceCounter::start)
        .def ("stop", &PerformanceCounter::stop)
        .def ("printStatistics", &PerformanceCounter::printStatistics)
        .def ("getStatisticsAndReset", &PerformanceCounter::getStatisticsAndReset)
        .def ("__enter__", [](PerformanceCounter& self)
        {
            self.start();
        }, py::return_value_policy::reference)
        .def ("__exit__", [](PerformanceCounter& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.stop();
            return self.getStatisticsAndReset();
        })
    ;

    // ============================================================================================ juce::CriticalSection

    py::class_<CriticalSection> classCriticalSection (m, "CriticalSection");

    classCriticalSection
        .def (py::init<>())
        .def ("enter", &CriticalSection::enter, py::call_guard<py::gil_scoped_release>())
        .def ("tryEnter", &CriticalSection::tryEnter)
        .def ("exit", &CriticalSection::exit)
    ;

    py::class_<PyGenericScopedLock<CriticalSection>> classScopedLockCriticalSection (classCriticalSection, "ScopedLockType");

    classScopedLockCriticalSection
        .def (py::init<CriticalSection&>(), "lock"_a)
        .def ("__enter__", [](PyGenericScopedLock<CriticalSection>& self) -> PyGenericScopedLock<CriticalSection>*
        {
            self.enter();
            return std::addressof (self);
        }, py::return_value_policy::reference, py::call_guard<py::gil_scoped_release>())
        .def ("__exit__", [](PyGenericScopedLock<CriticalSection>& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.exit();
        })
    ;

    py::class_<PyGenericScopedUnlock<CriticalSection>> classScopedUnlockCriticalSection (classCriticalSection, "ScopedUnlockType");

    classScopedUnlockCriticalSection
        .def (py::init<CriticalSection&>(), "lock"_a)
        .def ("__enter__", [](PyGenericScopedUnlock<CriticalSection>& self) -> PyGenericScopedUnlock<CriticalSection>*
        {
            self.enter();
            return std::addressof (self);
        }, py::return_value_policy::reference, py::call_guard<py::gil_scoped_release>())
        .def ("__exit__", [](PyGenericScopedUnlock<CriticalSection>& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.exit();
        })
    ;

    py::class_<PyGenericScopedTryLock<CriticalSection>> classScopedTryLockCriticalSection (classCriticalSection, "ScopedTryLock");

    classScopedTryLockCriticalSection
        .def (py::init<CriticalSection&, int>(), "lock"_a, "acquireLockOnInitialisation"_a = true)
        .def ("__enter__", [](PyGenericScopedTryLock<CriticalSection>& self) -> PyGenericScopedTryLock<CriticalSection>*
        {
            self.enter();
            return std::addressof (self);
        }, py::return_value_policy::reference, py::call_guard<py::gil_scoped_release>())
        .def ("__exit__", [](PyGenericScopedTryLock<CriticalSection>& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.exit();
        })
    ;

    // ============================================================================================ juce::SpinLock

    py::class_<SpinLock> classSpinLock (m, "SpinLock");

    classSpinLock
        .def (py::init<>())
        .def ("enter", &SpinLock::enter, py::call_guard<py::gil_scoped_release>())
        .def ("tryEnter", &SpinLock::tryEnter)
        .def ("exit", &SpinLock::exit)
    ;

    // ============================================================================================ juce::WaitableEvent

    py::class_<WaitableEvent> classWaitableEvent (m, "WaitableEvent");

    classWaitableEvent
        .def (py::init<bool>(), "manualReset"_a = false)
        .def ("wait", &WaitableEvent::wait, "timeOutMilliseconds"_a = -1.0, py::call_guard<py::gil_scoped_release>())
        .def ("signal", &WaitableEvent::signal)
        .def ("reset", &WaitableEvent::reset)
    ;

    // ============================================================================================ juce::ReadWriteLock

    py::class_<ReadWriteLock> classReadWriteLock (m, "ReadWriteLock");

    classReadWriteLock
        .def (py::init<>())
        .def ("enterRead", &ReadWriteLock::enterRead, py::call_guard<py::gil_scoped_release>())
        .def ("tryEnterRead", &ReadWriteLock::tryEnterRead)
        .def ("exitRead", &ReadWriteLock::exitRead, py::call_guard<py::gil_scoped_release>())
        .def ("enterWrite", &ReadWriteLock::enterWrite, py::call_guard<py::gil_scoped_release>())
        .def ("tryEnterWrite", &ReadWriteLock::tryEnterWrite)
        .def ("exitWrite", &ReadWriteLock::exitWrite, py::call_guard<py::gil_scoped_release>())
    ;

    // ============================================================================================ juce::InterProcessLock

    py::class_<InterProcessLock> classInterProcessLock (m, "InterProcessLock");

    classInterProcessLock
        .def (py::init<const String&>())
        .def ("enter", &InterProcessLock::enter, "timeOutMillisecs"_a = -1, py::call_guard<py::gil_scoped_release>())
        .def ("exit", &InterProcessLock::exit)
    ;

    // ============================================================================================ juce::HighResolutionTimer

    py::class_<HighResolutionTimer, PyHighResolutionTimer> classHighResolutionTimer (m, "HighResolutionTimer");

    classHighResolutionTimer
        .def (py::init<>())
        .def ("hiResTimerCallback", &HighResolutionTimer::hiResTimerCallback)
        .def ("startTimer", &HighResolutionTimer::startTimer)
        .def ("stopTimer", &HighResolutionTimer::stopTimer)
        .def ("isTimerRunning", &HighResolutionTimer::isTimerRunning)
        .def ("getTimerInterval", &HighResolutionTimer::getTimerInterval)
    ;

    // ============================================================================================ juce::ChildProcess

    py::class_<ChildProcess> classChildProcess (m, "ChildProcess");

    py::enum_<ChildProcess::StreamFlags> (classChildProcess, "StreamFlags", py::arithmetic())
        .value ("wantStdOut", ChildProcess::StreamFlags::wantStdOut)
        .value ("wantStdErr", ChildProcess::StreamFlags::wantStdErr);

    classChildProcess
        .def (py::init<>())
        .def ("start", py::overload_cast<const String&, int> (&ChildProcess::start), "command"_a, "streamFlags"_a = ChildProcess::wantStdOut | ChildProcess::wantStdErr)
        .def ("start", py::overload_cast<const StringArray&, int> (&ChildProcess::start), "command"_a, "streamFlags"_a = ChildProcess::wantStdOut | ChildProcess::wantStdErr)
        .def ("start", [](ChildProcess& self, const String& command, ChildProcess::StreamFlags streamFlags)
            { return self.start (command, streamFlags); }, "command"_a, "streamFlags"_a)
        .def ("start", [](ChildProcess& self, const StringArray& command, ChildProcess::StreamFlags streamFlags)
            { return self.start (command, streamFlags); }, "command"_a, "streamFlags"_a)
        .def ("isRunning", &ChildProcess::isRunning)
        .def ("readProcessOutput", [](ChildProcess& self, py::buffer data)
        {
            auto info = data.request();
            return self.readProcessOutput (info.ptr, static_cast<int> (info.size));
        })
        .def ("readAllProcessOutput", &ChildProcess::readAllProcessOutput)
        .def ("waitForProcessToFinish", &ChildProcess::waitForProcessToFinish)
        .def ("getExitCode", &ChildProcess::getExitCode)
        .def ("kill", &ChildProcess::kill)
    ;

    // ============================================================================================ juce::Thread

    py::class_<Thread, PyThread<>> classThread (m, "Thread");

    py::enum_<Thread::Priority> (classThread, "Priority")
        .value ("highest", Thread::Priority::highest)
        .value ("high", Thread::Priority::high)
        .value ("normal", Thread::Priority::normal)
        .value ("low", Thread::Priority::low)
        .value ("background", Thread::Priority::background);

    py::class_<Thread::RealtimeOptions> classThreadRealtimeOptions (classThread, "RealtimeOptions");

    classThreadRealtimeOptions
        .def (py::init<>())
        .def ("withPriority", [](Thread::RealtimeOptions& self, Thread::Priority priority) { return self.withPriority (static_cast<int> (priority)); })
        .def ("withProcessingTimeMs", &Thread::RealtimeOptions::withProcessingTimeMs)
        .def ("withMaximumProcessingTimeMs", &Thread::RealtimeOptions::withMaximumProcessingTimeMs)
        .def ("withApproximateAudioProcessingTime", &Thread::RealtimeOptions::withApproximateAudioProcessingTime)
        .def ("withPeriodMs", &Thread::RealtimeOptions::withPeriodMs)
        .def ("withPeriodHz", &Thread::RealtimeOptions::withPeriodHz)
        .def ("getPriority", [](Thread::RealtimeOptions& self) { return static_cast<Thread::Priority> (self.getPriority()); })
        .def ("getProcessingTimeMs", &Thread::RealtimeOptions::getProcessingTimeMs)
        .def ("getMaximumProcessingTimeMs", &Thread::RealtimeOptions::getMaximumProcessingTimeMs)
        .def ("getPeriodMs", &Thread::RealtimeOptions::getPeriodMs)
    ;

    py::class_<Thread::Listener, PyThreadListener> classThreadListener (classThread, "Listener");

    classThreadListener
        .def (py::init<>())
        .def ("exitSignalSent", &Thread::Listener::exitSignalSent)
    ;

    classThread
        .def (py::init<const String&, size_t>(), "threadName"_a, "threadStackSize"_a = Thread::osDefaultStackSize)
        .def ("run", &Thread::run)
        .def ("startThread", py::overload_cast<> (&Thread::startThread))
        .def ("startThread", py::overload_cast<Thread::Priority> (&Thread::startThread))
        .def ("startRealtimeThread", &Thread::startRealtimeThread)
        .def ("stopThread", &Thread::stopThread, py::call_guard<py::gil_scoped_release>())
    //.def_static ("launch", static_cast<bool (*)(std::function<void()>)> (&Thread::launch))
    //.def_static ("launch", static_cast<bool (*)(Thread::Priority priority, std::function<void()>)> (&Thread::launch))
        .def ("isThreadRunning", &Thread::isThreadRunning)
        .def ("signalThreadShouldExit", &Thread::signalThreadShouldExit)
        .def ("threadShouldExit", &Thread::threadShouldExit)
        .def_static ("currentThreadShouldExit", &Thread::currentThreadShouldExit)
        .def ("waitForThreadToExit", &Thread::waitForThreadToExit, py::call_guard<py::gil_scoped_release>())
        .def ("addListener", &Thread::addListener)
        .def ("removeListener", &Thread::removeListener)
        .def ("isRealtime", &Thread::isRealtime)
        .def ("setAffinityMask", &Thread::setAffinityMask)
        .def_static ("setCurrentThreadAffinityMask", &Thread::setCurrentThreadAffinityMask)
        .def_static ("sleep", &Thread::sleep, py::call_guard<py::gil_scoped_release>())
        .def_static ("yield", &Thread::yield, py::call_guard<py::gil_scoped_release>())
        .def ("wait", &Thread::wait, py::call_guard<py::gil_scoped_release>())
        .def ("notify", &Thread::notify)
        .def_static ("getCurrentThreadId", [] { return PyThreadID (Thread::getCurrentThreadId()); })
        .def_static ("getCurrentThread", &Thread::getCurrentThread, py::return_value_policy::reference)
        .def ("getThreadId", [](const Thread& self) { return PyThreadID (self.getThreadId()); })
        .def ("getThreadName", &Thread::getThreadName)
        .def_static ("setCurrentThreadName", &Thread::setCurrentThreadName)
    //.def ("getPriority", &Thread::getPriority)
    //.def ("setPriority", &Thread::setPriority)
    ;

    py::class_<PyThreadID> classThreadID (classThread, "ThreadID");

    classThreadID
        .def (py::init([](Thread::ThreadID value)
        {
            return PyThreadID (value);
        }))
        .def (py::init([](const PyThreadID& other)
        {
            return PyThreadID (static_cast<Thread::ThreadID> (other));
        }))
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("__str__", [](const PyThreadID& self)
        {
            return String::formatted ("%p", static_cast<Thread::ThreadID> (self));
        })
    ;

    // ============================================================================================ juce::ThreadPool

    py::class_<ThreadPoolJob, PyThreadPoolJob> classThreadPoolJob (m, "ThreadPoolJob");

    py::enum_<ThreadPoolJob::JobStatus> (classThreadPoolJob, "JobStatus")
        .value ("jobHasFinished", ThreadPoolJob::JobStatus::jobHasFinished)
        .value ("jobNeedsRunningAgain", ThreadPoolJob::JobStatus::jobNeedsRunningAgain)
        .export_values();

    classThreadPoolJob
        .def (py::init<const String&>())
        .def ("getJobName", &ThreadPoolJob::getJobName)
        .def ("setJobName", &ThreadPoolJob::setJobName)
        .def ("runJob", &ThreadPoolJob::runJob)
        .def ("isRunning", &ThreadPoolJob::isRunning)
        .def ("shouldExit", &ThreadPoolJob::shouldExit)
        .def ("signalJobShouldExit", &ThreadPoolJob::signalJobShouldExit)
        .def ("addListener", &ThreadPoolJob::addListener)
        .def ("removeListener", &ThreadPoolJob::removeListener)
        .def_static ("getCurrentThreadPoolJob", &ThreadPoolJob::getCurrentThreadPoolJob, py::return_value_policy::reference)
    ;

    py::class_<ThreadPoolOptions> classThreadPoolOptions (m, "ThreadPoolOptions");

    classThreadPoolOptions
        .def (py::init<>())
        .def ("withThreadName", &ThreadPoolOptions::withThreadName)
        .def ("withNumberOfThreads", &ThreadPoolOptions::withNumberOfThreads)
        .def ("withThreadStackSizeBytes", &ThreadPoolOptions::withThreadStackSizeBytes)
        .def ("withDesiredThreadPriority", &ThreadPoolOptions::withDesiredThreadPriority)
        .def_readwrite ("threadName", &ThreadPoolOptions::threadName)
        .def_readwrite ("numberOfThreads", &ThreadPoolOptions::numberOfThreads)
        .def_readwrite ("threadStackSizeBytes", &ThreadPoolOptions::threadStackSizeBytes)
        .def_readwrite ("desiredThreadPriority", &ThreadPoolOptions::desiredThreadPriority)
    ;

    py::class_<ThreadPool> classThreadPool (m, "ThreadPool");

    py::class_<ThreadPool::JobSelector, PyThreadPoolJobSelector> classThreadPoolJobSelector (classThreadPool, "JobSelector");

    classThreadPoolJobSelector
        .def (py::init<>())
        .def ("isJobSuitable", &ThreadPool::JobSelector::isJobSuitable)
    ;

    classThreadPool
        .def (py::init<>())
        .def (py::init<const ThreadPoolOptions&>(), "options"_a)
        .def (py::init<int, size_t, Thread::Priority>(),
            "numberOfThreads"_a, "threadStackSizeBytes"_a = Thread::osDefaultStackSize, "desiredThreadPriority"_a = Thread::Priority::normal)
        .def ("addJob", [](ThreadPool& self, ThreadPoolJob* job) { return self.addJob (job, false); })
        .def ("addJob", py::overload_cast<std::function<ThreadPoolJob::JobStatus()>> (&ThreadPool::addJob))
        .def ("addJob", py::overload_cast<std::function<void()>> (&ThreadPool::addJob))
        .def ("removeJob", &ThreadPool::removeJob, py::call_guard<py::gil_scoped_release>())
        .def ("removeAllJobs", [](ThreadPool& self, bool interruptRunningJobs, int timeOutMilliseconds, ThreadPool::JobSelector* selectedJobsToRemove)
        {
            self.removeAllJobs (interruptRunningJobs, timeOutMilliseconds, selectedJobsToRemove);
        }, "interruptRunningJobs"_a, "timeOutMilliseconds"_a, "selectedJobsToRemove"_a = nullptr, py::call_guard<py::gil_scoped_release>())
        .def ("getNumJobs", &ThreadPool::getNumJobs)
        .def ("getNumThreads", &ThreadPool::getNumThreads)
        .def ("getJob", &ThreadPool::getJob, py::return_value_policy::reference)
        .def ("contains", &ThreadPool::contains)
        .def ("isJobRunning", &ThreadPool::isJobRunning)
        .def ("waitForJobToFinish", &ThreadPool::waitForJobToFinish, py::call_guard<py::gil_scoped_release>())
        .def ("moveJobToFront", &ThreadPool::moveJobToFront, py::call_guard<py::gil_scoped_release>())
        .def ("getNamesOfAllJobs", &ThreadPool::getNamesOfAllJobs)
    ;

    // ============================================================================================ juce::TimeSliceThread

    py::class_<TimeSliceThread, Thread, PyThread<TimeSliceThread>> classTimeSliceThread (m, "TimeSliceThread");
    py::class_<TimeSliceClient, PyTimeSliceClient> classTimeSliceClient (m, "TimeSliceClient");

    classTimeSliceClient
        .def (py::init<>())
        .def ("useTimeSlice", &TimeSliceClient::useTimeSlice)
    ;

    classTimeSliceThread
        .def (py::init<const String&>(), "threadName"_a)
        .def ("addTimeSliceClient", &TimeSliceThread::addTimeSliceClient)
        .def ("moveToFrontOfQueue", &TimeSliceThread::moveToFrontOfQueue)
        .def ("removeTimeSliceClient", &TimeSliceThread::removeTimeSliceClient)
        .def ("removeAllClients", &TimeSliceThread::removeAllClients)
        .def ("getNumClients", &TimeSliceThread::getNumClients)
        .def ("getClient", &TimeSliceThread::getClient, py::return_value_policy::reference)
        .def ("contains", &TimeSliceThread::contains)
    ;

    // ============================================================================================ juce::Process

    py::class_<Process> classProcess (m, "Process");

    py::enum_<Process::ProcessPriority> (classProcess, "ProcessPriority")
        .value ("LowPriority", Process::ProcessPriority::LowPriority)
        .value ("NormalPriority", Process::ProcessPriority::NormalPriority)
        .value ("HighPriority", Process::ProcessPriority::HighPriority)
        .value ("RealtimePriority", Process::ProcessPriority::RealtimePriority)
        .export_values();

    classProcess
        .def_static ("setPriority", &Process::setPriority)
        .def_static ("terminate", &Process::terminate)
        .def_static ("isForegroundProcess", &Process::isForegroundProcess)
        .def_static ("makeForegroundProcess", &Process::makeForegroundProcess)
        .def_static ("hide", &Process::hide)
        .def_static ("raisePrivilege", &Process::raisePrivilege)
        .def_static ("lowerPrivilege", &Process::lowerPrivilege)
        .def_static ("isRunningUnderDebugger", &Process::isRunningUnderDebugger)
        .def_static ("openDocument", &Process::openDocument)
        .def_static ("openEmailWithAttachments", &Process::openEmailWithAttachments)
#if JUCE_MAC
        .def_static ("setDockIconVisible", &Process::setDockIconVisible)
#endif
#if JUCE_MAC || JUCE_LINUX || JUCE_BSD
        .def_static ("setMaxNumberOfFileHandles", &Process::setMaxNumberOfFileHandles)
#endif
    ;

    // ============================================================================================ juce::XmlElement

    py::class_<XmlElement, std::unique_ptr<XmlElement>> classXmlElement (m, "XmlElement");
    py::class_<XmlElement::TextFormat> classXmlElementTextFormat (classXmlElement, "TextFormat");
    py::class_<PyXmlElementComparator> classXmlElementComparator (classXmlElement, "Comparator");

    classXmlElementComparator
        .def (py::init<>())
        .def ("compareElements", &PyXmlElementComparator::compareElements)
    ;

    classXmlElementTextFormat
        .def (py::init<>())
        .def_readwrite ("dtd", &XmlElement::TextFormat::dtd)
        .def_readwrite ("customHeader", &XmlElement::TextFormat::customHeader)
        .def_readwrite ("customEncoding", &XmlElement::TextFormat::customEncoding)
        .def_readwrite ("addDefaultHeader", &XmlElement::TextFormat::addDefaultHeader)
        .def_readwrite ("lineWrapLength", &XmlElement::TextFormat::lineWrapLength)
        .def_readwrite ("newLineChars", &XmlElement::TextFormat::newLineChars)
        .def ("singleLine", &XmlElement::TextFormat::singleLine)
        .def ("withoutHeader", &XmlElement::TextFormat::withoutHeader)
    ;

    classXmlElement
        .def (py::init<const char*>())
        .def (py::init<const String&>())
        .def (py::init<const Identifier&>())
        .def (py::init<const XmlElement&>())
        .def ("isEquivalentTo", &XmlElement::isEquivalentTo, "other"_a, "ignoreOrderOfAttributes"_a)
        .def ("toString", &XmlElement::toString, "format"_a = XmlElement::TextFormat())
        .def ("writeTo", py::overload_cast<OutputStream&, const XmlElement::TextFormat&> (&XmlElement::writeTo, py::const_), "output"_a, "format"_a = XmlElement::TextFormat())
        .def ("writeTo", py::overload_cast<const File& , const XmlElement::TextFormat&> (&XmlElement::writeTo, py::const_), "output"_a, "format"_a = XmlElement::TextFormat())
        .def ("getTagName", &XmlElement::getTagName)
        .def ("getNamespace", &XmlElement::getNamespace)
        .def ("getTagNameWithoutNamespace", &XmlElement::getTagNameWithoutNamespace)
        .def ("hasTagName", &XmlElement::hasTagName)
        .def ("hasTagNameIgnoringNamespace", &XmlElement::hasTagNameIgnoringNamespace)
        .def ("setTagName", &XmlElement::setTagName)
        .def ("getNumAttributes", &XmlElement::getNumAttributes)
        .def ("getAttributeName", &XmlElement::getAttributeName)
        .def ("getAttributeValue", &XmlElement::getAttributeValue)
        .def ("hasAttribute", &XmlElement::hasAttribute)
        .def ("getStringAttribute", py::overload_cast<StringRef> (&XmlElement::getStringAttribute, py::const_))
        .def ("getStringAttribute", py::overload_cast<StringRef, const String&> (&XmlElement::getStringAttribute, py::const_))
        .def ("compareAttribute", &XmlElement::compareAttribute, "attributeName"_a, "stringToCompareAgainst"_a, "ignoreCase"_a = false)
        .def ("getIntAttribute", &XmlElement::getIntAttribute, "attributeName"_a, "defaultReturnValue"_a = 0)
        .def ("getDoubleAttribute", &XmlElement::getDoubleAttribute, "attributeName"_a, "defaultReturnValue"_a = 0.0)
        .def ("getBoolAttribute", &XmlElement::getBoolAttribute, "attributeName"_a, "defaultReturnValue"_a = false)
        .def ("setAttribute", py::overload_cast<const Identifier&, const String&> (&XmlElement::setAttribute))
        .def ("setAttribute", [](XmlElement& self, const String& name, const String& value) { self.setAttribute (name, value); })
        .def ("setAttribute", py::overload_cast<const Identifier&, int> (&XmlElement::setAttribute))
        .def ("setAttribute", [](XmlElement& self, const String& name, int value) { self.setAttribute (name, value); })
        .def ("setAttribute", py::overload_cast<const Identifier&, double> (&XmlElement::setAttribute))
        .def ("setAttribute", [](XmlElement& self, const String& name, double value) { self.setAttribute (name, value); })
        .def ("removeAttribute", &XmlElement::removeAttribute)
        .def ("removeAttribute", [](XmlElement& self, const String& name) { self.removeAttribute (name); })
        .def ("removeAllAttributes", &XmlElement::removeAllAttributes)
        .def ("getFirstChildElement", &XmlElement::getFirstChildElement, py::return_value_policy::reference_internal)
        .def ("getNextElement", &XmlElement::getNextElement, py::return_value_policy::reference_internal)
        .def ("getNextElementWithTagName", &XmlElement::getNextElementWithTagName, py::return_value_policy::reference_internal)
        .def ("getNumChildElements", &XmlElement::getNumChildElements)
        .def ("getChildElement", &XmlElement::getChildElement, py::return_value_policy::reference_internal)
        .def ("getChildByName", &XmlElement::getChildByName, py::return_value_policy::reference_internal)
        .def ("getChildByAttribute", &XmlElement::getChildByAttribute, py::return_value_policy::reference_internal)
        .def ("addChildElement", [](XmlElement& self, py::object newChildElement)
            { self.addChildElement (newChildElement.release().cast<XmlElement*>()); })
        .def ("insertChildElement", [](XmlElement& self, py::object newChildElement, int index)
            { self.insertChildElement (newChildElement.release().cast<XmlElement*>(), index); })
        .def ("prependChildElement", [](XmlElement& self, py::object newChildElement)
            { self.prependChildElement (newChildElement.release().cast<XmlElement*>()); })
        .def ("createNewChildElement", &XmlElement::createNewChildElement, py::return_value_policy::reference_internal)
        .def ("replaceChildElement", [](XmlElement& self, XmlElement* currentChildElement, py::object newChildElement)
            { self.replaceChildElement (currentChildElement, newChildElement.release().cast<XmlElement*>()); })
        .def ("removeChildElement", &XmlElement::removeChildElement)
        .def ("deleteAllChildElements", &XmlElement::deleteAllChildElements)
        .def ("deleteAllChildElementsWithTagName", &XmlElement::deleteAllChildElementsWithTagName)
        .def ("containsChildElement", &XmlElement::containsChildElement)
        .def ("findParentElementOf", &XmlElement::findParentElementOf, py::return_value_policy::reference_internal)
        .def ("sortChildElements", &XmlElement::template sortChildElements<PyXmlElementComparator>, "comparator"_a, "retainOrderOfEquivalentItems"_a = false)
        .def ("sortChildElements", [](XmlElement& self, py::function fn, bool retainOrderOfEquivalentItems)
        {
            PyXmlElementCallableComparator comparator (std::move (fn));
            return self.sortChildElements (comparator, retainOrderOfEquivalentItems);
        }, "comparator"_a, "retainOrderOfEquivalentItems"_a = false)
        .def ("isTextElement", &XmlElement::isTextElement)
        .def ("getText", &XmlElement::getText)
        .def ("setText", &XmlElement::setText)
        .def ("getAllSubText", &XmlElement::getAllSubText)
        .def ("getChildElementAllSubText", &XmlElement::getChildElementAllSubText, "childTagName"_a, "defaultReturnValue"_a)
        .def ("addTextElement", &XmlElement::addTextElement)
        .def ("deleteAllTextElements", &XmlElement::deleteAllTextElements)
        .def_static ("createTextElement", &XmlElement::createTextElement)
        .def_static ("isValidXmlName", &XmlElement::isValidXmlName)
        .def ("getChildIterator", [](const XmlElement& self)
        {
            auto range = self.getChildIterator();
            return py::make_iterator (std::begin (range), std::end (range));
        })
        .def ("getChildIterator", [](const XmlElement& self, StringRef name)
        {
            auto range = self.getChildWithTagNameIterator (name);
            return py::make_iterator (std::begin (range), std::end (range));
        })
    ;

    // ============================================================================================ juce::XmlDocument

    py::class_<XmlDocument> classXmlDocument (m, "XmlDocument");

    classXmlDocument
        .def (py::init<const File&>(), "file"_a)
        .def (py::init<const String&>(), "textToParse"_a)
        .def ("getDocumentElement", &XmlDocument::getDocumentElement, "onlyReadOuterDocumentElement"_a = false)
        .def ("getDocumentElementIfTagMatches", &XmlDocument::getDocumentElementIfTagMatches, "requiredTag"_a)
        .def ("getLastParseError", &XmlDocument::getLastParseError)
        .def ("setInputSource", [](XmlDocument& self, py::object source) { self.setInputSource (source.release().cast<InputSource*>()); })
        .def ("setEmptyTextElementsIgnored", &XmlDocument::setEmptyTextElementsIgnored, "shouldBeIgnored"_a)
        .def_static ("parse", static_cast<std::unique_ptr<XmlElement> (*)(const File&)> (&XmlDocument::parse), "file"_a)
        .def_static ("parse", static_cast<std::unique_ptr<XmlElement> (*)(const String&)> (&XmlDocument::parse), "textToParse"_a)
    ;

    // ============================================================================================ juce::PropertySet

    py::class_<PropertySet> classPropertySet (m, "PropertySet");

    classPropertySet
        .def (py::init<>())
        .def (py::init<const PropertySet&>())
        .def ("getValue", &PropertySet::getValue, "keyName"_a, "defaultReturnValue"_a = String())
        .def ("getIntValue", &PropertySet::getIntValue, "keyName"_a, "defaultReturnValue"_a = 0)
        .def ("getDoubleValue", &PropertySet::getDoubleValue, "keyName"_a, "defaultReturnValue"_a = 0.0)
        .def ("getBoolValue", &PropertySet::getBoolValue, "keyName"_a, "defaultReturnValue"_a = false)
        .def ("getXmlValue", &PropertySet::getXmlValue, "keyName"_a)
        .def ("setValue", py::overload_cast<StringRef, const var&> (&PropertySet::setValue), "keyName"_a, "value"_a)
        .def ("setValue", py::overload_cast<StringRef, const XmlElement*> (&PropertySet::setValue), "keyName"_a, "xml"_a)
        .def ("addAllPropertiesFrom", &PropertySet::addAllPropertiesFrom, "source"_a)
        .def ("removeValue", &PropertySet::removeValue, "keyName"_a)
        .def ("containsKey", &PropertySet::containsKey, "keyName"_a)
        .def ("clear", &PropertySet::clear)
        .def ("getAllProperties", &PropertySet::getAllProperties, py::return_value_policy::reference_internal)
        .def ("getLock", &PropertySet::getLock, py::return_value_policy::reference_internal)
        .def ("createXml", &PropertySet::createXml, "nodeName"_a)
        .def ("restoreFromXml", &PropertySet::restoreFromXml, "xml"_a)
        .def ("setFallbackPropertySet", &PropertySet::setFallbackPropertySet, "fallbackProperties"_a)
        .def ("getFallbackPropertySet", &PropertySet::getFallbackPropertySet, py::return_value_policy::reference_internal)
    ;

    // ============================================================================================ juce::ZipFile

    py::class_<ZipFile> classZipFile (m, "ZipFile");
    py::class_<ZipFile::ZipEntry> classZipFileZipEntry (classZipFile, "ZipEntry");
    py::class_<ZipFile::Builder> classZipFileBuilder (classZipFile, "Builder");

    py::enum_<ZipFile::OverwriteFiles> (classZipFile, "OverwriteFiles")
        .value ("no", ZipFile::OverwriteFiles::no)
        .value ("yes", ZipFile::OverwriteFiles::yes);

    py::enum_<ZipFile::FollowSymlinks> (classZipFile, "FollowSymlinks")
        .value ("no", ZipFile::FollowSymlinks::no)
        .value ("yes", ZipFile::FollowSymlinks::yes);

    classZipFileZipEntry
        .def (py::init<>())
        .def_readwrite ("filename", &ZipFile::ZipEntry::filename)
        .def_readwrite ("uncompressedSize", &ZipFile::ZipEntry::uncompressedSize)
        .def_readwrite ("fileTime", &ZipFile::ZipEntry::fileTime)
        .def_readwrite ("isSymbolicLink", &ZipFile::ZipEntry::isSymbolicLink)
        .def_readwrite ("externalFileAttributes", &ZipFile::ZipEntry::externalFileAttributes)
    ;

    classZipFileBuilder
        .def (py::init<>())
        .def ("addFile", &ZipFile::Builder::addFile, "fileToAdd"_a, "compressionLevel"_a, "storedPathName"_a = String())
        .def ("addEntry", [](ZipFile::Builder& self, py::object stream, int compression, const String& path, Time time)
        {
            self.addEntry (stream.release().cast<InputStream*>(), compression, path, time);
        }, "streamToRead"_a, "compressionLevel"_a, "storedPathName"_a, "fileModificationTime"_a)
        .def ("writeToStream", [](const ZipFile::Builder& self, OutputStream& target) { return self.writeToStream (target, nullptr); }, "target"_a)
    ;

    classZipFile
        .def (py::init<const File&>(), "file"_a)
        .def (py::init<InputStream&>(), "inputStream"_a)
        .def (py::init ([](py::object inputSource)
        {
            return new ZipFile (inputSource.release().cast<InputSource*>());
        }), "inputSource"_a)
        .def ("getNumEntries", &ZipFile::getNumEntries)
        .def ("getIndexOfFileName", &ZipFile::getIndexOfFileName, "fileName"_a, "ignoreCase"_a = false)
        .def ("getEntry", py::overload_cast<int> (&ZipFile::getEntry, py::const_), "index"_a, py::return_value_policy::reference_internal)
        .def ("getEntry", py::overload_cast<const String&, bool> (&ZipFile::getEntry, py::const_), "fileName"_a, "ignoreCase"_a = false, py::return_value_policy::reference_internal)
        .def ("sortEntriesByFilename", &ZipFile::sortEntriesByFilename)
        .def ("createStreamForEntry", py::overload_cast<int> (&ZipFile::createStreamForEntry))
        .def ("createStreamForEntry", py::overload_cast<const ZipFile::ZipEntry&> (&ZipFile::createStreamForEntry))
        .def ("uncompressTo", &ZipFile::uncompressTo, "targetDirectory"_a, "shouldOverwriteFiles"_a = true)
        .def ("uncompressEntry", py::overload_cast<int, const File&, bool> (&ZipFile::uncompressEntry), "index"_a, "targetDirectory"_a, "shouldOverwriteFiles"_a = true)
        .def ("uncompressEntry", py::overload_cast<int, const File&, ZipFile::OverwriteFiles, ZipFile::FollowSymlinks> (&ZipFile::uncompressEntry), "index"_a, "targetDirectory"_a, "overwriteFiles"_a, "followSymlinks"_a)
    ;

    // ============================================================================================ juce::SystemStats

    py::class_<SystemStats> classSystemStats (m, "SystemStats");

    Helpers::makeArithmeticEnum<SystemStats::OperatingSystemType> (classSystemStats, "OperatingSystemType")
        .value ("UnknownOS", SystemStats::OperatingSystemType::UnknownOS)
        .value ("MacOSX", SystemStats::OperatingSystemType::MacOSX)
        .value ("Windows", SystemStats::OperatingSystemType::Windows)
        .value ("Linux", SystemStats::OperatingSystemType::Linux)
        .value ("Android", SystemStats::OperatingSystemType::Android)
        .value ("iOS", SystemStats::OperatingSystemType::iOS)
        .value ("WASM", SystemStats::OperatingSystemType::WASM)
        .value ("MacOSX_10_7", SystemStats::OperatingSystemType::MacOSX_10_7)
        .value ("MacOSX_10_8", SystemStats::OperatingSystemType::MacOSX_10_8)
        .value ("MacOSX_10_9", SystemStats::OperatingSystemType::MacOSX_10_9)
        .value ("MacOSX_10_10", SystemStats::OperatingSystemType::MacOSX_10_10)
        .value ("MacOSX_10_11", SystemStats::OperatingSystemType::MacOSX_10_11)
        .value ("MacOSX_10_12", SystemStats::OperatingSystemType::MacOSX_10_12)
        .value ("MacOSX_10_13", SystemStats::OperatingSystemType::MacOSX_10_13)
        .value ("MacOSX_10_14", SystemStats::OperatingSystemType::MacOSX_10_14)
        .value ("MacOSX_10_15", SystemStats::OperatingSystemType::MacOSX_10_15)
        .value ("MacOS_11", SystemStats::OperatingSystemType::MacOS_11)
        .value ("MacOS_12", SystemStats::OperatingSystemType::MacOS_12)
        .value ("MacOS_13", SystemStats::OperatingSystemType::MacOS_13)
        .value ("Win2000", SystemStats::OperatingSystemType::Win2000)
        .value ("WinXP", SystemStats::OperatingSystemType::WinXP)
        .value ("WinVista", SystemStats::OperatingSystemType::WinVista)
        .value ("Windows7", SystemStats::OperatingSystemType::Windows7)
        .value ("Windows8_0", SystemStats::OperatingSystemType::Windows8_0)
        .value ("Windows8_1", SystemStats::OperatingSystemType::Windows8_1)
        .value ("Windows10", SystemStats::OperatingSystemType::Windows10)
        .value ("Windows11", SystemStats::OperatingSystemType::Windows11)
        .export_values();

    py::enum_<SystemStats::MachineIdFlags> (classSystemStats, "MachineIdFlags")
        .value ("macAddresses", SystemStats::MachineIdFlags::macAddresses)
        .value ("fileSystemId", SystemStats::MachineIdFlags::fileSystemId)
        .value ("legacyUniqueId", SystemStats::MachineIdFlags::legacyUniqueId)
        .value ("uniqueId", SystemStats::MachineIdFlags::uniqueId)
        .export_values();

    classSystemStats
        .def_static ("getJUCEVersion", &SystemStats::getJUCEVersion)
        .def_static ("getOperatingSystemType", &SystemStats::getOperatingSystemType)
        .def_static ("getOperatingSystemName", &SystemStats::getOperatingSystemName)
        .def_static ("isOperatingSystem64Bit", &SystemStats::isOperatingSystem64Bit)
        .def_static ("getEnvironmentVariable", &SystemStats::getEnvironmentVariable)
        .def_static ("getLogonName", &SystemStats::getLogonName)
        .def_static ("getFullUserName", &SystemStats::getFullUserName)
        .def_static ("getComputerName", &SystemStats::getComputerName)
        .def_static ("getUserLanguage", &SystemStats::getUserLanguage)
        .def_static ("getUserRegion", &SystemStats::getUserRegion)
        .def_static ("getDisplayLanguage", &SystemStats::getDisplayLanguage)
        .def_static ("getDeviceDescription", &SystemStats::getDeviceDescription)
        .def_static ("getDeviceManufacturer", &SystemStats::getDeviceManufacturer)
        .def_static ("getUniqueDeviceID", &SystemStats::getUniqueDeviceID)
        .def_static ("getMachineIdentifiers", &SystemStats::getMachineIdentifiers)
        .def_static ("getNumCpus", &SystemStats::getNumCpus)
        .def_static ("getNumPhysicalCpus", &SystemStats::getNumPhysicalCpus)
        .def_static ("getCpuSpeedInMegahertz", &SystemStats::getCpuSpeedInMegahertz)
        .def_static ("getCpuVendor", &SystemStats::getCpuVendor)
        .def_static ("getCpuModel", &SystemStats::getCpuModel)
        .def_static ("hasMMX", &SystemStats::hasMMX)
        .def_static ("has3DNow", &SystemStats::has3DNow)
        .def_static ("hasFMA3", &SystemStats::hasFMA3)
        .def_static ("hasFMA4", &SystemStats::hasFMA4)
        .def_static ("hasSSE", &SystemStats::hasSSE)
        .def_static ("hasSSE2", &SystemStats::hasSSE2)
        .def_static ("hasSSE3", &SystemStats::hasSSE3)
        .def_static ("hasSSSE3", &SystemStats::hasSSSE3)
        .def_static ("hasSSE41", &SystemStats::hasSSE41)
        .def_static ("hasSSE42", &SystemStats::hasSSE42)
        .def_static ("hasAVX", &SystemStats::hasAVX)
        .def_static ("hasAVX2", &SystemStats::hasAVX2)
        .def_static ("hasAVX512F", &SystemStats::hasAVX512F)
        .def_static ("hasAVX512BW", &SystemStats::hasAVX512BW)
        .def_static ("hasAVX512CD", &SystemStats::hasAVX512CD)
        .def_static ("hasAVX512DQ", &SystemStats::hasAVX512DQ)
        .def_static ("hasAVX512ER", &SystemStats::hasAVX512ER)
        .def_static ("hasAVX512IFMA", &SystemStats::hasAVX512IFMA)
        .def_static ("hasAVX512PF", &SystemStats::hasAVX512PF)
        .def_static ("hasAVX512VBMI", &SystemStats::hasAVX512VBMI)
        .def_static ("hasAVX512VL", &SystemStats::hasAVX512VL)
        .def_static ("hasAVX512VPOPCNTDQ", &SystemStats::hasAVX512VPOPCNTDQ)
        .def_static ("hasNeon", &SystemStats::hasNeon)
        .def_static ("getMemorySizeInMegabytes", &SystemStats::getMemorySizeInMegabytes)
        .def_static ("getPageSize", &SystemStats::getPageSize)
        .def_static ("getStackBacktrace", &SystemStats::getStackBacktrace)
    //.def_static ("setApplicationCrashHandler", &SystemStats::setApplicationCrashHandler)
    ;

    // ============================================================================================ juce::Array<>

    registerArray<Array, bool, int, float, String, File> (m);

    // ============================================================================================ juce::SparseSet<>

    registerSparseSet<SparseSet, int> (m);

    // ============================================================================================ testing

    m.def ("__raise_cpp_exception__", [](const juce::String& what) { throw std::runtime_error (what.toStdString()); });
    m.def ("__terminate__", [] { std::terminate(); });
}

} // namespace popsicle::Bindings
