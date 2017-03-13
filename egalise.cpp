#include "image_ppm.h"
#include <iostream>
using namespace std;


OCTET inv(double f,double* vals)
{
  OCTET i = 0;
  while(f >= 0 && i<256)
    f -= vals[i++];
  return i;
}

void egalise(OCTET* in,OCTET* out,int h,int w,int mh,int mw)
{
  int i,j;
  double vals[256] = {0};
  double f[256] = {0};
  


  for(i = 0;i < h;i++)
    for(j = 0;j < w;j++)
      vals[in[i*w + j]]++;
  for(i = 0;i < 256;i++)
    {
      vals[i] /= (double)(h*w);
      // cout << i << " " << vals[i] << endl;
    }
  f[0] = vals[0];
  for(j = 0;j < 256;j++)
    {
      f[j] = f[j - 1] + vals[j];
      cout << j << " " << f[j] << endl;
    }
   for(i = 0;i < h;i++)
     for(j = 0;j < w;j++)
       in[i*w + j] =(OCTET)( f[in[i*w + j]] * 255.0);
  
    for(i = 0;i < mh;i++)
     for(j = 0;j < mw;j++)
       {
	 out[i*mw + j] =(OCTET)(inv((double)(out[i*mw +j])/255.0,vals));
	 //cout << "inv = " << (OCTET)inv((double)(out[i*mw +j])/255.0,vals)<<endl;
       }
   
}


int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250];
  int mh,mw,nH, nW, nTaille, S;
 
   
  sscanf (argv[1],"%s",cNomImgLue) ;
  sscanf (argv[2],"%s",cNomImgEcrite);

  OCTET *ImgIn, *ImgOut;
   
  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
  lire_nb_lignes_colonnes_image_pgm(cNomImgEcrite, &mh, &mw);
  nTaille = nH * nW;

  allocation_tableau(ImgIn, OCTET, nTaille);
  lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
  allocation_tableau(ImgOut, OCTET, mw * mh);
  lire_image_pgm(cNomImgEcrite, ImgOut, mh * mw);

  egalise(ImgIn,ImgOut,nH,nW,mh,mw);

  ecrire_image_pgm(cNomImgEcrite, ImgOut,  mh, mw);
  free(ImgIn);
  
  return 1;
}
