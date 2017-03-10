#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
using namespace std;

void app_mask(OCTET* img, OCTET* mask, int n);

void app_mask_c(OCTET* img, OCTET* mask, int n);

void histo(int* h, OCTET* img, OCTET* mask, int n);

void uni(int* h, OCTET* mask, OCTET &t1, OCTET &t2, int n, float window);

void seuil_uni(OCTET* img, OCTET* mask, int n, float win);

void seuil_uni_rgb(OCTET* img, OCTET* mask, int n, float win);
//*********************************************************************************

void app_mask(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	img[i] = 0;
    }
}

void app_mask_c(OCTET* img, OCTET* mask, int n)
{
  int i,j;

  for(i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	{
	  for(j = 0; j < 3; j++)
	    img[3 * i + j] = 0;
	}
    }
}

void histo(int* h, OCTET* img, OCTET* mask, int n)
{
  int i;
  
  for(i = 0; i < 255; i++)
    h[i] = 0;

  for(i = 0; i < n; i++)
    {
      if(mask[i] == 255)
	h[img[i]]++;
    }
}

void uni(int* h, OCTET* mask, OCTET &t1, OCTET &t2, int n, float window)
{
  t1 = 0;
  t2 = 0;
  int i, ish, isb, acc = 0, nb = n;

  for(i = 0; i < n; i++)
    {
      if(!mask[i])
	nb--;
    }

  for(i = 0; i < 255; i++)
    acc += h[i];
  cout << " nbh = " << acc << endl;
  acc = 0;
  
  double sb, sh;
  sb = (double)(nb) * (0.5 - window);
  sh = (double)(nb) * (0.5 + window);
  
  i = 0;
  ish = (int)sh;
  isb = (int)sb;

  cout << " sb = " << (int)sb <<"   sh = " << (int)sh << "   n = " << n << "   nb = " << nb << endl;

  while(acc < ish)
    {
      acc += h[i];
      if(acc > isb && t1 == 0)
	t1 = i;
      i++;
    }
  t2 = i - 1;
}

void seuil_uni(OCTET* img, OCTET* mask, int n, float win)
{
  OCTET t1, t2;
  
  int i, h[256];
  histo(h, img, mask, n);
  uni(h, mask, t1, t2, n, win);
  
  cout << " t1 = " << (int)t1 << " t2 = " << (int)t2 << endl;

  for(i = 0; i < n; i++)
    {
      if(img[i] <= t1 || img[i] >= t2)
	img[i] = 255;
      else
	img[i] = 0;
    }
  app_mask(img, mask, n);
}

void seuil_uni_rgb(OCTET* img, OCTET* mask, int n, float win)
{
  int i,j;
  OCTET *r, *g, *b, val;

  allocation_tableau(r, OCTET, n);
  allocation_tableau(g, OCTET, n);
  allocation_tableau(b, OCTET, n);
  
  planR(r, img, n);
  planV(g, img, n);
  planB(b, img, n);

  seuil_uni(r, mask, n, win);
  seuil_uni(g, mask, n, win);
  seuil_uni(b, mask, n, win);

  for(i = 0; i < n; i++)
    {
      val = !(r[i] && g[i] && b[i]);
      for(j = 0; j < 3; j++)
	img[3 * i + j] = 255 * val;
    }

  app_mask_c(img, mask, n);
}

int main(int argc, char* argv[])
{
  char nomImg[250], nomImgRgb[250], nomImout[250], nomImoutRgb[250], nomMask[250];
  int nH, nW, nTaille, nbimg;
  float win;
  OCTET *ImgIn, *ImgInRgb, *mask;

  if (argc != 6) 
    {
      printf("Usage: ImageIn.pgm ImageIn.ppm ImageOut.pgm Window Mask \n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomImgRgb) ;
  sscanf (argv[3],"%s",nomImout);
  sscanf (argv[4],"%f",&win);
  sscanf (argv[5],"%s",nomMask);

  lire_nb_lignes_colonnes_image_pgm(nomImg, &nH, &nW);
  nTaille = nH * nW;
   
  allocation_tableau(ImgIn, OCTET, nTaille);
  allocation_tableau(ImgInRgb, OCTET, nTaille * 3);
  allocation_tableau(mask, OCTET, nTaille);
  
  lire_image_pgm(nomImg, ImgIn, nH * nW);
  lire_image_pgm(nomMask, mask, nH * nW);
  lire_image_ppm(nomImgRgb, ImgInRgb, nH * nW);

  seuil_uni_rgb(ImgInRgb, mask, nTaille, win);
  

  ecrire_image_ppm(nomImout, ImgInRgb, nH, nW);
  
  return 1;
}
