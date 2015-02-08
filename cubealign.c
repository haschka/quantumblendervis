#include<stdio.h>
#include<stdlib.h>

#ifdef __APPLE__
#include<vecLib/vecLib.h> 
#else
#include<clapack.h>
#endif

typedef struct{
  double* eigenvectors;
  double* eigenvalues;
} eigenspace;

eigenspace diagonalize(double* tensor) {

  int i;
  int matrixSize = 9;
  eigenspace space;

  // variables passed to lapack routine

  char wantEigenvectors = 'V';
  char upperTriangle = 'U';
  int order = 3;
  double* IO = (double*)malloc(matrixSize*sizeof(double));
  double* eigenvalues = (double*)malloc(sizeof(double)*matrixSize);
  double* work = (double*)malloc(sizeof(double)*37);
  int* iwork = (int*)malloc(sizeof(int)*18);
  int worklength = 37;
  int iworklength = 18;
  int info;

  // end of variables passed to lapack routine

  for(i=0;i<9;i++){
    IO[i] = tensor[i];
  }

  dsyevd_(&wantEigenvectors,
          &upperTriangle,
          &order,
          IO,
          &order,
          eigenvalues,
          work,
          &worklength,
          iwork,
          &iworklength,
          &info);
  
  if (info != 0) {
    printf("Warning! Diagonalisation was not successful!\n");
  }

  free(iwork);
  free(work);

  space.eigenvectors = IO;
  space.eigenvalues = eigenvalues;  
  
  return(space);
}

void transpose(double* tensor, double* transpose) {
  
  // 0 1 2     0 3 6
  // 3 4 5 ==> 1 4 7 
  // 6 7 8     2 5 8

  transpose[0]=tensor[0];
  transpose[1]=tensor[3];
  transpose[2]=tensor[6];

  transpose[3]=tensor[1];
  transpose[4]=tensor[4];
  transpose[5]=tensor[7];

  transpose[6]=tensor[2];
  transpose[7]=tensor[5];
  transpose[8]=tensor[8];
}

void usage() {
  printf("This program generates cubegen format input files \n");
  printf("By rotating your molecule so that it's principal axes \n");
  printf("are the bases of your coordiante system. \n");
  printf("======================================================\n");
  printf("This should allow you to have comparible cubes \n");
  printf("cubealign [input.cube] [outformat] [exceptions] \n");
  printf("   generates a cube with axeslengths and stepsizes as \n");
  printf("   in the input.cube file\n");
  printf("cubealign [input.cube] [outformat] [exceptions] steps stepsize\n");
  printf("   generates a cubic cube with [steps] steps of [stepsize] in\n");
  printf("   new x,y,z axes \n");
  printf("cubealign [i.cube] [o.format] [excpt] xs ys zs xsize ysize zsize\n");
  printf("   generates a cuboide with [xs] steps of [xsize] in new x axes\n");
  printf("                            [ys] steps of [ysize] in new y axes\n");
  printf("                            [zs] steps of [zsize] in new z axes\n");
}

int main(int argc, char** argv) {
  
  double* x;
  double* y;
  double* z;

  char buffer[80];
  
  int* exceptions;

  FILE* exceptions_file=fopen(argv[3],"r");
  FILE* cubefile=fopen(argv[1],"r");;
  FILE* gaussian_format=fopen(argv[2],"w+");

  int number_of_exceptions=0;
  
  int cubefile_num_of_atoms, num_of_atoms,sizex,sizey,sizez;
  double in_center_x, in_center_y, in_center_z;
  double out_center_x, out_center_y, out_center_z;
  double xx,xy,xz,yx,yy,yz,zx,zy,zz;
  double xxo,xyo,xzo,yxo,yyo,yzo,zxo,zyo,zzo;

  double center_x, center_y, center_z;
  double sumx,sumy,sumz;

  int atomtype_in;
  double charge_in, x_in, y_in, z_in;
  double tensor_in[9];
  
  eigenspace space;

  char exceptor;

  int i,j,k;

  if (argc != 4 && argc != 6 && argc != 11) {
    usage();
    return(1);
  }
  
  // Parsing Exceptions File
  
  while( fgets(buffer,80,exceptions_file) ) {
    number_of_exceptions++;
  }
  rewind(exceptions_file);
  
  exceptions = (int*)malloc(number_of_exceptions*sizeof(int));

  for(i=0;i<number_of_exceptions;i++) {
    fscanf(exceptions_file,"%i",exceptions+i);
  }
  fclose(exceptions_file);

  // Parsing Guassian File

  // Skip first two lines
  for(i=0;i<2;i++) {
    fgets(buffer,80,cubefile);
  }

  fgets(buffer,80, cubefile);
  sscanf(buffer, "%i %lf %lf %lf",&cubefile_num_of_atoms,
	 &in_center_x, &in_center_y, &in_center_z);
  fgets(buffer,80, cubefile);
  sscanf(buffer, "%i %lf %lf %lf",&sizex,&xx,&xy,&xz);
  fgets(buffer,80, cubefile);
  sscanf(buffer, "%i %lf %lf %lf",&sizey,&yx,&yy,&yz);
  fgets(buffer,80, cubefile);
  sscanf(buffer, "%i %lf %lf %lf",&sizez,&zx,&zy,&zz);

  num_of_atoms = cubefile_num_of_atoms - number_of_exceptions;
  
  x = (double*)malloc(sizeof(double)*num_of_atoms);
  y = (double*)malloc(sizeof(double)*num_of_atoms);
  z = (double*)malloc(sizeof(double)*num_of_atoms);

  printf("%i Exceptions!\n",number_of_exceptions);

  k=0;
  for(i=0;i<cubefile_num_of_atoms;i++){
    exceptor = 0;
    fgets(buffer,80,cubefile);
    sscanf(buffer,"%i %lf %lf %lf %lf",&atomtype_in,
	   &charge_in,&x_in,&y_in,&z_in);
    for(j=0;j<number_of_exceptions;j++) {
      if (i == exceptions[j]) {
	exceptor = 1;
      }
    }
    if (exceptor != 1) {
      x[k]=x_in;
      y[k]=y_in;
      z[k]=z_in;
      k++;
    } else {
      printf("Exception Atomtype=%i, Charge=%lf, X=%lf Y=%lf Z=%lf \n",
	     atomtype_in,
	     charge_in,
	     x_in,
	     y_in,
	     z_in);
    }
  }
  
  fclose(cubefile);
  free(exceptions);
  
  // Center protein

  sumx=0;
  sumy=0;
  sumz=0;
  
  for(i=0;i<num_of_atoms;i++) {
    sumx += x[i];
    sumy += y[i];
    sumz += z[i];
  }
  
  center_x = sumx/(double)num_of_atoms;
  center_y = sumy/(double)num_of_atoms;
  center_z = sumz/(double)num_of_atoms;

  for(i=0;i<num_of_atoms;i++) {
    x[i]=x[i]-center_x;
    y[i]=y[i]-center_y;
    z[i]=z[i]-center_z;
  }

  // Calculate inerital tensor
  // 0 1 2
  // 3 4 5
  // 6 7 8
  for(i=0;i<9;i++){
    tensor_in[i]=0;
  }

  for(i=0;i<num_of_atoms;i++) {
    tensor_in[0]+=y[i]*y[i]+z[i]*z[i];
    tensor_in[1]+=x[i]*y[i];
    tensor_in[2]+=x[i]*z[i];
    tensor_in[4]+=x[i]*x[i]+z[i]*z[i];
    tensor_in[5]+=y[i]*z[i];
    tensor_in[8]+=x[i]*x[i]+y[i]*y[i];
  }

  free(x);
  free(y);
  free(z);

  tensor_in[1]=-tensor_in[1];
  tensor_in[2]=-tensor_in[2];
  tensor_in[5]=-tensor_in[5];
  
  tensor_in[3]=tensor_in[1];
  tensor_in[6]=tensor_in[2];
  tensor_in[7]=tensor_in[5];

  space=diagonalize(tensor_in);


  
  // Require canonical input base;

  // 0 1 2
  // 3 4 5
  // 6 7 8

  // Cubic case

  if (argc == 6) {
    sscanf(argv[4],"%i",&sizex);
    sscanf(argv[5],"%lf",&xx);
    
    sizey = sizex;
    sizez = sizex;
    
    yy = xx;
    zz = xx;
    
  }

  // Cuboid case

  if (argc == 11) {
    sscanf(argv[4],"%i",&sizex);
    sscanf(argv[6],"%i",&sizey);
    sscanf(argv[7],"%i",&sizez);
    sscanf(argv[8],"%lf",&xx);
    sscanf(argv[9],"%lf",&yy);
    sscanf(argv[10],"%lf",&zz);
  }

  xxo = space.eigenvectors[0]*xx;
  xyo = space.eigenvectors[3]*xx;
  xzo = space.eigenvectors[6]*xx;

  yxo = space.eigenvectors[1]*yy;
  yyo = space.eigenvectors[4]*yy;
  yzo = space.eigenvectors[7]*yy;

  zxo = space.eigenvectors[2]*zz;
  zyo = space.eigenvectors[5]*zz;
  zzo = space.eigenvectors[8]*zz;

  free(space.eigenvectors);
  free(space.eigenvalues);

  out_center_x=center_x-(double)sizex/2.f*xxo-(double)sizey/2.f*yxo
    -(double)sizez/2.f*zxo;
  out_center_y=center_y-(double)sizex/2.f*xyo-(double)sizey/2.f*yyo
    -(double)sizez/2.f*zyo;
  out_center_z=center_z-(double)sizex/2.f*xzo-(double)sizey/2.f*yzo
    -(double)sizez/2.f*zzo;

 
  printf("X vector = [%lf %lf %lf]\n",
	 xxo,xyo,xzo);
  printf("Y vector = [%lf %lf %lf]\n",
	 yxo,yyo,yzo);
  printf("Z vector = [%lf %lf %lf]\n",
	 zxo,zyo,zzo);

  fprintf(gaussian_format,"-1,%lf,%lf,%lf\n",
	  out_center_x,out_center_y,out_center_z);
  fprintf(gaussian_format,"%i,%lf,%lf,%lf\n",sizex*(-1),xxo,xyo,xzo);
  fprintf(gaussian_format,"%i,%lf,%lf,%lf\n",sizey,yxo,yyo,yzo);
  fprintf(gaussian_format,"%i,%lf,%lf,%lf\n",sizez,zxo,zyo,zzo);

  fclose(gaussian_format);

}
