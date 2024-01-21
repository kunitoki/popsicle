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

#include "../jucer_JucerDocument.h"

//==============================================================================
class ButtonDocument   : public JucerDocument
{
public:
    ButtonDocument (SourceCodeDocument* cpp);
    ~ButtonDocument();

    //==============================================================================
    String getTypeName() const;

    JucerDocument* createCopy();
    Component* createTestComponent (bool alwaysFillBackground);

    int getNumPaintRoutines() const;
    StringArray getPaintRoutineNames() const;
    PaintRoutine* getPaintRoutine (int index) const;

    void setStatePaintRoutineEnabled (int index, bool b);
    bool isStatePaintRoutineEnabled (int index) const;

    int chooseBestEnabledPaintRoutine (int paintRoutineWanted) const;

    ComponentLayout* getComponentLayout() const                 { return nullptr; }

    void addExtraClassProperties (PropertyPanel&);

    //==============================================================================
    std::unique_ptr<XmlElement> createXml() const;
    bool loadFromXml (const XmlElement&);

    void fillInGeneratedCode (GeneratedCode& code) const;
    void fillInPaintCode (GeneratedCode& code) const;

    void getOptionalMethods (StringArray& baseClasses,
                             StringArray& returnValues,
                             StringArray& methods,
                             StringArray& initialContents) const;

    //==============================================================================
    std::unique_ptr<PaintRoutine> paintRoutines[7];
    bool paintStatesEnabled [7];
};
