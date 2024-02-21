import pytest
import popsicle as juce

#==================================================================================================

class CustomSynchroniser(juce.ValueTreeSynchroniser):
    changes = []

    def __init__(self, tree):
        juce.ValueTreeSynchroniser.__init__(self, tree)

    def stateChanged(self, encodedChange):
        self.changes.append(bytes(encodedChange))

#==================================================================================================

@pytest.fixture
def value_tree():
    return juce.ValueTree("TestTree")

#==================================================================================================

def test_value_tree_synchroniser_construction(value_tree):
    synchroniser = juce.ValueTreeSynchroniser(value_tree)
    assert synchroniser.getRoot() == value_tree

#==================================================================================================

def test_send_full_sync_callback(value_tree):
    synchroniser = CustomSynchroniser(value_tree)
    synchroniser.sendFullSyncCallback()
    assert len(synchroniser.changes) > 0

#==================================================================================================

def test_apply_change_success(value_tree):
    synchroniser = CustomSynchroniser(value_tree)
    assert len(synchroniser.changes) == 1

    synchroniser.sendFullSyncCallback()
    assert len(synchroniser.changes) == 2
    encoded_change = synchroniser.changes[1]
    assert juce.ValueTreeSynchroniser.applyChange(value_tree, encoded_change, None)

#==================================================================================================

@pytest.mark.skip(reason="Hit assertion in ValueTreeSynchroniser")
def test_apply_change_failure(value_tree):
    invalid_change_data = bytes([7])
    success = juce.ValueTreeSynchroniser.applyChange(value_tree, invalid_change_data, None)
    assert not success

#==================================================================================================

def test_get_root(value_tree):
    synchroniser = CustomSynchroniser(value_tree)
    assert synchroniser.getRoot() == value_tree
