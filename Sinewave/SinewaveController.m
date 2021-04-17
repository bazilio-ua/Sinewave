//
// a very simple Cocoa CoreAudio app
// by James McCartney  james@audiosynth.com  www.audiosynth.com
//
// SinewaveController - this controller class manages the GUI and forwards actions to the Sinewave oscillator.
//

#import "SinewaveController.h"
#import "Sinewave.h"

@implementation SinewaveController

- (void)awakeFromNib
{
    [oscillator setup];	// this will initialize our CoreAudio data
    [[ampControl window] makeKeyAndOrderFront: self];	// window to front
    [self setFreq: freqControl];	// setup static text fields
    [self setPan: panControl];
    [self setAmp: ampControl];
}

- (IBAction)run:(id)sender
{
    [oscillator start];
}

- (IBAction)setAmp:(id)sender
{
    double amp = [ampControl doubleValue];
    [ampField setDoubleValue: amp];
    [oscillator setAmpVal: amp];
}

- (IBAction)setFreq:(id)sender
{
    double freq = [freqControl doubleValue];
    [freqField setDoubleValue: freq];
    [oscillator setFreqVal: freq];
}

- (IBAction)setPan:(id)sender
{
    double pan = [panControl doubleValue];
    [panField setDoubleValue: pan];
    [oscillator setPanVal: pan];
}

- (IBAction)stop:(id)sender
{
    [oscillator stop];
}

@end
