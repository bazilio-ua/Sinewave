/*
 *  HAL.h
 *  Sinewave
 *
 *  Created by james on Fri Apr 27 2001.
 *  Copyright (c) 2001 __CompanyName__. All rights reserved.
 *
 */

#include <CoreServices/CoreServices.h>
#include <CoreAudio/AudioHardware.h>

typedef struct {
        double freq, pan, amp;
        double phase;
	Boolean		soundPlaying;
	AudioDeviceID	device;			// the default device
	UInt32		deviceBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
        AudioStreamBasicDescription	deviceFormat;	// info about the default device
} appGlobals, *appGlobalsPtr, **appGlobalsHandle;	

extern appGlobals gAppGlobals;

OSStatus  SetupHAL (appGlobalsPtr  globals);
OSStatus  StartPlayingThruHAL(appGlobalsPtr globals);
OSStatus  StopPlayingThruHAL(appGlobalsPtr globals);
