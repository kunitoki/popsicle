import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_core/juce_core.h")

    cppyy.cppdef("""
    namespace popsicle::detail::XmlElement
    {
        void sortChildElements(
            juce::XmlElement* self,
            std::function<int(const juce::XmlElement*, const juce::XmlElement*)> comparator,
            bool retainOrderOfEquivalentItems = false)
        {
            struct Sorter
            {
                std::function<int(const juce::XmlElement*, const juce::XmlElement*)> callback;

                int compareElements(const juce::XmlElement* lhs, const juce::XmlElement* rhs)
                {
                    return callback(lhs, rhs);
                }
            } sorter { std::move(comparator) };

            self->sortChildElements(sorter, retainOrderOfEquivalentItems);
        }
    }
    """)

    def __pythonize(klass, name):
        if name == "String":
            klass.__str__ = klass.toRawUTF8

        if name == "XmlElement":
            klass.getNextElement.__creates__ = False
            klass.getFirstChildElement.__creates__ = False
            klass.getNextElementWithTagName.__creates__ = False
            klass.getChildElement.__creates__ = False
            klass.getChildByName.__creates__ = False
            klass.getChildByAttribute.__creates__ = False

            klass.sortChildElements = cppyy.gbl.popsicle.detail.XmlElement.sortChildElements

    cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
