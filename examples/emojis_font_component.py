import re
import os
from enum import Enum
from textwrap import dedent
from typing import Dict, Final, List, NamedTuple

from emoji import unicode_codes
from PIL import Image, ImageDraw, ImageFont
from pil_image import ImageJuce

from juce_init import START_JUCE_COMPONENT
import popsicle as juce

language_pack: Dict[str, str] = unicode_codes.get_emoji_unicode_dict("en")
EMOJI_UNICODE_REGEX = "|".join(map(re.escape, sorted(language_pack.values(), key=len, reverse=True)))
EMOJI_REGEX: Final[re.Pattern[str]] = re.compile(f'({EMOJI_UNICODE_REGEX})')


class NodeType(Enum):
    text          = 0
    emoji         = 1


class Node(NamedTuple):
    type: NodeType
    content: str


class EmojiComponent(juce.Component):
    font: juce.Font = juce.Font(12.0)
    colour: juce.Colour = juce.Colours.black
    nodes: List[List[Node]]
    unicode_font = None
    unicode_size = 109

    def __init__(self):
        juce.Component.__init__(self)

        font_file = juce.File(os.path.abspath(__file__)).getSiblingFile("NotoColorEmoji.ttf")
        self.unicode_font = ImageFont.truetype(font_file.getFullPathName(), self.unicode_size)

        self.im = Image.new("RGBA", (self.unicode_size + 20, self.unicode_size + 20))
        self.draw = ImageDraw.Draw(self.im)

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
                    nodes.append(Node(NodeType.text, chunk))
                    continue

                nodes.append(Node(NodeType.emoji, chunk))

            lines.append(nodes)

        return lines

    def paint(self, g: juce.Graphics):
        if not self.nodes:
            return

        font_height = self.font.getHeight()
        emoji_size = int(font_height * 1.1)

        g.setFont(self.font)
        g.setColour(self.colour)

        def new_line(x, y):
            return 0, y + font_height + 4.0

        x = 0
        y = 0
        for lines in self.nodes:
            x, y = new_line(x, y)

            current_text = None

            for node in lines:
                if node.type == NodeType.text:
                    current_text = node.content

                    while current_text:
                        remaining_text = []

                        text_width = self.font.getStringWidthFloat(current_text)
                        while current_text and text_width > self.getWidth() - int(x):
                            words = current_text.split(" ")
                            current_text = " ".join(words[:-1])
                            if words:
                                remaining_text.append(words[-1])
                            text_width = self.font.getStringWidthFloat(current_text)

                        if current_text:
                            g.drawText(current_text,
                                    int(x), int(y), min(int(text_width), self.getWidth() - int(x)), int(font_height),
                                    juce.Justification.centredLeft, useEllipsesIfTooBig=False)

                        x += text_width
                        current_text = None

                        if remaining_text:
                            x, y = new_line(x, y)
                            current_text = " ".join(remaining_text)

                else:
                    self.draw.rectangle((0, 0, self.im.size[0], self.im.size[1]), fill=(0, 0, 0, 0))
                    self.draw.text((0, 0), node.content, embedded_color=True, font=self.unicode_font)

                    if emoji_size > self.getWidth() - int(x):
                        x, y = new_line(x, y)

                    g.drawImageWithin(ImageJuce(self.im), int(x), int(y), emoji_size, emoji_size,
                                      juce.RectanglePlacement.centred | juce.RectanglePlacement.onlyReduceInSize)

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