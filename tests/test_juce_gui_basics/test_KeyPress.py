import pytest

import popsicle as juce

#==================================================================================================

def test_key_press_default_constructor():
    key_press = juce.KeyPress()
    assert not key_press.isValid()

#==================================================================================================

def test_key_press_constructor_with_modifierkeys_parameters():
    key_code = juce.KeyPress.spaceKey
    modifiers = juce.ModifierKeys()
    text_character = 'a'

    key_press = juce.KeyPress(text_character, modifiers, text_character)
    assert key_press.getKeyCode() == ord(text_character)
    assert key_press.getModifiers() == modifiers
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, text_character)
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == modifiers
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, ord(text_character))
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == modifiers
    assert key_press.getTextCharacter() == text_character

#==================================================================================================

def test_key_press_constructor_with_flags_parameters():
    key_code = juce.KeyPress.spaceKey
    modifiers = juce.ModifierKeys.shiftModifier
    text_character = 'b'

    key_press = juce.KeyPress(text_character, modifiers, text_character)
    assert key_press.getKeyCode() == ord(text_character)
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, text_character)
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, ord(text_character))
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

#==================================================================================================

def test_key_press_constructor_with_ints_parameters():
    key_code = juce.KeyPress.spaceKey
    modifiers = juce.ModifierKeys.shiftModifier | juce.ModifierKeys.commandModifier
    text_character = 'c'

    key_press = juce.KeyPress(text_character, modifiers, text_character)
    assert key_press.getKeyCode() == ord(text_character)
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, text_character)
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

    key_press = juce.KeyPress(key_code, modifiers, ord(text_character))
    assert key_press.getKeyCode() == key_code
    assert key_press.getModifiers() == juce.ModifierKeys(modifiers)
    assert key_press.getTextCharacter() == text_character

#==================================================================================================

def test_key_press_constructor_with_key_code():
    key_code = juce.KeyPress.escapeKey
    key_press = juce.KeyPress(key_code)
    assert key_press.getKeyCode() == key_code

#==================================================================================================

def test_key_press_equality():
    key_press1 = juce.KeyPress(juce.KeyPress.spaceKey)
    key_press2 = juce.KeyPress(juce.KeyPress.spaceKey)
    key_press3 = juce.KeyPress(juce.KeyPress.escapeKey)
    assert key_press1 == key_press2
    assert key_press1 != key_press3

#==================================================================================================

def test_key_press_equality_with_key_code():
    key_press = juce.KeyPress(juce.KeyPress.spaceKey)
    assert key_press == juce.KeyPress.spaceKey
    assert key_press != juce.KeyPress.escapeKey

#==================================================================================================

def test_key_press_is_valid():
    key_press = juce.KeyPress(juce.KeyPress.spaceKey)
    assert key_press.isValid()
    invalid_key_press = juce.KeyPress()
    assert not invalid_key_press.isValid()

#==================================================================================================

def test_key_press_is_key_code():
    key_press = juce.KeyPress(juce.KeyPress.spaceKey)
    assert key_press.isKeyCode(juce.KeyPress.spaceKey)
    assert not key_press.isKeyCode(juce.KeyPress.escapeKey)

#==================================================================================================

def test_key_press_create_from_description():
    key_press_description = "ctrl + a"
    key_press = juce.KeyPress.createFromDescription(key_press_description)
    assert key_press.isValid()

#==================================================================================================

def test_key_press_get_text_description():
    key_press = juce.KeyPress(juce.KeyPress.spaceKey)
    assert key_press.getTextDescription() == "spacebar"

#==================================================================================================

def test_key_press_get_text_description_with_icons():
    key_press = juce.KeyPress(juce.KeyPress.spaceKey)
    assert key_press.getTextDescriptionWithIcons() == "␣"

#==================================================================================================

def test_key_press_is_currently_down():
    assert not juce.KeyPress.isKeyCurrentlyDown(juce.KeyPress.spaceKey)
