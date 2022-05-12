## Master

* ADDED: added a simple slider example.
* ADDED: added a new slider example controlling volume of noise in decibels.
* FIXED: XmlElement sort was not possible, added a workaround.
* FIXED: sorting in the `table_list_box.py` example has been fixed.
* FIXED: shutdown was not working in some examples using audio classes.

## Version 0.0.9

* ADDED: GitHub actions for building wheels on all platforms.
* ADDED: manylinux wheels.
* FIXED: AudioDeviceManager shutdown could crash in certain scenarios.

## Version 0.0.8

* FIXED: bug that was preventing cppyy to locate the dll, because it had a wrong name in windows.
