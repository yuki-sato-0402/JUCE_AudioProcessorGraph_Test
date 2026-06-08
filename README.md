# JUCE_AudioProcessorGraph_Test
Test of AudioProcessorGraph Class.
You can rearrange the order of EffectSlots by dragging and dropping them.
Each effect ([FIR Filter](https://docs.juce.com/master/classjuce_1_1dsp_1_1FIR_1_1Filter.html), [Delay](https://docs.juce.com/master/classjuce_1_1dsp_1_1DelayLine.html), [Reverb](https://docs.juce.com/master/classjuce_1_1dsp_1_1Reverb.html), [Compressor](https://docs.juce.com/master/classjuce_1_1dsp_1_1Compressor.html))  utilizes Juce's DSP module.

<img width="648" height="474" alt="Screenshot 2026-06-08 at 23 25 14" src="https://github.com/user-attachments/assets/dfc0973a-52e4-422d-8632-7ecf138f19c3" />

## 🛠️ Build Instructions
```
cd JUCE_AudioProcessorGraph_Test
git submodule update --init --recursive
cd build
cmake ..
cmake --build .
```

## Reference
- [Tutorial: Cascading plug-in effects](https://juce.com/tutorials/tutorial_audio_processor_graph/)

- [designFIRLowpassWindowMethod()](https://docs.juce.com/master/structdsp_1_1FilterDesign.html#a7de8d8793286c45e9a83c199de896b6e)

- [JUCE/examples/DSP/FIRFilterDemo.h](https://github.com/juce-framework/JUCE/blob/master/examples/DSP/FIRFilterDemo.h)

- [Juce Tutorial 32- Building a Filter Plugin Using the DSP Module (FIR Filter)](https://www.youtube.com/watch?v=nTVXy35rG4Q&ab_channel=TheAudioProgrammer)
