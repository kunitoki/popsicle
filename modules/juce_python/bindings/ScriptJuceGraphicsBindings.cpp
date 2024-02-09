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
#include "ScriptJuceGraphicsBindings.h"
#include "../utilities/ClassDemangling.h"
#include "../utilities/PythonInterop.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../pybind11/operators.h"

#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

template <template <class> class Class, class... Types>
void registerPoint (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Point", typeid (Types).name());

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
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getX() << ", " << self.getY() << ")";
                return result;
            })
            .def ("__str__", &T::toString)
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Point", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerLine (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Line", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def (py::init<Point<ValueType>, Point<ValueType>>())
            .def (py::init<const T&>())
            .def ("getStartX", &T::getStartX)
            .def ("getStartY", &T::getStartY)
            .def ("getEndX", &T::getEndX)
            .def ("getEndY", &T::getEndY)
            .def ("getStart", &T::getStart)
            .def ("getEnd", &T::getEnd)
            .def ("setStart", py::overload_cast<ValueType, ValueType> (&T::setStart))
            .def ("setStart", py::overload_cast<const Point<ValueType>> (&T::setStart))
            .def ("setEnd", py::overload_cast<ValueType, ValueType> (&T::setEnd))
            .def ("setEnd", py::overload_cast<const Point<ValueType>> (&T::setEnd))
            .def ("reversed", &T::reversed)
            .def ("applyTransform", &T::applyTransform)
            .def ("getLength", &T::getLength)
            .def ("getLengthSquared", &T::getLengthSquared)
            .def ("isVertical", &T::isVertical)
            .def ("isHorizontal", &T::isHorizontal)
            .def ("getAngle", &T::getAngle)
        //.def_static ("fromStartAndAngle", &T::fromStartAndAngle) // Fails for int
            .def ("toFloat", &T::toFloat)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("getIntersection", &T::getIntersection)
            .def ("intersects", py::overload_cast<T, Point<ValueType>&> (&T::intersects, py::const_))
            .def ("intersects", py::overload_cast<T> (&T::intersects, py::const_))
            .def ("getPointAlongLine", py::overload_cast<ValueType> (&T::getPointAlongLine, py::const_))
            .def ("getPointAlongLine", py::overload_cast<ValueType, ValueType> (&T::getPointAlongLine, py::const_))
            .def ("getPointAlongLineProportionally", &T::getPointAlongLineProportionally)
            .def ("getDistanceFromPoint", &T::getDistanceFromPoint)
            .def ("findNearestProportionalPositionTo", &T::findNearestProportionalPositionTo)
            .def ("findNearestPointTo", &T::findNearestPointTo)
            .def ("isPointAbove", &T::isPointAbove)
            .def ("withLengthenedStart", &T::withLengthenedStart)
            .def ("withShortenedStart", &T::withShortenedStart)
            .def ("withLengthenedEnd", &T::withLengthenedEnd)
            .def ("withShortenedEnd", &T::withShortenedEnd)
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getStartX() << ", " << self.getStartY() << ", " << self.getEndX() << ", " << self.getEndY() << ")";
                return result;
            })
            .def ("__str__", [](const T& self)
            {
                String result;
                result
                    << "(" << self.getStartX() << ", " << self.getStartY() << "), (" << self.getEndX() << ", " << self.getEndY() << ")";
                return result;
            })
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Line", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRectangle (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Rectangle", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def (py::init<Point<ValueType>, Point<ValueType>>())
            .def (py::init<const T&>())
            .def_static ("leftTopRightBottom", &T::leftTopRightBottom)
            .def ("isEmpty", &T::isEmpty)
            .def ("isFinite", &T::isFinite)
            .def ("getX", &T::getX)
            .def ("getY", &T::getY)
            .def ("getWidth", &T::getWidth)
            .def ("getHeight", &T::getHeight)
            .def ("getRight", &T::getRight)
            .def ("getBottom", &T::getBottom)
            .def ("getCentreX", &T::getCentreX)
            .def ("getCentreY", &T::getCentreY)
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
            .def (py::self + Point<ValueType>())
            .def (py::self += Point<ValueType>())
            .def (py::self - Point<ValueType>())
            .def (py::self -= Point<ValueType>())
            .def (py::self * float())
            .def (py::self *= float())
            .def (py::self * Point<float>())
            .def (py::self *= Point<float>())
            .def (py::self / float())
            .def (py::self /= float())
            .def (py::self / Point<float>())
            .def (py::self /= Point<float>())
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
            .def ("proportionOfWidth", &T::template proportionOfWidth<float>)
            .def ("proportionOfHeight", &T::template proportionOfHeight<float>)
            .def ("getProportion", &T::template getProportion<float>)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("contains", py::overload_cast<ValueType, ValueType> (&T::contains, py::const_))
            .def ("contains", py::overload_cast<Point<ValueType>> (&T::contains, py::const_))
            .def ("contains", py::overload_cast<T> (&T::contains, py::const_))
            .def ("intersects", py::overload_cast<T> (&T::intersects, py::const_))
            .def ("intersects", py::overload_cast<const Line<ValueType>&> (&T::intersects, py::const_))
            .def ("getIntersection", &T::getIntersection)
        //.def ("intersectRectangle", &T::intersectRectangle)
            .def ("intersectRectangle",py::overload_cast<T&> (&T::intersectRectangle, py::const_))
            .def ("getUnion", &T::getUnion)
            .def ("enlargeIfAdjacent", &T::enlargeIfAdjacent)
            .def ("reduceIfPartlyContainedIn", &T::reduceIfPartlyContainedIn)
            .def ("constrainedWithin", &T::constrainedWithin)
            .def ("transformedBy", &T::transformedBy)
            .def ("getSmallestIntegerContainer", &T::getSmallestIntegerContainer)
            .def ("toNearestInt", &T::toNearestInt)
            .def ("toNearestIntEdges", &T::toNearestIntEdges)
            .def ("toFloat", &T::toFloat)
        //.def_static ("findAreaContainingPoints", &T::findAreaContainingPoints)
        //.def_static ("intersectRectangles", &T::intersectRectangles)
            .def ("toString", &T::toString)
            .def_static ("fromString", &T::fromString)
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getX() << ", " << self.getY() << ", " << self.getWidth() << ", " << self.getHeight() << ")";
                return result;
            })
            .def ("__str__", &T::toString)
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Rectangle", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRectangleList (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("RectangleList", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<Rectangle<ValueType>>())
            .def (py::init<const T&>())
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
            .def ("getIntersectionWith", &T::getIntersectionWith)
            .def ("swapWith", &T::swapWith)
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
            .def("__iter__", [](const T& self)
            {
                return py::make_iterator (self.begin(), self.end());
            }, py::keep_alive<0, 1>())
            .def ("ensureStorageAllocated", &T::ensureStorageAllocated)
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("RectangleList", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerParallelogram (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("Parallelogram", typeid (Types).name());

        auto class_ = py::class_<T> (m, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<Point<ValueType>, Point<ValueType>, Point<ValueType>>())
            .def (py::init ([](ValueType x1, ValueType y1, ValueType x2, ValueType y2, ValueType x3, ValueType y3)
            {
                return T (Point<ValueType>(x1, y1), Point<ValueType>(x2, y2), Point<ValueType>(x3, y3));
            }))
            .def (py::init<Rectangle<ValueType>>())
            .def (py::init<const T&>())
            .def ("isEmpty", &T::isEmpty)
            .def ("isFinite", &T::isFinite)
            .def ("getWidth", &T::getWidth)
            .def ("getHeight", &T::getHeight)
            .def ("getTopLeft", &T::getTopLeft)
            .def ("getTopRight", &T::getTopRight)
            .def ("getBottomLeft", &T::getBottomLeft)
            .def ("getBottomRight", &T::getBottomRight)
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def (py::self + Point<ValueType>())
            .def (py::self += Point<ValueType>())
            .def (py::self - Point<ValueType>())
        //.def (py::self -= Point<ValueType>()) // TODO - JUCE Bug
            .def (py::self * float())
            .def (py::self *= float())
            .def (py::self * Point<float>())
            .def (py::self *= Point<float>())
            .def ("getRelativePoint", &T::getRelativePoint)
            .def ("transformedBy", &T::transformedBy)
            .def ("getBoundingBox", &T::getBoundingBox)
            .def_readwrite("topLeft", &T::topLeft)
            .def_readwrite("topRight", &T::topRight)
            .def_readwrite("bottomLeft", &T::bottomLeft)
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.topLeft.getX() << ", " << self.topLeft.getY() << ", "
                        << self.topRight.getX() << ", " << self.topRight.getY() << ", "
                        << self.bottomLeft.getX() << ", " << self.bottomLeft.getY() << ")";
                return result;
            })
        //.def ("__str__", &T::toString)
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("Parallelogram", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerBorderSize (py::module_& m)
{
    py::dict type;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        const auto className = popsicle::Helpers::pythonizeCompoundClassName ("BorderSize", typeid (Types).name());

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
            .def ("__repr__", [](const T& self)
            {
                String result;
                result
                    << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                    << "(" << self.getTop() << ", " << self.getLeft() << ", " << self.getBottom() << ", " << self.getRight() << ")";
                return result;
            })
        ;

        type[py::type::of (py::cast (Types{}))] = class_;

        return true;
    }() && ...);

    m.add_object ("BorderSize", type);
}

// ============================================================================================

void registerJuceGraphicsBindings (py::module_& m)
{
    // ============================================================================================ juce::Justification

    py::class_<Justification> classJustification (m, "Justification");

    Helpers::makeArithmeticEnum<Justification::Flags> (classJustification, "Flags")
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
        .def (py::init<int>())
        .def (py::init ([](Justification::Flags flags) { return Justification (static_cast<int> (flags)); }))
        .def (py::init<const Justification&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self == Justification::Flags())
        .def (py::self != Justification::Flags())
        .def (py::self == int())
        .def (py::self != int())
        .def ("getFlags", &Justification::getFlags)
        .def ("testFlags", &Justification::testFlags)
        .def ("testFlags", [](const Justification& self, Justification::Flags flags) { return self.testFlags (static_cast<int> (flags)); })
        .def ("getOnlyVerticalFlags", &Justification::getOnlyVerticalFlags)
        .def ("getOnlyHorizontalFlags", &Justification::getOnlyHorizontalFlags)
    //.def ("applyToRectangle", &Justification::template applyToRectangle<int>)
    //.def ("applyToRectangle", &Justification::template applyToRectangle<float>)
        .def ("appliedToRectangle", &Justification::template appliedToRectangle<int>)
        .def ("appliedToRectangle", &Justification::template appliedToRectangle<float>)
    ;

    //py::implicitly_convertible<Justification::Flags, Justification>();

    // ============================================================================================ juce::AffineTransform

    py::class_<AffineTransform> classAffineTransform (m, "AffineTransform");

    classAffineTransform
        .def (py::init<>())
        .def (py::init<float, float, float, float, float, float>())
        .def (py::init<const AffineTransform&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("transformPoint", [](const AffineTransform& self, int x, int y) { self.transformPoint (x, y); return py::make_tuple(x, y); })
        .def ("transformPoint", [](const AffineTransform& self, float x, float y) { self.transformPoint (x, y); return py::make_tuple(x, y); })
        .def ("transformPoints", [](const AffineTransform& self, int x1, int y1, int x2, int y2) { self.transformPoints (x1, y1, x2, y2); return py::make_tuple(x1, y1, x2, y2); })
        .def ("transformPoints", [](const AffineTransform& self, float x1, float y1, float x2, float y2) { self.transformPoints (x1, y1, x2, y2); return py::make_tuple(x1, y1, x2, y2); })
        .def ("transformPoints", [](const AffineTransform& self, int x1, int y1, int x2, int y2, int x3, int y3) { self.transformPoints (x1, y1, x2, y2, x3, y3); return py::make_tuple(x1, y1, x2, y2, x3, y3); })
        .def ("transformPoints", [](const AffineTransform& self, float x1, float y1, float x2, float y2, float x3, float y3) { self.transformPoints (x1, y1, x2, y2, x3, y3); return py::make_tuple(x1, y1, x2, y2, x3, y3); })
        .def ("translated", static_cast<AffineTransform (AffineTransform::*)(float, float) const> (&AffineTransform::translated))
        .def ("translated", &AffineTransform::template translated<Point<int>>)
        .def ("translated", &AffineTransform::template translated<Point<float>>)
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
        .def_static ("fromTargetPoints", [](float x00, float y00, float x10, float y10, float x01, float y01)
        {
            return AffineTransform::fromTargetPoints (x00, y00, x10, y10, x01, y01);
        })
        .def_static ("fromTargetPoints", [](float sourceX1, float sourceY1, float targetX1, float targetY1,
                                            float sourceX2, float sourceY2, float targetX2, float targetY2,
                                            float sourceX3, float sourceY3, float targetX3, float targetY3)
        {
            return AffineTransform::fromTargetPoints (sourceX1, sourceY1, targetX1, targetY1,
                                                      sourceX2, sourceY2, targetX2, targetY2,
                                                      sourceX3, sourceY3, targetX3, targetY3);
        })
        .def_static ("fromTargetPoints", &AffineTransform::template fromTargetPoints<Point<int>>)
        .def_static ("fromTargetPoints", &AffineTransform::template fromTargetPoints<Point<float>>)
        .def ("followedBy", &AffineTransform::followedBy)
        .def ("isIdentity", &AffineTransform::isIdentity)
        .def ("isSingularity", &AffineTransform::isSingularity)
        .def ("isOnlyTranslation", &AffineTransform::isOnlyTranslation)
        .def ("getTranslationX", &AffineTransform::getTranslationX)
        .def ("getTranslationY", &AffineTransform::getTranslationY)
        .def ("getDeterminant", &AffineTransform::getDeterminant)
        .def_readwrite ("mat00", &AffineTransform::mat00)
        .def_readwrite ("mat01", &AffineTransform::mat01)
        .def_readwrite ("mat02", &AffineTransform::mat02)
        .def_readwrite ("mat10", &AffineTransform::mat10)
        .def_readwrite ("mat11", &AffineTransform::mat11)
        .def_readwrite ("mat12", &AffineTransform::mat12)
        .def ("__repr__", [](const AffineTransform& self)
        {
            String result;
            result
                << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                << "(" << self.mat00 << ", " << self.mat01 << ", " << self.mat02 << ", " << self.mat10 << ", " << self.mat11 << ", " << self.mat12 << ")";
            return result;
        })
    ;

    // ============================================================================================ juce::Point<>

    registerPoint<Point, int, float> (m);

    // ============================================================================================ juce::Line<>

    registerLine<Line, int, float> (m);

    // ============================================================================================ juce::Rectangle<>

    registerRectangle<Rectangle, int, float> (m);

    // ============================================================================================ juce::RectangleList<>

    registerRectangleList<RectangleList, int, float> (m);

    // ============================================================================================ juce::RectangleList<>

    registerParallelogram<Parallelogram, int, float> (m);

    // ============================================================================================ juce::BorderSize<>

    registerBorderSize<BorderSize, int, float> (m);

    // ============================================================================================ juce::Path

    py::class_<Path> classPath (m, "Path");

    py::class_<Path::Iterator> classPathIterator (classPath, "Iterator");

    py::enum_<Path::Iterator::PathElementType> (classPathIterator, "PathElementType")
        .value ("startNewSubPath", Path::Iterator::PathElementType::startNewSubPath)
        .value ("lineTo", Path::Iterator::PathElementType::lineTo)
        .value ("quadraticTo", Path::Iterator::PathElementType::quadraticTo)
        .value ("cubicTo", Path::Iterator::PathElementType::cubicTo)
        .value ("closePath", Path::Iterator::PathElementType::closePath)
        .export_values();

    classPathIterator
        .def (py::init<const Path&>())
        .def ("next", &Path::Iterator::next)
        .def_readwrite ("elementType", &Path::Iterator::elementType)
        .def_readwrite ("x1", &Path::Iterator::x1)
        .def_readwrite ("y1", &Path::Iterator::y1)
        .def_readwrite ("x2", &Path::Iterator::x2)
        .def_readwrite ("y2", &Path::Iterator::y2)
        .def_readwrite ("x3", &Path::Iterator::x3)
        .def_readwrite ("y3", &Path::Iterator::y3)
    ;

    classPath
        .def (py::init<>())
        .def (py::init ([](py::str data) { Path result; result.restoreFromString (data.cast<std::string>().c_str()); return result; }))
        .def (py::init<const Path&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isEmpty", &Path::isEmpty)
        .def ("getBounds", &Path::getBounds)
        .def ("getBoundsTransformed", &Path::getBoundsTransformed, "transform"_a)
        .def ("contains", py::overload_cast<float, float, float> (&Path::contains, py::const_), "x"_a, "y"_a, "tolerance"_a = Path::defaultToleranceForTesting)
        .def ("contains", py::overload_cast<Point<float>, float> (&Path::contains, py::const_), "point"_a, "tolerance"_a = Path::defaultToleranceForTesting)
        .def ("intersectsLine", &Path::intersectsLine, "line"_a, "tolerance"_a = Path::defaultToleranceForTesting)
        .def ("getClippedLine", &Path::getClippedLine, "line"_a, "keepSectionOutsidePath"_a)
        .def ("getLength", &Path::getLength, "transform"_a = AffineTransform(), "tolerance"_a = Path::defaultToleranceForMeasurement)
        .def ("getPointAlongPath", &Path::getPointAlongPath, "distanceFromStart"_a, "transform"_a = AffineTransform(), "tolerance"_a = Path::defaultToleranceForMeasurement)
        .def ("getNearestPoint", &Path::getNearestPoint, "targetPoint"_a, "pointOnPath"_a, "transform"_a = AffineTransform(), "tolerance"_a = Path::defaultToleranceForMeasurement)
        .def ("clear", &Path::clear)
        .def ("startNewSubPath", py::overload_cast<float, float> (&Path::startNewSubPath), "startX"_a, "startY"_a)
        .def ("startNewSubPath", py::overload_cast<Point<float>> (&Path::startNewSubPath), "start"_a)
        .def ("closeSubPath", &Path::closeSubPath)
        .def ("lineTo", py::overload_cast<float, float> (&Path::lineTo), "endX"_a, "endY"_a)
        .def ("lineTo", py::overload_cast<Point<float>> (&Path::lineTo), "end"_a)
        .def ("quadraticTo", py::overload_cast<float, float, float, float> (&Path::quadraticTo), "controlPointX"_a, "controlPointY"_a, "endPointX"_a, "endPointX"_a)
        .def ("quadraticTo", py::overload_cast<Point<float>, Point<float>> (&Path::quadraticTo), "controlPoint"_a, "endPoint"_a)
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
        .def ("addArc", &Path::addArc,
            "x"_a, "y"_a, "width"_a, "height"_a, "fromRadians"_a, "toRadians"_a, "startAsNewSubPath"_a = false)
        .def ("addCentredArc", &Path::addCentredArc,
            "centreX"_a, "centreY"_a, "radiusX"_a, "radiusY"_a, "rotationOfEllipse"_a, "fromRadians"_a, "toRadians"_a, "startAsNewSubPath"_a = false)
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
        .def ("loadPathFromData", [](Path& self, py::buffer data)
        {
            auto info = data.request();
            self.loadPathFromData (info.ptr, static_cast<size_t> (info.size));
        })
        .def ("writePathToStream", &Path::writePathToStream)
        .def ("toString", &Path::toString)
        .def ("restoreFromString", &Path::restoreFromString)
        .def ("__repr__", [](const Path& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.toString() << "')";
            return result;
        })
        .def ("__str__", &Path::toString)
    ;

    classPath.attr ("defaultToleranceForTesting") = py::float_ (Path::defaultToleranceForTesting);
    classPath.attr ("defaultToleranceForMeasurement") = py::float_ (Path::defaultToleranceForMeasurement);

    // ============================================================================================ juce::PathFlatteningIterator

    py::class_<PathFlatteningIterator> classPathFlatteningIterator (m, "PathFlatteningIterator");

    classPathFlatteningIterator
        .def (py::init<const Path&, const AffineTransform&, float>(), "path"_a, "transform"_a = AffineTransform(), "tolerance"_a = Path::defaultToleranceForMeasurement)
        .def ("next", &PathFlatteningIterator::next)
        .def ("isLastInSubpath", &PathFlatteningIterator::isLastInSubpath)
        .def_readwrite ("x1", &PathFlatteningIterator::x1)
        .def_readwrite ("y1", &PathFlatteningIterator::y1)
        .def_readwrite ("x2", &PathFlatteningIterator::x2)
        .def_readwrite ("y2", &PathFlatteningIterator::y2)
        .def_readwrite ("closesSubPath", &PathFlatteningIterator::closesSubPath)
        .def_readwrite ("subPathIndex", &PathFlatteningIterator::subPathIndex)
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
        .def (py::init<float>(), "strokeThickness"_a)
        .def (py::init<float, PathStrokeType::JointStyle, PathStrokeType::EndCapStyle>(), "strokeThickness"_a, "jointStyle"_a, "endStyle"_a = PathStrokeType::butt)
        .def (py::init<const PathStrokeType&>())
        .def ("createStrokedPath", &PathStrokeType::createStrokedPath,
            "destPath"_a, "sourcePath"_a, "transform"_a = AffineTransform(), "extraAccuracy"_a = 1.0f)
        .def ("createDashedStroke", [](const PathStrokeType& self, Path* destPath, const Path& sourcePath, py::list dashLengths, const AffineTransform& transform, float extraAccuracy)
        {
            std::vector<float> dashes;
            dashes.reserve (static_cast<size_t> (dashLengths.size()));

            for (const auto& item : dashLengths)
                dashes.push_back (item.cast<float>());

            self.createDashedStroke (*destPath, sourcePath, dashes.data(), static_cast<int> (dashes.size()), transform, extraAccuracy);
        }, "destPath"_a, "sourcePath"_a, "dashLengths"_a, "transform"_a = AffineTransform(), "extraAccuracy"_a = 1.0f)
        .def ("createStrokeWithArrowheads", &PathStrokeType::createStrokeWithArrowheads,
            "destPath"_a, "sourcePath"_a, "arrowheadStartWidth"_a, "arrowheadStartLength"_a, "arrowheadEndWidth"_a, "arrowheadEndLength"_a, "transform"_a = AffineTransform(), "extraAccuracy"_a = 1.0f)
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

    py::class_<PixelARGB> classPixelARGB (m, "PixelARGB");

    classPixelARGB
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
    ;

    classPixelARGB.attr ("indexA") = py::int_ (static_cast<int> (PixelARGB::indexA));
    classPixelARGB.attr ("indexR") = py::int_ (static_cast<int> (PixelARGB::indexR));
    classPixelARGB.attr ("indexG") = py::int_ (static_cast<int> (PixelARGB::indexG));
    classPixelARGB.attr ("indexB") = py::int_ (static_cast<int> (PixelARGB::indexB));

    py::class_<PixelRGB> classPixelRGB (m, "PixelRGB");

    classPixelRGB
        .def (py::init<>())
        .def (py::init ([](uint8 red, uint8 green, uint8 blue)
        {
            auto c = PixelRGB();
            c.setARGB (255, red, green, blue);
            return c;
        }))
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
    ;

    classPixelRGB.attr ("indexR") = py::int_ (static_cast<int> (PixelRGB::indexR));
    classPixelRGB.attr ("indexG") = py::int_ (static_cast<int> (PixelRGB::indexG));
    classPixelRGB.attr ("indexB") = py::int_ (static_cast<int> (PixelRGB::indexB));

    py::class_<PixelAlpha> classPixelAlpha (m, "PixelAlpha");

    classPixelAlpha
        .def (py::init<>())
        .def (py::init ([](uint8 alpha) { auto c = PixelAlpha(); c.setAlpha (alpha); return c; }))
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
    ;

    classPixelAlpha.attr ("indexA") = py::int_ (static_cast<int> (PixelAlpha::indexA));

    // ============================================================================================ juce::Colour

    py::class_<Colour> (m, "Colour")
        .def (py::init<>())
        .def (py::init<uint32>(), "argb"_a.noconvert())
        .def (py::init<uint8, uint8, uint8>(), "red"_a.noconvert(), "green"_a.noconvert(), "blue"_a.noconvert())
        .def (py::init<uint8, uint8, uint8, uint8>(), "red"_a.noconvert(), "green"_a.noconvert(), "blue"_a.noconvert(), "alpha"_a.noconvert())
        .def (py::init<uint8, uint8, uint8, float>(), "red"_a.noconvert(), "green"_a.noconvert(), "blue"_a.noconvert(), "alpha"_a.noconvert())
        .def (py::init<float, float, float, uint8>(), "hue"_a.noconvert(), "saturation"_a.noconvert(), "brightness"_a.noconvert(), "alpha"_a.noconvert())
        .def (py::init<float, float, float, float>(), "hue"_a.noconvert(), "saturation"_a.noconvert(), "brightness"_a.noconvert(), "alpha"_a.noconvert())
        .def (py::init<PixelARGB>())
        .def (py::init<PixelRGB>())
        .def (py::init<PixelAlpha>())
        .def (py::init<const Colour&>())
        .def_static ("fromRGB", &Colour::fromRGB)
        .def_static ("fromRGBA", &Colour::fromRGBA)
        .def_static ("fromFloatRGBA", &Colour::fromFloatRGBA)
        .def_static ("fromHSV", &Colour::fromHSV)
        .def_static ("fromHSL", &Colour::fromHSL)
        .def (py::self == py::self)
        .def (py::self != py::self)
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
        .def ("withAlpha", py::overload_cast<uint8> (&Colour::withAlpha, py::const_), "alpha"_a.noconvert())
        .def ("withAlpha", py::overload_cast<float> (&Colour::withAlpha, py::const_), "alpha"_a.noconvert())
        .def ("withMultipliedAlpha", &Colour::withMultipliedAlpha)
        .def ("overlaidWith", &Colour::overlaidWith)
        .def ("interpolatedWith", &Colour::interpolatedWith)
        .def ("getHue", &Colour::getHue)
        .def ("getSaturation", &Colour::getSaturation)
        .def ("getSaturationHSL", &Colour::getSaturationHSL)
        .def ("getBrightness", &Colour::getBrightness)
        .def ("getLightness", &Colour::getLightness)
        .def ("getPerceivedBrightness", &Colour::getPerceivedBrightness)
        .def ("getHSB", [](const Colour& self) { float h, s, b; self.getHSB (h, s, b); return py::make_tuple(h, s, b); })
        .def ("getHSL", [](const Colour& self) { float h, s, l; self.getHSL (h, s, l); return py::make_tuple(h, s, l); })
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
        .def ("__repr__", [](const Colour& self)
        {
            String result;
            result
                << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name())
                << "(" << self.getRed() << ", " << self.getGreen() << ", " << self.getBlue() << ", " << self.getAlpha() << ")";
            return result;
        })
        .def ("__str__", &Colour::toString)
    ;

    // ============================================================================================ juce::Colour

    py::class_<ColourGradient> (m, "ColourGradient")
        .def (py::init<>())
        .def (py::init<Colour, float, float, Colour, float, float, bool>(),
            "colour1"_a, "x1"_a, "y1"_a, "colour2"_a, "x2"_a, "y2"_a, "isRadial"_a)
        .def (py::init<Colour, Point<float>, Colour, Point<float>, bool>(),
            "colour1"_a, "point1"_a, "colour2"_a, "point2"_a, "isRadial"_a)
        .def (py::init<const ColourGradient&>())
        .def_static ("vertical", [](Colour c1, float y1, Colour c2, float y2) { return ColourGradient::vertical (c1, y1, c2, y2); },
            "colour1"_a, "y1"_a, "colour2"_a, "y2"_a)
        .def_static ("vertical", [](Colour top, Colour bottom, Rectangle<int> area) { return ColourGradient::vertical (top, bottom, area); },
            "colourTop"_a, "colourBottom"_a, "area"_a)
        .def_static ("vertical", [](Colour top, Colour bottom, Rectangle<float> area) { return ColourGradient::vertical (top, bottom, area); },
            "colourTop"_a, "colourBottom"_a, "area"_a)
        .def_static ("horizontal", [](Colour c1, float x1, Colour c2, float x2) { return ColourGradient::horizontal (c1, x1, c2, x2); },
            "colour1"_a, "x1"_a, "colour2"_a, "x2"_a)
        .def_static ("horizontal", [](Colour left, Colour right, Rectangle<int> area) { return ColourGradient::horizontal (left, right, area); },
            "colourLeft"_a, "colourRight"_a, "area"_a)
        .def_static ("horizontal", [](Colour left, Colour right, Rectangle<float> area) { return ColourGradient::horizontal (left, right, area); },
            "colourLeft"_a, "colourRight"_a, "area"_a)
        .def ("clearColours", &ColourGradient::clearColours)
        .def ("addColour", &ColourGradient::addColour, "proportionAlongGradient"_a, "colour"_a)
        .def ("removeColour", &ColourGradient::removeColour, "index"_a)
        .def ("multiplyOpacity", &ColourGradient::multiplyOpacity, "multiplier"_a)
        .def ("getNumColours", &ColourGradient::getNumColours)
        .def ("getColourPosition", &ColourGradient::getColourPosition, "index"_a)
        .def ("getColour", &ColourGradient::getColour, "index"_a)
        .def ("setColour", &ColourGradient::setColour, "index"_a, "colour"_a)
        .def ("getColourAtPosition", &ColourGradient::getColourAtPosition, "position"_a)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
        .def ("isOpaque", &ColourGradient::isOpaque)
        .def ("isInvisible", &ColourGradient::isInvisible)
        .def_readwrite ("point1", &ColourGradient::point1)
        .def_readwrite ("point2", &ColourGradient::point2)
        .def_readwrite ("isRadial", &ColourGradient::isRadial)
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

    py::class_<Image::BitmapData> classImageBitmapData (classImage, "BitmapData", py::buffer_protocol());

    py::enum_<Image::BitmapData::ReadWriteMode> (classImageBitmapData, "ReadWriteMode")
        .value ("readOnly", Image::BitmapData::ReadWriteMode::readOnly)
        .value ("writeOnly", Image::BitmapData::ReadWriteMode::writeOnly)
        .value ("readWrite", Image::BitmapData::ReadWriteMode::readWrite)
        .export_values();

    classImageBitmapData
        .def (py::init<Image&, int, int, int, int, Image::BitmapData::ReadWriteMode>())
        .def (py::init<const Image&, int, int, int, int>())
        .def (py::init<const Image&, Image::BitmapData::ReadWriteMode>())
        .def ("getLinePointer", [](const Image::BitmapData& self, int y)
            { return py::memoryview::from_memory (self.getLinePointer(y), static_cast<Py_ssize_t> (self.size) - y * self.lineStride); })
        .def ("getPixelPointer", [](const Image::BitmapData& self, int x, int y)
            { return py::memoryview::from_memory (self.getPixelPointer(x, y), static_cast<Py_ssize_t> (self.size) - (y * self.lineStride + x * self.pixelStride)); })
        .def ("getPixelColour", &Image::BitmapData::getPixelColour)
        .def ("setPixelColour", &Image::BitmapData::setPixelColour)
        .def ("getBounds", &Image::BitmapData::getBounds)
        .def_property ("data",
            [](const Image::BitmapData& self)
                { return py::memoryview::from_memory (self.data, static_cast<Py_ssize_t> (self.size)); },
            [](Image::BitmapData& self, py::buffer data)
                { auto info = data.request(); std::memcpy (self.data, info.ptr, static_cast<size_t> (std::min (info.size, static_cast<Py_ssize_t> (self.size)))); })
        .def_readwrite ("size", &Image::BitmapData::size)
        .def_readwrite ("pixelFormat", &Image::BitmapData::pixelFormat)
        .def_readwrite ("lineStride", &Image::BitmapData::lineStride)
        .def_readwrite ("pixelStride", &Image::BitmapData::pixelStride)
        .def_readwrite ("width", &Image::BitmapData::width)
        .def_readwrite ("height", &Image::BitmapData::height)
        .def_buffer ([](Image::BitmapData& self)
        {
            return py::buffer_info
            (
                self.data,
                sizeof (unsigned char),
                py::format_descriptor<unsigned char>::format(),
                self.pixelStride,
                {
                    self.height,
                    self.width,
                    self.pixelStride
                },
                {
                    sizeof (unsigned char) * static_cast<size_t> (self.pixelStride) * static_cast<size_t> (self.width),
                    sizeof (unsigned char) * static_cast<size_t> (self.pixelStride),
                    sizeof (unsigned char)
                }
            );
        });
    ;

    classImage
        .def (py::init<>())
        .def (py::init<Image::PixelFormat, int, int, bool>())
        .def (py::init<Image::PixelFormat, int, int, bool, const ImageType&>())
        .def (py::init<const Image&>())
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
        .def ("getProperties", &Image::getProperties, py::return_value_policy::reference_internal)
    //.def ("createLowLevelContext", &Image::createLowLevelContext)
        .def ("getReferenceCount", &Image::getReferenceCount)
        .def ("getPixelData", &Image::getPixelData, py::return_value_policy::reference_internal)
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

    // ============================================================================================ juce::ImageType

    py::class_<ImageType, PyImageType> classImageType (m, "ImageType");

    classImageType
        .def (py::init<>())
        .def ("create", &ImageType::create)
        .def ("getTypeID", &ImageType::getTypeID)
        .def ("convert", &ImageType::convert)
    ;

    py::class_<SoftwareImageType, ImageType> classSoftwareImageType (m, "SoftwareImageType");

    classSoftwareImageType
        .def (py::init<>())
    ;

    py::class_<NativeImageType, ImageType> classNativeImageType (m, "NativeImageType");

    classNativeImageType
        .def (py::init<>())
    ;

    // ============================================================================================ juce::ImageCache

    py::class_<ImageCache, std::unique_ptr<ImageCache, py::nodelete>> classImageCache (m, "ImageCache");

    classImageCache
        .def_static ("getFromFile", &ImageCache::getFromFile)
        .def_static ("getFromMemory", [](py::buffer data)
        {
            auto info = data.request();
            return ImageCache::getFromMemory (info.ptr, static_cast<int> (info.size));
        })
        .def_static ("getFromHashCode", &ImageCache::getFromHashCode)
        .def_static ("addImageToCache", &ImageCache::addImageToCache)
        .def_static ("setCacheTimeout", &ImageCache::setCacheTimeout)
        .def_static ("releaseUnusedImages", &ImageCache::releaseUnusedImages)
    ;

    // ============================================================================================ juce::ImageCache

    py::class_<ImageFileFormat, PyImageFileFormat> classImageFileFormat (m, "ImageFileFormat");

    classImageFileFormat
        .def (py::init<>())
        .def ("getFormatName", &ImageFileFormat::getFormatName)
        .def ("canUnderstand", &ImageFileFormat::canUnderstand)
        .def ("usesFileExtension", &ImageFileFormat::usesFileExtension)
        .def ("decodeImage", &ImageFileFormat::decodeImage)
        .def ("writeImageToStream", &ImageFileFormat::writeImageToStream)
        .def_static ("findImageFormatForStream", &ImageFileFormat::findImageFormatForStream, py::return_value_policy::reference_internal)
        .def_static ("findImageFormatForFileExtension", &ImageFileFormat::findImageFormatForFileExtension, py::return_value_policy::reference_internal)
        .def_static ("loadFrom", static_cast<Image (*)(InputStream&)> (&ImageFileFormat::loadFrom))
        .def_static ("loadFrom", static_cast<Image (*)(const File&)> (&ImageFileFormat::loadFrom))
        .def_static ("loadFrom", [](py::buffer data)
        {
            auto info = data.request();
            return ImageFileFormat::loadFrom (info.ptr, static_cast<size_t> (info.size));
        })
    ;

    py::class_<PNGImageFormat, ImageFileFormat> classPNGImageFormat (m, "PNGImageFormat");
    classPNGImageFormat
        .def (py::init<>());

    py::class_<JPEGImageFormat, ImageFileFormat> classJPEGImageFormat (m, "JPEGImageFormat");
    classJPEGImageFormat
        .def (py::init<>());

    py::class_<GIFImageFormat, ImageFileFormat> classGIFImageFormat (m, "GIFImageFormat");
    classGIFImageFormat
        .def (py::init<>());

    // ============================================================================================ juce::ScaledImage

    py::class_<ScaledImage> classScaledImage (m, "ScaledImage");

    classScaledImage
        .def (py::init<>())
        .def (py::init<const Image&>())
        .def (py::init<const Image&, double>())
        .def (py::init<const ScaledImage&>())
        .def ("getImage", &ScaledImage::getImage)
        .def ("getScale", &ScaledImage::getScale)
        .def ("getScaledBounds", &ScaledImage::getScaledBounds)
    ;

    // ============================================================================================ juce::ScaledImage

    py::class_<ImageConvolutionKernel> classImageConvolutionKernel (m, "ImageConvolutionKernel");

    classImageConvolutionKernel
        .def (py::init<int>(), "size"_a)
        .def ("clear", &ImageConvolutionKernel::clear)
        .def ("getKernelValue", &ImageConvolutionKernel::getKernelValue)
        .def ("setKernelValue", &ImageConvolutionKernel::setKernelValue)
        .def ("setOverallSum", &ImageConvolutionKernel::setOverallSum)
        .def ("rescaleAllValues", &ImageConvolutionKernel::rescaleAllValues)
        .def ("createGaussianBlur", &ImageConvolutionKernel::createGaussianBlur)
        .def ("getKernelSize", &ImageConvolutionKernel::getKernelSize)
        .def ("applyToImage", &ImageConvolutionKernel::applyToImage)
    ;

    // ============================================================================================ juce::Font

    py::class_<Font> classFont (m, "Font");

    Helpers::makeArithmeticEnum<Font::FontStyleFlags> (classFont, "FontStyleFlags")
        .value ("plain", Font::FontStyleFlags::plain)
        .value ("bold", Font::FontStyleFlags::bold)
        .value ("italic", Font::FontStyleFlags::italic)
        .value ("underlined", Font::FontStyleFlags::underlined)
        .export_values();

    classFont
        .def (py::init<>())
        .def (py::init<float, int>(), "fontHeight"_a, "styleFlags"_a = Font::plain)
        .def (py::init<float, Font::FontStyleFlags>(), "fontHeight"_a, "styleFlags"_a)
        .def (py::init<const String&, float, Font::FontStyleFlags>())
        .def (py::init<const String&, const String&, float>())
        .def (py::init<const Font&>())
    //.def (py::init<const Typeface::Ptr&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("setTypefaceName", &Font::setTypefaceName)
        .def ("getTypefaceName", &Font::getTypefaceName)
        .def ("getTypefaceStyle", &Font::getTypefaceStyle)
        .def ("setTypefaceStyle", &Font::setTypefaceStyle)
        .def ("withTypefaceStyle", &Font::withTypefaceStyle)
        .def_static ("getDefaultSansSerifFontName", &Font::getDefaultSansSerifFontName)
        .def_static ("getDefaultSerifFontName", &Font::getDefaultSerifFontName)
        .def_static ("getDefaultMonospacedFontName", &Font::getDefaultMonospacedFontName)
        .def_static ("getDefaultStyle", &Font::getDefaultStyle)
        .def ("withHeight", &Font::withHeight)
        .def ("withPointHeight", &Font::withPointHeight)
        .def ("setHeight", &Font::setHeight)
        .def ("setHeightWithoutChangingWidth", &Font::setHeightWithoutChangingWidth)
        .def ("getHeight", &Font::getHeight)
        .def ("getHeightInPoints", &Font::getHeightInPoints)
        .def ("getAscent", &Font::getAscent)
        .def ("getAscentInPoints", &Font::getAscentInPoints)
        .def ("getDescent", &Font::getDescent)
        .def ("getDescentInPoints", &Font::getDescentInPoints)
        .def ("getStyleFlags", &Font::getStyleFlags)
        .def ("withStyle", &Font::withStyle)
        .def ("withStyle", [](const Font& self, Font::FontStyleFlags flags) { return self.withStyle (static_cast<int> (flags)); })
        .def ("setStyleFlags", &Font::setStyleFlags)
        .def ("setStyleFlags", [](Font& self, Font::FontStyleFlags flags) { self.setStyleFlags (static_cast<int> (flags)); })
        .def ("setBold", &Font::setBold)
        .def ("boldened", &Font::boldened)
        .def ("isBold", &Font::isBold)
        .def ("setItalic", &Font::setItalic)
        .def ("italicised", &Font::italicised)
        .def ("isItalic", &Font::isItalic)
        .def ("setUnderline", &Font::setUnderline)
        .def ("isUnderlined", &Font::isUnderlined)
        .def ("getHorizontalScale", &Font::getHorizontalScale)
        .def ("withHorizontalScale", &Font::withHorizontalScale)
        .def ("setHorizontalScale", &Font::setHorizontalScale)
        .def_static ("getDefaultMinimumHorizontalScaleFactor", &Font::getDefaultMinimumHorizontalScaleFactor)
        .def_static ("setDefaultMinimumHorizontalScaleFactor", &Font::setDefaultMinimumHorizontalScaleFactor)
        .def ("getExtraKerningFactor", &Font::getExtraKerningFactor)
        .def ("withExtraKerningFactor", &Font::withExtraKerningFactor)
        .def ("setExtraKerningFactor", &Font::setExtraKerningFactor)
        .def ("setSizeAndStyle", py::overload_cast<float, int, float, float> (&Font::setSizeAndStyle))
        .def ("setSizeAndStyle", [](Font& self, float newHeight, Font::FontStyleFlags newStyleFlags, float newHorizontalScale, float newKerningAmount)
        {
            self.setSizeAndStyle (newHeight, static_cast<int> (newStyleFlags), newHorizontalScale, newKerningAmount);
        })
        .def ("setSizeAndStyle", py::overload_cast<float, const String&, float, float> (&Font::setSizeAndStyle))
        .def ("getStringWidth", &Font::getStringWidth)
        .def ("getStringWidthFloat", &Font::getStringWidthFloat)
    //.def ("getGlyphPositions", &Font::getGlyphPositions)
    //.def ("getTypefacePtr", &Font::getTypefacePtr)
    //.def_static ("findFonts", &Font::findFonts)
        .def_static ("findAllTypefaceNames", &Font::findAllTypefaceNames)
        .def_static ("findAllTypefaceStyles", &Font::findAllTypefaceStyles)
        .def_static ("getFallbackFontName", &Font::getFallbackFontName)
        .def_static ("setFallbackFontName", &Font::setFallbackFontName)
        .def_static ("getFallbackFontStyle", &Font::getFallbackFontStyle)
        .def_static ("setFallbackFontStyle", &Font::setFallbackFontStyle)
        .def ("toString", &Font::toString)
        .def_static ("fromString", &Font::fromString)
        .def ("__repr__", [](const Font& self)
        {
            String result;
            result << Helpers::pythonizeModuleClassName (PythonModuleName, typeid (self).name()) << "('" << self.toString() << ")";
            return result;
        })
        .def ("__str__", &Font::toString)
    ;

    // ============================================================================================ juce::AttributedString

    py::class_<AttributedString> classAttributedString (m, "AttributedString");

    py::enum_<AttributedString::WordWrap> (classAttributedString, "WordWrap")
        .value("none", AttributedString::WordWrap::none)
        .value("byWord", AttributedString::WordWrap::byWord)
        .value("byChar", AttributedString::WordWrap::byChar)
        .export_values();

    py::enum_<AttributedString::ReadingDirection> (classAttributedString, "ReadingDirection")
        .value("natural", AttributedString::ReadingDirection::natural)
        .value("leftToRight", AttributedString::ReadingDirection::leftToRight)
        .value("rightToLeft", AttributedString::ReadingDirection::rightToLeft)
        .export_values();

    py::class_<AttributedString::Attribute> (classAttributedString, "Attribute")
        .def (py::init<>())
        .def (py::init<Range<int>, const Font&, Colour>())
        .def (py::init<const AttributedString::Attribute&>())
        .def_readwrite ("range", &AttributedString::Attribute::range)
        .def_readwrite ("font", &AttributedString::Attribute::font)
        .def_readwrite ("colour", &AttributedString::Attribute::colour)
    ;

    classAttributedString
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const AttributedString&>())
        .def ("getText", &AttributedString::getText)
        .def ("setText", &AttributedString::setText)
        .def ("append", py::overload_cast<const String&> (&AttributedString::append))
        .def ("append", py::overload_cast<const String&, const Font&> (&AttributedString::append))
        .def ("append", py::overload_cast<const String&, Colour> (&AttributedString::append))
        .def ("append", py::overload_cast<const String&, const Font&, Colour> (&AttributedString::append))
        .def ("append", py::overload_cast<const AttributedString&> (&AttributedString::append))
        .def ("clear", &AttributedString::clear)
        .def ("draw", &AttributedString::draw)
        .def ("getJustification", &AttributedString::getJustification)
        .def ("getWordWrap", &AttributedString::getWordWrap)
        .def ("setWordWrap", &AttributedString::setWordWrap)
        .def ("getReadingDirection", &AttributedString::getReadingDirection)
        .def ("setReadingDirection", &AttributedString::setReadingDirection)
        .def ("getLineSpacing", &AttributedString::getLineSpacing)
        .def ("setLineSpacing", &AttributedString::setLineSpacing)
        .def ("getNumAttributes", &AttributedString::getNumAttributes)
        .def ("getAttribute", &AttributedString::getAttribute, py::return_value_policy::reference)
        .def ("setColour", py::overload_cast<Range<int>, Colour> (&AttributedString::setColour))
        .def ("setColour", py::overload_cast<Colour> (&AttributedString::setColour))
        .def ("setFont", py::overload_cast<Range<int>, const Font&> (&AttributedString::setFont))
        .def ("setFont", py::overload_cast<const Font&> (&AttributedString::setFont))
    ;

    // ============================================================================================ juce::FillType

    py::class_<FillType> classFillType (m, "FillType");

    classFillType
        .def (py::init<>())
        .def (py::init<Colour>())
        .def (py::init<const ColourGradient&>())
        .def (py::init<const Image&, const AffineTransform&>())
        .def (py::init<const FillType&>())
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
                      [](FillType& self, ColourGradient* v) { self.gradient.reset(); if (v != nullptr) self.gradient = std::make_unique<ColourGradient>(*v); },
                      py::return_value_policy::reference_internal)
        .def_readwrite("image", &FillType::image)
        .def_readwrite("transform", &FillType::transform)
    ;

    // ============================================================================================ juce::RectanglePlacement

    py::class_<RectanglePlacement> classRectanglePlacement (m, "RectanglePlacement");

    Helpers::makeArithmeticEnum<RectanglePlacement::Flags> (classRectanglePlacement, "Flags")
        .value ("xLeft", RectanglePlacement::Flags::xLeft)
        .value ("xRight", RectanglePlacement::Flags::xRight)
        .value ("xMid", RectanglePlacement::Flags::xMid)
        .value ("yTop", RectanglePlacement::Flags::yTop)
        .value ("yBottom", RectanglePlacement::Flags::yBottom)
        .value ("yMid", RectanglePlacement::Flags::yMid)
        .value ("stretchToFit", RectanglePlacement::Flags::stretchToFit)
        .value ("fillDestination", RectanglePlacement::Flags::fillDestination)
        .value ("onlyReduceInSize", RectanglePlacement::Flags::onlyReduceInSize)
        .value ("onlyIncreaseInSize", RectanglePlacement::Flags::onlyIncreaseInSize)
        .value ("doNotResize", RectanglePlacement::Flags::doNotResize)
        .value ("centred", RectanglePlacement::Flags::centred)
        .export_values();

    classRectanglePlacement
        .def (py::init<>())
        .def (py::init<int>())
        .def (py::init<RectanglePlacement::Flags>())
        .def (py::init<const RectanglePlacement&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("getFlags", &RectanglePlacement::getFlags)
        .def ("testFlags", &RectanglePlacement::testFlags)
        .def ("testFlags", [](const RectanglePlacement& self, RectanglePlacement::Flags flags) { return self.testFlags (static_cast<int> (flags)); })
        .def ("applyTo", [](const RectanglePlacement& self, double& sourceX, double& sourceY, double& sourceW, double& sourceH, double destinationX, double destinationY, double destinationW, double destinationH)
        {
            self.applyTo (sourceX, sourceY, sourceW, sourceH, destinationX, destinationY, destinationW, destinationH);
            return py::make_tuple (sourceX, sourceY, sourceW, sourceH);
        })
        .def ("appliedTo", &RectanglePlacement::template appliedTo<int>)
        .def ("appliedTo", &RectanglePlacement::template appliedTo<float>)
        .def ("getTransformToFit", &RectanglePlacement::getTransformToFit)
    ;

    py::implicitly_convertible<RectanglePlacement::Flags, RectanglePlacement>();

    // ============================================================================================ juce::LowLevelGraphicsContext

    py::class_<LowLevelGraphicsContext, PyLowLevelGraphicsContext<>> classLowLevelGraphicsContext (m, "LowLevelGraphicsContext");

    classLowLevelGraphicsContext
        .def (py::init<>())
        // TODO
    ;

    // ============================================================================================ juce::LowLevelGraphicsSoftwareRenderer

    py::class_<LowLevelGraphicsSoftwareRenderer, LowLevelGraphicsContext, PyLowLevelGraphicsContext<LowLevelGraphicsSoftwareRenderer>>
        classLowLevelGraphicsSoftwareRenderer (m, "LowLevelGraphicsSoftwareRenderer");

    classLowLevelGraphicsSoftwareRenderer
        .def (py::init<const Image&>(), "imageToRenderOnto"_a)
        .def (py::init<const Image&, Point<int>, const RectangleList<int>&>(), "imageToRenderOnto"_a, "origin"_a, "initialClip"_a)
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
        .def (py::init<LowLevelGraphicsContext&>())
        .def ("setColour", &Graphics::setColour)
        .def ("setOpacity", &Graphics::setOpacity)
        .def ("setGradientFill", py::overload_cast<const ColourGradient&> (&Graphics::setGradientFill))
        .def ("setTiledImageFill", &Graphics::setTiledImageFill)
        .def ("setFillType", &Graphics::setFillType)
        .def ("setFont", py::overload_cast<const Font&>(&Graphics::setFont))
        .def ("setFont", py::overload_cast<float>(&Graphics::setFont))
        .def ("getCurrentFont", &Graphics::getCurrentFont)
        .def ("drawSingleLineText", &Graphics::drawSingleLineText,
            "text"_a, "startX"_a, "baselineY"_a, "justification"_a = Justification::left)
        .def ("drawSingleLineText", [](const Graphics& g, const String& text, int startX, int baselineY, Justification::Flags justification)
            { g.drawSingleLineText (text, startX, baselineY, justification); },
            "text"_a, "startX"_a, "baselineY"_a, "justification"_a = Justification::left)
        .def ("drawSingleLineText", [](const Graphics& g, const String& text, int startX, int baselineY, int justification)
            { g.drawSingleLineText (text, startX, baselineY, justification); },
            "text"_a, "startX"_a, "baselineY"_a, "justification"_a = Justification::left)
        .def ("drawMultiLineText", &Graphics::drawMultiLineText,
            "text"_a, "startX"_a, "baselineY"_a, "maximumLineWidth"_a, "justification"_a = Justification::left, "loading"_a = 0.0f)
        .def ("drawMultiLineText", [](const Graphics& g, const String& text, int startX, int baselineY, int maximumLineWidth, Justification::Flags justification, float leading)
            { g.drawMultiLineText (text, startX, baselineY, maximumLineWidth, justification, leading); },
            "text"_a, "startX"_a, "baselineY"_a, "maximumLineWidth"_a, "justification"_a = Justification::left, "loading"_a = 0.0f)
        .def ("drawMultiLineText", [](const Graphics& g, const String& text, int startX, int baselineY, int maximumLineWidth, int justification, float leading)
            { g.drawMultiLineText (text, startX, baselineY, maximumLineWidth, justification, leading); },
            "text"_a, "startX"_a, "baselineY"_a, "maximumLineWidth"_a, "justification"_a = Justification::left, "loading"_a = 0.0f)
        .def ("drawText", py::overload_cast<const String&, int, int, int, int, Justification, bool> (&Graphics::drawText, py::const_),
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, int x, int y, int width, int height, Justification::Flags justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, x, y, width, height, justificationType, useEllipsesIfTooBig); },
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, int x, int y, int width, int height, int justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, x, y, width, height, justificationType, useEllipsesIfTooBig); },
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", py::overload_cast<const String&, Rectangle<int>, Justification, bool> (&Graphics::drawText, py::const_),
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, Rectangle<int> area, Justification::Flags justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, area, justificationType, useEllipsesIfTooBig); },
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, Rectangle<int> area, int justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, area, justificationType, useEllipsesIfTooBig); },
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", py::overload_cast<const String&, Rectangle<float>, Justification, bool> (&Graphics::drawText, py::const_),
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, Rectangle<float> area, Justification::Flags justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, area, justificationType, useEllipsesIfTooBig); },
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawText", [](const Graphics& g, const String& text, Rectangle<float> area, int justificationType, bool useEllipsesIfTooBig)
            { g.drawText (text, area, justificationType, useEllipsesIfTooBig); },
            "text"_a, "area"_a, "justificationType"_a, "useEllipsesIfTooBig"_a = true)
        .def ("drawFittedText", py::overload_cast<const String&, int, int, int, int, Justification, int, float> (&Graphics::drawFittedText, py::const_),
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
        .def ("drawFittedText", [](const Graphics& g, const String& text, int x, int y, int width, int height, Justification::Flags justificationType, int maximumNumberOfLines, float minimumHorizontalScale)
            { g.drawFittedText (text, x, y, width, height, justificationType, maximumNumberOfLines, minimumHorizontalScale); },
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
        .def ("drawFittedText", [](const Graphics& g, const String& text, int x, int y, int width, int height, int justificationType, int maximumNumberOfLines, float minimumHorizontalScale)
            { g.drawFittedText (text, x, y, width, height, justificationType, maximumNumberOfLines, minimumHorizontalScale); },
            "text"_a, "x"_a, "y"_a, "width"_a, "height"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
        .def ("drawFittedText", py::overload_cast<const String&, Rectangle<int>, Justification, int, float> (&Graphics::drawFittedText, py::const_),
            "text"_a, "area"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
        .def ("drawFittedText", [](const Graphics& g, const String& text, Rectangle<int> area, Justification::Flags justificationType, int maximumNumberOfLines, float minimumHorizontalScale)
            { g.drawFittedText (text, area, justificationType, maximumNumberOfLines, minimumHorizontalScale); },
            "text"_a, "area"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
        .def ("drawFittedText", [](const Graphics& g, const String& text, Rectangle<int> area, int justificationType, int maximumNumberOfLines, float minimumHorizontalScale)
            { g.drawFittedText (text, area, justificationType, maximumNumberOfLines, minimumHorizontalScale); },
            "text"_a, "area"_a, "justificationType"_a, "maximumNumberOfLines"_a, "minimumHorizontalScale"_a = 0.0f)
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
        .def ("drawRect", py::overload_cast<int, int, int, int, int> (&Graphics::drawRect, py::const_), "x"_a, "y"_a, "width"_a, "height"_a, "lineThickness"_a = 1)
        .def ("drawRect", py::overload_cast<float, float, float, float, float> (&Graphics::drawRect, py::const_), "x"_a, "y"_a, "width"_a, "height"_a, "lineThickness"_a = 1.0f)
        .def ("drawRect", py::overload_cast<Rectangle<int>, int> (&Graphics::drawRect, py::const_), "rectangle"_a, "lineThickness"_a = 1)
        .def ("drawRect", py::overload_cast<Rectangle<float>, float> (&Graphics::drawRect, py::const_), "rectangle"_a, "lineThickness"_a = 1.0f)
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
    //.def ("drawDashedLine", &Graphics::drawDashedLine
    //, "line"_a, "dashLengths"_a, "numDashLengths"_a, "lineThickness"_a = 1.0f, "dashIndexToStartFrom"_a = 0)
        .def ("drawVerticalLine", &Graphics::drawVerticalLine)
        .def ("drawHorizontalLine", &Graphics::drawHorizontalLine)
        .def ("fillPath", py::overload_cast<const Path&> (&Graphics::fillPath, py::const_))
        .def ("fillPath", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::fillPath, py::const_))
        .def ("strokePath", &Graphics::strokePath, "path"_a, "strokeType"_a, "transform"_a = AffineTransform())
        .def ("drawArrow", &Graphics::drawArrow)
        .def ("setImageResamplingQuality", &Graphics::setImageResamplingQuality)
        .def ("drawImageAt", &Graphics::drawImageAt, "imageToDraw"_a, "topLeftX"_a, "topLeftY"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImage", py::overload_cast<const Image&, int, int, int, int, int, int, int, int, bool> (&Graphics::drawImage, py::const_),
            "imageToDraw"_a, "destX"_a, "destY"_a, "destWidth"_a, "destHeight"_a, "sourceX"_a, "sourceY"_a, "sourceWidth"_a, "sourceHeight"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImageTransformed", &Graphics::drawImageTransformed, "imageToDraw"_a, "transform"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImage", py::overload_cast<const Image&, Rectangle<float>, RectanglePlacement, bool> (&Graphics::drawImage, py::const_),
            "imageToDraw"_a, "targetArea"_a, "placementWithinTarget"_a = RectanglePlacement::stretchToFit, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImage", [](const Graphics& g, const Image& imageToDraw, Rectangle<float> targetArea, RectanglePlacement::Flags placementWithinTarget, bool fillAlphaChannelWithCurrentBrush)
            { g.drawImage (imageToDraw, targetArea, placementWithinTarget, fillAlphaChannelWithCurrentBrush); },
            "imageToDraw"_a, "targetArea"_a, "placementWithinTarget"_a = RectanglePlacement::stretchToFit, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImage", [](const Graphics& g, const Image& imageToDraw, Rectangle<float> targetArea, int placementWithinTarget, bool fillAlphaChannelWithCurrentBrush)
            { g.drawImage (imageToDraw, targetArea, placementWithinTarget, fillAlphaChannelWithCurrentBrush); },
            "imageToDraw"_a, "targetArea"_a, "placementWithinTarget"_a = RectanglePlacement::stretchToFit, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImageWithin", &Graphics::drawImageWithin,
            "imageToDraw"_a, "destX"_a, "destY"_a, "destWidth"_a, "destHeight"_a, "placementWithinTarget"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImageWithin", [](const Graphics& g, const Image& imageToDraw, float destX, float destY, float destWidth, float destHeight, RectanglePlacement::Flags placementWithinTarget, bool fillAlphaChannelWithCurrentBrush)
            { g.drawImageWithin (imageToDraw, destX, destY, destWidth, destHeight, placementWithinTarget, fillAlphaChannelWithCurrentBrush); },
            "imageToDraw"_a, "destX"_a, "destY"_a, "destWidth"_a, "destHeight"_a, "placementWithinTarget"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("drawImageWithin", [](const Graphics& g, const Image& imageToDraw, float destX, float destY, float destWidth, float destHeight, int placementWithinTarget, bool fillAlphaChannelWithCurrentBrush)
            { g.drawImageWithin (imageToDraw, destX, destY, destWidth, destHeight, placementWithinTarget, fillAlphaChannelWithCurrentBrush); },
            "imageToDraw"_a, "destX"_a, "destY"_a, "destWidth"_a, "destHeight"_a, "placementWithinTarget"_a, "fillAlphaChannelWithCurrentBrush"_a = false)
        .def ("getClipBounds", &Graphics::getClipBounds)
        .def ("clipRegionIntersects", &Graphics::clipRegionIntersects)
        .def ("reduceClipRegion", py::overload_cast<int, int, int, int> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<Rectangle<int>> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const RectangleList<int>&> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::reduceClipRegion), "path"_a, "transform"_a = AffineTransform())
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
            : g (g)
        {
        }

        Graphics& g;
        std::variant<std::monostate, Graphics::ScopedSaveState> state;
    };

    py::class_<PyGraphicsScopedSaveState> (classGraphics, "ScopedSaveState")
        .def (py::init<Graphics&>())
        .def ("__enter__", [](PyGraphicsScopedSaveState& self)
        {
            self.state.emplace<Graphics::ScopedSaveState> (self.g);
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
