//
// a very simple Cocoa CoreAudio app
// by James McCartney  james@audiosynth.com  www.audiosynth.com
//
// SinewaveController - this controller class manages the GUI and forwards actions to the Sinewave oscillator.
//

#import <Cocoa/Cocoa.h>

@interface SinewaveController : NSObject
{
    IBOutlet id ampControl;
    IBOutlet id ampField;
    IBOutlet id freqControl;
    IBOutlet id freqField;
    IBOutlet id oscillator;
    IBOutlet id panControl;
    IBOutlet id panField;
}
- (IBAction)run:(id)sender;
- (IBAction)setAmp:(id)sender;
- (IBAction)setFreq:(id)sender;
- (IBAction)setPan:(id)sender;
- (IBAction)stop:(id)sender;
@end
