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

#include <juce_core/juce_core.h>

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../utilities/ClassDemangling.h"
#include "../utilities/PythonInterop.h"
#include "../utilities/PythonTypes.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <utility>

namespace PYBIND11_NAMESPACE {
namespace detail {

// =================================================================================================

template <>
struct type_caster<juce::StringRef>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringRef, const_name (PYBIND11_STRING_NAME));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringRef& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

// =================================================================================================

template <>
struct type_caster<juce::String>
{
public:
    PYBIND11_TYPE_CASTER (juce::String, const_name (PYBIND11_STRING_NAME));

    bool load (handle src, bool convert);

    static handle cast (const juce::String& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

// =================================================================================================

template <>
struct type_caster<juce::Identifier> : public type_caster_base<juce::Identifier>
{
    using base_type = type_caster_base<juce::Identifier>;

public:
    PYBIND11_TYPE_CASTER (juce::Identifier, const_name ("popsicle.Identifier"));

    bool load (handle src, bool convert);

    static handle cast (const juce::Identifier& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

// =================================================================================================

template <>
struct type_caster<juce::var>
{
public:
    PYBIND11_TYPE_CASTER (juce::var, const_name ("popsicle.var"));

    bool load (handle src, bool convert);

    static handle cast (const juce::var& src, return_value_policy policy, handle parent);
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceCoreBindings (pybind11::module_& m);

// ============================================================================================

template <class T, class = void>
struct isEqualityComparable : std::false_type {};

template <class T>
struct isEqualityComparable<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};

// ============================================================================================

template <template <class, class, int> class Class, class... Types>
void registerArray (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    auto type = py::hasattr (m, "Array") ? m.attr ("Array").cast<py::dict>() : py::dict{};

    ([&]
    {
        using ValueType = underlying_type_t<Types>;
        using T = Class<ValueType, DummyCriticalSection, 0>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Array", typeid (ValueType).name());

        py::class_<T> class_ (m, className.toRawUTF8());

        class_
            .def (py::init<>())
            .def (py::init<const ValueType&>())
            .def (py::init<const T&>())
            .def ("clear", &T::clear)
            .def ("clearQuick", &T::clearQuick)
            .def ("fill", &T::fill)
            .def ("size", &T::size)
            .def ("isEmpty", &T::isEmpty)
            .def ("__getitem__", &T::operator[])
            .def ("__setitem__", &T::set)
            .def ("getUnchecked", &T::getUnchecked)
            .def ("getReference", py::overload_cast<int> (&T::getReference), py::return_value_policy::reference)
            .def ("getFirst", &T::getFirst)
            .def ("getLast", &T::getLast)
        //.def ("getRawDataPointer", &T::getRawDataPointer)
            .def("__iter__", [](T& self)
            {
                return py::make_iterator (self.begin(), self.end());
            }, py::keep_alive<0, 1>())
            .def ("add", [](T& self, const ValueType& arg)
            {
                self.add (arg);
            })
            .def ("add", [](T& self, py::args args)
            {
                for (auto item : args)
                {
                    py::detail::make_caster<ValueType> conv;

                    if (! conv.load (item, true))
                        py::pybind11_fail("Invalid property type of a \"Array\", needs to be \"?\"");

                    self.add (py::detail::cast_op<ValueType&&> (std::move (conv)));
                }
            })
            .def ("insert", &T::insert)
            .def ("insertMultiple", &T::insertMultiple)
        //.def ("insertArray", &T::insertArray)
            .def ("set", &T::set)
            .def ("setUnchecked", &T::setUnchecked)
        //.def ("addArray", &T::addArray)
            .def ("addArray", [](T& self, py::list list)
            {
                for (auto item : list)
                {
                    py::detail::make_caster<ValueType> conv;

                    if (! conv.load (item, true))
                        py::pybind11_fail("Invalid property type of a \"Array\", needs to be \"?\"");

                    self.add (py::detail::cast_op<ValueType&&> (std::move (conv)));
                }
            })
            .def ("swapWith", &T::template swapWith<T>)
            .def ("addArray", py::overload_cast<const T&> (&T::template addArray<T>))
            .def ("resize", &T::resize)
            .def ("remove", py::overload_cast<int> (&T::remove))
            .def ("removeAndReturn", &T::removeAndReturn)
            .def ("remove", py::overload_cast<const ValueType*> (&T::remove))
        //.def ("removeIf", &T::removeIf)
            .def ("removeRange", &T::removeRange)
            .def ("removeLast", &T::removeLast)
            .def ("swap", &T::swap)
            .def ("move", &T::move)
            .def ("minimiseStorageOverheads", &T::minimiseStorageOverheads)
            .def ("ensureStorageAllocated", &T::ensureStorageAllocated)
        //.def ("getLock", &T::getLock)
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

        if constexpr (! std::is_same_v<ValueType, Types>)
            class_.def (py::init ([](Types value) { return T (static_cast<ValueType> (value)); }));

        if constexpr (isEqualityComparable<ValueType>::value)
        {
            class_
                .def (py::self == py::self)
                .def (py::self != py::self)
                .def ("sort", [](T& self) { self.sort(); })
                .def ("indexOf", &T::indexOf)
                .def ("contains", &T::contains)
                .def ("addIfNotAlreadyThere", &T::addIfNotAlreadyThere)
                .def ("addUsingDefaultSort", &T::addUsingDefaultSort)
            //.def ("addSorted", &T::addSorted)
            //.def ("indexOfSorted", &T::indexOfSorted)
                .def ("removeValuesIn", &T::template removeValuesIn<T>)
                .def ("removeValuesNotIn", &T::template removeValuesNotIn<T>)
                .def ("removeFirstMatchingValue", &T::removeFirstMatchingValue)
                .def ("removeAllInstancesOf", &T::removeAllInstancesOf)
            //.def ("sort", &T::sort)
            ;
        }

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.attr ("Array") = type;
}

// =================================================================================================

struct PyThreadID
{
    explicit PyThreadID (juce::Thread::ThreadID value) noexcept
        : value (value)
    {
    }

    operator juce::Thread::ThreadID() const noexcept
    {
        return value;
    }

    bool operator==(const PyThreadID& other) const noexcept
    {
        return value == other.value;
    }

    bool operator!=(const PyThreadID& other) const noexcept
    {
        return value != other.value;
    }

private:
    juce::Thread::ThreadID value;
};

// =================================================================================================

template <class Base = juce::InputStream>
struct PyInputStream : Base
{
private:
#if JUCE_WINDOWS && ! JUCE_MINGW
    using ssize_t = juce::pointer_sized_int;
#endif

public:
    using Base::Base;

    juce::int64 getTotalLength() override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, juce::InputStream, getTotalLength);
    }

    bool isExhausted() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::InputStream, isExhausted);
    }

    int read (void* destBuffer, int maxBytesToRead) override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::InputStream, read, destBuffer, maxBytesToRead);
    }

    char readByte() override
    {
        PYBIND11_OVERRIDE (char, juce::InputStream, readByte);
    }

    short readShort() override
    {
        PYBIND11_OVERRIDE (short, juce::InputStream, readShort);
    }

    short readShortBigEndian() override
    {
        PYBIND11_OVERRIDE (short, juce::InputStream, readShortBigEndian);
    }

    int readInt() override
    {
        PYBIND11_OVERRIDE (int, juce::InputStream, readInt);
    }

    int readIntBigEndian() override
    {
        PYBIND11_OVERRIDE (int, juce::InputStream, readIntBigEndian);
    }

    juce::int64 readInt64() override
    {
        PYBIND11_OVERRIDE (juce::int64, juce::InputStream, readInt64);
    }

    juce::int64 readInt64BigEndian() override
    {
        PYBIND11_OVERRIDE (juce::int64, juce::InputStream, readInt64BigEndian);
    }

    float readFloat() override
    {
        PYBIND11_OVERRIDE (float, juce::InputStream, readFloat);
    }

    float readFloatBigEndian() override
    {
        PYBIND11_OVERRIDE (float, juce::InputStream, readFloatBigEndian);
    }

    double readDouble() override
    {
        PYBIND11_OVERRIDE (double, juce::InputStream, readDouble);
    }

    double readDoubleBigEndian() override
    {
        PYBIND11_OVERRIDE (double, juce::InputStream, readDoubleBigEndian);
    }

    int readCompressedInt() override
    {
        PYBIND11_OVERRIDE (int, juce::InputStream, readCompressedInt);
    }

    juce::String readNextLine() override
    {
        PYBIND11_OVERRIDE (juce::String, juce::InputStream, readNextLine);
    }

    juce::String readString() override
    {
        PYBIND11_OVERRIDE (juce::String, juce::InputStream, readString);
    }

    juce::String readEntireStreamAsString() override
    {
        PYBIND11_OVERRIDE (juce::String, juce::InputStream, readEntireStreamAsString);
    }

    size_t readIntoMemoryBlock (juce::MemoryBlock& destBlock, ssize_t maxNumBytesToRead) override
    {
        PYBIND11_OVERRIDE (size_t, juce::InputStream, readIntoMemoryBlock, destBlock, maxNumBytesToRead);
    }

    juce::int64 getPosition() override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, juce::InputStream, getPosition);
    }

    bool setPosition (juce::int64 newPosition) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::InputStream, setPosition, newPosition);
    }

    void skipNextBytes (juce::int64 newPosition) override
    {
        PYBIND11_OVERRIDE (void, juce::InputStream, skipNextBytes, newPosition);
    }
};

// =================================================================================================

struct PyInputSource : juce::InputSource
{
    using juce::InputSource::InputSource;

    juce::InputStream* createInputStream() override
    {
        PYBIND11_OVERRIDE_PURE (juce::InputStream*, juce::InputSource, createInputStream);
    }

    juce::InputStream* createInputStreamFor (const juce::String& relatedItemPath) override
    {
        PYBIND11_OVERRIDE_PURE (juce::InputStream*, juce::InputSource, createInputStreamFor, relatedItemPath);
    }

    juce::int64 hashCode() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, juce::InputSource, hashCode);
    }
};

// =================================================================================================

template <class Base = juce::OutputStream>
struct PyOutputStream : Base
{
    using Base::Base;

    void flush() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::OutputStream, flush);
    }

    bool setPosition (juce::int64 newPosition) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::OutputStream, setPosition, newPosition);
    }

    juce::int64 getPosition() override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, juce::OutputStream, getPosition);
    }

    bool write (const void* dataToWrite, size_t numberOfBytes) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::OutputStream, write, dataToWrite, numberOfBytes);
    }

    bool writeByte (char value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeByte, value);
    }

    bool writeBool (bool value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeBool, value);
    }

    bool writeShort (short value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeShort, value);
    }

    bool writeShortBigEndian (short value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeShortBigEndian, value);
    }


    bool writeInt (int value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeInt, value);
    }

    bool writeIntBigEndian (int value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeIntBigEndian, value);
    }

    bool writeInt64 (juce::int64 value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeInt64, value);
    }

    bool writeInt64BigEndian (juce::int64 value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeInt64BigEndian, value);
    }

    bool writeFloat (float value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeFloat, value);
    }

    bool writeFloatBigEndian (float value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeFloatBigEndian, value);
    }

    bool writeDouble (double value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeDouble, value);
    }

    bool writeDoubleBigEndian (double value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeDoubleBigEndian, value);
    }

    bool writeRepeatedByte (juce::uint8 byte, size_t numTimesToRepeat) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeRepeatedByte, byte, numTimesToRepeat);
    }

    bool writeCompressedInt (int value) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeCompressedInt, value);
    }

    bool writeString (const juce::String& text) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeString, text);
    }

    bool writeText (const juce::String& text, bool asUTF16, bool writeUTF16ByteOrderMark, const char* lineEndings) override
    {
        PYBIND11_OVERRIDE (bool, juce::OutputStream, writeText, text, asUTF16, writeUTF16ByteOrderMark, lineEndings);
    }

    juce::int64 writeFromInputStream (juce::InputStream& source, juce::int64 maxNumBytesToWrite) override
    {
        PYBIND11_OVERRIDE (juce::int64, juce::OutputStream, writeFromInputStream, source, maxNumBytesToWrite);
    }
};

// =================================================================================================

struct PyFileFilter : juce::FileFilter
{
    using juce::FileFilter::FileFilter;

    bool isFileSuitable (const juce::File& file) const override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::FileFilter, isFileSuitable, file);
    }

    bool isDirectorySuitable (const juce::File& file) const override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::FileFilter, isDirectorySuitable, file);
    }
};

// =================================================================================================

struct PyURLDownloadTaskListener : public juce::URL::DownloadTaskListener
{
    void finished (juce::URL::DownloadTask* task, bool success) override
    {
        PYBIND11_OVERRIDE_PURE(void, juce::URL::DownloadTaskListener, finished, task, success);
    }

    void progress (juce::URL::DownloadTask* task, juce::int64 bytesDownloaded, juce::int64 totalLength) override
    {
        PYBIND11_OVERRIDE_PURE(void, juce::URL::DownloadTaskListener, progress, task, bytesDownloaded, totalLength);
    }
};

// =================================================================================================

struct PyXmlElementComparator
{
    PyXmlElementComparator() = default;

    int compareElements (const juce::XmlElement* first, const juce::XmlElement* second)
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<PyXmlElementComparator*> (this), "compareElements"); override_)
        {
            auto result = override_ (first, second);

            return result.cast<int>();
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"XmlElement::Comparator::compareElements\"");
    }
};

struct PyXmlElementCallableComparator
{
    explicit PyXmlElementCallableComparator(pybind11::function f)
        : fn (std::move (f))
    {
    }

    int compareElements (const juce::XmlElement* first, const juce::XmlElement* second)
    {
        pybind11::gil_scoped_acquire gil;

        if (fn)
        {
            auto result = fn (first, second);

            return result.cast<int>();
        }

        pybind11::pybind11_fail("Tried to call function \"XmlElement::Comparator::compareElements\" without a callable");
    }

private:
    pybind11::function fn;
};

// =================================================================================================

struct PyHighResolutionTimer : public juce::HighResolutionTimer
{
    void hiResTimerCallback() override
    {
        PYBIND11_OVERRIDE_PURE(void, juce::HighResolutionTimer, hiResTimerCallback);
    }
};

// =================================================================================================

template <class T>
struct PyGenericScopedLock
{
    PyGenericScopedLock (const T& mutex)
        : mutex (mutex)
    {
    }

    PyGenericScopedLock (const PyGenericScopedLock&) = delete;
    PyGenericScopedLock (PyGenericScopedLock&&) = default;

    ~PyGenericScopedLock()
    {
        exit();
    }

    void enter()
    {
        mutex.enter();
    }

    void exit()
    {
        mutex.exit();
    }

private:
    const T& mutex;
};

template <class T>
struct PyGenericScopedUnlock
{
    PyGenericScopedUnlock (const T& mutex)
        : mutex (mutex)
    {
    }

    PyGenericScopedUnlock (const PyGenericScopedUnlock&) = delete;
    PyGenericScopedUnlock (PyGenericScopedUnlock&&) = default;

    ~PyGenericScopedUnlock()
    {
        exit();
    }

    void enter()
    {
        mutex.exit();
    }

    void exit()
    {
        mutex.enter();
    }

private:
    const T& mutex;
};

template <class T>
struct PyGenericScopedTryLock
{
    PyGenericScopedTryLock (const T& mutex, bool acquireLockOnInitialisation = true)
        : mutex (mutex)
        , lockWasSuccessful (acquireLockOnInitialisation && mutex.tryEnter())
        , acquireLockOnInitialisation (acquireLockOnInitialisation)
    {
    }

    PyGenericScopedTryLock (const PyGenericScopedTryLock&) = delete;
    PyGenericScopedTryLock (PyGenericScopedTryLock&&) = default;

    ~PyGenericScopedTryLock()
    {
        exit();
    }

    bool isLocked() const noexcept
    {
        return lockWasSuccessful;
    }

    bool retryLock() const
    {
        lockWasSuccessful = mutex.tryEnter();
        return lockWasSuccessful;
    }

    void enter()
    {
        if (! acquireLockOnInitialisation)
            retryLock();
    }

    void exit()
    {
        if (lockWasSuccessful)
            mutex.exit();
    }

private:
    const T& mutex;
    mutable bool lockWasSuccessful;
    bool acquireLockOnInitialisation;
};

// =================================================================================================

template <class Base = juce::Thread>
struct PyThread : Base
{
    using Base::Base;

    void run() override
    {
#if JUCE_PYTHON_THREAD_CATCH_EXCEPTION
        try
        {
#endif
            PYBIND11_OVERRIDE_PURE (void, Base, run);

#if JUCE_PYTHON_THREAD_CATCH_EXCEPTION
        }
        catch (const pybind11::error_already_set& e)
        {
            pybind11::gil_scoped_acquire acquire;
            pybind11::print ("Your run() method mustn't throw any exceptions!");
            pybind11::module_::import ("traceback").attr ("print_exception") (e.type(), e.value(), e.trace());
        }
        catch (const std::exception& e)
        {
            pybind11::gil_scoped_acquire acquire;
            auto exception = juce::String ("Your run() method mustn't throw any exceptions: ") + e.what();
            pybind11::print (exception);
        }
        catch (...)
        {
            pybind11::gil_scoped_acquire acquire;
            auto exception = juce::String ("Your run() method mustn't throw any exceptions: Unhandled python exeption");
            pybind11::print (exception);
        }
#endif
    }
};

// =================================================================================================

struct PyThreadListener : juce::Thread::Listener
{
    using juce::Thread::Listener::Listener;

    void exitSignalSent() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Thread::Listener, exitSignalSent);
    }
};

// =================================================================================================

struct PyThreadPoolJob : juce::ThreadPoolJob
{
    using juce::ThreadPoolJob::ThreadPoolJob;

    JobStatus runJob() override
    {
        PYBIND11_OVERRIDE_PURE (JobStatus, juce::ThreadPoolJob, runJob);
    }
};

// =================================================================================================

struct PyThreadPoolJobSelector : juce::ThreadPool::JobSelector
{
    using juce::ThreadPool::JobSelector::JobSelector;

    bool isJobSuitable (juce::ThreadPoolJob* job) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::ThreadPool::JobSelector, isJobSuitable, job);
    }
};

// =================================================================================================

struct PyTimeSliceClient : juce::TimeSliceClient
{
    using juce::TimeSliceClient::TimeSliceClient;

    int useTimeSlice() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::TimeSliceClient, useTimeSlice);
    }
};

} // namespace popsicle::Bindings
