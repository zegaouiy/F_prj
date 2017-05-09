#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
using namespace std;

OCTET max(OCTET a, OCTET b)
{
  if(a > b)
    return a;
  else
    return b;
}

OCTET min(OCTET a, OCTET b)
{
  if(a < b)
    return a;
  else
    return b;
}

void erosion(OCTET* img, OCTET* out, OCTET* mask, int h, int w, int nb)
{
  int i, j, n, pix;
  OCTET *tmp;
  
  allocation_tableau(tmp, OCTET, h * w);

  for(i = 0; i < h*w; i++)
    tmp[i] = img[i];
  


  for(n = 0; n < nb; n++)
    {
      for(i = 0; i < h; i++)
	for(j = 0; j < w; j++)
	  {
	    if(mask[i * w + j])
	      {
		pix = i * w + j;
		
		out[pix] = min(tmp[pix], min(min(tmp[pix - 1], tmp[pix + 1]), min(tmp[pix - w], tmp[pix + w])));
	      }
	  }
      for(i = 0; i < h*w; i++)
	tmp[i] = out[i];
    }
}

void dilat(OCTET* img, OCTET* out, OCTET* mask, int h, int w, int nb)
{
  int i, j, n, pix;
  OCTET *tmp;

  allocation_tableau(tmp, OCTET, h * w);

 
  for(i = 0; i < h*w; i++)
    tmp[i] = img[i];
 
  for(n = 0; n < nb; n++)
    {
      for(i = 0; i < h; i++)
	for(j = 0; j < w; j++)
	  {
	    if(mask[i * w + j])
	      {
		pix = i * w + j;
	
		out[pix] = max(tmp[pix], max(max(tmp[pix - 1], tmp[pix + 1]), max(tmp[pix - w], tmp[pix + w])));
	
	      }
	  }
      for(i = 0; i < h*w; i++)
	tmp[i] = out[i];
    }
}
