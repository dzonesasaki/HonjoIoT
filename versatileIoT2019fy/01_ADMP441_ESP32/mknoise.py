import wave
import numpy as np

fnames='noise.wav'
fs=44100
timlen=10 #sec

N=fs*timlen
nstat = np.random.RandomState()
stream=nstat.randn(N)
stream= np.int16(stream / np.max(np.abs(stream)) * 32767)
objw = wave.Wave_write(fnames)
objw.setnchannels(1)
objw.setsampwidth(2)
objw.setframerate(fs)
objw.writeframes(stream)
objw.close()

