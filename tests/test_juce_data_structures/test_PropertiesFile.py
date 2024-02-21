import sys
import pytest

from ..utilities import get_runtime_data_file
import popsicle as juce

#==================================================================================================

@pytest.fixture
def options():
    opts = juce.PropertiesFile.Options()
    opts.applicationName = "TestApp"

    if sys.platform == "darwin":
        opts.osxLibrarySubFolder = "Application Support"

    return opts

#==================================================================================================

def test_options_default_file(options):
    options.applicationName = "CustomApp"
    options.filenameSuffix = ".settings"
    options.folderName = "CustomFolder"
    options.commonToAllUsers = False
    options.ignoreCaseOfKeyNames = False
    options.doNotSave = False
    options.millisecondsBeforeSaving = 3000
    options.storageFormat = juce.PropertiesFile.storeAsXML

    assert "CustomApp.settings" in options.getDefaultFile().getFullPathName()
    assert "CustomFolder" in options.getDefaultFile().getFullPathName()

#==================================================================================================

def test_properties_file_creation_with_options(options):
    properties_file = juce.PropertiesFile(options)
    assert properties_file.isValidFile()

#==================================================================================================

def test_properties_file_creation_with_file_and_options(options):
    file = get_runtime_data_file("test_settings.settings")
    properties_file = juce.PropertiesFile(file, options)
    assert properties_file.getFile() == file

#==================================================================================================

def test_properties_file_save_if_needed_initial_state(options):
    properties_file = juce.PropertiesFile(options)
    assert properties_file.saveIfNeeded()

#==================================================================================================

def test_properties_file_needs_to_be_saved(options):
    properties_file = juce.PropertiesFile(options)
    properties_file.setNeedsToBeSaved(True)
    assert properties_file.needsToBeSaved()

#==================================================================================================

def test_properties_file_set_needs_to_be_saved(options):
    properties_file = juce.PropertiesFile(options)
    properties_file.setNeedsToBeSaved(True)
    assert properties_file.needsToBeSaved()

    properties_file.setNeedsToBeSaved(False)
    assert not properties_file.needsToBeSaved()

#==================================================================================================

def test_properties_file_save(options):
    file = get_runtime_data_file("test_settings.settings")
    properties_file = juce.PropertiesFile(file, options)
    assert properties_file.save()

    properties_file.setValue("testKey", "testValue")
    assert properties_file.save()
    assert properties_file.save()

#==================================================================================================

def test_properties_file_reload(options):
    properties_file = juce.PropertiesFile(options)
    assert properties_file.reload()

#==================================================================================================

def test_properties_file_value_manipulation(options):
    properties_file = juce.PropertiesFile(options)
    properties_file.setValue("testKey", "testValue")
    assert properties_file.getValue("testKey") == "testValue"

    properties_file.removeValue("testKey")
    assert properties_file.getValue("testKey") == ""

#==================================================================================================

def test_properties_file_reload_after_value_change(options):
    properties_file = juce.PropertiesFile(options)

    properties_file.setValue("testKey", "testValue")
    properties_file.save()

    properties_file.setValue("testKey", "newValue")
    properties_file.reload()

    assert properties_file.getValue("testKey") == "testValue"

#==================================================================================================

class CustomListener(juce.ChangeListener):
    changedObject = None

    def changeListenerCallback(self, object):
        self.changedObject = object

def test_properties_file_property_changed_callback(options, juce_app):
    options.millisecondsBeforeSaving = 0
    properties_file = juce.PropertiesFile(options)
    properties_file.setValue("callbackKey", "callbackValue")

    listener = CustomListener()
    properties_file.addChangeListener(listener)

    properties_file.setValue("callbackKey", "anotherValue")
    properties_file.dispatchPendingMessages()
    assert listener.changedObject == properties_file
