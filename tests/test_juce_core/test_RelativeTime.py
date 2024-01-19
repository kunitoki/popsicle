import popsicle as juce


millisecondsInSecond = 1000
secondsInMinute = 60
minutesInHour = 60
hoursInDay = 24
daysInWeek = 7


def test_construct_empty():
    a = juce.RelativeTime.seconds(0)
    assert a.inMilliseconds() == 0
    assert a.inSeconds() == 0
    assert a.inMinutes() == 0
    assert a.inHours() == 0
    assert a.inDays() == 0
    assert a.inWeeks() == 0


def test_construct_valid():
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


def test_operation_getDescription():
    a = juce.RelativeTime.milliseconds(0)
    assert a.getDescription() == "0"
    assert a.getDescription("infinite") == "infinite"

    a = juce.RelativeTime.milliseconds(500)
    assert a.getDescription() == "500 ms"
    assert a.getDescription("infinite") == "500 ms"

    a += juce.RelativeTime.seconds(1)
    assert a.getDescription() == "1 sec"
    assert a.getDescription("infinite") == "1 sec"

    a += juce.RelativeTime.minutes(1)
    assert a.getDescription() == "1 min 1 sec"
    assert a.getDescription("infinite") == "1 min 1 sec"

    a += juce.RelativeTime.hours(1)
    assert a.getDescription() == "1 hr 1 min"
    assert a.getDescription("infinite") == "1 hr 1 min"

    a += juce.RelativeTime.days(1)
    assert a.getDescription() == "1 day 1 hr"
    assert a.getDescription("infinite") == "1 day 1 hr"

    a += juce.RelativeTime.weeks(1)
    assert a.getDescription() == "1 week 1 day"
    assert a.getDescription("infinite") == "1 week 1 day"


def test_operation_getApproximateDescription():
    a = juce.RelativeTime.milliseconds(0)
    assert a.getApproximateDescription() == "< 1 sec"

    a = juce.RelativeTime.milliseconds(500)
    assert a.getApproximateDescription() == "< 1 sec"

    a = juce.RelativeTime.seconds(140)
    assert a.getApproximateDescription() == "2 mins"

    a = juce.RelativeTime.minutes(110)
    assert a.getApproximateDescription() == "1 hr"

    a = juce.RelativeTime.hours(23) + juce.RelativeTime.minutes(60)
    assert a.getApproximateDescription() == "24 hrs"

    a = juce.RelativeTime.days(8)
    assert a.getApproximateDescription() == "8 days"

    a = juce.RelativeTime.weeks(2)
    assert a.getApproximateDescription() == "2 weeks"
