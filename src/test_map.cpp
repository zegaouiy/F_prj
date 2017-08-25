#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>    
#include "morpho.cpp"
#include <omp.h>
using namespace std;
#include "chk_data.cpp"

void write_map(char* nom, int* map, int n, vector<int> &sizes)
{
  FILE *f_image;
  int i, tab[sizes.size()];

  for(i = 0; i < sizes.size(); i++)
    tab[i] = sizes[i];
  
  if( (f_image = fopen(nom, "wb")) == NULL)
    {
      printf("\nPas d'acces en ecriture sur l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }

  fprintf(f_image,"%d", sizes.size()) ;

  i = sizes.size();
  if( (fwrite((int*)tab, sizeof(int), i, f_image))
      != (int)(i))
	    {
	       printf("\nErreur d'ecriture de l'image %s \n", nom);
	       exit(EXIT_FAILURE);
	    }

  if( (fwrite((int*)map, sizeof(int), n, f_image))
	     != (int)(n))
	    {
	       printf("\nErreur d'ecriture de l'image %s \n", nom);
	       exit(EXIT_FAILURE);
	    }
	 fclose(f_image);
}


int main()
{
  int i, j, tab[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  int out[9];
  vector<int> v, u;
  v.push_back(2);
  v.push_back(7);
  char nom[250] = "nom_test";

  write_map(nom, tab, 9, v);
  read_map(nom, out, 9, u);

  for(i = 0; i < 3; i++)
    {
      for(j = 0; j < 3; j++)
	cout << out[i*3+j] << " ";
      cout << endl;
    }

  cout << " u = " << u[0] << " " << u[1] << endl;
}
