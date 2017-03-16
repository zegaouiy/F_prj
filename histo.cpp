#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void print_histo(OCTET* img, OCTET* mask, int n);

void print_histo_coul(OCTET* img, OCTET* mask, int n);

void histo(int* h, OCTET* img, OCTET* mask, int n);

//*************************************************************

void print_histo(OCTET* img, OCTET* mask, int n)
{
  int i,h[256];
  ofstream lum;
  OCTET* nb;
  float r, g, b;
  
  allocation_tableau(nb, OCTET, n);

  for(i = 0; i < n; i++)
    {
      r = img[3 * i];
      g = img[3 * i + 1];
      b = img[3 * i + 2];
      nb[i] = (OCTET)(0.3 * r + 0.6 * g + 0.1 * b);
    }

  histo(h, nb, mask, n);
  
  lum.open("repart.dat");

  for(i = 0; i < 256; i++)
    lum << i << " " << h[i] << endl;
  
  lum.close();
}

void print_histo_coul(OCTET* img, OCTET* mask, int n)
{
  OCTET *r, *b, *g;
  int hr[256], hb[256], hg[256];
  ofstream red, green, blue;

  allocation_tableau(r, OCTET, n);
  allocation_tableau(g, OCTET, n);
  allocation_tableau(b, OCTET, n);
  
  planR(r, img, n);
  planV(g, img, n);
  planB(b, img, n);

  histo(hr, r, mask, n);
  histo(hg, g, mask, n);
  histo(hb, b, mask, n);

  red.open("red.dat");
  green.open("green.dat");
  blue.open("blue.dat");

  for(int i = 0; i < 256; i++)
    {
      red << i << " " << hr[i] << endl;
      green << i << " " << hg[i] << endl;
      blue << i << " " << hb[i] << endl;
    }  
  
  red.close();
  green.close();
  blue.close();
}


void histo(int* h, OCTET* img, OCTET* mask, int n)
{
  int i;
  
  for(i = 0; i < 256; i++)
    h[i] = 0;

  for(i = 0; i < n; i++)
    {
      if(mask[i] == 255)
	h[img[i]]++;
    }
}


int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250];
  int nH, nW, nTaille, coul;
  
  OCTET *ImgIn, *mask;
  if (argc != 4) 
    {
      printf("Usage: ImageIn.pgm/ppm Mask couleur?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomMask);
  sscanf (argv[3],"%d",&coul);

  lire_nb_lignes_colonnes_image_ppm(nomImg, &nH, &nW);
  nTaille = nH * nW;
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);
  lire_image_ppm(nomImg, ImgIn, nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  if(coul)
    print_histo_coul(ImgIn, mask, nTaille);
  else
    print_histo(ImgIn, mask, nTaille);
     
  return 1;
}
