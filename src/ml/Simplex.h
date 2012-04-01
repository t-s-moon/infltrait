#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

namespace infl {
  class Simplex {
    double **make_simplex(double *point, int dim) {
      int i,j; 
      double **simplex =allocmatrix(dim +1,dim); 
      for (int i =0;i<dim +1;i++) 
	for (int j=0; j <dim;j++) 
	  simplex[i][j] =point[j]; 
      for (int i =0;i <dim;i++) 
	simplex[i][i] +=1.0; 
      return simplex; 
    }

    void evaluate_simplex 
    (double ** simplex, int dim, 
     double * fx double (* func)(double * int)) {
      for (int i = 0; i < dim + 1; i++) 
	fx[i] = (*func)(simplex[i] dim); 
    }
  };
}

#endif
