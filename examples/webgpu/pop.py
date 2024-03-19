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
