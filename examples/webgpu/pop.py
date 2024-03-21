"""
Support for rendering in a JUCE component. Provides a component subclass that
can be used as a standalone window or in a larger GUI.
"""

import sys
import ctypes

from wgpu.gui.base import WgpuCanvasBase, WgpuAutoGui
from wgpu.gui._gui_utils import get_alt_x11_display, get_alt_wayland_display, weakbind

import popsicle as juce


is_wayland = False  # We force Qt to use X11 in _gui_utils.py


def enable_hidpi():
    """Enable high-res displays."""
    try:
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
        ctypes.windll.shcore.SetProcessDpiAwareness(2)
    except Exception:
        pass  # fail on non-windows


enable_hidpi()


def convert_buttons(event: juce.MouseEvent):
    button = 0
    buttons = []
    if event.mods.isLeftButtonDown():
        button = 1
        buttons.append(1)
    if event.mods.isRightButtonDown():
        button = 2
        buttons.append(2)
    if event.mods.isMiddleButtonDown():
        button = 3
        buttons.append(3)

    modifiers = []
    if event.mods.getCurrentModifiers() == juce.ModifierKeys.altModifier:
        modifiers.append("Alt")
    if event.mods.getCurrentModifiers() == juce.ModifierKeys.shiftModifier:
        modifiers.append("Shift")
    if event.mods.getCurrentModifiers() == juce.ModifierKeys.ctrlModifier:
        modifiers.append("Control")
    if event.mods.getCurrentModifiers() == juce.ModifierKeys.commandModifier:
        modifiers.append("Meta")

    return button, buttons, modifiers


def call_later(delay, callback, *args):
    juce.MessageManager.callAsync(lambda: callback(*args)) # int(delay * 1000)


class JUCECallbackTimer(juce.Timer):
    def __init__(self, callback):
        juce.Timer.__init__(self)
        self._callback = callback

    def timerCallback(self):
        self._callback()
        self.stopTimer()


class JUCEWgpuComponentBase(juce.Component):
    def __init__(self, *args, **kwargs):
        juce.Component.__init__(self)


class JUCEWgpuComponent(WgpuAutoGui, WgpuCanvasBase, JUCEWgpuComponentBase):
    """A JUCE Component representing a wgpu canvas that can be embedded in a Popsicle application."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        if size := kwargs.get("size", None):
            self.setSize(*size)

        # A timer for limiting fps
        self._request_draw_timer = JUCECallbackTimer(lambda: self.repaint())

    def paint(self, g): # noqa: N802 - this is a Popsicle method
        self._draw_frame_and_present()

    def resized(self):
        ev = {
            "event_type": "resize",
            "width": float(self.getWidth()),
            "height": float(self.getHeight()),
            "pixel_ratio": self.get_pixel_ratio(),
        }
        self._handle_event_and_flush(ev)

    # Methods that we add from wgpu (snake_case)

    def get_surface_info(self):
        if sys.platform.startswith("win") or sys.platform.startswith("darwin"):
            return {
                "window": int(self.getWindowHandle()),
            }
        elif sys.platform.startswith("linux"):
            # The trick to use an alt display pointer works for X11, but results in a segfault on Wayland ...
            if is_wayland:
                return {
                    "platform": "wayland",
                    "window": int(self.getWindowHandle()),
                    "display": int(get_alt_wayland_display()),
                }
            else:
                return {
                    "platform": "x11",
                    "window": int(self.getWindowHandle()),
                    "display": int(get_alt_x11_display()),
                }
        else:
            raise RuntimeError(f"Cannot get JUCE surface info on {sys.platform}.")

    def get_pixel_ratio(self):
        return juce.Desktop.getInstance().getGlobalScaleFactor()

    def get_logical_size(self):
        lsize = self.getWidth(), self.getHeight()
        return float(lsize[0]), float(lsize[1])

    def get_physical_size(self):
        lsize = self.getWidth(), self.getHeight()
        lsize = float(lsize[0]), float(lsize[1])
        ratio = juce.Desktop.getInstance().getGlobalScaleFactor()
        return round(lsize[0] * ratio + 0.01), round(lsize[1] * ratio + 0.01)

    def set_logical_size(self, width, height):
        if width < 0 or height < 0:
            raise ValueError("Window width and height must not be negative")
        self.setSize(width, height)

    def _request_draw(self):
        if not self._request_draw_timer.isTimerRunning():
            self._request_draw_timer.startTimer(int(self._get_draw_wait_time() * 1000))

    def is_closed(self):
        return not self.isVisible()

    # User events to jupyter_rfb events

    def _mouse_event(self, event_type, event, touches=True):
        button, buttons, modifiers = convert_buttons(event)

        ev = {
            "event_type": event_type,
            "x": event.getPosition().x,
            "y": event.getPosition().y,
            "button": button,
            "buttons": buttons,
            "modifiers": modifiers,
        }

        if touches:
            ev.update({
                "ntouches": 0,
                "touches": {}
            })

        if event_type == "pointer_move":
            match_keys = {"buttons", "modifiers", "ntouches"}
            accum_keys = {}
            self._handle_event_rate_limited(ev, call_later, match_keys, accum_keys)
        else:
            self._handle_event_and_flush(ev)

    def mouseDown(self, event):  # noqa: N802
        self._mouse_event("pointer_down", event)

    def mouseMove(self, event):  # noqa: N802
        self._mouse_event("pointer_move", event)

    def mouseDrag(self, event):  # noqa: N802
        self._mouse_event("pointer_move", event)

    def mouseUp(self, event):  # noqa: N802
        self._mouse_event("pointer_up", event)

    def mouseDoubleClick(self, event):  # noqa: N802
        self._mouse_event("double_click", event, touches=False)

    def mouseWheelMove(self, event, wheel):
        _, buttons, modifiers = convert_buttons(event)

        ev = {
            "event_type": "wheel",
            "dx": -wheel.deltaX * 200,
            "dy": -wheel.deltaY * 200,
            "x": event.getPosition().x,
            "y": event.getPosition().y,
            "buttons": buttons,
            "modifiers": modifiers
        }

        match_keys = {"modifiers"}
        accum_keys = {"dx", "dy"}

        self._handle_event_rate_limited(ev, call_later, match_keys, accum_keys)

    # Custom methods to make Component work when used standalone

    def reparentToWindow(self, window):
        self.addToDesktop(juce.ComponentPeer.windowIsTemporary, window.getWindowHandle())


class JUCEWgpuWindow(juce.DocumentWindow):
    def __init__(self, title=None, *args, **kwargs):
        juce.DocumentWindow.__init__(
            self,
            title or "wgpu-juce",
            juce.Desktop.getInstance().getDefaultLookAndFeel()
                .findColour(juce.ResizableWindow.backgroundColourId),
            juce.DocumentWindow.allButtons,
            True)


class JUCEWgpuCanvas(WgpuAutoGui, WgpuCanvasBase, JUCEWgpuWindow):
    """A toplevel JUCE window providing a wgpu canvas."""

    _subwidget = None

    def __init__(self, size=None, title=None, max_fps=30, **kwargs):
        super().__init__(title=title, max_fps=max_fps, **kwargs)

        size = size or (640, 480)
        self.set_logical_size(*size)

        self._subwidget = JUCEWgpuComponent(self, size=size, max_fps=max_fps)
        self._subwidget.add_event_handler(weakbind(self.handle_event), "*")

        self.setUsingNativeTitleBar(True)
        self.setResizable(True, True)
        self.setContentNonOwned(self._subwidget, True)
        self.centreWithSize(*size)
        self.setVisible(True)

    # Methods that we add from juce

    def __del__(self):
        self.clearContentComponent()
        if self._subwidget:
            del self._subwidget

    def closeButtonPressed(self):
        juce.JUCEApplication.getInstance().systemRequestedQuit()

    # Methods that we add from wgpu (snake_case)

    @property
    def draw_frame(self):
        return self._subwidget.draw_frame

    @draw_frame.setter
    def draw_frame(self, f):
        self._subwidget.draw_frame = f

    def get_surface_info(self):
        return self._subwidget.get_surface_info()

    def get_pixel_ratio(self):
        return self._subwidget.get_pixel_ratio()

    def get_logical_size(self):
        return self._subwidget.get_logical_size()

    def get_physical_size(self):
        return self._subwidget.get_physical_size()

    def set_logical_size(self, width, height):
        if width < 0 or height < 0:
            raise ValueError("Window width and height must not be negative")
        self.setSize(width, height)  # See comment on pixel ratio

    def _request_draw(self):
        return self._subwidget._request_draw()

    def is_closed(self):
        return not self.isVisible()

    # Methods that we need to explicitly delegate to the subwidget

    def get_context(self, *args, **kwargs):
        return self._subwidget.get_context(*args, **kwargs)

    def request_draw(self, *args, **kwargs):
        return self._subwidget.request_draw(*args, **kwargs)

    # Custom methods used by the run() function

    def getSubwidget(self):
        return self._subwidget


# Make available under a name that is the same for all gui backends
WgpuWidget = JUCEWgpuComponent
WgpuCanvas = JUCEWgpuCanvas


def run(canvas, make_device, **kwargs):
    class WgpuApplication(juce.JUCEApplication):
        window = None

        def getApplicationName(self):
            return kwargs.get("appName", "wgpu-juce")

        def getApplicationVersion(self):
            return kwargs.get("appVersion", "1.0")

        def initialise(self, commandLineParameters: str):
            self.window = canvas

            make_device(canvas.getSubwidget())

            juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

        def shutdown(self):
            if self.window:
                del self.window

        def systemRequestedQuit(self):
            self.quit()

    juce.START_JUCE_APPLICATION(WgpuApplication)
