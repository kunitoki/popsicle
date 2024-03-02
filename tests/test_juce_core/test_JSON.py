import pytest

from ..utilities import get_runtime_data_file
import popsicle as juce

#==================================================================================================

def test_parse_with_valid_json_returns_correct_result():
    text = '{"key": "value"}'
    expected = {"key": "value"}
    result = juce.JSON.parse(text)
    assert result == expected

#==================================================================================================

def test_parse_with_invalid_json_returns_empty_var():
    text = '{"key": "value"'
    result = juce.JSON.parse(text)
    assert not result

#==================================================================================================

def test_parse_with_file_returns_correct_result():
    file_path = get_runtime_data_file("valid_json_file.json")
    file_path.replaceWithText('{"key": "value"}')
    file = juce.File(file_path)
    expected = {"key": "value"}
    result = juce.JSON.parse(file)
    assert result == expected

#==================================================================================================

def test_parse_with_stream_returns_correct_result():
    stream = juce.MemoryInputStream(b'{"key": "value"}', True)
    expected = {"key": "value"}
    result = juce.JSON.parse(stream)
    assert result == expected

#==================================================================================================

def test_to_string_with_default_options():
    obj_to_format = {"key": "value"}
    expected = '{"key": "value"}'
    result = juce.JSON.toString(obj_to_format, allOnOneLine=True)
    assert result == expected

#==================================================================================================

def test_format_options():
    format_options = juce.JSON.FormatOptions().withSpacing(juce.JSON.Spacing.none)
    assert format_options.getSpacing() == juce.JSON.Spacing.none

    format_options = juce.JSON.FormatOptions().withIndentLevel(10)
    assert format_options.getIndentLevel() == 10

    format_options = juce.JSON.FormatOptions().withMaxDecimalPlaces(4)
    assert format_options.getMaxDecimalPlaces() == 4

#==================================================================================================

def test_to_string_with_format_options_no_spacing():
    obj_to_format = {"key1": "value", "key2": 1, "key3": 2}
    format_options = juce.JSON.FormatOptions().withSpacing(juce.JSON.Spacing.none)
    expected = '{"key1":"value","key2":1,"key3":2}'
    result = juce.JSON.toString(obj_to_format, format_options)
    assert result == expected

#==================================================================================================

def test_to_string_with_format_options_single_line():
    obj_to_format = {"key1": "value", "key2": 1, "key3": 2}
    format_options = juce.JSON.FormatOptions().withSpacing(juce.JSON.Spacing.singleLine)
    expected = '{"key1": "value", "key2": 1, "key3": 2}'
    result = juce.JSON.toString(obj_to_format, format_options)
    assert result == expected

#==================================================================================================

def test_to_string_with_format_options_multi_line():
    obj_to_format = {"key1": "value", "key2": 1, "key3": 2, "obj": { "obj1": 1, "obj2": [1, 2, 3, 4] }}
    format_options = juce.JSON.FormatOptions().withSpacing(juce.JSON.Spacing.multiLine).withIndentLevel(2)
    expected = """ {
    "key1": "value",
    "key2": 1,
    "key3": 2,
    "obj": {
      "obj1": 1,
      "obj2": [
        1,
        2,
        3,
        4
      ]
    }
  }"""
    result = juce.JSON.toString(obj_to_format, format_options)
    assert result.replace("\r", "").strip() == expected.replace("\r", "").strip()

#==================================================================================================

@pytest.mark.skip(reason="Bug in JUCE")
def test_to_string_with_format_options_max_decimal_places():
    obj_to_format = {"key": 0.123456789}
    format_options = juce.JSON.FormatOptions().withMaxDecimalPlaces(2)
    expected = '{"key": 0.12}'
    result = juce.JSON.toString(obj_to_format, format_options)
    assert result == expected

#==================================================================================================

def test_write_to_stream():
    stream = juce.MemoryOutputStream()
    expected = {"key": "value"}
    juce.JSON.writeToStream(stream, expected, allOnOneLine=True)
    assert stream.toString() == '{"key": "value"}'

#==================================================================================================

def test_from_string_with_valid_json():
    json_string = '{"key": "value"}'
    expected = {"key": "value"}
    result = juce.JSON.fromString(json_string)
    assert result == expected

#==================================================================================================

def test_escape_string_escapes_special_characters():
    original_string = '"Hello, \nWorld!"'
    expected = '\\"Hello, \\nWorld!\\"'
    result = juce.JSON.escapeString(original_string)
    assert result == expected

#==================================================================================================

@pytest.mark.skip(reason="unfinished")
def test_parse_quoted_string_with_valid_input():
    text = '"Hello, World!"'
    expected = "Hello, World!"
    char_pointer = text
    result_var = None
    result = juce.JSON.parseQuotedString(char_pointer, result_var)
    assert result.wasOk() and result_var == expected