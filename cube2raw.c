#include<stdio.h> 
#include<stdlib.h>
#include<fcntl.h>
#include<math.h>

void usage() {
  printf("This generates volumetric raw files for blender from\n");
  printf("Gaussian Cube Files!\n");
  printf("cube2raw [cubefile.cube] [outputfile.raw] number_of_frames\n");
}

int main(int argc, char** argv) {
  
  double *bigtable;
  char *smalltable;
  int sizex, sizey, sizez, sizexBYsizey;
  int index_x,index_y, index_z, index;
  int num_frames;
  
  char buffer[200];
  int num_of_atoms;
  int i;
  
  double new; 
  // negative infinity
  double max= -1./0.;
  // positive infinity
  double min= -1.*max;
  double rescalefactor;
  int shift;
  
  // process input

  if(argc!=4) {
    usage();
    return 1;
  }
  
  FILE* cubefile = fopen(argv[1],"r");
  int rawfile = open(argv[2],O_CREAT|O_WRONLY,000644);
  printf(" INPUT:  %s\n", argv[1]);
  printf("OUTPUT:  %s\n", argv[2]);
  printf("FRAMES:  %s\n", argv[3]);
  
  if(cubefile == NULL) {
    printf("Error in opening cube file!");
    usage();
    return 1; 
  }

  if(rawfile == -1) {
    printf("Error in opening raw file!");
    usage();
    return 1;
  }

  sscanf(argv[3], "%i",&num_frames);

  // process cube files header

  //skip the first two lines
  for(i=0;i<2;i++){
    fgets(buffer, 200, cubefile);
  }
  
  fgets(buffer,200, cubefile);
  sscanf(buffer, "%i", &num_of_atoms);
  
  fgets(buffer, 200, cubefile);
  sscanf(buffer, "%i", &sizex);
  fgets(buffer, 200, cubefile);
  sscanf(buffer, "%i", &sizey);
  fgets(buffer, 200, cubefile);
  sscanf(buffer, "%i", &sizez);
  
  // skip atom information

  for(i=0;i<num_of_atoms;i++){
    fgets(buffer, 200, cubefile);
  }
  
  // conversion

  printf("%lf %lf \n",max,min);  

  bigtable = (double*)malloc(sizeof(double)*sizex*sizey*sizez);
  smalltable = (char*)malloc(sizex*sizey*sizez);
  
  sizexBYsizey = sizex*sizey;

  for(index_x=0;index_x<sizex;index_x++) {
    for(index_y=0;index_y<sizey;index_y++) {
      for(index_z=0;index_z<sizez;index_z++) {
	fscanf(cubefile,"%lf",&new);
	new = atan(new);
	bigtable[index_x+index_y*sizex+index_z*sizexBYsizey] = new;
	if(new > max) {
	  max = new;
	}
	if(new < min) {
	  min = new;
	}
      }
    }
  }
  
  rescalefactor = 254./(fabs(max-min));
  printf("%lf %lf \n",max,min);  
  for(index_x=0;index_x<sizex;index_x++) {
    for(index_y=0;index_y<sizey;index_y++) {
      for(index_z=0;index_z<sizez;index_z++) {
	index = index_x+index_y*sizex+index_z*sizexBYsizey;
	smalltable[index] = (char)rint((bigtable[index]-min)*rescalefactor);
      }
    }
  }
  for(i=0;i<num_frames;i++) {
    write(rawfile,smalltable,sizex*sizey*sizez);
  }
  fclose(cubefile);
  close(rawfile);
}

      
