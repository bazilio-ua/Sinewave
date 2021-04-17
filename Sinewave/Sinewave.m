//
// a very simple Cocoa CoreAudio app
// by James McCartney  james@audiosynth.com  www.audiosynth.com
//
// Sinewave - this class implements a sine oscillator with dezippered control of frequency, pan and amplitude
//

#import "Sinewave.h"

// this is the audio processing callback.
OSStatus appIOProc (AudioDeviceID  inDevice, const AudioTimeStamp*  inNow, const AudioBufferList*   inInputData, 
                             const AudioTimeStamp*  inInputTime, AudioBufferList*  outOutputData, const AudioTimeStamp* inOutputTime, 
                             void* defptr)
{    
    Sinewave *def = defptr; // get access to Sinewave's data
    int i;
    
    // load instance vars into registers
    double phase = def->phase;
    double amp = def->amp;
    double pan = def->pan;
    double freq = def->freq;

    double ampz = def->ampz;
    double panz = def->panz;
    double freqz = def->freqz;
    
    int numSamples = def->deviceBufferSize / def->deviceFormat.mBytesPerFrame;
    
    // assume floats for now....
    float *out = outOutputData->mBuffers[0].mData;
    
    for (i=0; i<numSamples; ++i) {
    
        float wave = sin(phase) * ampz;		// generate sine wave
        phase = phase + freqz;			// increment phase
        
        // write output
        *out++ = wave * (1.0-panz);		// left channel
        *out++ = wave * panz;			// right channel
        
        // de-zipper controls
        panz  = 0.001 * pan  + 0.999 * panz;
        ampz  = 0.001 * amp  + 0.999 * ampz;
        freqz = 0.001 * freq + 0.999 * freqz;
    }
    
    // save registers back to object
    def->phase = phase;
    def->freqz = freqz;
    def->ampz = ampz;
    def->panz = panz;
    
    return kAudioHardwareNoError;     
}


@implementation Sinewave

- (void) setup
{
    OSStatus				err = kAudioHardwareNoError;
    UInt32				count;    
    device = kAudioDeviceUnknown;
 
    initialized = NO;
   
    // get the default output device for the HAL
    count = sizeof(device);		// it is required to pass the size of the data to be returned
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,  &count, (void *) &device);
    if (err != kAudioHardwareNoError) {
    	fprintf(stderr, "get kAudioHardwarePropertyDefaultOutputDevice error %ld\n", err);
        return;
    }
    
    // get the buffersize that the default device uses for IO
    count = sizeof(deviceBufferSize);	// it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyBufferSize, &count, &deviceBufferSize);
    if (err != kAudioHardwareNoError) {
    	fprintf(stderr, "get kAudioDevicePropertyBufferSize error %ld\n", err);
        return;
    }
    fprintf(stderr, "deviceBufferSize = %ld\n", deviceBufferSize);
   
    // get a description of the data format used by the default device
    count = sizeof(deviceFormat);	// it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyStreamFormat, &count, &deviceFormat);
    if (err != kAudioHardwareNoError) {
    	fprintf(stderr, "get kAudioDevicePropertyStreamFormat error %ld\n", err);
        return;
    }
    if (deviceFormat.mFormatID != kAudioFormatLinearPCM) {
    	fprintf(stderr, "mFormatID !=  kAudioFormatLinearPCM\n");
        return;
    }
    if (!(deviceFormat.mFormatFlags & kLinearPCMFormatFlagIsFloat)) {
    	fprintf(stderr, "Sorry, currently only works with float format....\n");
        return;
    }
    
    initialized = YES;

    fprintf(stderr, "mSampleRate = %g\n", deviceFormat.mSampleRate);
    fprintf(stderr, "mFormatFlags = %08lX\n", deviceFormat.mFormatFlags);
    fprintf(stderr, "mBytesPerPacket = %ld\n", deviceFormat.mBytesPerPacket);
    fprintf(stderr, "mFramesPerPacket = %ld\n", deviceFormat.mFramesPerPacket);
    fprintf(stderr, "mChannelsPerFrame = %ld\n", deviceFormat.mChannelsPerFrame);
    fprintf(stderr, "mBytesPerFrame = %ld\n", deviceFormat.mBytesPerFrame);
    fprintf(stderr, "mBitsPerChannel = %ld\n", deviceFormat.mBitsPerChannel);
}


- (void)setAmpVal:(double)val
{
    amp = val;
}

- (void)setFreqVal:(double)val
{
    freq = val * 2. * 3.14159265359 / deviceFormat.mSampleRate;
}

- (void)setPanVal:(double)val
{
    pan = val;
}

- (BOOL)start
{
    OSStatus		err = kAudioHardwareNoError;

    if (!initialized) return false;
    if (soundPlaying) return false;
    
    // initialize phase and de-zipper filters.
    phase = 0.0;
    freqz = freq;
    ampz = amp;
    panz = pan;

    err = AudioDeviceAddIOProc(device, appIOProc, (void *) self);	// setup our device with an IO proc
    if (err != kAudioHardwareNoError) return false;
    
    err = AudioDeviceStart(device, appIOProc);				// start playing sound through the device
    if (err != kAudioHardwareNoError) return false;

    soundPlaying = true;						// set the playing status global to true
    return true;
}

- (BOOL)stop
{
    OSStatus 	err = kAudioHardwareNoError;
    
    if (!initialized) return false;
    if (!soundPlaying) return false;
    
    err = AudioDeviceStop(device, appIOProc);				// stop playing sound through the device
    if (err != kAudioHardwareNoError) return false;

    err = AudioDeviceRemoveIOProc(device, appIOProc);			// remove the IO proc from the device
    if (err != kAudioHardwareNoError) return false;
    
    soundPlaying = false;						// set the playing status global to false
    return true;
}

@end
