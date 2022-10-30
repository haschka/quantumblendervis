#include<stdio.h> 
#include<stdlib.h>
#include<fcntl.h>
#include<math.h>

void usage() {
  printf("This generates volumetric raw files for blender from\n");
  printf("Gaussian Cube Files!\n");
  printf("cube2raw [file1.cube] [file2.cube] ... [filen.cube] [outfile.raw]\n");
}

int main(int argc, char** argv) {
  
  double *bigtable;
  char *smalltable;
  int sizex, sizey, sizez, sizexBYsizey;
  int sizex_old, sizey_old, sizez_old;
  int index_x,index_y, index_z, index;
  int num_of_cubes = argc-2;
  
  char buffer[200];
  int num_of_atoms;
  int i,j;
  
  double new; 
  // negative infinity
  double max= -1./0.;
  // positive infinity
  double min= -1.*max;
  double rescalefactor;
  int shift;

  FILE* cubefile;
  int rawfile;

  // process input
  
  if(argc<3) {
    printf("This program requires at least 3 arguments\n");
    usage();
    return 1;
  }
  
  rawfile = open(argv[argc-1],O_CREAT|O_WRONLY,000644);

  printf("Number of files to be processed: %i \n", num_of_cubes);

  if(rawfile == -1) {
    printf("Error in opening raw file!");
    usage();
    return 1;
  }
  
 
  for(i=0;i<num_of_cubes;i++) {
    printf(" INPUT:  %s\n", argv[i+1]);
  }
  printf("OUTPUT:  %s\n", argv[num_of_cubes+1]);

  for(i=0;i<num_of_cubes;i++) {
    cubefile = fopen(argv[i+1],"r");
    
    if(cubefile == NULL) {
      printf("Error in opening cube file!");
      usage();
      return 1; 
    }
    
    // process cube files header

    //skip the first two lines
    for(j=0;j<2;j++){
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
    
    // do some inter cube file consistency checks
    if(i>0) {
      if (sizex != sizex_old || sizey != sizey_old || sizez != sizez_old) {
	printf("Cube file dimenstions are inconsistent @ cube %i \n",i);
	usage();
	return 1; 
      }
    }
    
    for(j=0;j<num_of_atoms;j++){
      fgets(buffer, 200, cubefile);
    }
    
    printf("%lf %lf \n",max,min);  
    
    // allocate some conversion memory in the first run;
    if(i==0) {
      smalltable = (char*)malloc(sizex*sizey*sizez);
    }
    sizexBYsizey = sizex*sizey;

    // get the minimum and maximum value in order to calculate some
    // the rescalefactor
    for(index_x=0;index_x<sizex;index_x++) {
      for(index_y=0;index_y<sizey;index_y++) {
	for(index_z=0;index_z<sizez;index_z++) {
	  fscanf(cubefile,"%lf",&new);
	  
	  if(new > max) {
	    max = new;
	  }
	  if(new < min) {
	    min = new;
	  }
	}
      }
    }
    sizex_old = sizex;
    sizey_old = sizey; 
    sizez_old = sizez;

    fclose(cubefile); 
  }
  
  // rescalefactor from minimum and maximum value in all cubes
  rescalefactor = 254/(max-min);
  printf("%lf %lf \n",max,min);  

  // actual conversion taking place
  for (i=0;i<num_of_cubes;i++) {
    cubefile = fopen(argv[i+1],"r");
    
    for(j=0;j<2;j++){
      fgets(buffer, 200, cubefile);
    }
  
    fgets(buffer,200, cubefile);
    sscanf(buffer, "%i", &num_of_atoms);
    
    for(j=0;j<num_of_atoms+3;j++){
      fgets(buffer, 200, cubefile);
    }

    for(index_x=0;index_x<sizex;index_x++) {
      for(index_y=0;index_y<sizey;index_y++) {
	for(index_z=0;index_z<sizez;index_z++) {
	  fscanf(cubefile,"%lf",&new);
	  index = index_x+index_y*sizex+index_z*sizexBYsizey;
	  smalltable[index] = (char)rint((new-min)*rescalefactor);
	}
      }
    }
    write(rawfile,smalltable,sizex*sizey*sizez);    
    fclose(cubefile);
    printf("Processed File: %i \n",i);  
  }
  close(rawfile);
}

      
