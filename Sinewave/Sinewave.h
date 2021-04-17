//
// a very simple Cocoa CoreAudio app
// by James McCartney  james@audiosynth.com  www.audiosynth.com
//
// Sinewave - this class implements a sine oscillator with dezippered control of frequency, pan and amplitude
//

#import <Cocoa/Cocoa.h>
#include <CoreAudio/AudioHardware.h>

@interface Sinewave : NSObject
{
    @public
        double freq, pan, amp;		// controls
        double freqz, panz, ampz;	// for dezipper filter
        double phase;			// oscillator phase in radians
        
	Boolean		initialized;	// successful init?
	Boolean		soundPlaying;	// playing now?
	AudioDeviceID	device;		// the default device
	UInt32	deviceBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
        AudioStreamBasicDescription	deviceFormat;	// info about the default device
}

- (void)setup;
- (BOOL)start;
- (BOOL)stop;

- (void)setAmpVal:(double)val;
- (void)setFreqVal:(double)val;
- (void)setPanVal:(double)val;
@end
