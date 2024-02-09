import nimpy
import nimpy/raw_buffers
import std/[math, random]

proc `+`[T](a: ptr T, b: int): ptr T =
    cast[ptr T](cast[uint](a) + cast[uint](b * a[].sizeof))

proc process_output(a: PyObject, numSamples: int, gain: float, t: float): float {.exportpy.} =
    var buffer: RawPyBuffer
    a.getBuffer(buffer, PyBUF_WRITABLE or PyBUF_ND)

    var p = cast[ptr float32](buffer.buf)
    var time = t

    for i in 0 ..< numSamples:
        p[] = (time.degToRad().sin() + (rand(2.0) - 1.0) * 0.125) * 0.5 * gain
        p = p + 1
        time += 2.0

    buffer.release()

    return time
