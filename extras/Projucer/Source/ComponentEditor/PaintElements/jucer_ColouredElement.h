/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../jucer_PaintRoutine.h"
#include "../jucer_JucerDocument.h"
#include "jucer_StrokeType.h"

//==============================================================================
/**
    Base class for paint elements that have a fill colour and stroke.

*/
class ColouredElement   : public PaintElement
{
public:
    ColouredElement (PaintRoutine* owner,
                     const String& name,
                     bool showOutline_,
                     bool showJointAndEnd_);

    ~ColouredElement() override;

    //==============================================================================
    void getEditableProperties (Array<PropertyComponent*>& props, bool multipleSelected) override;
    void getColourSpecificProperties (Array<PropertyComponent*>& props);

    //==============================================================================
    const JucerFillType& getFillType() noexcept;
    void setFillType (const JucerFillType& newType, bool undoable);

    bool isStrokeEnabled() const noexcept;
    void enableStroke (bool enable, bool undoable);

    const StrokeType& getStrokeType() noexcept;
    void setStrokeType (const PathStrokeType& newType, bool undoable);
    void setStrokeFill (const JucerFillType& newType, bool undoable);

    //==============================================================================
    Rectangle<int> getCurrentBounds (const Rectangle<int>& parentArea) const override;
    void setCurrentBounds (const Rectangle<int>& newBounds, const Rectangle<int>& parentArea, bool undoable) override;

    void createSiblingComponents() override;

    //==============================================================================
    void addColourAttributes (XmlElement* const e) const;
    bool loadColourAttributes (const XmlElement& xml);

protected:
    JucerFillType fillType;

    bool isStrokePresent;
    const bool showOutline, showJointAndEnd;
    StrokeType strokeType;

    void convertToNewPathElement (const Path& path);
};
