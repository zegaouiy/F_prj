#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
#include <algorithm>    
#include <vector>       

using namespace std;

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);
  
  lire_image_ppm(imnom, img, n);
}


int main(int argc, char* argv[])
{
  char nomImg[250], nomF[250], suf[250], tru[250], img_mask[250];
  int nH, nW, nTaille, i, j, nbimg, ind;
  
  OCTET *img, *mask, *out;
  if (argc != 6) 
    {
      printf("Usage: ImageIn.pgm/ppm Image2 ?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%d",&ind);
  sscanf (argv[3],"%s",suf) ;
  sscanf (argv[4],"%s",nomF);
  sscanf (argv[5],"%d",&nbimg);

  sprintf(tru, "%s%d%s", nomImg, ind, suf);
  sprintf(img_mask, "%s%d%s", nomF, ind, suf);
  
  lire_nb_lignes_colonnes_image_ppm(tru, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(out, OCTET, 3 * nTaille * 2);
  allocation_tableau(mask, OCTET, 3 * nTaille);
  allocation_tableau(img, OCTET, 3 * nTaille);

  lire_image_ppm(tru, img, nTaille);
  lire_image_ppm(img_mask, mask, nH * nW);

  for(j = 0; j < nbimg; j++)
    {
      for(i = 0; i < 3 * nTaille; i++)
	{
	  out[i] = img[i];
	  out[i + 3 * nTaille] = mask[i];
	}
      next_img(img, nomImg, ind + j, suf, nTaille);
      next_img(mask, nomF, ind + j, suf, nTaille);

      sprintf(tru, "collage%d%s", ind + j, suf);
      ecrire_image_ppm(tru, out, nH*2, nW);
    }
  return 1;
}
