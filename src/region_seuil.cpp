#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n)
{
  int i;
  float a;

  for(i = 0; i < n; i++)
    {
      float color[4][3] = { {20,20,255}, {20,255,20}, {255,255,20}, {255,20,20} };
      
      int id1;
      int id2;
      float val, dis = 0;  
      
      if(mask[i])
	{
	  val = ((float)(img[i]) / 255.0) * 3.0;
	  id1  = floor(val);            
	  id2  = id1+1;                   
	  dis = val - float(id1);
	  
	  out[3 * i]   = (color[id2][0] - color[id1][0]) * dis + color[id1][0];
	  out[3 * i + 1] = (color[id2][1] - color[id1][1]) * dis + color[id1][1];
	  out[3 * i + 2] = (color[id2][2] - color[id1][2]) * dis + color[id1][2];
	}
      else
	{
	  out[3 * i] = 0;
	  out[3 * i + 1] = 0;
	  out[3 * i + 2] = 0;
	}
    }
}

int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250];
  int nH, nW, nTaille, s;
  
  OCTET *ImgIn, *mask, *out;
  if (argc != 4) 
    {
      printf("Usage: ImageIn.pgm/ppm Mask seuilr?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomMask);
  sscanf (argv[3],"%d",&s);

  lire_nb_lignes_colonnes_image_pgm(nomImg, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(out, OCTET, 3 * nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);

  lire_image_pgm(nomImg, ImgIn, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  h_map(ImgIn, mask, out, nTaille);

  ecrire_image_ppm("carte_chal.ppm", out, nH, nW);

  return 1;
}
