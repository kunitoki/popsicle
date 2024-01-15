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

#if __has_include(<juce_graphics/juce_graphics.h>)

#include "ScriptJuceGraphicsBindings.h"
#include "../utilities/ClassDemangling.h"
#include "../utilities/PythonInterop.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../pybind11/operators.h"

#include <functional>
#include <string_view>
#include <tuple>
#include <variant>

namespace popsicle::Bindings {

// ============================================================================================

template <template <class> class Class, class... Types>
void registerPoint (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Point[" << popsicle::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("isOrigin", &T::isOrigin)
            .def ("isFinite", &T::isFinite)
            .def ("getX", &T::getX)
            .def ("getY", &T::getY)
            .def ("setX", &T::setX)
            .def ("setY", &T::setY)
            .def ("withX", &T::withX)
            .def ("withY", &T::withY)
            .def ("setXY", &T::setXY)
            .def ("addXY", &T::addXY)
            .def ("translated", &T::translated)
            .def (py::self + py::self)
            .def (py::self += py::self)
            .def (py::self - py::self)
            .def (py::self -= py::self)
            .def (py::self * py::self)
            .def (py::self *= py::self)
            .def (py::self * float())
            .def (py::self *= float())
            .def (py::self / py::self)
            .def (py::self /= py::self)
            .def (py::self / float())
            .def (py::self /= float())
            .def (-py::self)
            .def ("getDistanceFromOrigin", &T::getDistanceFromOrigin)
            .def ("getDistanceFrom", &T::getDistanceFrom)
            .def ("getDistanceSquaredFromOrigin", &T::getDistanceSquaredFromOrigin)
            .def ("getDistanceSquaredFrom", &T::getDistanceSquaredFrom)
            .def ("getAngleToPoint", &T::getAngleToPoint)
            .def ("rotatedAboutOrigin", &T::rotatedAboutOrigin)
            .def ("getPointOnCircumference", py::overload_cast<float, float>(&T::getPointOnCircumference, py::const_))
            .def ("getPointOnCircumference", py::overload_cast<float, float, float>(&T::getPointOnCircumference, py::const_))
            .def ("getDotProduct", &T::getDotProduct)
            .def ("applyTransform", &T::applyTransform)
            .def ("transformedBy", &T::transformedBy)
            .def ("toInt", &T::toInt)
            .def ("toFloat", &T::toFloat)
            .def ("toDouble", &T::toDouble)
            .def ("roundToInt", &T::roundToInt)
            .def ("toString", &T::toString)
            .def_property("x", &T::getX, &T::setX)
            .def_property("y", &T::getY, &T::setY)
            .def ("__str__", &T::toString)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Point", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerLine (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Line[" << popsicle::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def ("getStartX", &T::getStartX)
            .def ("getStartY", &T::getStartY)
            .def ("getEndX", &T::getEndX)
            .def ("getEndY", &T::getEndY)
            .def ("getStart", &T::getStart)
            .def ("getEnd", &T::getEnd)
        //.def ("setStart", &T::setStart)
        //.def ("setStart", &T::setStart)
        //.def ("setEnd", &T::setEnd)
        //.def ("setEnd", &T::setEnd)
            .def ("reversed", &T::reversed)
            .def ("applyTransform", &T::applyTransform)
            .def ("getLength", &T::getLength)
            .def ("getLengthSquared", &T::getLengthSquared)
            .def ("isVertical", &T::isVertical)
            .def ("isHorizontal", &T::isHorizontal)
            .def ("getAngle", &T::getAngle)
        //.def_static ("fromStartAndAngle", &T::fromStartAndAngle)
            .def ("toFloat", &T::toFloat)
            .def ("toDouble", &T::toDouble)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("getIntersection", &T::getIntersection)
        //.def ("intersects", &T::intersects)
        //.def ("intersects", &T::intersects)
        //.def ("getPointAlongLine", &T::getPointAlongLine)
        //.def ("getPointAlongLine", &T::getPointAlongLine)
            .def ("getPointAlongLineProportionally", &T::getPointAlongLineProportionally)
            .def ("getDistanceFromPoint", &T::getDistanceFromPoint)
            .def ("findNearestProportionalPositionTo", &T::findNearestProportionalPositionTo)
            .def ("findNearestPointTo", &T::findNearestPointTo)
            .def ("isPointAbove", &T::isPointAbove)
            .def ("withLengthenedStart", &T::withLengthenedStart)
            .def ("withShortenedStart", &T::withShortenedStart)
            .def ("withLengthenedEnd", &T::withLengthenedEnd)
            .def ("withShortenedEnd", &T::withShortenedEnd)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Line", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRectangle (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Rectangle[" << popsicle::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def (py::init<Point<ValueType>, Point<ValueType>>())
            .def ("isEmpty", &T::isEmpty)
            .def ("isFinite", &T::isFinite)
            .def ("getX", &T::getX)
            .def ("getY", &T::getY)
            .def ("getWidth", &T::getWidth)
            .def ("getHeight", &T::getHeight)
            .def ("getRight", &T::getRight)
            .def ("getBottom", &T::getBottom)
            .def ("getCentreX", &T::getCentreX)
            .def ("getCentreX", &T::getCentreX)
            .def ("getAspectRatio", &T::getAspectRatio)
            .def ("getPosition", &T::getPosition)
            .def ("setPosition", py::overload_cast<Point<ValueType>> (&T::setPosition))
            .def ("setPosition", py::overload_cast<ValueType, ValueType> (&T::setPosition))
            .def ("getTopLeft", &T::getTopLeft)
            .def ("getTopRight", &T::getTopRight)
            .def ("getBottomLeft", &T::getBottomLeft)
            .def ("getBottomRight", &T::getBottomRight)
            .def ("getHorizontalRange", &T::getHorizontalRange)
            .def ("getVerticalRange", &T::getVerticalRange)
            .def ("setSize", &T::setSize)
            .def ("setBounds", &T::setBounds)
            .def ("setX", &T::setX)
            .def ("setY", &T::setY)
            .def ("setWidth", &T::setWidth)
            .def ("setHeight", &T::setHeight)
            .def ("setCentre", py::overload_cast<Point<ValueType>> (&T::setCentre))
            .def ("setCentre", py::overload_cast<ValueType, ValueType> (&T::setCentre))
            .def ("setHorizontalRange", &T::setHorizontalRange)
            .def ("setVerticalRange", &T::setVerticalRange)
            .def ("withX", &T::withX)
            .def ("withY", &T::withY)
            .def ("withRightX", &T::withRightX)
            .def ("withBottomY", &T::withBottomY)
            .def ("withPosition", py::overload_cast<Point<ValueType>> (&T::withPosition, py::const_))
            .def ("withPosition", py::overload_cast<ValueType, ValueType> (&T::withPosition, py::const_))
            .def ("withZeroOrigin", &T::withZeroOrigin)
            .def ("withCentre", &T::withCentre)
            .def ("withWidth", &T::withWidth)
            .def ("withHeight", &T::withHeight)
            .def ("withSize", &T::withSize)
            .def ("withSizeKeepingCentre", &T::withSizeKeepingCentre)
            .def ("setLeft", &T::setLeft)
            .def ("withLeft", &T::withLeft)
            .def ("setTop", &T::setTop)
            .def ("withTop", &T::withTop)
            .def ("setRight", &T::setRight)
            .def ("withRight", &T::withRight)
            .def ("setBottom", &T::setBottom)
            .def ("withBottom", &T::withBottom)
            .def ("withTrimmedLeft", &T::withTrimmedLeft)
            .def ("withTrimmedRight", &T::withTrimmedRight)
            .def ("withTrimmedTop", &T::withTrimmedTop)
            .def ("withTrimmedBottom", &T::withTrimmedBottom)
            .def ("translate", &T::translate)
            .def ("translated", &T::translated)
            // TODO - operators
            .def ("expand", &T::expand)
            .def ("expanded", py::overload_cast<ValueType> (&T::expanded, py::const_))
            .def ("expanded", py::overload_cast<ValueType, ValueType> (&T::expanded, py::const_))
            .def ("reduce", &T::reduce)
            .def ("reduced", py::overload_cast<ValueType> (&T::reduced, py::const_))
            .def ("reduced", py::overload_cast<ValueType, ValueType> (&T::reduced, py::const_))
            .def ("removeFromTop", &T::removeFromTop)
            .def ("removeFromLeft", &T::removeFromLeft)
            .def ("removeFromRight", &T::removeFromRight)
            .def ("removeFromBottom", &T::removeFromBottom)
            .def ("getConstrainedPoint", &T::getConstrainedPoint)
            .def ("getRelativePoint", &T::template getRelativePoint<float>)
            .def ("getRelativePoint", &T::template getRelativePoint<double>)
            .def ("proportionOfWidth", &T::template proportionOfWidth<float>)
            .def ("proportionOfWidth", &T::template proportionOfWidth<double>)
            .def ("proportionOfHeight", &T::template proportionOfHeight<float>)
            .def ("proportionOfHeight", &T::template proportionOfHeight<double>)
            .def ("getProportion", &T::template getProportion<float>)
            .def ("getProportion", &T::template getProportion<double>)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Rectangle", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRectangleList (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "RectangleList[" << popsicle::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<Rectangle<ValueType>>())
            .def ("isEmpty", &T::isEmpty)
            .def ("getNumRectangles", &T::getNumRectangles)
            .def ("getRectangle", &T::getRectangle)
            .def ("clear", &T::clear)
            .def ("add", py::overload_cast<Rectangle<ValueType>> (&T::add))
            .def ("add", py::overload_cast<ValueType, ValueType, ValueType, ValueType> (&T::add))
            .def ("addWithoutMerging", &T::addWithoutMerging)
            .def ("add", py::overload_cast<const T&> (&T::add))
            .def ("subtract", py::overload_cast<const Rectangle<ValueType>> (&T::subtract))
            .def ("subtract", py::overload_cast<const T&> (&T::subtract))
            .def ("clipTo", static_cast<bool (T::*)(Rectangle<ValueType>)> (&T::clipTo))
            .def ("clipTo", py::overload_cast<const Class<int>&> (&T::template clipTo<int>))
            .def ("clipTo", py::overload_cast<const Class<float>&> (&T::template clipTo<float>))
        //.def ("getIntersectionWith", &T::getIntersectionWith)
        //.def ("swapWith", &T::swapWith)
            .def ("containsPoint", py::overload_cast<Point<ValueType>> (&T::containsPoint, py::const_))
            .def ("containsPoint", py::overload_cast<ValueType, ValueType> (&T::containsPoint, py::const_))
            .def ("containsRectangle", &T::containsRectangle)
            .def ("intersectsRectangle", &T::intersectsRectangle)
            .def ("intersects", &T::intersects)
            .def ("getBounds", &T::getBounds)
            .def ("consolidate", &T::consolidate)
            .def ("offsetAll", py::overload_cast<Point<ValueType>> (&T::offsetAll))
            .def ("offsetAll", py::overload_cast<ValueType, ValueType> (&T::offsetAll))
            .def ("scaleAll", py::overload_cast<int> (&T::template scaleAll<int>))
            .def ("scaleAll", py::overload_cast<float> (&T::template scaleAll<float>))
            .def ("transformAll", &T::transformAll)
            .def ("toPath", &T::toPath)
        //.def ("begin", &T::begin)
        //.def ("end", &T::end)
            .def ("ensureStorageAllocated", &T::ensureStorageAllocated)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("RectangleList", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerBorderSize (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "BorderSize[" << popsicle::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def ("getTop", &T::getTop)
            .def ("getLeft", &T::getLeft)
            .def ("getBottom", &T::getBottom)
            .def ("getRight", &T::getRight)
            .def ("getTopAndBottom", &T::getTopAndBottom)
            .def ("getLeftAndRight", &T::getLeftAndRight)
            .def ("isEmpty", &T::isEmpty)
            .def ("setTop", &T::setTop)
            .def ("setLeft", &T::setLeft)
            .def ("setBottom", &T::setBottom)
            .def ("setRight", &T::setRight)
            .def ("subtractedFrom", py::overload_cast<const Rectangle<ValueType>&> (&T::subtractedFrom, py::const_))
            .def ("subtractFrom", &T::subtractFrom)
            .def ("addedTo", py::overload_cast<const Rectangle<ValueType>&> (&T::addedTo, py::const_))
            .def ("addTo", &T::addTo)
            .def ("subtractedFrom", py::overload_cast<const T&> (&T::subtractedFrom, py::const_))
            .def ("addedTo", py::overload_cast<const T&> (&T::addedTo, py::const_))
            .def ("multipliedBy", &T::template multipliedBy<int>)
            .def ("multipliedBy", &T::template multipliedBy<float>)
            .def (py::self == py::self)
            .def (py::self != py::self)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("BorderSize", type);
}

void registerJuceGraphicsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::AffineTransform

    py::class_<Justification> classJustification (m, "Justification");

    py::enum_<Justification::Flags> (classJustification, "Flags")
        .value ("left", Justification::Flags::left)
        .value ("right", Justification::Flags::right)
        .value ("horizontallyCentred", Justification::Flags::horizontallyCentred)
        .value ("top", Justification::Flags::top)
        .value ("bottom", Justification::Flags::bottom)
        .value ("verticallyCentred", Justification::Flags::verticallyCentred)
        .value ("horizontallyJustified", Justification::Flags::horizontallyJustified)
        .value ("centred", Justification::Flags::centred)
        .value ("centredLeft", Justification::Flags::centredLeft)
        .value ("centredRight", Justification::Flags::centredRight)
        .value ("centredTop", Justification::Flags::centredTop)
        .value ("centredBottom", Justification::Flags::centredBottom)
        .value ("topLeft", Justification::Flags::topLeft)
        .value ("topRight", Justification::Flags::topRight)
        .value ("bottomLeft", Justification::Flags::bottomLeft)
        .value ("bottomRight", Justification::Flags::bottomRight)
        .export_values();

    classJustification
        .def (py::init<Justification::Flags>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("getFlags", &Justification::getFlags)
        .def ("testFlags", &Justification::testFlags)
        .def ("getOnlyVerticalFlags", &Justification::getOnlyVerticalFlags)
        .def ("getOnlyHorizontalFlags", &Justification::getOnlyHorizontalFlags)
        .def ("applyToRectangle", &Justification::template applyToRectangle<int>)
        .def ("applyToRectangle", &Justification::template applyToRectangle<float>)
        .def ("appliedToRectangle", &Justification::template appliedToRectangle<int>)
        .def ("appliedToRectangle", &Justification::template appliedToRectangle<float>)
    ;

    // ============================================================================================ juce::AffineTransform

    py::class_<AffineTransform> (m, "AffineTransform")
        .def (py::init<>())
        .def (py::init<float, float, float, float, float, float>())
        .def (py::self == py::self)
        .def (py::self != py::self)
    //.def ("transformPoint", &AffineTransform::transformPoint)
    //.def ("transformPoints", &AffineTransform::transformPoints)
        .def ("translated", static_cast<AffineTransform (AffineTransform::*)(float, float) const> (&AffineTransform::translated))
        .def_static ("translation", static_cast<AffineTransform (*)(float, float)> (&AffineTransform::translation))
        .def ("withAbsoluteTranslation", &AffineTransform::withAbsoluteTranslation)
        .def ("rotated", py::overload_cast<float> (&AffineTransform::rotated, py::const_))
        .def ("rotated", py::overload_cast<float, float, float> (&AffineTransform::rotated, py::const_))
        .def_static ("rotation", py::overload_cast<float> (&AffineTransform::rotation))
        .def_static ("rotation", py::overload_cast<float, float, float> (&AffineTransform::rotation))
        .def ("scaled", py::overload_cast<float, float> (&AffineTransform::scaled, py::const_))
        .def ("scaled", py::overload_cast<float> (&AffineTransform::scaled, py::const_))
        .def ("scaled", py::overload_cast<float, float, float, float> (&AffineTransform::scaled, py::const_))
        .def_static ("scale", py::overload_cast<float, float> (&AffineTransform::scale))
        .def_static ("scale", py::overload_cast<float> (&AffineTransform::scale))
        .def_static ("scale", py::overload_cast<float, float, float, float> (&AffineTransform::scale))
        .def ("sheared", &AffineTransform::sheared)
        .def_static ("shear", &AffineTransform::shear)
        .def_static ("verticalFlip", &AffineTransform::verticalFlip)
        .def ("inverted", &AffineTransform::inverted)
    //.def ("fromTargetPoints", &AffineTransform::fromTargetPoints)
        .def ("followedBy", &AffineTransform::followedBy)
        .def ("isIdentity", &AffineTransform::isIdentity)
        .def ("isSingularity", &AffineTransform::isSingularity)
        .def ("isOnlyTranslation", &AffineTransform::isOnlyTranslation)
        .def ("getTranslationX", &AffineTransform::getTranslationX)
        .def ("getTranslationY", &AffineTransform::getTranslationY)
        .def ("getDeterminant", &AffineTransform::getDeterminant)
        .def_property("mat00",
                      [](const AffineTransform& self) { return self.mat00; },
                      [](AffineTransform& self, float v) { self.mat00 = v; })
        .def_property("mat01",
                      [](const AffineTransform& self) { return self.mat01; },
                      [](AffineTransform& self, float v) { self.mat01 = v; })
        .def_property("mat02",
                      [](const AffineTransform& self) { return self.mat02; },
                      [](AffineTransform& self, float v) { self.mat02 = v; })
        .def_property("mat10",
                      [](const AffineTransform& self) { return self.mat10; },
                      [](AffineTransform& self, float v) { self.mat10 = v; })
        .def_property("mat11",
                      [](const AffineTransform& self) { return self.mat11; },
                      [](AffineTransform& self, float v) { self.mat11 = v; })
        .def_property("mat12",
                      [](const AffineTransform& self) { return self.mat12; },
                      [](AffineTransform& self, float v) { self.mat12 = v; })
    ;

    // ============================================================================================ juce::Point<>

    registerPoint<Point, int, float> (m);

    // ============================================================================================ juce::Line<>

    registerLine<Line, int, float> (m);

    // ============================================================================================ juce::Rectangle<>

    registerRectangle<Rectangle, int, float> (m);

    // ============================================================================================ juce::RectangleList<>

    registerRectangleList<RectangleList, int, float> (m);

    // ============================================================================================ juce::BorderSize<>

    registerBorderSize<BorderSize, int, float> (m);

    // ============================================================================================ juce::Path

    py::class_<Path> classPath (m, "Path");
    classPath
        .def (py::init<>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isEmpty", &Path::isEmpty)
        .def ("getBounds", &Path::getBounds)
        .def ("getBoundsTransformed", &Path::getBoundsTransformed)
        .def ("contains", py::overload_cast<float, float, float> (&Path::contains, py::const_))
        .def ("contains", py::overload_cast<Point<float>, float> (&Path::contains, py::const_))
        .def ("intersectsLine", &Path::intersectsLine)
        .def ("getClippedLine", &Path::getClippedLine)
        .def ("getLength", &Path::getLength)
        .def ("getPointAlongPath", &Path::getPointAlongPath)
        .def ("getNearestPoint", &Path::getNearestPoint)
        .def ("clear", &Path::clear)
        .def ("startNewSubPath", py::overload_cast<float, float> (&Path::startNewSubPath))
        .def ("startNewSubPath", py::overload_cast<Point<float>> (&Path::startNewSubPath))
        .def ("closeSubPath", &Path::closeSubPath)
        .def ("lineTo", py::overload_cast<float, float> (&Path::lineTo))
        .def ("lineTo", py::overload_cast<Point<float>> (&Path::lineTo))
        .def ("quadraticTo", py::overload_cast<float, float, float, float> (&Path::quadraticTo))
        .def ("quadraticTo", py::overload_cast<Point<float>, Point<float>> (&Path::quadraticTo))
        .def ("cubicTo", py::overload_cast<float, float, float, float, float, float> (&Path::cubicTo))
        .def ("cubicTo", py::overload_cast<Point<float>, Point<float>, Point<float>> (&Path::cubicTo))
        .def ("getCurrentPosition", &Path::getCurrentPosition)
        .def ("addRectangle", static_cast<void (Path::*)(float, float, float, float)> (&Path::addRectangle))
        .def ("addRectangle", static_cast<void (Path::*)(Rectangle<int>)> (&Path::template addRectangle<int>))
        .def ("addRectangle", static_cast<void (Path::*)(Rectangle<float>)> (&Path::template addRectangle<float>))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(float, float, float, float, float)> (&Path::addRoundedRectangle))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(float, float, float, float, float, float)> (&Path::addRoundedRectangle))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(float, float, float, float, float, float, bool, bool, bool, bool)> (&Path::addRoundedRectangle))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(Rectangle<int>, float)> (&Path::template addRoundedRectangle<int>))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(Rectangle<float>, float)> (&Path::template addRoundedRectangle<float>))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(Rectangle<int>, float, float)> (&Path::template addRoundedRectangle<int>))
        .def ("addRoundedRectangle", static_cast<void (Path::*)(Rectangle<float>, float, float)> (&Path::template addRoundedRectangle<float>))
        .def ("addTriangle", py::overload_cast<float, float, float, float, float, float> (&Path::addTriangle))
        .def ("addTriangle", py::overload_cast<Point<float>, Point<float>, Point<float>> (&Path::addTriangle))
        .def ("addQuadrilateral", &Path::addQuadrilateral)
        .def ("addEllipse", py::overload_cast<float, float, float, float> (&Path::addEllipse))
        .def ("addEllipse", py::overload_cast<Rectangle<float>> (&Path::addEllipse))
        .def ("addArc", &Path::addArc)
        .def ("addCentredArc", &Path::addCentredArc)
        .def ("addPieSegment", py::overload_cast<float, float, float, float, float, float, float> (&Path::addPieSegment))
        .def ("addPieSegment", py::overload_cast<Rectangle<float>, float, float, float> (&Path::addPieSegment))
        .def ("addLineSegment", &Path::addLineSegment)
        .def ("addArrow", &Path::addArrow)
        .def ("addPolygon", &Path::addPolygon)
        .def ("addStar", &Path::addStar)
        .def ("addBubble", &Path::addBubble)
        .def ("addPath", py::overload_cast<const Path&> (&Path::addPath))
        .def ("addPath", py::overload_cast<const Path&, const AffineTransform&> (&Path::addPath))
        .def ("swapWithPath", &Path::swapWithPath)
        .def ("preallocateSpace", &Path::preallocateSpace)
        .def ("applyTransform", &Path::applyTransform)
        .def ("scaleToFit", &Path::scaleToFit)
    //.def ("getTransformToScaleToFit", py::overload_cast<float, float, float, float, bool, Justification> (&Path::getTransformToScaleToFit))
    //.def ("getTransformToScaleToFit", py::overload_cast<Rectangle<float>, bool, Justification> (&Path::getTransformToScaleToFit))
        .def ("createPathWithRoundedCorners", &Path::createPathWithRoundedCorners)
        .def ("setUsingNonZeroWinding", &Path::setUsingNonZeroWinding)
        .def ("isUsingNonZeroWinding", &Path::isUsingNonZeroWinding)
        .def ("loadPathFromStream", &Path::loadPathFromStream)
        .def ("loadPathFromData", &Path::loadPathFromData)
        .def ("writePathToStream", &Path::writePathToStream)
        .def ("toString", &Path::toString)
        .def ("restoreFromString", &Path::restoreFromString)
        .def ("__str__", &Path::toString)
    ;

    // ============================================================================================ juce::Path

    py::class_<PathStrokeType> classPathStrokeType (m, "PathStrokeType");

    py::enum_<PathStrokeType::JointStyle> (classPathStrokeType, "JointStyle")
        .value ("mitered", PathStrokeType::mitered)
        .value ("curved", PathStrokeType::curved)
        .value ("beveled", PathStrokeType::beveled)
        .export_values();

    py::enum_<PathStrokeType::EndCapStyle> (classPathStrokeType, "EndCapStyle")
        .value ("butt", PathStrokeType::butt)
        .value ("square", PathStrokeType::square)
        .value ("rounded", PathStrokeType::rounded)
        .export_values();

    classPathStrokeType
        .def (py::init<float>())
        .def (py::init<float, PathStrokeType::JointStyle>())
        .def (py::init<float, PathStrokeType::JointStyle, PathStrokeType::EndCapStyle>())
        .def ("createStrokedPath", [](const PathStrokeType& self, Path* destPath, const Path& sourcePath, const AffineTransform& transform, float extraAccuracy)
        {
            self.createStrokedPath (*destPath, sourcePath, transform, extraAccuracy);
        })
        //.def ("createDashedStroke", ...)
        //.def ("createStrokeWithArrowheads", ...)
        .def ("getStrokeThickness", &PathStrokeType::getStrokeThickness)
        .def ("setStrokeThickness", &PathStrokeType::setStrokeThickness)
        .def ("getJointStyle", &PathStrokeType::getJointStyle)
        .def ("setJointStyle", &PathStrokeType::setJointStyle)
        .def ("getEndStyle", &PathStrokeType::getEndStyle)
        .def ("setEndStyle", &PathStrokeType::setEndStyle)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    // ============================================================================================ juce::PixelARGB

    py::class_<PixelARGB> (m, "PixelARGB")
        .def (py::init<>())
        .def (py::init<uint8, uint8, uint8, uint8>())
        .def ("getNativeARGB", &PixelARGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelARGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelARGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelARGB::getEvenBytes)
        .def ("getOddBytes", &PixelARGB::getOddBytes)
        .def ("getAlpha", &PixelARGB::getAlpha)
        .def ("getRed", &PixelARGB::getRed)
        .def ("getGreen", &PixelARGB::getGreen)
        .def ("getBlue", &PixelARGB::getBlue)
        .def ("set", &PixelARGB::template set<PixelRGB>)
        .def ("set", &PixelARGB::template set<PixelARGB>)
        .def ("set", &PixelARGB::template set<PixelAlpha>)
        .def ("setARGB", &PixelARGB::setARGB)
        .def ("blend", static_cast<void (PixelARGB::*)(PixelRGB)> (&PixelARGB::blend))
        .def ("blend", py::overload_cast<const PixelARGB&> (&PixelARGB::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&> (&PixelARGB::template blend<PixelAlpha>))
        .def ("blend", py::overload_cast<const PixelRGB&, uint32> (&PixelARGB::template blend<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&, uint32> (&PixelARGB::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&, uint32> (&PixelARGB::template blend<PixelAlpha>))
        .def ("tween", py::overload_cast<const PixelRGB&, uint32> (&PixelARGB::template tween<PixelRGB>))
        .def ("tween", py::overload_cast<const PixelARGB&, uint32> (&PixelARGB::template tween<PixelARGB>))
        .def ("tween", py::overload_cast<const PixelAlpha&, uint32> (&PixelARGB::template tween<PixelAlpha>))
        .def ("setAlpha", &PixelARGB::setAlpha)
        .def ("multiplyAlpha", py::overload_cast<int> (&PixelARGB::multiplyAlpha))
        .def ("multiplyAlpha", py::overload_cast<float> (&PixelARGB::multiplyAlpha))
        .def ("getUnpremultiplied", &PixelARGB::getUnpremultiplied)
        .def ("premultiply", &PixelARGB::premultiply)
        .def ("unpremultiply", &PixelARGB::unpremultiply)
        .def ("desaturate", &PixelARGB::desaturate)
        .def_property_readonly_static ("indexA", [] { return PixelARGB::indexA; })
        .def_property_readonly_static ("indexR", [] { return PixelARGB::indexR; })
        .def_property_readonly_static ("indexG", [] { return PixelARGB::indexG; })
        .def_property_readonly_static ("indexB", [] { return PixelARGB::indexB; })
    ;

    py::class_<PixelRGB> (m, "PixelRGB")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelRGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelRGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelRGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelRGB::getEvenBytes)
        .def ("getOddBytes", &PixelRGB::getOddBytes)
        .def ("getAlpha", &PixelRGB::getAlpha)
        .def ("getRed", &PixelRGB::getRed)
        .def ("getGreen", &PixelRGB::getGreen)
        .def ("getBlue", &PixelRGB::getBlue)
        .def ("set", &PixelRGB::template set<PixelRGB>)
        .def ("set", &PixelRGB::template set<PixelARGB>)
        .def ("set", &PixelRGB::template set<PixelAlpha>)
        .def ("setARGB", &PixelRGB::setARGB)
        .def ("blend", static_cast<void (PixelRGB::*)(PixelRGB)> (&PixelRGB::blend))
        .def ("blend", py::overload_cast<const PixelARGB&> (&PixelRGB::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&> (&PixelRGB::template blend<PixelAlpha>))
        .def ("blend", py::overload_cast<const PixelRGB&, uint32> (&PixelRGB::template blend<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&, uint32> (&PixelRGB::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&, uint32> (&PixelRGB::template blend<PixelAlpha>))
        .def ("blend", py::overload_cast<const PixelRGB&, uint32> (&PixelRGB::template tween<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&, uint32> (&PixelRGB::template tween<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&, uint32> (&PixelRGB::template tween<PixelAlpha>))
        .def ("tween", py::overload_cast<const PixelRGB&, uint32> (&PixelRGB::template tween<PixelRGB>))
        .def ("tween", py::overload_cast<const PixelARGB&, uint32> (&PixelRGB::template tween<PixelARGB>))
        .def ("tween", py::overload_cast<const PixelAlpha&, uint32> (&PixelRGB::template tween<PixelAlpha>))
        .def ("setAlpha", &PixelRGB::setAlpha)
        .def ("multiplyAlpha", py::overload_cast<int> (&PixelRGB::multiplyAlpha))
        .def ("multiplyAlpha", py::overload_cast<float> (&PixelRGB::multiplyAlpha))
        .def ("premultiply", &PixelRGB::premultiply)
        .def ("unpremultiply", &PixelRGB::unpremultiply)
        .def ("desaturate", &PixelRGB::desaturate)
        .def_property_readonly_static ("indexR", [] { return PixelARGB::indexR; })
        .def_property_readonly_static ("indexG", [] { return PixelARGB::indexG; })
        .def_property_readonly_static ("indexB", [] { return PixelARGB::indexB; })
    ;

    py::class_<PixelAlpha> (m, "PixelAlpha")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelAlpha::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelAlpha::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelAlpha::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelAlpha::getEvenBytes)
        .def ("getOddBytes", &PixelAlpha::getOddBytes)
        .def ("getAlpha", &PixelAlpha::getAlpha)
        .def ("getRed", &PixelAlpha::getRed)
        .def ("getGreen", &PixelAlpha::getGreen)
        .def ("getBlue", &PixelAlpha::getBlue)
        .def ("set", &PixelAlpha::template set<PixelRGB>)
        .def ("set", &PixelAlpha::template set<PixelARGB>)
        .def ("set", &PixelAlpha::template set<PixelAlpha>)
        .def ("setARGB", &PixelAlpha::setARGB)
        .def ("blend", py::overload_cast<const PixelRGB&> (&PixelAlpha::template blend<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&> (&PixelAlpha::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&> (&PixelAlpha::template blend<PixelAlpha>))
        .def ("blend", py::overload_cast<const PixelRGB&, uint32> (&PixelAlpha::template blend<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&, uint32> (&PixelAlpha::template blend<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&, uint32> (&PixelAlpha::template blend<PixelAlpha>))
        .def ("blend", py::overload_cast<const PixelRGB&, uint32> (&PixelAlpha::template tween<PixelRGB>))
        .def ("blend", py::overload_cast<const PixelARGB&, uint32> (&PixelAlpha::template tween<PixelARGB>))
        .def ("blend", py::overload_cast<const PixelAlpha&, uint32> (&PixelAlpha::template tween<PixelAlpha>))
        .def ("tween", py::overload_cast<const PixelRGB&, uint32> (&PixelAlpha::template tween<PixelRGB>))
        .def ("tween", py::overload_cast<const PixelARGB&, uint32> (&PixelAlpha::template tween<PixelARGB>))
        .def ("tween", py::overload_cast<const PixelAlpha&, uint32> (&PixelAlpha::template tween<PixelAlpha>))
        .def ("setAlpha", &PixelAlpha::setAlpha)
        .def ("multiplyAlpha", py::overload_cast<int> (&PixelAlpha::multiplyAlpha))
        .def ("multiplyAlpha", py::overload_cast<float> (&PixelAlpha::multiplyAlpha))
        .def ("premultiply", &PixelAlpha::premultiply)
        .def ("unpremultiply", &PixelAlpha::unpremultiply)
        .def ("desaturate", &PixelAlpha::desaturate)
        .def_property_readonly_static ("indexA", [] { return PixelARGB::indexA; })
    ;

    // ============================================================================================ juce::Colour

    py::class_<Colour> (m, "Colour")
        .def (py::init<>())
        .def (py::init<uint32>())
        .def (py::init<uint8, uint8, uint8>())
        .def (py::init<uint8, uint8, uint8, uint8>())
    //.def (py::init<uint8, uint8, uint8, float>())
    //.def (py::init<float, float, float, uint8>())
    //.def (py::init<float, float, float, float>())
        .def (py::init<PixelARGB>())
        .def (py::init<PixelRGB>())
        .def (py::init<PixelAlpha>())
        .def_static ("fromRGB", &Colour::fromRGB)
        .def_static ("fromRGBA", &Colour::fromRGBA)
        .def_static ("fromFloatRGBA", &Colour::fromFloatRGBA)
        .def_static ("fromHSV", &Colour::fromHSV)
        .def_static ("fromHSL", &Colour::fromHSL)
        .def ("getRed", &Colour::getRed)
        .def ("getGreen", &Colour::getGreen)
        .def ("getBlue", &Colour::getBlue)
        .def ("getFloatRed", &Colour::getFloatRed)
        .def ("getFloatGreen", &Colour::getFloatGreen)
        .def ("getFloatBlue", &Colour::getFloatBlue)
        .def ("getPixelARGB", &Colour::getPixelARGB)
        .def ("getNonPremultipliedPixelARGB", &Colour::getNonPremultipliedPixelARGB)
        .def ("getARGB", &Colour::getARGB)
        .def ("getAlpha", &Colour::getAlpha)
        .def ("getFloatAlpha", &Colour::getFloatAlpha)
        .def ("isOpaque", &Colour::isOpaque)
        .def ("isTransparent", &Colour::isTransparent)
        .def ("withAlpha", py::overload_cast<float> (&Colour::withAlpha, py::const_))
        .def ("withMultipliedAlpha", &Colour::withMultipliedAlpha)
        .def ("overlaidWith", &Colour::overlaidWith)
        .def ("interpolatedWith", &Colour::interpolatedWith)
        .def ("getHue", &Colour::getHue)
        .def ("getSaturation", &Colour::getSaturation)
        .def ("getSaturationHSL", &Colour::getSaturationHSL)
        .def ("getBrightness", &Colour::getBrightness)
        .def ("getLightness", &Colour::getLightness)
        .def ("getPerceivedBrightness", &Colour::getPerceivedBrightness)
        .def ("withHue", &Colour::withHue)
        .def ("withSaturation", &Colour::withSaturation)
        .def ("withSaturationHSL", &Colour::withSaturationHSL)
        .def ("withBrightness", &Colour::withBrightness)
        .def ("withLightness", &Colour::withLightness)
        .def ("withRotatedHue", &Colour::withRotatedHue)
        .def ("withMultipliedSaturation", &Colour::withMultipliedSaturation)
        .def ("withMultipliedSaturationHSL", &Colour::withMultipliedSaturationHSL)
        .def ("withMultipliedBrightness", &Colour::withMultipliedBrightness)
        .def ("withMultipliedLightness", &Colour::withMultipliedLightness)
        .def ("brighter", &Colour::brighter)
        .def ("darker", &Colour::darker)
        .def ("contrasting", py::overload_cast<float> (&Colour::contrasting, py::const_))
        .def ("contrasting", py::overload_cast<Colour, float> (&Colour::contrasting, py::const_))
    //.def_static ("contrasting", static_cast<Colour (*)(Colour, Colour)>(&Colour::contrasting)) // Not supported by pybind11
        .def_static ("greyLevel", &Colour::greyLevel)
        .def ("toString", &Colour::toString)
        .def_static ("fromString", &Colour::fromString)
        .def ("toDisplayString", &Colour::toDisplayString)
        .def ("__str__", &Colour::toString)
    ;

    // ============================================================================================ juce::Colour

    py::class_<ColourGradient> (m, "ColourGradient")
        .def (py::init<>())
        .def_static ("vertical", [](Colour c1, float y1, Colour c2, float y2) { return ColourGradient::vertical(c1, y1, c2, y2); })
        .def_static ("horizontal", [](Colour c1, float y1, Colour c2, float y2) { return ColourGradient::horizontal(c1, y1, c2, y2); })
        .def ("clearColours", &ColourGradient::clearColours)
        .def ("addColour", &ColourGradient::addColour)
        .def ("removeColour", &ColourGradient::removeColour)
        .def ("multiplyOpacity", &ColourGradient::multiplyOpacity)
        .def ("getNumColours", &ColourGradient::getNumColours)
        .def ("getColourPosition", &ColourGradient::getColourPosition)
        .def ("getColour", &ColourGradient::getColour)
        .def ("setColour", &ColourGradient::setColour)
        .def ("getColourAtPosition", &ColourGradient::getColourAtPosition)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
        .def ("isOpaque", &ColourGradient::isOpaque)
        .def ("isInvisible", &ColourGradient::isInvisible)
        .def_property("point1",
                      [](const ColourGradient& self) { return self.point1; },
                      [](ColourGradient& self, const Point<float>& v) { self.point1 = v; })
        .def_property("point2",
                      [](const ColourGradient& self) { return self.point2; },
                      [](ColourGradient& self, const Point<float>& v) { self.point2 = v; })
        .def_property("isRadial",
                      [](const ColourGradient& self) { return self.isRadial; },
                      [](ColourGradient& self, bool v) { self.isRadial = v; })
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    // ============================================================================================ juce::Image

    py::class_<Image> classImage (m, "Image");

    py::enum_<Image::PixelFormat> (classImage, "PixelFormat")
        .value ("UnknownFormat", Image::PixelFormat::UnknownFormat)
        .value ("RGB", Image::PixelFormat::RGB)
        .value ("ARGB", Image::PixelFormat::ARGB)
        .value ("SingleChannel", Image::PixelFormat::SingleChannel)
        .export_values();

    classImage
        .def (py::init<>())
        .def (py::init<Image::PixelFormat, int, int, bool>())
        .def (py::init<Image::PixelFormat, int, int, bool, const ImageType&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isValid", &Image::isValid)
        .def ("isNull", &Image::isNull)
        .def ("getWidth", &Image::getWidth)
        .def ("getHeight", &Image::getHeight)
        .def ("getBounds", &Image::getBounds)
        .def ("getFormat", &Image::getFormat)
        .def ("isARGB", &Image::isARGB)
        .def ("isRGB", &Image::isRGB)
        .def ("isSingleChannel", &Image::isSingleChannel)
        .def ("hasAlphaChannel", &Image::hasAlphaChannel)
        .def ("clear", &Image::clear)
        .def ("rescaled", &Image::rescaled)
        .def ("createCopy", &Image::createCopy)
        .def ("convertedToFormat", &Image::convertedToFormat)
        .def ("duplicateIfShared", &Image::duplicateIfShared)
        .def ("getClippedImage", &Image::getClippedImage)
        .def ("getPixelAt", &Image::getPixelAt)
        .def ("setPixelAt", &Image::setPixelAt)
        .def ("multiplyAlphaAt", &Image::multiplyAlphaAt)
        .def ("multiplyAllAlphas", &Image::multiplyAllAlphas)
        .def ("desaturate", &Image::desaturate)
        .def ("moveImageSection", &Image::moveImageSection)
        .def ("createSolidAreaMask", &Image::createSolidAreaMask)
        .def ("getProperties", &Image::getProperties)
    //.def ("createLowLevelContext", &Image::createLowLevelContext)
        .def ("getReferenceCount", &Image::getReferenceCount)
    //.def ("getPixelData", &Image::getPixelData)
    ;

    // ============================================================================================ juce::ImagePixelData

    py::class_<ImagePixelData> classImagePixelData (m, "ImagePixelData");

    classImagePixelData
    //.def (py::init<Image::PixelFormat, int, int>())
    //.def ("createLowLevelContext", &ImagePixelData::createLowLevelContext)
    //.def ("clone", &ImagePixelData::clone)
    //.def ("createType", &ImagePixelData::createType)
    //.def ("initialiseBitmapData", &ImagePixelData::initialiseBitmapData)
        .def ("getSharedCount", &ImagePixelData::getSharedCount)
        .def_readonly ("pixelFormat", &ImagePixelData::pixelFormat)
        .def_readonly ("width", &ImagePixelData::width)
        .def_readonly ("height", &ImagePixelData::height)
        .def_readwrite ("userData", &ImagePixelData::userData)
    //.def_readwrite ("listeners", &ImagePixelData::listeners)
        .def ("sendDataChangeMessage", &ImagePixelData::sendDataChangeMessage)
    ;

    // ============================================================================================ juce::ImagePixelData

    py::class_<ImageType> classImageType (m, "ImageType");

    classImageType
    //.def (py::init<>())
        .def ("create", &ImageType::create)
        .def ("getTypeID", &ImageType::getTypeID)
        .def ("convert", &ImageType::convert)
    ;

    // ============================================================================================ juce::FillType

    py::class_<FillType> (m, "FillType")
        .def (py::init<>())
        .def (py::init<Colour>())
        .def (py::init<const ColourGradient&>())
        .def (py::init<const Image&, const AffineTransform&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isColour", &FillType::isColour)
        .def ("isGradient", &FillType::isGradient)
        .def ("isTiledImage", &FillType::isTiledImage)
        .def ("setColour", &FillType::setColour)
        .def ("setGradient", &FillType::setGradient)
        .def ("setTiledImage", &FillType::setTiledImage)
        .def ("setOpacity", &FillType::setOpacity)
        .def ("getOpacity", &FillType::getOpacity)
        .def ("isInvisible", &FillType::isInvisible)
        .def ("transformed", &FillType::transformed)
        .def_readwrite("colour", &FillType::colour)
        .def_property("gradient",
                      [](const FillType& self) { return self.gradient.get(); },
                      [](FillType& self, ColourGradient* v) { self.gradient = v ? std::make_unique<ColourGradient>(*v) : nullptr; })
        .def_readwrite("image", &FillType::image)
        .def_readwrite("transform", &FillType::transform)
    ;

    // ============================================================================================ juce::Graphics

    py::class_<Graphics> classGraphics (m, "Graphics");

    py::enum_<Graphics::ResamplingQuality> (classGraphics, "ResamplingQuality")
        .value("lowResamplingQuality", Graphics::ResamplingQuality::lowResamplingQuality)
        .value("mediumResamplingQuality", Graphics::ResamplingQuality::mediumResamplingQuality)
        .value("highResamplingQuality", Graphics::ResamplingQuality::highResamplingQuality)
        .export_values();

    classGraphics
        .def (py::init<const Image&>())
        .def ("setColour", &Graphics::setColour)
        .def ("setOpacity", &Graphics::setOpacity)
        .def ("setGradientFill", py::overload_cast<const ColourGradient&> (&Graphics::setGradientFill))
        .def ("setTiledImageFill", &Graphics::setTiledImageFill)
        .def ("setFillType", &Graphics::setFillType)
    //.def ("setFont", py::overload_cast<const Font&>(&Graphics::setFont))
    //.def ("setFont", py::overload_cast<float>(&Graphics::setFont))
    //.def ("getCurrentFont", &Graphics::getCurrentFont)
        .def ("drawSingleLineText", &Graphics::drawSingleLineText)
        .def ("drawMultiLineText", &Graphics::drawMultiLineText)
        .def ("drawText", py::overload_cast<const String&, int, int, int, int, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawText", py::overload_cast<const String&, Rectangle<int>, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawText", py::overload_cast<const String&, Rectangle<float>, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawFittedText", py::overload_cast<const String&, int, int, int, int, Justification, int, float> (&Graphics::drawFittedText, py::const_))
        .def ("drawFittedText", py::overload_cast<const String&, Rectangle<int>, Justification, int, float> (&Graphics::drawFittedText, py::const_))
        .def ("fillAll", py::overload_cast<> (&Graphics::fillAll, py::const_))
        .def ("fillAll", py::overload_cast<Colour> (&Graphics::fillAll, py::const_))
        .def ("fillRect", py::overload_cast<Rectangle<int>> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<Rectangle<float>> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<int, int, int, int> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<float, float, float, float> (&Graphics::fillRect, py::const_))
        .def ("fillRectList", py::overload_cast<const RectangleList<float>&> (&Graphics::fillRectList, py::const_))
        .def ("fillRectList", py::overload_cast<const RectangleList<int>&> (&Graphics::fillRectList, py::const_))
        .def ("fillRoundedRectangle", py::overload_cast<float, float, float, float, float> (&Graphics::fillRoundedRectangle, py::const_))
        .def ("fillRoundedRectangle", py::overload_cast<Rectangle<float>, float> (&Graphics::fillRoundedRectangle, py::const_))
        .def ("fillCheckerBoard", &Graphics::fillCheckerBoard)
        .def ("drawRect", py::overload_cast<int, int, int, int, int> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<float, float, float, float, float> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<Rectangle<int>, int> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<Rectangle<float>, float> (&Graphics::drawRect, py::const_))
        .def ("drawRoundedRectangle", py::overload_cast<float, float, float, float, float, float> (&Graphics::drawRoundedRectangle, py::const_))
        .def ("drawRoundedRectangle", py::overload_cast<Rectangle<float>, float, float> (&Graphics::drawRoundedRectangle, py::const_))
        .def ("fillEllipse", py::overload_cast<float, float, float, float> (&Graphics::fillEllipse, py::const_))
        .def ("fillEllipse", py::overload_cast<Rectangle<float>> (&Graphics::fillEllipse, py::const_))
        .def ("drawEllipse", py::overload_cast<float, float, float, float, float> (&Graphics::drawEllipse, py::const_))
        .def ("drawEllipse", py::overload_cast<Rectangle<float>, float> (&Graphics::drawEllipse, py::const_))
        .def ("drawLine", py::overload_cast<float, float, float, float> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<float, float, float, float, float> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<Line<float>> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<Line<float>, float> (&Graphics::drawLine, py::const_))
    //.def ("drawDashedLine", &Graphics::drawDashedLine)
        .def ("drawVerticalLine", &Graphics::drawVerticalLine)
        .def ("drawHorizontalLine", &Graphics::drawHorizontalLine)
        .def ("fillPath", py::overload_cast<const Path&> (&Graphics::fillPath, py::const_))
        .def ("fillPath", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::fillPath, py::const_))
        .def ("strokePath", &Graphics::strokePath)
        .def ("drawArrow", &Graphics::drawArrow)
        .def ("setImageResamplingQuality", &Graphics::setImageResamplingQuality)
        .def ("drawImageAt", &Graphics::drawImageAt)
        .def ("drawImage", py::overload_cast<const Image&, int, int, int, int, int, int, int, int, bool> (&Graphics::drawImage, py::const_))
        .def ("drawImageTransformed", &Graphics::drawImageTransformed)
        .def ("drawImage", py::overload_cast<const Image&, Rectangle<float>, RectanglePlacement, bool> (&Graphics::drawImage, py::const_))
        .def ("drawImageWithin", &Graphics::drawImageWithin)
        .def ("getClipBounds", &Graphics::getClipBounds)
        .def ("clipRegionIntersects", &Graphics::clipRegionIntersects)
        .def ("reduceClipRegion", py::overload_cast<int, int, int, int> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<Rectangle<int>> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const RectangleList<int>&> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const Image&, const AffineTransform&> (&Graphics::reduceClipRegion))
        .def ("excludeClipRegion", &Graphics::excludeClipRegion)
        .def ("isClipEmpty", &Graphics::isClipEmpty)
        .def ("saveState", &Graphics::saveState)
        .def ("restoreState", &Graphics::restoreState)
        .def ("beginTransparencyLayer", &Graphics::beginTransparencyLayer)
        .def ("endTransparencyLayer", &Graphics::endTransparencyLayer)
        .def ("setOrigin", py::overload_cast<Point<int>> (&Graphics::setOrigin))
        .def ("setOrigin", py::overload_cast<int, int> (&Graphics::setOrigin))
        .def ("addTransform", &Graphics::addTransform)
        .def ("resetToDefaultState", &Graphics::resetToDefaultState)
        .def ("isVectorDevice", &Graphics::isVectorDevice)
    ;

    struct PyGraphicsScopedSaveState
    {
        PyGraphicsScopedSaveState (Graphics& g)
            : g(g)
        {
        }

        Graphics& g;
        std::variant<std::monostate, Graphics::ScopedSaveState> state;
    };

    py::class_<PyGraphicsScopedSaveState> (classGraphics, "ScopedSaveState")
        .def (py::init<Graphics&>())
        .def ("__enter__", [](PyGraphicsScopedSaveState& self)
        {
            self.state.emplace<Graphics::ScopedSaveState>(self.g);
        })
        .def ("__exit__", [](PyGraphicsScopedSaveState& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.state.emplace<std::monostate>();
        })
    ;

    // ============================================================================================ juce::Colours

    auto submoduleColours = m.def_submodule ("Colours");
    submoduleColours.def ("findColourForName", &Colours::findColourForName);
    submoduleColours.attr ("transparentBlack") = Colours::transparentBlack;
    submoduleColours.attr ("transparentWhite") = Colours::transparentWhite;
    submoduleColours.attr ("aliceblue") = Colours::aliceblue;
    submoduleColours.attr ("antiquewhite") = Colours::antiquewhite;
    submoduleColours.attr ("aqua") = Colours::aqua;
    submoduleColours.attr ("aquamarine") = Colours::aquamarine;
    submoduleColours.attr ("azure") = Colours::azure;
    submoduleColours.attr ("beige") = Colours::beige;
    submoduleColours.attr ("bisque") = Colours::bisque;
    submoduleColours.attr ("black") = Colours::black;
    submoduleColours.attr ("blanchedalmond") = Colours::blanchedalmond;
    submoduleColours.attr ("blue") = Colours::blue;
    submoduleColours.attr ("blueviolet") = Colours::blueviolet;
    submoduleColours.attr ("brown") = Colours::brown;
    submoduleColours.attr ("burlywood") = Colours::burlywood;
    submoduleColours.attr ("cadetblue") = Colours::cadetblue;
    submoduleColours.attr ("chartreuse") = Colours::chartreuse;
    submoduleColours.attr ("chocolate") = Colours::chocolate;
    submoduleColours.attr ("coral") = Colours::coral;
    submoduleColours.attr ("cornflowerblue") = Colours::cornflowerblue;
    submoduleColours.attr ("cornsilk") = Colours::cornsilk;
    submoduleColours.attr ("crimson") = Colours::crimson;
    submoduleColours.attr ("cyan") = Colours::cyan;
    submoduleColours.attr ("darkblue") = Colours::darkblue;
    submoduleColours.attr ("darkcyan") = Colours::darkcyan;
    submoduleColours.attr ("darkgoldenrod") = Colours::darkgoldenrod;
    submoduleColours.attr ("darkgrey") = Colours::darkgrey;
    submoduleColours.attr ("darkgreen") = Colours::darkgreen;
    submoduleColours.attr ("darkkhaki") = Colours::darkkhaki;
    submoduleColours.attr ("darkmagenta") = Colours::darkmagenta;
    submoduleColours.attr ("darkolivegreen") = Colours::darkolivegreen;
    submoduleColours.attr ("darkorange") = Colours::darkorange;
    submoduleColours.attr ("darkorchid") = Colours::darkorchid;
    submoduleColours.attr ("darkred") = Colours::darkred;
    submoduleColours.attr ("darksalmon") = Colours::darksalmon;
    submoduleColours.attr ("darkseagreen") = Colours::darkseagreen;
    submoduleColours.attr ("darkslateblue") = Colours::darkslateblue;
    submoduleColours.attr ("darkslategrey") = Colours::darkslategrey;
    submoduleColours.attr ("darkturquoise") = Colours::darkturquoise;
    submoduleColours.attr ("darkviolet") = Colours::darkviolet;
    submoduleColours.attr ("deeppink") = Colours::deeppink;
    submoduleColours.attr ("deepskyblue") = Colours::deepskyblue;
    submoduleColours.attr ("dimgrey") = Colours::dimgrey;
    submoduleColours.attr ("dodgerblue") = Colours::dodgerblue;
    submoduleColours.attr ("firebrick") = Colours::firebrick;
    submoduleColours.attr ("floralwhite") = Colours::floralwhite;
    submoduleColours.attr ("forestgreen") = Colours::forestgreen;
    submoduleColours.attr ("fuchsia") = Colours::fuchsia;
    submoduleColours.attr ("gainsboro") = Colours::gainsboro;
    submoduleColours.attr ("ghostwhite") = Colours::ghostwhite;
    submoduleColours.attr ("gold") = Colours::gold;
    submoduleColours.attr ("goldenrod") = Colours::goldenrod;
    submoduleColours.attr ("grey") = Colours::grey;
    submoduleColours.attr ("green") = Colours::green;
    submoduleColours.attr ("greenyellow") = Colours::greenyellow;
    submoduleColours.attr ("honeydew") = Colours::honeydew;
    submoduleColours.attr ("hotpink") = Colours::hotpink;
    submoduleColours.attr ("indianred") = Colours::indianred;
    submoduleColours.attr ("indigo") = Colours::indigo;
    submoduleColours.attr ("ivory") = Colours::ivory;
    submoduleColours.attr ("khaki") = Colours::khaki;
    submoduleColours.attr ("lavender") = Colours::lavender;
    submoduleColours.attr ("lavenderblush") = Colours::lavenderblush;
    submoduleColours.attr ("lawngreen") = Colours::lawngreen;
    submoduleColours.attr ("lemonchiffon") = Colours::lemonchiffon;
    submoduleColours.attr ("lightblue") = Colours::lightblue;
    submoduleColours.attr ("lightcoral") = Colours::lightcoral;
    submoduleColours.attr ("lightcyan") = Colours::lightcyan;
    submoduleColours.attr ("lightgoldenrodyellow") = Colours::lightgoldenrodyellow;
    submoduleColours.attr ("lightgreen") = Colours::lightgreen;
    submoduleColours.attr ("lightgrey") = Colours::lightgrey;
    submoduleColours.attr ("lightpink") = Colours::lightpink;
    submoduleColours.attr ("lightsalmon") = Colours::lightsalmon;
    submoduleColours.attr ("lightseagreen") = Colours::lightseagreen;
    submoduleColours.attr ("lightskyblue") = Colours::lightskyblue;
    submoduleColours.attr ("lightslategrey") = Colours::lightslategrey;
    submoduleColours.attr ("lightsteelblue") = Colours::lightsteelblue;
    submoduleColours.attr ("lightyellow") = Colours::lightyellow;
    submoduleColours.attr ("lime") = Colours::lime;
    submoduleColours.attr ("limegreen") = Colours::limegreen;
    submoduleColours.attr ("linen") = Colours::linen;
    submoduleColours.attr ("magenta") = Colours::magenta;
    submoduleColours.attr ("maroon") = Colours::maroon;
    submoduleColours.attr ("mediumaquamarine") = Colours::mediumaquamarine;
    submoduleColours.attr ("mediumblue") = Colours::mediumblue;
    submoduleColours.attr ("mediumorchid") = Colours::mediumorchid;
    submoduleColours.attr ("mediumpurple") = Colours::mediumpurple;
    submoduleColours.attr ("mediumseagreen") = Colours::mediumseagreen;
    submoduleColours.attr ("mediumslateblue") = Colours::mediumslateblue;
    submoduleColours.attr ("mediumspringgreen") = Colours::mediumspringgreen;
    submoduleColours.attr ("mediumturquoise") = Colours::mediumturquoise;
    submoduleColours.attr ("mediumvioletred") = Colours::mediumvioletred;
    submoduleColours.attr ("midnightblue") = Colours::midnightblue;
    submoduleColours.attr ("mintcream") = Colours::mintcream;
    submoduleColours.attr ("mistyrose") = Colours::mistyrose;
    submoduleColours.attr ("moccasin") = Colours::moccasin;
    submoduleColours.attr ("navajowhite") = Colours::navajowhite;
    submoduleColours.attr ("navy") = Colours::navy;
    submoduleColours.attr ("oldlace") = Colours::oldlace;
    submoduleColours.attr ("olive") = Colours::olive;
    submoduleColours.attr ("olivedrab") = Colours::olivedrab;
    submoduleColours.attr ("orange") = Colours::orange;
    submoduleColours.attr ("orangered") = Colours::orangered;
    submoduleColours.attr ("orchid") = Colours::orchid;
    submoduleColours.attr ("palegoldenrod") = Colours::palegoldenrod;
    submoduleColours.attr ("palegreen") = Colours::palegreen;
    submoduleColours.attr ("paleturquoise") = Colours::paleturquoise;
    submoduleColours.attr ("palevioletred") = Colours::palevioletred;
    submoduleColours.attr ("papayawhip") = Colours::papayawhip;
    submoduleColours.attr ("peachpuff") = Colours::peachpuff;
    submoduleColours.attr ("peru") = Colours::peru;
    submoduleColours.attr ("pink") = Colours::pink;
    submoduleColours.attr ("plum") = Colours::plum;
    submoduleColours.attr ("powderblue") = Colours::powderblue;
    submoduleColours.attr ("purple") = Colours::purple;
    submoduleColours.attr ("rebeccapurple") = Colours::rebeccapurple;
    submoduleColours.attr ("red") = Colours::red;
    submoduleColours.attr ("rosybrown") = Colours::rosybrown;
    submoduleColours.attr ("royalblue") = Colours::royalblue;
    submoduleColours.attr ("saddlebrown") = Colours::saddlebrown;
    submoduleColours.attr ("salmon") = Colours::salmon;
    submoduleColours.attr ("sandybrown") = Colours::sandybrown;
    submoduleColours.attr ("seagreen") = Colours::seagreen;
    submoduleColours.attr ("seashell") = Colours::seashell;
    submoduleColours.attr ("sienna") = Colours::sienna;
    submoduleColours.attr ("silver") = Colours::silver;
    submoduleColours.attr ("skyblue") = Colours::skyblue;
    submoduleColours.attr ("slateblue") = Colours::slateblue;
    submoduleColours.attr ("slategrey") = Colours::slategrey;
    submoduleColours.attr ("snow") = Colours::snow;
    submoduleColours.attr ("springgreen") = Colours::springgreen;
    submoduleColours.attr ("steelblue") = Colours::steelblue;
    submoduleColours.attr ("tan") = Colours::tan;
    submoduleColours.attr ("teal") = Colours::teal;
    submoduleColours.attr ("thistle") = Colours::thistle;
    submoduleColours.attr ("tomato") = Colours::tomato;
    submoduleColours.attr ("turquoise") = Colours::turquoise;
    submoduleColours.attr ("violet") = Colours::violet;
    submoduleColours.attr ("wheat") = Colours::wheat;
    submoduleColours.attr ("white") = Colours::white;
    submoduleColours.attr ("whitesmoke") = Colours::whitesmoke;
    submoduleColours.attr ("yellow") = Colours::yellow;
    submoduleColours.attr ("yellowgreen") = Colours::yellowgreen;
}

} // namespace popsicle::Bindings

#endif
