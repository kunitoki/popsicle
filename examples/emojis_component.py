import re
import os
import hashlib
from enum import Enum
from io import BytesIO
from textwrap import dedent
from urllib.request import Request, urlopen
from urllib.error import HTTPError
from urllib.parse import quote_plus
from typing import Any, Dict, Final, List, Optional, NamedTuple

from emoji import unicode_codes

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


language_pack: Dict[str, str] = unicode_codes.get_emoji_unicode_dict("en")
EMOJI_UNICODE_REGEX = "|".join(map(re.escape, sorted(language_pack.values(), key=len, reverse=True)))

EMOJI_REGEX: Final[re.Pattern[str]] = re.compile(f'({EMOJI_UNICODE_REGEX})')
EMOJI_REQUEST_KWARGS: Dict[str, Any] = { "headers": {"User-Agent": "Mozilla/5.0"} }
EMOJI_BASE_CDN_URL: str = "https://emojicdn.elk.sh/"
EMOJI_STYLE: str = "google"


def get_emoji(emoji: str) -> Optional[BytesIO]:
    emoji_cache = juce.File(os.path.abspath(__file__)).getParentDirectory().getChildFile("emoji_cache")
    if not emoji_cache.isDirectory():
        emoji_cache.createDirectory()

    url = EMOJI_BASE_CDN_URL + quote_plus(emoji) + "?style=" + quote_plus(EMOJI_STYLE)
    url_hash = hashlib.md5()
    url_hash.update(url.encode("utf8"))

    emoji_file = emoji_cache.getChildFile(url_hash.hexdigest())
    if emoji_file.existsAsFile():
        with open(emoji_file.getFullPathName(), "rb") as f:
            return BytesIO(f.read())

    try:
        req = Request(url, **EMOJI_REQUEST_KWARGS)
        with urlopen(req) as response:
            response = response.read()

            with open(emoji_file.getFullPathName(), "wb") as f:
                f.write(response)

            return BytesIO(response)

    except HTTPError:
        pass

    return None


class NodeType(Enum):
    text          = 0
    emoji         = 1


class Node(NamedTuple):
    type: NodeType
    content: str
    emoji: Optional[BytesIO]


class EmojiComponent(juce.Component):
    font: juce.Font = juce.Font(12.0)
    colour: juce.Colour = juce.Colours.black
    nodes: List[List[Node]]

    def __init__(self):
        juce.Component.__init__(self)

        self.setOpaque(False)

    def setFont(self, font: juce.Font):
        self.font = font
        self.repaint()

    def setColour(self, colour: juce.Colour):
        self.colour = colour
        self.repaint()

    def setText(self, text: str):
        self.nodes = self.splitTextIntoNodes(text)
        self.repaint()

    def splitTextIntoNodes(self, text: str) -> List[List[Node]]:
        lines = []

        for line in text.splitlines():
            nodes = []
            for i, chunk in enumerate(EMOJI_REGEX.split(line)):
                if not chunk:
                    continue

                if not i % 2:
                    nodes.append(Node(NodeType.text, chunk, None))
                    continue

                nodes.append(Node(NodeType.emoji, chunk, get_emoji(chunk)))

            lines.append(nodes)

        return lines

    def paint(self, g: juce.Graphics):
        if not self.nodes:
            return

        font_height = self.font.getHeight()
        emoji_size = int(font_height * 1.1)

        g.setFont(self.font)
        g.setColour(self.colour)

        y = 0
        for lines in self.nodes:
            x = 0
            y += font_height + 2.0
            height = int(font_height)

            for node in lines:
                if node.type == NodeType.text:
                    text_width = self.font.getStringWidthFloat(node.content)
                    g.drawText(node.content,
                               int(x), int(y), min(int(text_width), self.getWidth() - int(x)), height,
                               juce.Justification.centredLeft, useEllipsesIfTooBig=False)
                    x += text_width
                elif node.emoji:
                    emoji = juce.ImageCache.getFromMemory(node.emoji.getbuffer())
                    if emoji and emoji.isValid():
                        g.drawImageWithin(emoji, int(x), int(y), emoji_size, emoji_size, juce.RectanglePlacement.centred)
                        x += emoji_size


class ExampleComponent(juce.Component):

    def __init__(self):
        juce.Component.__init__(self)

        self.emoji_one = EmojiComponent()
        self.emoji_one.setFont(juce.Font(16.0))
        self.emoji_one.setColour(juce.Colours.white)
        self.emoji_one.setText(dedent("""
            I 🕴️ 100% 💶 agree 💯 that 👉💀🔕🐑 this automated 🏧 generator does 👩‍🦲 NOT 🚯🚯🚯 provide 👋 the same 😯
            quality 👌 as hand 👊 crafted emoji 🤟 pasta. 🍝 But 😥 I 🤖 think 🤔 there's 🛒 something ❓❔ cool 🧊
            about 🌈 being 😑 able 💪💪 to take 👏 a 10,000 word 📓 wikipedia 💻 article 📄 and instantly add 👈
            emojis 🐅🐅🐢🦅🦅🦋🐒
        """).strip())
        self.addAndMakeVisible(self.emoji_one)

        self.slider = juce.Slider()
        self.slider.setRange(1.0, 100, 0.1)
        self.slider.setValue(16.0)
        self.slider.onValueChange = lambda: self.emoji_one.setFont(juce.Font(self.slider.getValue()))
        self.addAndMakeVisible(self.slider)

        self.setOpaque(True)
        self.setSize(600, 400)

    def paint(self, g: juce.Graphics):
        g.fillAll(self.findColour(juce.DocumentWindow.backgroundColourId, True))

    def resized(self):
        bounds = self.getLocalBounds()
        self.emoji_one.setBounds(bounds)

        self.slider.setBounds(bounds.removeFromBottom(20))


if __name__ == "__main__":
    START_JUCE_COMPONENT(ExampleComponent, name="Emoji Example")