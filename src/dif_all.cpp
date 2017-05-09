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


void app_mask(OCTET* img, OCTET* mask, int n);

void diff(OCTET* img, OCTET* ref, int n);

void erosion(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void dilat(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void to_nb(OCTET* img, OCTET* out, int n);

void median(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void order_dif(OCTET* img, OCTET* mask, OCTET* ref, int win, int nH, int nW);

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n);

void median(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w);

void order_dif(OCTET* img, OCTET* mask, OCTET* ref, OCTET* coul, int win, int nH, int nW);
//*****************************************************************

void maxi(OCTET* img, OCTET &max, int n)
{
  int i;
  max = 0;
  
  for(i = 0; i < n; i++)
    if(img[i] > max)
      max = img[i];
}

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n)
{
  int i;
  float a, max;
  OCTET mx;

  maxi(img, mx, n);
  max = mx;
  
  for(i = 0; i < n; i++)
    {
      float color[4][3] = { {20,20,255}, {20,255,20}, {255,255,20}, {255,20,20} };
      
      int id1;
      int id2;
      float val, dis = 0;  
      
      if(mask[i])
	{
	  val = ((float)(img[i]) / max) * 3.0;
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

void order_dif(OCTET* img, OCTET* mask, OCTET* ref, OCTET* coul, int win, int nH, int nW)
{
  OCTET *res, *tmp;
  int i, j, n = nH * nW;
  
  app_mask(img, mask, n);
  app_mask(ref, mask, n);

  allocation_tableau(res, OCTET, n);
  allocation_tableau(tmp, OCTET, n);
  
  median(img, res, mask, win, nH, nW);
  diff(res, ref, n);
  erosion(res, tmp, mask, win, nH, nW);
  dilat(tmp, res, mask, win, nH, nW);

  h_map(res, mask, coul, nH * nW);
}
    
void do_diff(OCTET* img, OCTET* mask, OCTET* ref, int win, char nom[250], int id, char suf[250], int h, int w, int nbimg)
{
  int i = 1, n = h * w;
  char file[250];
  OCTET* coul, *imtemp;
  
  allocation_tableau(coul, OCTET, 3 * h * w);
  allocation_tableau(imtemp, OCTET, 3 * h * w);

  order_dif(img, mask, ref, coul, win, h, w);
  sprintf(file, "heat_map_%d%s", id, suf);
  ecrire_image_ppm(file, coul, h, w);

  while(i < nbimg)
    {
      next_img(imtemp, nom, id + i, suf, n);
      to_nb(imtemp, img, n);
      order_dif(img, mask, ref, coul, win, h, w);
      sprintf(file, "heat_map_%d%s", id + i, suf);
      ecrire_image_ppm(file, coul, h, w);
      i++;
    }
}

int main(int argc, char* argv[])
{
  char name[250], nomImg[250], nomMask[250], nomRef[250], suf[250];
  int nH, nW, nTaille, win, ind, nbimg;
  
  OCTET *ImgIn, *mask, *ref, *out, *out_ref;
  if (argc != 8) 
    {
      printf("Usage: Image indice .ppm Mask Ref WindowSize nbImg?\n"); 
      exit (1) ;
    }
  sscanf (argv[1],"%s",nomImg);
  sscanf (argv[2],"%d",&ind);
  sscanf (argv[3],"%s",suf);
  sscanf (argv[4],"%s",nomMask);
  sscanf (argv[5],"%s",nomRef);
  sscanf (argv[6],"%d",&win);
  sscanf (argv[7],"%d",&nbimg);

  sprintf(name, "%s%d%s", nomImg, ind, suf);
  
  lire_nb_lignes_colonnes_image_ppm(name, &nH, &nW);
  nTaille = nH * nW;
  
  allocation_tableau(ref, OCTET, 3 * nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);
  allocation_tableau(out, OCTET, nTaille);
  allocation_tableau(out_ref, OCTET, nTaille);

  lire_image_ppm(name, ImgIn, nTaille);
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

  do_diff(out, mask, out_ref, win, nomImg, ind, suf, nH, nW, nbimg);

  return 1;
}
