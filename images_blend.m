//
// Copyright(c) 2009 Thomas Haschka
// http://haschka.dyndns.org
//
// This file may be licensed under the terms of of the
// GNU General Public License Version 3 (the ``GPL'').
//
// Software distributed under the License is distributed
// on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied. See the GPL for the specific language
// governing rights and limitations.
//
// You should have received a copy of the GPL along with this
// program. If not, go to http://www.gnu.org/licenses/gpl.html
// or write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

// Overblends images using Mac OS X CoreImage technology. 
// Requires a Mac OS X system > 10.5

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <AppKit/AppKit.h>
#import	<stdio.h>

void description(){
  printf("blend [spaceseperated filelist] outputfile.png\n\n");
  printf("You need to define at least two (2) input files \n");
  printf("and one (1) output file.\n");
}

int main(int argc, char *argv[]) {
  
  int i;
  if (argc<4) {
    description();
    return 1;
  }
  
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  
  NSMutableArray* images = [[NSMutableArray alloc] init];
  
  NSString* path = [NSString alloc];
  NSString* outpath = 
    [[NSString alloc] initWithCString:argv[argc-1] encoding:4];
  
  NSURL* url = [NSURL alloc];
  CIImage* imagebuffer = [CIImage alloc];
  CIImage* result = [CIImage alloc];
  CIImage* downvaluedimage = [CIImage alloc];
  
  CIFilter* addfilter = [CIFilter alloc];
  
  CIFilter* divfilter = [CIFilter alloc];
  
  // Filtermatrix definitions
  
  CIVector* rdiv = [[CIVector alloc] initWithX:1.f Y:0. Z:0. W:0.];
  CIVector* gdiv = [[CIVector alloc] initWithX:0. Y:1.f Z:0. W:0.];
  CIVector* bdiv = [[CIVector alloc] initWithX:0. Y:0. Z:1.f W:0.];
  CIVector* adiv = 
    [[CIVector alloc] initWithX:0. Y:0. Z:0 W:1.f/(float)(argc-2)];
  CIVector* biasvector = [[CIVector alloc] initWithX:0 Y:0 Z:0. W:0.];
		
  for (i=1;i<argc-1;i++){
    url = [NSURL fileURLWithPath:[path initWithCString:argv[i] encoding:4]];
    imagebuffer = [CIImage imageWithContentsOfURL:url]; 
    [images addObject:imagebuffer];
  }
  
  result = [images objectAtIndex:0];
  
  divfilter = [CIFilter filterWithName:@"CIColorMatrix"];
  
  [divfilter setValue:rdiv forKey:@"inputRVector"];
  [divfilter setValue:gdiv forKey:@"inputGVector"];
  [divfilter setValue:bdiv forKey:@"inputBVector"];
  [divfilter setValue:adiv forKey:@"inputAVector"];
  
  [divfilter setValue:biasvector forKey:@"inputBiasVector"];
  [divfilter setValue:result forKey:@"inputImage"];
  
  result = [divfilter valueForKey:@"outputImage"];
  
  for (i=1;i<argc-2;i++) {
    addfilter = [CIFilter filterWithName:@"CIAdditionCompositing"];
    [divfilter setValue:[images objectAtIndex:i] forKey:@"inputImage"];
    downvaluedimage = [divfilter valueForKey:@"outputImage"];
    [addfilter setValue:downvaluedimage forKey:@"inputImage"];
    [addfilter setValue:result forKey:@"inputBackgroundImage"];
    
    
    result = [addfilter valueForKey:@"outputImage"];
  }
  
  NSBitmapImageRep* imagedata = 
    [[NSBitmapImageRep alloc] initWithCIImage:result];
  
  [[imagedata representationUsingType:NSPNGFileType properties:nil] 
    writeToFile:outpath
     atomically:NO];
  
  [pool release];
  return 0;
}


