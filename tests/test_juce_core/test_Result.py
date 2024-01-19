import popsicle as juce


def test_ok():
    result = juce.Result.ok()
    assert not result.failed()
    assert result.wasOk()

    if result: pass
    else: assert False

    if not result: assert False


def test_fail():
    result = juce.Result.fail("The cricket splatted")
    assert result.failed()
    assert not result.wasOk()
    assert result.getErrorMessage() == "The cricket splatted"

    if result: assert False

    if not result: pass
    else: assert False


def test_comparison():
    ok1 = juce.Result.ok()
    ok2 = juce.Result.ok()
    fail1 = juce.Result.fail("The cricket splatted")
    fail2 = juce.Result.fail("The fox died")
    fail3 = juce.Result.fail("The cricket splatted")

    assert ok1 == ok2
    assert ok1 == ok1
    assert ok1 != fail1
    assert fail1 != fail2
    assert fail1 == fail3
    assert fail1 == fail1
