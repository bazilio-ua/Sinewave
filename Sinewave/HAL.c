/*
 *  HAL.c
 *  Sinewave
 *
 *  Created by james on Fri Apr 27 2001.
 *  Copyright (c) 2001 __CompanyName__. All rights reserved.
 *
 */

#include "HAL.h"
#include "math.h"

appGlobals gAppGlobals;

OSStatus appIOProc (AudioDeviceID  inDevice, const AudioTimeStamp*  inNow, const AudioBufferList*   inInputData, 
                             const AudioTimeStamp*  inInputTime, AudioBufferList*  outOutputData, const AudioTimeStamp* inOutputTime, 
                             void* device);


#define FailIf(cond, handler)						\
	if (cond) {										\
		goto handler;								\
	}

#define FailWithAction(cond, action, handler)		\
	if (cond) {										\
		{ action; }									\
		goto handler;								\
	}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HAL Sample Code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define noErr 0
//#define false 0

OSStatus	SetupHAL (appGlobalsPtr	globals)
{
    OSStatus				err = noErr;
    UInt32				count,
                                        bufferSize;
    AudioDeviceID			device = kAudioDeviceUnknown;
    AudioStreamBasicDescription		format;
    
    // get the default output device for the HAL
    count = sizeof(globals->device);					// it is required to pass the size of the data to be returned
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,  &count, (void *) &device);
    fprintf(stderr, "kAudioHardwarePropertyDefaultOutputDevice %d\n", err);
    if (err != noErr) goto Bail;
    
    // get the buffersize that the default device uses for IO
    count = sizeof(globals->deviceBufferSize);				// it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyBufferSize, &count, &bufferSize);
    fprintf(stderr, "kAudioDevicePropertyBufferSize %d %d\n", err, bufferSize);
    if (err != noErr) goto Bail;
   
    // get a description of the data format used by the default device
    count = sizeof(globals->deviceFormat);				// it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyStreamFormat, &count, &format);
    fprintf(stderr, "kAudioDevicePropertyStreamFormat %d\n", err);
    fprintf(stderr, "sampleRate %g\n", format.mSampleRate);
    fprintf(stderr, "mFormatFlags %08X\n", format.mFormatFlags);
    fprintf(stderr, "mBytesPerPacket %d\n", format.mBytesPerPacket);
    fprintf(stderr, "mFramesPerPacket %d\n", format.mFramesPerPacket);
    fprintf(stderr, "mChannelsPerFrame %d\n", format.mChannelsPerFrame);
    fprintf(stderr, "mBytesPerFrame %d\n", format.mBytesPerFrame);
    fprintf(stderr, "mBitsPerChannel %d\n", format.mBitsPerChannel);
    if (err != kAudioHardwareNoError) goto Bail;
    FailWithAction(format.mFormatID != kAudioFormatLinearPCM, err = paramErr, Bail);	// bail if the format is not linear pcm

    // everything is ok so fill in these globals
    globals->device = device;
    globals->deviceBufferSize = bufferSize;
    globals->deviceFormat = format;

Bail:
    return (err);
}

/*
struct AudioStreamBasicDescription
{
	Float64	mSampleRate;		//	the native sample rate of the audio stream
	UInt32	mFormatID;			//	the specific encoding type of audio stream
	UInt32	mFormatFlags;		//	flags specific to each format
	UInt32	mBytesPerPacket;	//	the number of bytes in a packet
	UInt32	mFramesPerPacket;	//	the number of frames in each packet
	UInt32	mBytesPerFrame;		//	the number of bytes in a frame
	UInt32	mChannelsPerFrame;	//	the number of channels in each frame
	UInt32	mBitsPerChannel;	//	the number of bits in each channel
};
typedef struct AudioStreamBasicDescription	AudioStreamBasicDescription;
*/
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This is a simple playThru ioProc. It simply places the data in the input buffer back into the output buffer.
// Watch out for feedback from Speakers to Microphone
OSStatus appIOProc (AudioDeviceID  inDevice, const AudioTimeStamp*  inNow, const AudioBufferList*   inInputData, 
                             const AudioTimeStamp*  inInputTime, AudioBufferList*  outOutputData, const AudioTimeStamp* inOutputTime, 
                             void* appGlobals)
{    
    appGlobalsPtr	globals = appGlobals;
    int i;
    double phase = gAppGlobals.phase;
    double amp = gAppGlobals.amp;
    double pan = gAppGlobals.pan;
    double freq = gAppGlobals.freq * 2. * 3.14159265359 / globals->deviceFormat.mSampleRate;
    
    int numSamples = globals->deviceBufferSize / globals->deviceFormat.mBytesPerFrame;
    // assume floats for now....
    float *out = outOutputData->mBuffers[0].mData;
    for (i=0; i<numSamples; ++i) {
        float wave = sin(phase) * amp;
        phase = phase + freq;
        *out++ = wave * (1.0-pan);
        *out++ = wave * pan;
    }
    gAppGlobals.phase = phase;
    return (kAudioHardwareNoError);     
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus  StartPlayingThruHAL(appGlobalsPtr	globals)
{
    OSStatus		err = kAudioHardwareNoError;
   
    if (globals->soundPlaying) return 0;
    globals->phase = 0.0;

    err = AudioDeviceAddIOProc(globals->device, appIOProc, (void *) globals);	// setup our device with an IO proc
    if (err != kAudioHardwareNoError) goto Bail;
    
    err = AudioDeviceStart(globals->device, appIOProc);				// start playing sound through the device
    if (err != kAudioHardwareNoError) goto Bail;

    globals->soundPlaying = true;						// set the playing status global to true

Bail:
    return (err);
} 

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus  StopPlayingThruHAL(appGlobalsPtr	globals)
{
    OSStatus 	err = kAudioHardwareNoError;
    
    if (!globals->soundPlaying) return 0;
    err = AudioDeviceStop(globals->device, appIOProc);				// stop playing sound through the device
    if (err != kAudioHardwareNoError) goto Bail;

    err = AudioDeviceRemoveIOProc(globals->device, appIOProc);			// remove the IO proc from the device
    if (err != kAudioHardwareNoError) goto Bail;
    
    globals->soundPlaying = false;						// set the playing status global to false

Bail:
    return (err);
} 


