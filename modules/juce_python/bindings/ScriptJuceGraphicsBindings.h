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

#if !JUCE_MODULE_AVAILABLE_juce_graphics
 #error This binding file requires adding the juce_graphics module in the project
#else
 #include <juce_core/juce_core.h>

 JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4244)
 #include <juce_graphics/juce_graphics.h>
 JUCE_END_IGNORE_WARNINGS_MSVC
#endif

#include "../utilities/PyBind11Includes.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceGraphicsBindings (pybind11::module_& m);

// =================================================================================================

struct PyImageType : juce::ImageType
{
    juce::ImagePixelData::Ptr create (juce::Image::PixelFormat format, int width, int height, bool shouldClearImage) const override
    {
        PYBIND11_OVERRIDE_PURE (juce::ImagePixelData::Ptr, juce::ImageType, create, format, width, height, shouldClearImage);
    }

    int getTypeID() const override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::ImageType, getTypeID);
    }

    juce::Image convert (const juce::Image& source) const override
    {
        PYBIND11_OVERRIDE (juce::Image, juce::ImageType, convert, source);
    }
};

// =================================================================================================

struct PyImageFileFormat : juce::ImageFileFormat
{
    using juce::ImageFileFormat::ImageFileFormat;

    juce::String getFormatName() override
    {
        PYBIND11_OVERRIDE_PURE (juce::String, juce::ImageFileFormat, getFormatName);
    }

    bool canUnderstand (juce::InputStream& input) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::ImageFileFormat, canUnderstand, input);
    }

    bool usesFileExtension (const juce::File& possibleFile) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::ImageFileFormat, usesFileExtension, possibleFile);
    }

    juce::Image decodeImage (juce::InputStream& input) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Image, juce::ImageFileFormat, decodeImage, input);
    }

    bool writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& destStream) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::ImageFileFormat, writeImageToStream, sourceImage, destStream);
    }
};

// =================================================================================================

template <class Base = juce::LowLevelGraphicsContext>
struct PyLowLevelGraphicsContext : Base
{
    using Base::Base;

    bool isVectorDevice() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, isVectorDevice);
    }

    void setOrigin (juce::Point<int> origin) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setOrigin, origin);
    }

    void addTransform (const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, addTransform, transform);
    }

    float getPhysicalPixelScaleFactor() override
    {
        PYBIND11_OVERRIDE_PURE (float, Base, getPhysicalPixelScaleFactor);
    }

    bool clipToRectangle (const juce::Rectangle<int>& rect) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, clipToRectangle, rect);
    }

    bool clipToRectangleList (const juce::RectangleList<int>& rects) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, clipToRectangleList, rects);
    }

    void excludeClipRectangle (const juce::Rectangle<int>& rect) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, excludeClipRectangle, rect);
    }

    void clipToPath (const juce::Path& path, const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, clipToPath, path, transform);
    }

    void clipToImageAlpha (const juce::Image& image, const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, clipToImageAlpha, image, transform);
    }

    bool clipRegionIntersects (const juce::Rectangle<int>& rect) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, clipRegionIntersects, rect);
    }

    juce::Rectangle<int> getClipBounds() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Rectangle<int>, Base, getClipBounds);
    }

    bool isClipEmpty() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, isClipEmpty);
    }

    void saveState() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, saveState);
    }

    void restoreState() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, restoreState);
    }

    void beginTransparencyLayer (float opacity) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, beginTransparencyLayer, opacity);
    }

    void endTransparencyLayer() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, endTransparencyLayer);
    }

    void setFill (const juce::FillType& fill) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setFill, fill);
    }

    void setOpacity (float opacity) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setOpacity, opacity);
    }

    void setInterpolationQuality (juce::Graphics::ResamplingQuality quality) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setInterpolationQuality, quality);
    }

    void fillAll() override
    {
        PYBIND11_OVERRIDE (void, Base, fillAll);
    }

    void fillRect (const juce::Rectangle<int>& rect, bool replaceExistingContents) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, fillRect, rect, replaceExistingContents);
    }

    void fillRect (const juce::Rectangle<float>& rect) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, fillRect, rect);
    }

    void fillRectList (const juce::RectangleList<float>& rects) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, fillRectList, rects);
    }

    void fillPath (const juce::Path& path, const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, fillPath, path, transform);
    }

    void drawImage (const juce::Image& image, const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, drawImage, image, transform);
    }

    void drawLine (const juce::Line<float>& line) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, drawLine, line);
    }

    void setFont (const juce::Font& font) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setFont, font);
    }

    const juce::Font& getFont() override
    {
        PYBIND11_OVERRIDE_PURE (const juce::Font&, Base, getFont);
    }

    void drawGlyph (int glyphNumber, const juce::AffineTransform& transform) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, drawGlyph, glyphNumber, transform);
    }

    bool drawTextLayout (const juce::AttributedString& string, const juce::Rectangle<float>& rect) override
    {
        PYBIND11_OVERRIDE (bool, Base, drawTextLayout, string, rect);
    }
};

} // namespace popsicle::Bindings
