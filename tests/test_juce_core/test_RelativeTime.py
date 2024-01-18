from .. import common
import popsicle as juce

millisecondsInSecond = 1000
secondsInMinute = 60
minutesInHour = 60
hoursInDay = 24
daysInWeek = 7

def test_construct():
    a = juce.RelativeTime.milliseconds(1000)
    assert juce.approximatelyEqual (a.inMilliseconds(), 1000)
    assert juce.approximatelyEqual (a.inSeconds(), 1000 / millisecondsInSecond)
    assert juce.approximatelyEqual (a.inMinutes(), 1000 / millisecondsInSecond / secondsInMinute)
    assert juce.approximatelyEqual (a.inHours(), 1000 / millisecondsInSecond / secondsInMinute / minutesInHour)
    assert juce.approximatelyEqual (a.inDays(), 1000 / millisecondsInSecond / secondsInMinute / minutesInHour / hoursInDay)
    assert juce.approximatelyEqual (a.inWeeks(), 1000 / millisecondsInSecond / secondsInMinute / minutesInHour / hoursInDay / daysInWeek)

    a = juce.RelativeTime.seconds(5)
    assert juce.approximatelyEqual (a.inMilliseconds(), millisecondsInSecond * 5)
    assert juce.approximatelyEqual (a.inSeconds(), 5)
    assert juce.approximatelyEqual (a.inMinutes(), 5 / secondsInMinute)
    assert juce.approximatelyEqual (a.inHours(), 5 / secondsInMinute / minutesInHour)
    assert juce.approximatelyEqual (a.inDays(), 5 / secondsInMinute / minutesInHour / hoursInDay)
    assert juce.approximatelyEqual (a.inWeeks(), 5 / secondsInMinute / minutesInHour / hoursInDay / daysInWeek)

    a = juce.RelativeTime.minutes(120)
    assert juce.approximatelyEqual (a.inMilliseconds(), millisecondsInSecond * secondsInMinute * 120)
    assert juce.approximatelyEqual (a.inSeconds(), secondsInMinute * 120)
    assert juce.approximatelyEqual (a.inMinutes(), 120)
    assert juce.approximatelyEqual (a.inHours(), 120 / minutesInHour)
    assert juce.approximatelyEqual (a.inDays(), 120 / minutesInHour / hoursInDay)
    assert juce.approximatelyEqual (a.inWeeks(), 120 / minutesInHour / hoursInDay / daysInWeek)

    a = juce.RelativeTime.hours(25)
    assert juce.approximatelyEqual (a.inMilliseconds(), millisecondsInSecond * secondsInMinute * minutesInHour * 25)
    assert juce.approximatelyEqual (a.inSeconds(), secondsInMinute * minutesInHour * 25)
    assert juce.approximatelyEqual (a.inMinutes(), minutesInHour * 25)
    assert juce.approximatelyEqual (a.inHours(), 25)
    assert juce.approximatelyEqual (a.inDays(), 25 / hoursInDay)
    assert juce.approximatelyEqual (a.inWeeks(), 25 / hoursInDay / daysInWeek)

    a = juce.RelativeTime.days(5)
    assert juce.approximatelyEqual (a.inMilliseconds(), millisecondsInSecond * secondsInMinute * minutesInHour * hoursInDay * 5)
    assert juce.approximatelyEqual (a.inSeconds(), secondsInMinute * minutesInHour * hoursInDay * 5)
    assert juce.approximatelyEqual (a.inMinutes(), minutesInHour * hoursInDay * 5)
    assert juce.approximatelyEqual (a.inHours(), hoursInDay * 5)
    assert juce.approximatelyEqual (a.inDays(), 5)
    assert juce.approximatelyEqual (a.inWeeks(), 5 / daysInWeek)

    a = juce.RelativeTime.weeks(2)
    assert juce.approximatelyEqual (a.inMilliseconds(), millisecondsInSecond * secondsInMinute * minutesInHour * hoursInDay * daysInWeek * 2)
    assert juce.approximatelyEqual (a.inSeconds(), secondsInMinute * minutesInHour * hoursInDay * daysInWeek * 2)
    assert juce.approximatelyEqual (a.inMinutes(), minutesInHour * hoursInDay * daysInWeek * 2)
    assert juce.approximatelyEqual (a.inHours(), hoursInDay * daysInWeek * 2)
    assert juce.approximatelyEqual (a.inDays(), daysInWeek * 2)
    assert juce.approximatelyEqual (a.inWeeks(), 2)
