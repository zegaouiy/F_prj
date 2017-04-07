#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
#include <algorithm>    
#include <vector>       

using namespace std;

void app_mask(OCTET* img, OCTET* mask, int n);

void diff(OCTET* img, OCTET* ref, int n);

void erosion(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void dilat(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void to_nb(OCTET* img, OCTET* out, int n);

void median(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void order_dif(OCTET* img, OCTET* mask, OCTET* ref, int win, int nH, int nW);
//*****************************************************************

int abs(int a)
{
  if (a < 0)
    a *= -1;
  return a;
}

void app_mask(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	img[i] = 0;
    }
}

void diff(OCTET* img, OCTET* ref, int n)
{
  int i, dif;

  for(i = 0; i < n; i++)
    {
      dif = (int)ref[i] - (int)img[i];
      img[i] = (OCTET)abs(dif);
    }
}

void to_nb(OCTET* img, OCTET* out, int n)
{
  int i;
  float r, g, b;
  
  for(i = 0; i < n; i++)
    {
      r = img[3 * i];
      g = img[3 * i + 1];
      b = img[3 * i + 2];

      out[i] = 0.3 * r + 0.6 * g + 0.1 * b;
    }
}

void erosion(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w)
{
  int i, j, n, m;
  OCTET min;

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(mask[i * w + j])
	  {
	    min = img[i * w + j];

	    for(n = (i - win < 0?0:i - win); n < (i + win < h?i + win:h - 1); n++)
	      for(m = (j - win < 0?0:j - win); m < (j + win < w?j + win:w - 1); m++)
		if(img[n * w + m] < min)
		  min = img[n * w + m];
			
	    out[i * w + j] = min;
	  }
      }
}

void dilat(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w)
{
  int i, j, n, m;
  OCTET max;

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(mask[i * w + j])
	  {
	    max = img[i * w + j];

	    for(n = (i - win < 0?0:i - win); n < (i + win < h?i + win:h - 1); n++)
	      for(m = (j - win < 0?0:j - win); m < (j + win < w?j + win:w - 1); m++)
		if(img[n * w + m] > max)
		  max = img[n * w + m];
			
	    out[i * w + j] = max;
	  }
      }
} 

void median(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w)
{
  int i, j, n, m;
  vector<OCTET> vals(win * win);

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(mask[i * w + j])
	  {
	    vals.clear();
	    
	    for(n = (i - win < 0?0:i - win); n < (i + win < h?i + win:h - 1); n++)
	      for(m = (j - win < 0?0:j - win); m < (j + win < w?j + win:w - 1); m++)
		vals.push_back(img[n * w + m]);
	    
	    sort(vals.begin(), vals.end());

	    out[i * w + j] = vals[win*win/2];
	  }
      }
}
	    
void order_dif(OCTET* img, OCTET* mask, OCTET* ref, int win, int nH, int nW)
{
  int i, n = nH * nW;
  OCTET* res, *tmp;
  
  app_mask(img, mask, n);
  app_mask(ref, mask, n);

  allocation_tableau(res, OCTET, n);
  allocation_tableau(tmp, OCTET, n);
  
  //order1
  erosion(img, res, mask, win, nH, nW);
  dilat(res, tmp, mask, win, nH, nW);
  diff(tmp, ref, n);
  median(tmp, res, mask, win, nH, nW);
  
  ecrire_image_pgm("order1.pgm", res, nH, nW);

  //order2
  median(img, res, mask, win, nH, nW);
  diff(res, ref, n);
  erosion(res, tmp, mask, win, nH, nW);
  dilat(tmp, res, mask, win, nH, nW);
		      
  ecrire_image_pgm("order2.pgm", res, nH, nW);

  //order3
  erosion(img, res, mask, 0, nH, nW);
  diff(res, ref, n);
  median(res, tmp, mask, win, nH, nW);
  erosion(tmp, res, mask, win, nH, nW);
  dilat(res, tmp, mask, win, nH, nW);
  
  ecrire_image_pgm("order3.pgm", tmp, nH, nW);

  //order4
  median(img, res, mask, win, nH, nW);
  erosion(res, tmp, mask, win, nH, nW);
  dilat(tmp, res, mask, win, nH, nW);
  diff(res, ref, n);
 
  ecrire_image_pgm("order4.pgm", res, nH, nW);
  //ecrire_image_pgm("ref.pgm", ref, nH, nW);
}


int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250], nomRef[250];
  int nH, nW, nTaille, win;
  
  OCTET *ImgIn, *mask, *ref, *out, *out_ref;
  if (argc != 5) 
    {
      printf("Usage: ImageIn.ppm Mask Ref WindowSize?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomMask);
  sscanf (argv[3],"%s",nomRef);
  sscanf (argv[4],"%d",&win);

  lire_nb_lignes_colonnes_image_ppm(nomImg, &nH, &nW);
  nTaille = nH * nW;
  
  allocation_tableau(ref, OCTET, 3 * nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);
  allocation_tableau(out, OCTET, nTaille);
  allocation_tableau(out_ref, OCTET, nTaille);

  lire_image_ppm(nomImg, ImgIn, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);
  lire_image_ppm(nomRef, ref, nTaille);

  /*
    order1 : erod - diff - median
    order2 : median - diff - erod
    order3 : dif - median - erod
    order4 : median - erod - dif
  */

  to_nb(ImgIn, out, nTaille);
  to_nb(ref, out_ref, nTaille);

  order_dif(out, mask, out_ref, win, nH, nW);

  return 1;
}
