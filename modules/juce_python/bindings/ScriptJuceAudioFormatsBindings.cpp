/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
 *
 * popsicle is an open source library subject to commercial or open-source licensing.
 *
 * By using popsicle, you agree to the terms of the popsicle License Agreement, which can
 * be found at https://raw.githubusercontent.com/kunitoki/popsicle/master/LICENSE
 *
 * Or: You may also use this code under the terms of the GPL v3 (see www.gnu.org/licenses).
 *
 * POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER EXPRESSED
 * OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE DISCLAIMED.
 */

#include "ScriptJuceAudioFormatsBindings.h"

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

void registerJuceAudioFormatsBindings (py::module_& m)
{
    // ============================================================================================ juce::AudioFormatReader

    py::class_<AudioFormatReader, PyAudioFormatReader<>> classAudioFormatReader (m, "AudioFormatReader");

    classAudioFormatReader
        .def (py::init<InputStream*, const String&>())
        .def ("getFormatName", &AudioFormatReader::getFormatName)
    //.def ("read", py::overload_cast<float* const*, int, juce::int64, int> (&AudioFormatReader::read))
    //.def ("read", py::overload_cast<int* const*, int, juce::int64, int, bool> (&AudioFormatReader::read))
        .def ("read", py::overload_cast<AudioBuffer<float>*, int, int, juce::int64, bool, bool> (&AudioFormatReader::read))
        .def ("readMaxLevels", py::overload_cast<juce::int64, juce::int64, Range<float>*, int> (&AudioFormatReader::readMaxLevels))
    //.def ("readMaxLevels", py::overload_cast<juce::int64, juce::int64, float&, float&, float&, float&> (&AudioFormatReader::readMaxLevels))
        .def ("searchForLevel", &AudioFormatReader::searchForLevel)
        .def_readwrite ("sampleRate", &AudioFormatReader::sampleRate)
        .def_readwrite ("bitsPerSample", &AudioFormatReader::bitsPerSample)
        .def_readwrite ("lengthInSamples", &AudioFormatReader::lengthInSamples)
        .def_readwrite ("numChannels", &AudioFormatReader::numChannels)
        .def_readwrite ("usesFloatingPointData", &AudioFormatReader::usesFloatingPointData)
        .def_readwrite ("metadataValues", &AudioFormatReader::metadataValues)
        .def_readwrite ("input", &AudioFormatReader::input)
        .def ("getChannelLayout", &AudioFormatReader::getChannelLayout)
    //.def ("readSamples", &AudioFormatReader::readSamples)
    ;

    // ============================================================================================ juce::AudioSubsectionReader

    py::class_<AudioSubsectionReader, AudioFormatReader, PyAudioFormatReader<AudioSubsectionReader>> classAudioSubsectionReader (m, "AudioSubsectionReader");

    classAudioSubsectionReader
        .def (py::init<AudioFormatReader*, int64, int64, bool>(),
            "sourceReader"_a, "subsectionStartSample"_a, "subsectionLength"_a, "deleteSourceWhenDeleted"_a = false)
    ;

    // ============================================================================================ juce::BufferingAudioReader

    py::class_<BufferingAudioReader, AudioFormatReader, PyAudioFormatReader<BufferingAudioReader>> classBufferingAudioReader (m, "BufferingAudioReader");

    classBufferingAudioReader
        .def (py::init<AudioFormatReader*, TimeSliceThread&, int>(),
            "sourceReader"_a, "timeSliceThread"_a, "samplesToBuffer"_a)
        .def ("setReadTimeout", &BufferingAudioReader::setReadTimeout)
    ;

    // ============================================================================================ juce::MemoryMappedAudioFormatReader

    py::class_<MemoryMappedAudioFormatReader, AudioFormatReader, PyMemoryMappedAudioFormatReader<>> classMemoryMappedAudioFormatReader (m, "MemoryMappedAudioFormatReader");

    classMemoryMappedAudioFormatReader
        .def (py::init<const File&, const AudioFormatReader&, int64, int64, int>(),
            "file"_a, "details"_a, "dataChunkStart"_a, "dataChunkLength"_a, "bytesPerFrame"_a)
        .def ("getFile", &MemoryMappedAudioFormatReader::getFile, py::return_value_policy::reference)
        .def ("mapEntireFile", &MemoryMappedAudioFormatReader::mapEntireFile)
        .def ("mapSectionOfFile", &MemoryMappedAudioFormatReader::mapSectionOfFile)
        .def ("getMappedSection", &MemoryMappedAudioFormatReader::getMappedSection)
        .def ("touchSample", &MemoryMappedAudioFormatReader::touchSample)
        .def ("touchSample", &MemoryMappedAudioFormatReader::touchSample)
    //.def ("getSample", &MemoryMappedAudioFormatReader::getSample)
        .def ("getNumBytesUsed", &MemoryMappedAudioFormatReader::getNumBytesUsed)
    ;

    // ============================================================================================ juce::AudioFormatReaderSource

    py::class_<AudioFormatReaderSource, PositionableAudioSource, PyPositionableAudioSource<AudioFormatReaderSource>> classAudioFormatReaderSource (m, "AudioFormatReaderSource");

    classAudioFormatReaderSource
        .def (py::init<AudioFormatReader*, bool>(),
            "sourceReader"_a, "deleteReaderWhenThisIsDeleted"_a = false)
        .def ("getAudioFormatReader", &AudioFormatReaderSource::getAudioFormatReader, py::return_value_policy::reference)
    ;

    // ============================================================================================ juce::AudioFormatWriter

    py::class_<AudioFormatWriter, PyAudioFormatWriter<>> classAudioFormatWriter (m, "AudioFormatWriter");

    classAudioFormatWriter
        .def (py::init<OutputStream*, const String&, double, unsigned int, unsigned int>(),
            "destStream"_a, "formatName"_a, "sampleRate"_a, "numberOfChannels"_a, "bitsPerSample"_a)
        .def (py::init<OutputStream*, const String&, double, const AudioChannelSet&, unsigned int>(),
            "destStream"_a, "formatName"_a, "sampleRate"_a, "audioChannelLayout"_a, "bitsPerSample"_a)
        .def ("getFormatName", &AudioFormatWriter::getFormatName)
    //.def ("write", &AudioFormatWriter::write)
        .def ("flush", &AudioFormatWriter::flush)
        .def ("writeFromAudioReader", &AudioFormatWriter::writeFromAudioReader, "reader"_a, "startSample"_a, "numSamplesToRead"_a)
        .def ("writeFromAudioSource", &AudioFormatWriter::writeFromAudioSource, "source"_a, "numSamplesToRead"_a, "samplesPerBlock"_a = 2048)
        .def ("writeFromAudioSampleBuffer", &AudioFormatWriter::writeFromAudioSampleBuffer, "source"_a, "startSample"_a, "numSamples"_a)
    //.def ("writeFromFloatArrays", &AudioFormatWriter::writeFromFloatArrays)
        .def ("getSampleRate", &AudioFormatWriter::getSampleRate)
        .def ("getNumChannels", &AudioFormatWriter::getNumChannels)
        .def ("getBitsPerSample", &AudioFormatWriter::getBitsPerSample)
        .def ("isFloatingPoint", &AudioFormatWriter::isFloatingPoint)
        .def ("writeFromAudioSampleBuffer", &AudioFormatWriter::writeFromAudioSampleBuffer)
    ;

    // ============================================================================================ juce::AudioFormat

    py::class_<AudioFormat, PyAudioFormat<>> classAudioFormat (m, "AudioFormat");

    classAudioFormat
        .def (py::init<String, StringArray>())
        .def (py::init<StringRef, StringRef>())
        .def ("getFormatName", &AudioFormat::getFormatName)
        .def ("getFileExtensions", &AudioFormat::getFileExtensions)
        .def ("canHandleFile", &AudioFormat::canHandleFile)
        .def ("getPossibleSampleRates", &AudioFormat::getPossibleSampleRates)
        .def ("getPossibleBitDepths", &AudioFormat::getPossibleBitDepths)
        .def ("canDoStereo", &AudioFormat::canDoStereo)
        .def ("canDoMono", &AudioFormat::canDoMono)
        .def ("isCompressed", &AudioFormat::isCompressed)
        .def ("isChannelLayoutSupported", &AudioFormat::isChannelLayoutSupported)
        .def ("getQualityOptions", &AudioFormat::getQualityOptions)
        .def ("createReaderFor", &AudioFormat::createReaderFor)
        .def ("createMemoryMappedReader", py::overload_cast<const File&> (&AudioFormat::createMemoryMappedReader))
        .def ("createMemoryMappedReader", py::overload_cast<FileInputStream*> (&AudioFormat::createMemoryMappedReader))
        .def ("createWriterFor", py::overload_cast<OutputStream*, double, unsigned int, int, const StringPairArray&, int> (&AudioFormat::createWriterFor))
        .def ("createWriterFor", py::overload_cast<OutputStream*, double, const AudioChannelSet&, int, const StringPairArray&, int> (&AudioFormat::createWriterFor))
    ;

    // ============================================================================================ juce::WavAudioFormat

    py::class_<WavAudioFormat, AudioFormat, PyAudioFormat<WavAudioFormat>> classWavAudioFormat (m, "WavAudioFormat");

    classWavAudioFormat
        .def (py::init<>())
        .def ("createBWAVMetadata", &WavAudioFormat::createBWAVMetadata)
        .def ("replaceMetadataInFile", &WavAudioFormat::replaceMetadataInFile)
    ;

    classWavAudioFormat.attr ("bwavDescription") = py::str (WavAudioFormat::bwavDescription);
    classWavAudioFormat.attr ("bwavOriginator") = py::str (WavAudioFormat::bwavOriginator);
    classWavAudioFormat.attr ("bwavOriginatorRef") = py::str (WavAudioFormat::bwavOriginatorRef);
    classWavAudioFormat.attr ("bwavOriginationDate") = py::str (WavAudioFormat::bwavOriginationDate);
    classWavAudioFormat.attr ("bwavOriginationTime") = py::str (WavAudioFormat::bwavOriginationTime);
    classWavAudioFormat.attr ("bwavCodingHistory") = py::str (WavAudioFormat::bwavCodingHistory);
    classWavAudioFormat.attr ("bwavTimeReference") = py::str (WavAudioFormat::bwavTimeReference);
    // TODO - missing chunk properties

    // ============================================================================================ juce::AiffAudioFormat

    py::class_<AiffAudioFormat, AudioFormat, PyAudioFormat<AiffAudioFormat>> classAiffAudioFormat (m, "AiffAudioFormat");

    classAiffAudioFormat
        .def (py::init<>())
    ;

    classAiffAudioFormat.attr ("appleOneShot") = py::str (AiffAudioFormat::appleOneShot);
    classAiffAudioFormat.attr ("appleRootSet") = py::str (AiffAudioFormat::appleRootSet);
    classAiffAudioFormat.attr ("appleRootNote") = py::str (AiffAudioFormat::appleRootNote);
    classAiffAudioFormat.attr ("appleBeats") = py::str (AiffAudioFormat::appleBeats);
    classAiffAudioFormat.attr ("appleDenominator") = py::str (AiffAudioFormat::appleDenominator);
    classAiffAudioFormat.attr ("appleNumerator") = py::str (AiffAudioFormat::appleNumerator);
    classAiffAudioFormat.attr ("appleTag") = py::str (AiffAudioFormat::appleTag);
    classAiffAudioFormat.attr ("appleKey") = py::str (AiffAudioFormat::appleKey);

#if JUCE_USE_MP3AUDIOFORMAT
    // ============================================================================================ juce::MP3AudioFormat

    py::class_<MP3AudioFormat, AudioFormat, PyAudioFormat<MP3AudioFormat>> classMP3AudioFormat (m, "MP3AudioFormat");

    classMP3AudioFormat
        .def (py::init<>())
    ;
#endif

#if JUCE_USE_LAME_AUDIO_FORMAT
    // ============================================================================================ juce::LAMEEncoderAudioFormat

    py::class_<LAMEEncoderAudioFormat, AudioFormat, PyAudioFormat<LAMEEncoderAudioFormat>> classLAMEEncoderAudioFormat (m, "LAMEEncoderAudioFormat");

    classLAMEEncoderAudioFormat
        .def (py::init<const File&>(), "lameExecutableToUse"_a)
    ;
#endif

#if JUCE_USE_OGGVORBIS
    // ============================================================================================ juce::OggVorbisAudioFormat

    py::class_<OggVorbisAudioFormat, AudioFormat, PyAudioFormat<OggVorbisAudioFormat>> classOggVorbisAudioFormat (m, "OggVorbisAudioFormat");

    classOggVorbisAudioFormat
        .def (py::init<>())
        .def ("estimateOggFileQuality", &OggVorbisAudioFormat::estimateOggFileQuality)
    ;

    classOggVorbisAudioFormat.attr ("encoderName") = py::str (OggVorbisAudioFormat::encoderName);
    classOggVorbisAudioFormat.attr ("id3title") = py::str (OggVorbisAudioFormat::id3title);
    classOggVorbisAudioFormat.attr ("id3artist") = py::str (OggVorbisAudioFormat::id3artist);
    classOggVorbisAudioFormat.attr ("id3album") = py::str (OggVorbisAudioFormat::id3album);
    classOggVorbisAudioFormat.attr ("id3comment") = py::str (OggVorbisAudioFormat::id3comment);
    classOggVorbisAudioFormat.attr ("id3date") = py::str (OggVorbisAudioFormat::id3date);
    classOggVorbisAudioFormat.attr ("id3genre") = py::str (OggVorbisAudioFormat::id3genre);
    classOggVorbisAudioFormat.attr ("id3trackNumber") = py::str (OggVorbisAudioFormat::id3trackNumber);
#endif

#if JUCE_USE_FLAC
    // ============================================================================================ juce::FlacAudioFormat

    py::class_<FlacAudioFormat, AudioFormat, PyAudioFormat<FlacAudioFormat>> classFlacAudioFormat (m, "FlacAudioFormat");

    classFlacAudioFormat
        .def (py::init<>())
    ;
#endif

#if JUCE_MAC
    // ============================================================================================ juce::CoreAudioFormat

    py::class_<CoreAudioFormat, AudioFormat, PyAudioFormat<CoreAudioFormat>> classCoreAudioFormat (m, "CoreAudioFormat");

    py::enum_<CoreAudioFormat::StreamKind> (classCoreAudioFormat, "StreamKind")
        .value ("kNone", CoreAudioFormat::StreamKind::kNone)
        .value ("kAiff", CoreAudioFormat::StreamKind::kAiff)
        .value ("kAifc", CoreAudioFormat::StreamKind::kAifc)
        .value ("kWave", CoreAudioFormat::StreamKind::kWave)
        .value ("kSoundDesigner2", CoreAudioFormat::StreamKind::kSoundDesigner2)
        .value ("kNext", CoreAudioFormat::StreamKind::kNext)
        .value ("kMp3", CoreAudioFormat::StreamKind::kMp3)
        .value ("kMp2", CoreAudioFormat::StreamKind::kMp2)
        .value ("kMp1", CoreAudioFormat::StreamKind::kMp1)
        .value ("kAc3", CoreAudioFormat::StreamKind::kAc3)
        .value ("kAacAdts", CoreAudioFormat::StreamKind::kAacAdts)
        .value ("kMpeg4", CoreAudioFormat::StreamKind::kMpeg4)
        .value ("kM4a", CoreAudioFormat::StreamKind::kM4a)
        .value ("kM4b", CoreAudioFormat::StreamKind::kM4b)
        .value ("kCaf", CoreAudioFormat::StreamKind::kCaf)
        .value ("k3gp", CoreAudioFormat::StreamKind::k3gp)
        .value ("k3gp2", CoreAudioFormat::StreamKind::k3gp2)
        .value ("kAmr", CoreAudioFormat::StreamKind::kAmr)
    ;

    classCoreAudioFormat
        .def (py::init<>())
        .def (py::init<CoreAudioFormat::StreamKind>())
    ;

    classCoreAudioFormat.attr ("midiDataBase64") = py::str (CoreAudioFormat::midiDataBase64);
    classCoreAudioFormat.attr ("tempo") = py::str (CoreAudioFormat::tempo);
    classCoreAudioFormat.attr ("timeSig") = py::str (CoreAudioFormat::timeSig);
    classCoreAudioFormat.attr ("keySig") = py::str (CoreAudioFormat::keySig);
#endif

#if JUCE_WINDOWS
    // ============================================================================================ juce::WindowsMediaAudioFormat

    py::class_<WindowsMediaAudioFormat, AudioFormat, PyAudioFormat<WindowsMediaAudioFormat>> classWindowsMediaAudioFormat (m, "WindowsMediaAudioFormat");

    classWindowsMediaAudioFormat
        .def (py::init<>())
    ;
#endif

    // ============================================================================================ juce::AudioFormatManager

    py::class_<AudioFormatManager> classAudioFormatManager (m, "AudioFormatManager");

    classAudioFormatManager
        .def (py::init<>())
        .def ("registerFormat", &AudioFormatManager::registerFormat)
        .def ("registerBasicFormats", &AudioFormatManager::registerBasicFormats)
        .def ("clearFormats", &AudioFormatManager::clearFormats)
        .def ("getNumKnownFormats", &AudioFormatManager::getNumKnownFormats)
        .def ("getKnownFormat", &AudioFormatManager::getKnownFormat, py::return_value_policy::reference)
        .def ("__iter__", [](AudioFormatManager& self)
        {
            return py::make_iterator (self.begin(), self.end());
        }, py::return_value_policy::reference)
        .def ("findFormatForFileExtension", &AudioFormatManager::findFormatForFileExtension, py::return_value_policy::reference)
        .def ("getDefaultFormat", &AudioFormatManager::getDefaultFormat, py::return_value_policy::reference)
        .def ("getWildcardForAllFormats", &AudioFormatManager::getWildcardForAllFormats)
        .def ("createReaderFor", py::overload_cast<const File&> (&AudioFormatManager::createReaderFor))
    //.def ("createReaderFor", py::overload_cast<std::unique_ptr<InputStream>> (&AudioFormatManager::createReaderFor))
    ;
}

} // namespace popsicle::Bindings
