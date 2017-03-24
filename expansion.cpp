#include "image_ppm.h"
#include <iostream>
using namespace std;

void exp(OCTET* img, int s, int n)
{
  int i, val;

  for(i = 0; i < n; i++)
    {
      val = s * img[i];
      if(val > 255)
	img[i] = 255;
      else
	img[i] = val;
    }
}

int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250];
  int mh,mw,nH, nW, nTaille, val;
 
  sscanf (argv[1],"%s",cNomImgLue) ;
  sscanf (argv[2],"%d",&val);

  OCTET *ImgIn, *ImgOut;
   
  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(ImgIn, OCTET, nTaille);
  lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
  
  exp(ImgIn, val, nTaille);

  ecrire_image_pgm("carte_dif.pgm", ImgIn,  nH, nW);
  free(ImgIn);
  
  return 1;
}
