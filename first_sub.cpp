#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

// n =/= true_n

void histo(int* h, OCTET* img, OCTET* mask, int n);

void repart(OCTET* img, OCTET* mask, int* hist, double* ddp, int n);

void egalise(OCTET* img, OCTET* mask, int* hist, double* ddp, int n);

void spec(OCTET* img, OCTET* ref, OCTET* mask, int n);

void spec_rgb(OCTET* img, OCTET* ref, OCTET* mask, int n);

void app_mask(OCTET* img, OCTET* mask, int n);

void diff(OCTET* img, int* dif, int n);

void thresh(OCTET* img, OCTET* ref, int* dif, OCTET s, int n);

void bs(OCTET* img, OCTET* ref, OCTET* mask, int n);

double abs(double a)
{
  if (a < 0)
    a *= -1;
  return a;
}
//*************************************************************

void app_mask(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	for(int j = 0; j < 3; j ++)
	  img[3 * i + j] = 0;
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

void diff(OCTET* img, OCTET* ref, int* dif, int n)
{
  int i;

  for(i = 0; i < n; i++)
    dif[i] = (int)ref[i] - (int)img[i];
}

void thresh(OCTET* img, int* dif, OCTET s, int n)
{
  int i;
  double r, g, b;

  for(i = 0; i < n;i++)
    {
      r = abs(dif[3 * i]);
      g = abs(dif[3 * i + 1]);
      b = abs(dif[3 * i + 2]);
      
      if(r < s || b < s || g < s)
	for(int j = 0; j < 3; j++)
	  img[3 * i + j] = 0;
      else
	for(int j = 0; j < 3; j++)
	  img[3 * i + j] = 255;
    }
}

void repart(OCTET* img, OCTET* mask, int* hist, double* ddp, int n)
{
  int i;
  double sum = 0;

  for(i = 0; i < 256; i++)
    {
      ddp[i] = (double)hist[i]/(double)n;
      
    }
  for(i = 0; i < 256; i++)
    {
      sum += ddp[i];
      ddp[i] = sum;
      
    }  
}
      
void egalise(OCTET* img, OCTET* mask, int* hist, double* ddp, int n)
{
  int i;
  
  for(i = 0; i < n; i++)
    {
      img[i] = (OCTET)(ddp[img[i]] * 255.0);
      
    }
}

void spec(OCTET* img, OCTET* ref, OCTET* mask, int n)
{
  int i, k, h[256], href[256], true_n = n;
  double ddp[256], d_ref[256];
  OCTET val;
  
  for(i = 0; i < n; i++)
    {
      if(!mask[i])
	true_n--;
    }
  
  histo(h, img, mask, n);
  repart(img, mask, h, ddp, true_n);
  histo(href, ref, mask, n);
  repart(ref, mask, href, d_ref, true_n);
 
  egalise(img, mask, h, ddp, n);
  
  for(i = 0; i < n; i++)
    {
      for(k = 0; k < 256; k++)
	{
	  if((OCTET)(255.0 * d_ref[k] < img[i]))
	    val = (OCTET)k;
	  //cout << "i = " << i << "  k = " << k << "  im = " << (int)img[i] << endl;
	}
      img[i] = val;
    }

}

void spec_rgb(OCTET* img, OCTET* ref, OCTET* mask, int n)
{
  int i;
  OCTET *r, *b, *g, *r_ref, *g_ref, *b_ref;

  allocation_tableau(r, OCTET, n);
  allocation_tableau(g, OCTET, n);
  allocation_tableau(b, OCTET, n);
  
  planR(r, img, n);
  planV(g, img, n);
  planB(b, img, n);

  allocation_tableau(r_ref, OCTET, n);
  allocation_tableau(g_ref, OCTET, n);
  allocation_tableau(b_ref, OCTET, n);
  
  planR(r_ref, ref, n);
  planV(g_ref, ref, n);
  planB(b_ref, ref, n);

  spec(r, r_ref, mask, n);
  spec(g, g_ref, mask, n);
  spec(b, b_ref, mask, n);

  for(i = 0; i < n; i++)
    {
      img[3*i] = r[i];
      img[3*i + 1] = g[i];
      img[3*i + 2] = b[i];
    }
}
  

void bs(OCTET* img, OCTET* ref, OCTET* mask, int n, OCTET s)
{
  int* dif;
  
  allocation_tableau(dif, int, 3 * n);
  
  spec_rgb(img, ref, mask, n);
  //diff(img, ref, dif, 3*n);
  //thresh(img, s, n);
  //app_mask(img, mask, n);

}

int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250];
  int nH, nW, nTaille, s;
  
  OCTET *ImgIn, *mask, *ref;
  if (argc != 4) 
    {
      printf("Usage: ImageIn.pgm/ppm Mask seuilr?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomMask);
  sscanf (argv[3],"%d",&s);

  lire_nb_lignes_colonnes_image_ppm(nomImg, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(ref, OCTET, 3 * nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);

  lire_image_ppm(nomImg, ImgIn, nTaille);
  lire_image_ppm("../dat/lena.ppm", ref, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  bs(ImgIn, ref, mask, nTaille, s);

  ecrire_image_ppm("spec_test.ppm", ImgIn, nH, nW);

  return 1;
}
