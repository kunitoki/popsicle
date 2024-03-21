"""

PBR Rendering 2
===============

This example shows the lighting rendering affect of materials with different
metalness and roughness. Every second sphere has an IBL environment map on it.
"""

# run_example = false
# sphinx_gallery_pygfx_animate = True
# sphinx_gallery_pygfx_duration = 3
# sphinx_gallery_pygfx_lossless = False

import math
from colorsys import hls_to_rgb
from itertools import count

import imageio.v3 as iio
import numpy as np
from pop import WgpuWidget

import popsicle as juce
import pygfx as gfx
import pylinalg as la


class WgpuComponent(juce.Component):
    canvas = None

    def __init__(self):
        super().__init__()

        self.canvas = WgpuWidget()
        self.addAndMakeVisible(self.canvas)

        self.slider = juce.Slider()
        self.slider.setRange(1.0, 100.0, 0.1)
        self.slider.onValueChange = lambda: self.sliderChanged()
        self.addAndMakeVisible(self.slider)

        self.setSize(640, 480)
        self.setVisible(True)

    def __del__(self):
        if self.renderer:
            del self.renderer

        if self.canvas:
            del self.canvas

    def resized(self):
        bounds = self.getLocalBounds()

        sliderBounds = bounds.removeFromBottom(40).reduced(10)
        self.slider.setBounds(sliderBounds)

        self.canvas.setBounds(bounds.reduced(10))

    def sliderChanged(self):
        value = self.slider.getValue()

        if self.point_light:
            self.point_light.intensity = value

    def attachToWindow(self, window):
        self.canvas.reparentToWindow(window)

        self.frame_idx = count()  # counter for animation
        self.renderer = gfx.renderers.WgpuRenderer(self.canvas)
        self.scene = gfx.Scene()

        # Lights
        self.scene.add(gfx.AmbientLight("#fff", 0.2))
        self.directional_light = gfx.DirectionalLight("#fff", 3)
        self.directional_light.local.position = la.vec_normalize((1, 1, 1))
        self.scene.add(self.directional_light)
        self.point_light = gfx.PointLight("#fff", 3)
        self.scene.add(self.point_light)
        self.point_light.add(gfx.PointLightHelper(4))

        # Read cube image and turn it into a 3D image (a 4d array)
        env_img = iio.imread("imageio:meadow_cube.jpg")
        self.cube_size = env_img.shape[1]
        env_img.shape = 6, self.cube_size, self.cube_size, env_img.shape[-1]

        # Create environment map
        env_tex = gfx.Texture(env_img, dim=2, size=(self.cube_size, self.cube_size, 6), generate_mipmaps=True)

        # Apply env map to skybox
        self.background = gfx.Background(None, gfx.BackgroundSkyboxMaterial(map=env_tex))
        self.scene.add(self.background)

        # Now create spheres ...
        cube_width = 400
        numbers_per_side = 5
        sphere_radius = (cube_width / numbers_per_side) * 0.8 * 0.5
        step_size = 1.0 / numbers_per_side

        self.geometry = gfx.sphere_geometry(sphere_radius, 32, 16)

        index = 0
        alpha = 0.0
        while alpha <= 1.0:
            beta = 0.0
            while beta <= 1.0:
                gamma = 0.0
                while gamma <= 1.0:
                    material = gfx.MeshStandardMaterial(
                        color=hls_to_rgb(alpha, 0.5, gamma * 0.5 + 0.1),
                        metalness=beta,
                        roughness=1.0 - alpha,
                    )

                    if index % 2 != 0:
                        material.env_map = env_tex

                    mesh = gfx.Mesh(self.geometry, material)

                    mesh.local.position = (
                        alpha * 400 - 200,
                        beta * 400 - 200,
                        gamma * 400 - 200,
                    )

                    self.scene.add(mesh)
                    index += 1

                    gamma += step_size
                beta += step_size
            alpha += step_size

        # Create camera and controls
        self.camera = gfx.PerspectiveCamera(45, self.getWidth() / self.getHeight())
        self.camera.show_object(self.scene, view_dir=(-2, -1.5, -3), scale=1.2)
        self.controller = gfx.OrbitController(self.camera, register_events=self.renderer)

        def animate():
            timer = next(self.frame_idx) / 30

            self.point_light.local.position = (
                math.sin(timer / 30 * (2 * np.pi)) * 300,
                math.cos(timer * 2 / 30 * (2 * np.pi)) * 400,
                math.cos(timer / 30 * (2 * np.pi)) * 300,
            )

            self.renderer.render(self.scene, self.camera)
            self.renderer.request_draw()

        self.renderer.request_draw(animate)

    def detach(self):
        if self.canvas:
            self.canvas.removeFromDesktop()


class WgpuWindow(juce.DocumentWindow):
    component = None

    def __init__(self):
        super().__init__(
            "wgpu triangle embedded in a popsicle app",
            juce.Desktop.getInstance().getDefaultLookAndFeel()
                .findColour(juce.ResizableWindow.backgroundColourId),
            juce.DocumentWindow.allButtons,
            True)

        self.component = WgpuComponent()

        self.setUsingNativeTitleBar(True)
        self.setResizable(True, True)
        self.setContentNonOwned(self.component, True)
        self.centreWithSize(self.component.getWidth(), self.component.getHeight())
        self.setVisible(True)

        self.component.attachToWindow(self)
        self.getContentComponent().resized()

    def __del__(self):
        self.clearContentComponent()

        if self.component:
            self.component.detach()
            del self.component

    def closeButtonPressed(self):
        juce.JUCEApplication.getInstance().systemRequestedQuit()


class WgpuApplication(juce.JUCEApplication):
    window = None

    def getApplicationName(self):
        return "triangle-embed-juce"

    def getApplicationVersion(self):
        return "1.0"

    def initialise(self, commandLineParameters: str):
        self.window = WgpuWindow()

        juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

    def shutdown(self):
        if self.window:
            del self.window

    def systemRequestedQuit(self):
        self.quit()


if __name__ == "__main__":
    juce.START_JUCE_APPLICATION(WgpuApplication)
