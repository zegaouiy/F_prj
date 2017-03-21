#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void histo(int* h, OCTET* img, OCTET* mask, int n);

void repart(OCTET* img, OCTET* mask, int* hist, double* ddp, int n);

void egalise(OCTET* img, OCTET* mask, int* hist, double* ddp, int n);

void spec(OCTET* img, OCTET* ref, OCTET* mask, int n);

void spec_rgb(OCTET* img, OCTET* ref, OCTET* mask, int n);

void app_mask(OCTET* img, OCTET* mask, int n);

void diff(OCTET* img, OCTET* ref, int n);

void thresh(OCTET* img, OCTET s, int n);

void bs(OCTET* img, OCTET* ref, OCTET* mask, int n);

double abs(double a)
{
  if (a < 0)
    a *= -1;
  return a;
}

int abs(int a)
{
  if (a < 0)
    a *= -1;
  return a;
}


void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);

  lire_image_ppm(imnom, img, n);
}

//*************************************************************


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

void diff(OCTET* img, OCTET* ref, int n)
{
  int i, dif;

  for(i = 0; i < n; i++)
    {
      dif = (int)ref[i] - (int)img[i];
      img[i] = (OCTET)abs(dif);
    }
}

void thresh(OCTET* img, OCTET s, int n)
{
  int i;
  double r, g, b;

  for(i = 0; i < n;i++)
    {
      r = img[3 * i];
      g = img[3 * i + 1];
      b = img[3 * i + 2];
      
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
      val = 0;
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
  
  // allocation_tableau(dif, int, 3 * n);
  
  spec_rgb(img, ref, mask, n);
  // cout << "spec done " << endl;
  // diff(img, ref, 3*n);
  // cout << "diff done" << endl;
  // thresh(img, s, n);
  // cout << "seuil done" << endl;
  // app_mask(img, mask, n);

}

void do_spec(OCTET* img, OCTET* ref, OCTET* mask, char* nom, int id, char* suf, int h, int w, int nbimg)//////////////:SEUIL
{
  int i, n = h * w;
  char out[250];

  for(i = 1; i < nbimg; i++)
    {
      bs(img, ref, mask, n, 1);
      sprintf(out, "%s%d%s", "../../prj/dat/spec_", id+i, suf);
      ecrire_image_ppm(out, img, h, w);
      next_img(img, nom, id + i, suf, n);
    }
}
  
int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250], suf[250], fimg[250];;
  int nH, nW, nTaille, ind, nbimg;
  
  OCTET *ImgIn, *mask, *ref;
  if (argc != 6) 
    {
      printf("Usage: nomImg indice suffixe Mask nbimg?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg);
  sscanf (argv[2],"%d",&ind);
  sscanf (argv[3],"%s",suf);
  sscanf (argv[4],"%s",nomMask);
  sscanf (argv[5],"%d",&nbimg);

  sprintf(fimg, "%s%d%s", nomImg, ind, suf);
  cout << fimg << endl;
  ind++;

  lire_nb_lignes_colonnes_image_ppm(fimg, &nH, &nW);
  nTaille = nH * nW;
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);
  allocation_tableau(ref, OCTET, 3 * nTaille);
  lire_image_ppm("../../prj/dat/reference_mean.ppm", ref, nTaille);
  lire_image_ppm(fimg, ImgIn, nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  do_spec(ImgIn, ref, mask, nomImg, ind, suf, nH, nW, nbimg);

  return 1;
}
