/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptJuceCoreBindings.h"
#include "../utilities/ClassDemangling.h"
#include "../utilities/PythonInterop.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#include "../utilities/PyBind11Includes.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS

namespace PYBIND11_NAMESPACE {
namespace detail {

//=================================================================================================

bool type_caster<juce::StringRef>::load (handle src, bool)
{
    if (! src)
        return false;

    if (! PyUnicode_Check (src.ptr()))
        return load_raw(src);

    Py_ssize_t size = -1;
    const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (src.ptr(), &size));
    if (buffer == nullptr)
        return false;

    value = buffer;
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

//=================================================================================================

bool type_caster<juce::String>::load (handle src, bool)
{
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

//=================================================================================================

bool type_caster<juce::Identifier>::load (handle src, bool)
{
    PyObject* source = src.ptr();

    auto baseType = Py_TYPE (source)->tp_base;
    if (baseType == &PyUnicode_Type)
    {
        Py_ssize_t size = -1;
        const auto* buffer = reinterpret_cast<const char*> (PyUnicode_AsUTF8AndSize (source, &size));
        if (! buffer)
            return false;

        value = juce::Identifier (juce::String::fromUTF8 (buffer, static_cast<int> (size)));
        return true;
    }

    return false;
}

handle type_caster<juce::Identifier>::cast (const juce::Identifier& src, return_value_policy policy, handle parent)
{
    juce::ignoreUnused (policy, parent);

    return make_caster<juce::String>::cast (src.toString(), policy, parent);
}

//=================================================================================================

bool type_caster<juce::var>::load (handle src, bool convert)
{
    PyObject* source = src.ptr();
    auto baseType = Py_TYPE (source)->tp_base;

    if (PyNone_Check(source))
        value = juce::var::undefined();

    else if (baseType == &PyBool_Type)
        value = PyObject_IsTrue (source) ? true : false;

    else if (baseType == &PyLong_Type)
        value = static_cast<int> (PyLong_AsLong (source));

    else if (baseType == &PyFloat_Type)
        value = PyFloat_AsDouble (source);

    else if (baseType == &PyUnicode_Type)
        value = juce::String::fromUTF8 (PyUnicode_AsUTF8 (source));

    else if (baseType == &PyTuple_Type)
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

    else if (baseType == &PyList_Type)
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

    else
        value = juce::var::undefined();

    // TODO - raise

    return !PyErr_Occurred();
}

handle type_caster<juce::var>::cast (const juce::var& src, return_value_policy policy, handle parent)
{
    if (src.isBool())
        return PyBool_FromLong (static_cast<bool> (src));

    else if (src.isInt())
        return PyLong_FromLong (static_cast<int> (src));

    else if (src.isInt64())
        return PyLong_FromLongLong (static_cast<int64_t> (src));

    else if (src.isDouble())
        return PyFloat_FromDouble (static_cast<double> (src));

    else if (src.isString())
        return make_caster<juce::String>::cast (src, policy, parent);

    else if (src.isVoid() || src.isUndefined())
        return Py_None;

    else if (src.isObject())
    {
        auto dynamicObject = src.getDynamicObject();
        if (dynamicObject == nullptr)
            return Py_None;

        object result;

        for (const auto& props : dynamicObject->getProperties())
            result [props.name.toString().toRawUTF8()] = props.value;

        return result.release();
    }

    else if (src.isArray())
    {
        list list;

        if (auto array = src.getArray())
        {
            for (const auto& value : *array)
                list.append (value);
        }

        return list.release();
    }

    else if (src.isBinaryData())
    {
        if (auto data = src.getBinaryData())
            return bytes (static_cast<const char*> (data->getData()), static_cast<Py_ssize_t> (data->getSize())).release();
    }

    else if (src.isMethod())
    {
        return cpp_function ([src]
        {
            juce::var::NativeFunctionArgs args (juce::var(), nullptr, 0);
            return src.getNativeFunction() (args);
        }).release();
    }

    return Py_None;
}

//=================================================================================================

bool type_caster<juce::StringArray>::load (handle src, bool convert)
{
    if (! isinstance<list> (src))
        return false;

    value.clear();

    auto d = reinterpret_borrow<list> (src);
    for (auto it : d)
    {
        make_caster<juce::String> conv;

        if (! conv.load (it.ptr(), convert))
            return false;

        value.add (cast_op<juce::String&&> (std::move (conv)));
    }

    return true;
}

handle type_caster<juce::StringArray>::cast (const juce::StringArray& src, return_value_policy policy, handle parent)
{
    list l;

    for (const auto& value : src)
    {
        auto item = reinterpret_steal<object> (make_caster<juce::String>::cast (value, policy, parent));

        if (! item)
            return handle();

        l.append (std::move (item));
    }

    return l.release();
}

//=================================================================================================

bool type_caster<juce::NamedValueSet>::load (handle src, bool convert)
{
    if (! isinstance<dict> (src))
        return false;

    value.clear();

    auto d = reinterpret_borrow<dict> (src);
    for (auto it : d)
    {
        make_caster<juce::Identifier> kconv;
        make_caster<juce::var> vconv;

        if (! kconv.load (it.first.ptr(), convert) || ! vconv.load (it.second.ptr(), convert))
            return false;

        value.set (cast_op<juce::Identifier&&> (std::move (kconv)), cast_op<juce::var&&> (std::move (vconv)));
    }

    return true;
}

handle type_caster<juce::NamedValueSet>::cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent)
{
    dict d;

    for (const auto& kv : src)
    {
        auto key = reinterpret_steal<object> (make_caster<juce::Identifier>::cast (kv.name, policy, parent));
        auto value = reinterpret_steal<object> (make_caster<juce::var>::cast (kv.value, policy, parent));

        if (! key || ! value)
            return handle();

        d [std::move (key)] = std::move (value);
    }

    return d.release();
}

}} // namespace PYBIND11_NAMESPACE::detail

namespace jucepy::Bindings {

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRange (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;
    py::object scope;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Range[" << jucepy::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (scope, className.toRawUTF8())
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
        //.def_static ("findMinAndMax", &T::template findMinAndMax<int>)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Range", type);
}

void registerJuceCoreBindings ([[maybe_unused]] pybind11::module_& m)
{
    using namespace juce;
    namespace py = pybind11;

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
        .def_static ("littleEndianInt", &ByteOrder::littleEndianInt)
        .def_static ("littleEndianInt64", &ByteOrder::littleEndianInt64)
        .def_static ("littleEndianShort", &ByteOrder::littleEndianShort)
        .def_static ("littleEndian24Bit", &ByteOrder::littleEndian24Bit)
        .def_static ("littleEndian24BitToChars", &ByteOrder::littleEndian24BitToChars)
        .def_static ("bigEndianInt", &ByteOrder::bigEndianInt)
        .def_static ("bigEndianInt64", &ByteOrder::bigEndianInt64)
        .def_static ("bigEndianShort", &ByteOrder::bigEndianShort)
        .def_static ("bigEndian24Bit", &ByteOrder::bigEndian24Bit)
        .def_static ("bigEndian24BitToChars", &ByteOrder::bigEndian24BitToChars)
        .def_static ("makeInt", static_cast<uint16 (*)(uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("makeInt", static_cast<uint32 (*)(uint8, uint8, uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("makeInt", static_cast<uint64 (*)(uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint8)> (&ByteOrder::makeInt))
        .def_static ("isBigEndian", &ByteOrder::isBigEndian)
    ;

    // ============================================================================================ juce::BigInteger

    py::class_<BigInteger> classBigInteger (m, "BigInteger");
    classBigInteger
        .def (py::init<>())
        .def (py::init<int32>())
        .def (py::init<int64>())
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
        .def ("__str__", &BigInteger::toString)
    ;

    // ============================================================================================ juce::String

    py::class_<String> classString (m, "String");
    classString
        .def (py::init<>())
        .def_static ("fromUTF8", [](const char* const buffer) { return String::fromUTF8(buffer); })
        .def_static ("fromUTF8", [](const char* const buffer, int bufferSizeBytes) { return String::fromUTF8(buffer, bufferSizeBytes); })
    ;

    // ============================================================================================ juce::Base64

    py::class_<Base64> classBase64 (m, "Base64");
    classBase64
        .def_static ("convertToBase64", &Base64::convertToBase64)
        .def_static ("convertFromBase64", &Base64::convertFromBase64)
    //.def_static ("toBase64", static_cast<String (*)(const void *, int)> (&Base64::toBase64))
        .def_static ("toBase64", static_cast<String (*)(const String &)> (&Base64::toBase64))
    ;

    // ============================================================================================ juce::Result

    py::class_<Result> classResult (m, "Result");
    classResult
        .def_static ("ok", &Result::ok)
        .def_static ("fail", &Result::fail)
        .def ("wasOk", &Result::wasOk)
        .def ("failed", &Result::failed)
        .def ("getErrorMessage", &Result::getErrorMessage)
    // .def ("operator=", py::overload_cast<const Result &> (&Result::operator=))
    // .def ("operator=", py::overload_cast<Result &&> (&Result::operator=))
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (!py::self)
    ;

    // ============================================================================================ juce::Uuid

    py::class_<Uuid> classUuid (m, "Uuid");
    classUuid
        .def (py::init<>())
        .def ("isNull", &Uuid::isNull)
        .def_static ("null", &Uuid::null)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self > py::self)
        .def (py::self <= py::self)
        .def (py::self >= py::self)
    //.def (py::self = String())
        .def ("toString", &Uuid::toString)
        .def ("toDashedString", &Uuid::toDashedString)
        .def ("getTimeLow", &Uuid::getTimeLow)
        .def ("getTimeMid", &Uuid::getTimeMid)
        .def ("getTimeHighAndVersion", &Uuid::getTimeHighAndVersion)
        .def ("getClockSeqAndReserved", &Uuid::getClockSeqAndReserved)
        .def ("getClockSeqLow", &Uuid::getClockSeqLow)
        .def ("getNode", &Uuid::getNode)
        .def ("hash", &Uuid::hash)
        .def ("getRawData", &Uuid::getRawData)
        .def ("__str__", &Uuid::toString)
    ;

    // ============================================================================================ juce::RelativeTime

    py::class_<RelativeTime> classRelativeTime (m, "RelativeTime");
    classRelativeTime
        .def (py::init<>())
    //.def_static ("milliseconds", static_cast<RelativeTime (*)(int)> (&RelativeTime::milliseconds))
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
        .def ("getDescription", &RelativeTime::getDescription)
        .def ("getApproximateDescription", &RelativeTime::getApproximateDescription)
        .def (py::self += py::self)
        .def (py::self -= py::self)
        .def (py::self += double())
        .def (py::self -= double())
    ;

    // ============================================================================================ juce::Time

    py::class_<Time> classTime (m, "Time");
    classTime
    //.def (py::self = Time())
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
        .def ("toString", &Time::toString)
        .def ("formatted", &Time::formatted)
        .def ("toISO8601", &Time::toISO8601)
        .def_static ("fromISO8601", &Time::fromISO8601)
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
    ;

    // ============================================================================================ juce::Range<>

    registerRange<Range, int, float>(m);

    // ============================================================================================ juce::MemoryBlock

    py::class_<MemoryBlock> classMemoryBlock (m, "MemoryBlock");
    classMemoryBlock
        .def (py::init<>())
        .def (py::init<const size_t, bool>())
        .def (py::init<const MemoryBlock&>())
    //.def (py::init<const void*, size_t>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("matches", &MemoryBlock::matches)
        .def ("getData", py::overload_cast<> (&MemoryBlock::getData))
        .def ("getData", py::overload_cast<> (&MemoryBlock::getData, py::const_))
        .def ("isEmpty", &MemoryBlock::isEmpty)
        .def ("getSize", &MemoryBlock::getSize)
        .def ("setSize", &MemoryBlock::setSize)
        .def ("ensureSize", &MemoryBlock::ensureSize)
        .def ("reset", &MemoryBlock::reset)
        .def ("fillWith", &MemoryBlock::fillWith)
        .def ("append", &MemoryBlock::append)
        .def ("replaceAll", &MemoryBlock::replaceAll)
        .def ("insert", &MemoryBlock::insert)
        .def ("removeSection", &MemoryBlock::removeSection)
        .def ("copyFrom", &MemoryBlock::copyFrom)
        .def ("copyTo", &MemoryBlock::copyTo)
        .def ("swapWith", &MemoryBlock::swapWith)
        .def ("toString", &MemoryBlock::toString)
        .def ("loadFromHexString", &MemoryBlock::loadFromHexString)
        .def ("setBitRange", &MemoryBlock::setBitRange)
        .def ("getBitRange", &MemoryBlock::getBitRange)
        .def ("toBase64Encoding", &MemoryBlock::toBase64Encoding)
        .def ("fromBase64Encoding", &MemoryBlock::fromBase64Encoding)
        .def ("__str__", &MemoryBlock::toString)
    ;

    // ============================================================================================ juce::InputStream

    py::class_<InputStream> classInputStream (m, "InputStream");
    classInputStream
        .def ("getTotalLength", &InputStream::getTotalLength)
        .def ("getNumBytesRemaining", &InputStream::getNumBytesRemaining)
        .def ("isExhausted", &InputStream::isExhausted)
    //.def ("read", py::overload_cast<void *, int>(&InputStream::read))
    //.def ("read", py::overload_cast<void *, int>(&InputStream::read))
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
        .def ("readIntoMemoryBlock", &InputStream::readIntoMemoryBlock)
        .def ("getPosition", &InputStream::getPosition)
        .def ("setPosition", &InputStream::setPosition)
        .def ("skipNextBytes", &InputStream::skipNextBytes)
    ;

    py::class_<BufferedInputStream, InputStream> classBufferedInputStream (m, "BufferedInputStream");
    classBufferedInputStream
        .def ("peekByte", &BufferedInputStream::peekByte)
        .def ("getTotalLength", &BufferedInputStream::getTotalLength)
        .def ("getPosition", &BufferedInputStream::getPosition)
        .def ("setPosition", &BufferedInputStream::setPosition)
        .def ("read", &BufferedInputStream::read)
        .def ("readString", &BufferedInputStream::readString)
        .def ("isExhausted", &BufferedInputStream::isExhausted)
    ;

    py::class_<MemoryInputStream, InputStream> classMemoryInputStream (m, "MemoryInputStream");
    classMemoryInputStream
        .def ("getData", &MemoryInputStream::getData)
        .def ("getDataSize", &MemoryInputStream::getDataSize)
        .def ("getPosition", &MemoryInputStream::getPosition)
        .def ("setPosition", &MemoryInputStream::setPosition)
        .def ("getTotalLength", &MemoryInputStream::getTotalLength)
        .def ("isExhausted", &MemoryInputStream::isExhausted)
        .def ("read", &MemoryInputStream::read)
        .def ("skipNextBytes", &MemoryInputStream::skipNextBytes)
    ;

    py::class_<SubregionStream, InputStream> classSubregionStream (m, "SubregionStream");
    classSubregionStream
        .def ("getTotalLength", &SubregionStream::getTotalLength)
        .def ("getPosition", &SubregionStream::getPosition)
        .def ("setPosition", &SubregionStream::setPosition)
        .def ("read", &SubregionStream::read)
        .def ("isExhausted", &SubregionStream::isExhausted)
    ;

    py::class_<FileInputStream, InputStream> classFileInputStream (m, "FileInputStream");
    classFileInputStream
        .def ("getFile", &FileInputStream::getFile)
        .def ("getStatus", &FileInputStream::getStatus)
        .def ("failedToOpen", &FileInputStream::failedToOpen)
        .def ("openedOk", &FileInputStream::openedOk)
        .def ("getTotalLength", &FileInputStream::getTotalLength)
        .def ("read", &FileInputStream::read)
        .def ("isExhausted", &FileInputStream::isExhausted)
        .def ("getPosition", &FileInputStream::getPosition)
        .def ("setPosition", &FileInputStream::setPosition)
    ;

    // ============================================================================================ juce::OutputStream

    py::class_<OutputStream> classOutputStream (m, "OutputStream");
    classOutputStream
        .def ("flush", &OutputStream::flush)
        .def ("setPosition", &OutputStream::setPosition)
        .def ("getPosition", &OutputStream::getPosition)
        .def ("write", &OutputStream::write)
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
        .def ("getNewLineString", &OutputStream::getNewLineString)
    ;

    py::class_<MemoryOutputStream, OutputStream> classMemoryOutputStream (m, "MemoryOutputStream");
    classMemoryOutputStream
        .def ("getData", &MemoryOutputStream::getData)
        .def ("getDataSize", &MemoryOutputStream::getDataSize)
        .def ("reset", &MemoryOutputStream::reset)
        .def ("preallocate", &MemoryOutputStream::preallocate)
        .def ("appendUTF8Char", &MemoryOutputStream::appendUTF8Char)
        .def ("toUTF8", &MemoryOutputStream::toUTF8)
        .def ("toString", &MemoryOutputStream::toString)
        .def ("getMemoryBlock", &MemoryOutputStream::getMemoryBlock)
        .def ("flush", &MemoryOutputStream::flush)
        .def ("write", &MemoryOutputStream::write)
        .def ("getPosition", &MemoryOutputStream::getPosition)
        .def ("setPosition", &MemoryOutputStream::setPosition)
        .def ("writeFromInputStream", &MemoryOutputStream::writeFromInputStream)
        .def ("writeRepeatedByte", &MemoryOutputStream::writeRepeatedByte)
        .def ("__str__", &MemoryOutputStream::toString)
    ;

    py::class_<FileOutputStream, OutputStream> classFileOutputStream (m, "FileOutputStream");
    classFileOutputStream
        .def ("getFile", &FileOutputStream::getFile)
        .def ("getStatus", &FileOutputStream::getStatus)
        .def ("failedToOpen", &FileOutputStream::failedToOpen)
        .def ("openedOk", &FileOutputStream::openedOk)
        .def ("truncate", &FileOutputStream::truncate)
        .def ("flush", &FileOutputStream::flush)
        .def ("getPosition", &FileOutputStream::getPosition)
        .def ("setPosition", &FileOutputStream::setPosition)
        .def ("write", &FileOutputStream::write)
        .def ("writeRepeatedByte", &FileOutputStream::writeRepeatedByte)
    ;

    // ============================================================================================ juce::CriticalSection

    py::class_<CriticalSection> classCriticalSection (m, "CriticalSection");
    classCriticalSection
        .def (py::init<>())
        .def ("enter", &CriticalSection::enter)
        .def ("tryEnter", &CriticalSection::tryEnter)
        .def ("exit", &CriticalSection::exit)
    ;

    // ============================================================================================ juce::SpinLock

    py::class_<SpinLock> classSpinLock (m, "SpinLock");
    classSpinLock
        .def (py::init<>())
        .def ("enter", &SpinLock::enter)
        .def ("tryEnter", &SpinLock::tryEnter)
        .def ("exit", &SpinLock::exit)
    ;

    // ============================================================================================ juce::WaitableEvent

    py::class_<WaitableEvent> classWaitableEvent (m, "WaitableEvent");
    classWaitableEvent
        .def (py::init<>())
        .def (py::init<bool>())
        .def ("wait", &WaitableEvent::wait)
        .def ("signal", &WaitableEvent::signal)
        .def ("reset", &WaitableEvent::reset)
    ;

    // ============================================================================================ juce::ReadWriteLock

    py::class_<ReadWriteLock> classReadWriteLock (m, "ReadWriteLock");
    classReadWriteLock
        .def (py::init<>())
        .def ("enterRead", &ReadWriteLock::enterRead)
        .def ("tryEnterRead", &ReadWriteLock::tryEnterRead)
        .def ("exitRead", &ReadWriteLock::exitRead)
        .def ("enterWrite", &ReadWriteLock::enterWrite)
        .def ("tryEnterWrite", &ReadWriteLock::tryEnterWrite)
        .def ("exitWrite", &ReadWriteLock::exitWrite)
    ;

    // ============================================================================================ juce::InterProcessLock

    py::class_<InterProcessLock> classInterProcessLock (m, "InterProcessLock");
    classInterProcessLock
        .def (py::init<const String&>())
        .def ("enter", &InterProcessLock::enter)
        .def ("exit", &InterProcessLock::exit)
    ;

    // ============================================================================================ juce::HighResolutionTimer

    struct PyHighResolutionTimer : public HighResolutionTimer
    {
        void hiResTimerCallback() override
        {
            PYBIND11_OVERRIDE_PURE(void, HighResolutionTimer, hiResTimerCallback);
        }
    };

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
    classChildProcess
        .def (py::init<>())
        .def ("start", py::overload_cast<const String &, int> (&ChildProcess::start))
        .def ("start", py::overload_cast<const StringArray &, int> (&ChildProcess::start))
        .def ("isRunning", &ChildProcess::isRunning)
        .def ("readProcessOutput", &ChildProcess::readProcessOutput)
        .def ("readAllProcessOutput", &ChildProcess::readAllProcessOutput)
        .def ("waitForProcessToFinish", &ChildProcess::waitForProcessToFinish)
        .def ("getExitCode", &ChildProcess::getExitCode)
        .def ("kill", &ChildProcess::kill)
    ;

    // ============================================================================================ juce::Random

    py::class_<Random> classRandom (m, "Random");
    classRandom
        .def (py::init<>())
        .def (py::init<int64>())
        .def ("nextInt", py::overload_cast<> (&Random::nextInt))
        .def ("nextInt", py::overload_cast<int> (&Random::nextInt))
        .def ("nextInt", py::overload_cast<Range<int>> (&Random::nextInt))
        .def ("nextInt64", &Random::nextInt64)
        .def ("nextFloat", &Random::nextFloat)
        .def ("nextDouble", &Random::nextDouble)
        .def ("nextBool", &Random::nextBool)
    //.def ("nextLargeNumber", &Random::nextLargeNumber)
    //.def ("fillBitsRandomly", py::overload_cast<BigInteger&, int, int> (&Random::fillBitsRandomly))
        .def ("setSeed", &Random::setSeed)
        .def ("getSeed", &Random::getSeed)
        .def ("combineSeed", &Random::combineSeed)
        .def ("setSeedRandomly", &Random::setSeedRandomly)
        .def_static ("getSystemRandom", &Random::getSystemRandom, py::return_value_policy::reference)
    ;

    // ============================================================================================ juce::PropertySet

    py::class_<PropertySet> classPropertySet (m, "PropertySet");
    classPropertySet
        .def (py::init<>())
        .def (py::init<const PropertySet&>())
        .def ("getValue", &PropertySet::getValue)
        .def ("getIntValue", &PropertySet::getIntValue)
        .def ("getDoubleValue", &PropertySet::getDoubleValue)
        .def ("getBoolValue", &PropertySet::getBoolValue)
        .def ("getXmlValue", &PropertySet::getXmlValue)
        .def ("setValue", py::overload_cast<StringRef, const var&> (&PropertySet::setValue))
        //.def ("setValue", py::overload_cast<StringRef, const XmlElement *> (&PropertySet::setValue))
        .def ("addAllPropertiesFrom", &PropertySet::addAllPropertiesFrom)
        .def ("removeValue", &PropertySet::removeValue)
        .def ("containsKey", &PropertySet::containsKey)
        .def ("clear", &PropertySet::clear)
        .def ("getAllProperties", &PropertySet::getAllProperties)
        .def ("getLock", &PropertySet::getLock)
        //.def ("createXml", &PropertySet::createXml)
        //.def ("restoreFromXml", &PropertySet::restoreFromXml)
        .def ("setFallbackPropertySet", &PropertySet::setFallbackPropertySet)
        .def ("getFallbackPropertySet", &PropertySet::getFallbackPropertySet)
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
        .def (py::init<const String&>())
        .def (py::init<const File&>())
        .def ("exists", &File::exists)
        .def ("existsAsFile", &File::existsAsFile)
        .def ("isDirectory", &File::isDirectory)
        .def ("isRoot", &File::isRoot)
        .def ("getSize", &File::getSize)
        .def_static ("descriptionOfSizeInBytes", &File::descriptionOfSizeInBytes)
        .def ("getFullPathName", &File::getFullPathName)
        .def ("getFileName", &File::getFileName)
        .def ("getRelativePathFrom", &File::getRelativePathFrom)
        .def ("getFileExtension", &File::getFileExtension)
        .def ("hasFileExtension", &File::hasFileExtension)
        .def ("withFileExtension", &File::withFileExtension)
        .def ("getFileNameWithoutExtension", &File::getFileNameWithoutExtension)
        .def ("hashCode", &File::hashCode)
        .def ("hashCode64", &File::hashCode64)
        .def ("getChildFile", &File::getChildFile)
        .def ("getSiblingFile", &File::getSiblingFile)
        .def ("getParentDirectory", &File::getParentDirectory)
        .def ("isAChildOf", &File::isAChildOf)
        .def ("getNonexistentChildFile", &File::getNonexistentChildFile)
        .def ("getNonexistentSibling", &File::getNonexistentSibling)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self < py::self)
        .def (py::self > py::self)
        .def ("hasWriteAccess", &File::hasWriteAccess)
        .def ("hasReadAccess", &File::hasReadAccess)
        .def ("setReadOnly", &File::setReadOnly)
        .def ("setExecutePermission", &File::setExecutePermission)
        .def ("isHidden", &File::isHidden)
        .def ("getFileIdentifier", &File::getFileIdentifier)
        .def ("getLastModificationTime", &File::getLastModificationTime)
        .def ("getLastAccessTime", &File::getLastAccessTime)
        .def ("getCreationTime", &File::getCreationTime)
        .def ("setLastModificationTime", &File::setLastModificationTime)
        .def ("setLastAccessTime", &File::setLastAccessTime)
        .def ("setCreationTime", &File::setCreationTime)
        .def ("getVersion", &File::getVersion)
        .def ("create", &File::create)
        .def ("createDirectory", &File::createDirectory)
        .def ("deleteFile", &File::deleteFile)
        .def ("deleteRecursively", &File::deleteRecursively)
        .def ("moveToTrash", &File::moveToTrash)
        .def ("moveFileTo", &File::moveFileTo)
        .def ("copyFileTo", &File::copyFileTo)
        .def ("replaceFileIn", &File::replaceFileIn)
        .def ("copyDirectoryTo", &File::copyDirectoryTo)
    //.def ("findChildFiles", py::overload_cast<int, bool, const String &, File::FollowSymlinks> (&File::findChildFiles, py::const_))
    //.def ("findChildFiles", py::overload_cast<int &, int, bool, const String &, File::FollowSymlinks> (&File::findChildFiles, py::const_))
        .def ("getNumberOfChildFiles", &File::getNumberOfChildFiles)
        .def ("containsSubDirectories", &File::containsSubDirectories)
        .def ("createInputStream", &File::createInputStream)
        .def ("createOutputStream", &File::createOutputStream)
        .def ("loadFileAsData", &File::loadFileAsData)
        .def ("loadFileAsString", &File::loadFileAsString)
        .def ("readLines", &File::readLines)
        .def ("appendData", &File::appendData)
        .def ("replaceWithData", &File::replaceWithData)
        .def ("appendText", &File::appendText)
        .def ("replaceWithText", &File::replaceWithText)
        .def ("hasIdenticalContentTo", &File::hasIdenticalContentTo)
        .def_static ("findFileSystemRoots", &File::findFileSystemRoots)
        .def ("getVolumeLabel", &File::getVolumeLabel)
        .def ("getVolumeSerialNumber", &File::getVolumeSerialNumber)
        .def ("getBytesFreeOnVolume", &File::getBytesFreeOnVolume)
        .def ("getVolumeTotalSize", &File::getVolumeTotalSize)
        .def ("isOnCDRomDrive", &File::isOnCDRomDrive)
        .def ("isOnHardDisk", &File::isOnHardDisk)
        .def ("isOnRemovableDrive", &File::isOnRemovableDrive)
        .def ("startAsProcess", &File::startAsProcess)
        .def ("revealToUser", &File::revealToUser)
        .def_static ("getSpecialLocation", &File::getSpecialLocation)
        .def_static ("createTempFile", &File::createTempFile)
        .def_static ("getCurrentWorkingDirectory", &File::getCurrentWorkingDirectory)
        .def ("setAsCurrentWorkingDirectory", &File::setAsCurrentWorkingDirectory)
        .def_static ("getSeparatorChar", &File::getSeparatorChar)
        .def_static ("getSeparatorString", &File::getSeparatorString)
        .def_static ("createLegalFileName", &File::createLegalFileName)
        .def_static ("createLegalPathName", &File::createLegalPathName)
        .def_static ("areFileNamesCaseSensitive", &File::areFileNamesCaseSensitive)
        .def_static ("isAbsolutePath", &File::isAbsolutePath)
        .def_static ("createFileWithoutCheckingPath", &File::createFileWithoutCheckingPath)
        .def_static ("addTrailingSeparator", &File::addTrailingSeparator)
        .def ("createSymbolicLink", py::overload_cast<const File &, bool> (&File::createSymbolicLink, py::const_))
    //.def_static ("createSymbolicLink", static_cast<bool (*)(const File &, const String &, bool)> (&File::createSymbolicLink))
        .def ("isSymbolicLink", &File::isSymbolicLink)
        .def ("getLinkedTarget", &File::getLinkedTarget)
        .def ("getNativeLinkedTarget", &File::getNativeLinkedTarget)
    // .def ("getMacOSType", &File::getMacOSType)
        .def ("isBundle", &File::isBundle)
        .def ("addToDock", &File::addToDock)
        .def_static ("getContainerForSecurityApplicationGroupIdentifier", &File::getContainerForSecurityApplicationGroupIdentifier)
    ;

    // ============================================================================================ juce::URL

    py::class_<URL> classURL (m, "URL");
    classURL
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<File>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("toString", &URL::toString)
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
        .def ("__str__", &URL::toString)
    ;

    py::class_<URL::InputStreamOptions> classURLInputStreamOptions (classURL, "InputStreamOptions");
    classURLInputStreamOptions
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
    classURLDownloadTask
        .def ("getTotalLength", &URL::DownloadTask::getTotalLength)
        .def ("getLengthDownloaded", &URL::DownloadTask::getLengthDownloaded)
        .def ("isFinished", &URL::DownloadTask::isFinished)
        .def ("statusCode", &URL::DownloadTask::statusCode)
        .def ("hadError", &URL::DownloadTask::hadError)
        .def ("getTargetLocation", &URL::DownloadTask::getTargetLocation)
    ;

    struct PyURLDownloadTaskListener : public URL::DownloadTaskListener
    {
        void finished (URL::DownloadTask* task, bool success) override
        {
            PYBIND11_OVERRIDE_PURE(void, URL::DownloadTaskListener, finished, task, success);
        }

        void progress (URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength) override
        {
            PYBIND11_OVERRIDE_PURE(void, URL::DownloadTaskListener, progress, task, bytesDownloaded, totalLength);
        }
    };

    py::class_<URL::DownloadTaskListener, PyURLDownloadTaskListener> classURLDownloadTaskListener (classURL, "DownloadTaskListener");
    classURLDownloadTaskListener
        .def ("finished", &URL::DownloadTaskListener::finished)
        .def ("progress", &URL::DownloadTaskListener::progress)
    ;

    py::class_<URL::DownloadTaskOptions> classURLDownloadTaskOptions (classURL, "DownloadTaskOptions");
    classURLDownloadTaskOptions
        .def ("withExtraHeaders", &URL::DownloadTaskOptions::withExtraHeaders)
        .def ("withSharedContainer", &URL::DownloadTaskOptions::withSharedContainer)
        .def ("withListener", &URL::DownloadTaskOptions::withListener)
        .def ("withUsePost", &URL::DownloadTaskOptions::withUsePost)
        .def_property ("extraHeaders",
                       [](const URL::DownloadTaskOptions& self) { return self.extraHeaders; },
                       [](URL::DownloadTaskOptions& self, const String& v) { self.extraHeaders = v; })
        .def_property ("sharedContainer",
                       [](const URL::DownloadTaskOptions& self) { return self.sharedContainer; },
                       [](URL::DownloadTaskOptions& self, const String& v) { self.sharedContainer = v; })
        .def_property ("listener",
                       [](const URL::DownloadTaskOptions& self) { return self.listener; },
                       [](URL::DownloadTaskOptions& self, URL::DownloadTaskListener* v) { self.listener = v; })
        .def_property ("usePost",
                       [](const URL::DownloadTaskOptions& self) { return self.usePost; },
                       [](URL::DownloadTaskOptions& self, bool v) { self.usePost = v; })
    ;

    // ============================================================================================ juce::SystemStats

    py::class_<SystemStats> classSystemStats (m, "SystemStats");

    py::enum_<SystemStats::OperatingSystemType> (classSystemStats, "OperatingSystemType")
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
}

} // namespace jucepy::Bindings
