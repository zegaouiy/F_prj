void to_oct(float* map, OCTET* out, int n)
{
  int i;
  float fmax, fmin;
  OCTET min, max;
  
  max_i(map, n, fmax);
  fmin = fmax;
  min_i(map, n, fmin);
  fmin++;
  cout << " min = " << min << "  max = " << max << endl;

  min = (OCTET)fmin;
  max = (OCTET)fmax;
  
  for(i = 0; i < n; i++)
    {
      if(map[i] > 0)
	out[i] = (OCTET)((255.0/(max - min))*(map[i] - min));
      else
	out[i] = 0;
    }
}


void to_oct(int* map, OCTET* out, int n)
{
  int i, max, min;
  
  max_i(map, n, max);
  min = max;
  min_i(map, n, min);

  for(i = 0; i < n; i++)
    {
      if(map[i] > 0)
	out[i] = (OCTET)((255.0/(max - min))*(map[i] - min));
      else
	out[i] = 0;
    }
}

void crit_size(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w)
{
  int i, *tmp, *red_tmp, n = h*w;
  OCTET* out, *coul;
  float* ratio;
  char s[250];

  allocation_tableau(out, OCTET, n);
  allocation_tableau(coul, OCTET, 3*n);
  allocation_tableau(tmp, int, n);
  allocation_tableau(red_tmp, int, n);
  allocation_tableau(ratio, float, n);
  
  
  for(i = 0; i < n; i++)
    {
      if(map[i])
	tmp[i] = sizes[map[i] - 1];
      else
	tmp[i] = 0;
    }


  to_oct(tmp, out, n);
  h_map(out, mask, coul, n);
  sprintf(s, "size_%d.ppm", ind);
  ecrire_image_ppm(s, coul, h, w);
  
  for(i = 0; i < n; i++)
    {
      if(red_map[i])
	red_tmp[i] = red_sizes[red_map[i] - 1];
      else
	red_tmp[i] = 0;
    }
  cout << "being oct" << endl;
  to_oct(red_tmp, out, n);
  cout << "end oct" << endl;
  h_map(out, mask, coul, n);
  cout << "hmap done" << endl;
  sprintf(s, "red_size_%d.ppm", ind);
  ecrire_image_ppm(s, coul, h, w);

  cout << "ratio begin" << endl;
  for(i = 0; i < n; i++)
    {
      if(red_map[i] > 0)
	ratio[i] = (float)tmp[i] / (float)red_tmp[i] + 1;
      else
	ratio[i] = 0;
    }

  cout << "ratio end" << endl;
  
  to_oct(ratio, out, n);
  cout << "end oct" << endl;
  h_map(out, mask, coul, n);
  sprintf(s, "red_ratio_%d.ppm", ind);
  ecrire_image_ppm(s, coul, h, w);
  
}

void crit_dens(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w)
{
  int correct, mi, mj, i, j, k, l, kmin, kmax, lmin, lmax, ww = 400, wh = 400, n= h*w;
  int* tmp_map, sum, dens, *density, *true_dens;
  OCTET* out, *coul;
  char s[250];

  allocation_tableau(tmp_map, int, n);
  allocation_tableau(density, int, n);
  allocation_tableau(true_dens, int, n);
  allocation_tableau(coul, OCTET, 3*n);
  allocation_tableau(out, OCTET, 3*n);
  
  for(i = 0; i < h*w; i++)
    tmp_map[i] = red_map[i];
  
  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if (red_map[i * w + j])
	  {
	    kmin = max(0, i - wh);
	    kmax = min(h, i + wh);
	    lmin = max(0, j - ww);
	    lmax = min(w, j + ww);
	  
	    dens = 0;
	    sum = 0;
	    
	    for(k = kmin; k < kmax; k++)
	      for(l = lmin; l < lmax; l++)
		{
		  if(red_map[k * w + l] != red_map[i * w + j])
		    sum++;		  
		   
		  if(tmp_map[k * w + l] > 0)
		    {
		      correct = tmp_map[k * w + l];
		      dens++;
		      for(mi = kmin; mi < kmax; mi++)
			for(mj = lmin; mj < lmax; mj++)
			  if(tmp_map[mi * w + mj] == correct)
			    tmp_map[mi * w + mj] = -1;
		    }
		}
	    for(k = kmin; k < kmax; k++)
	      for(l = lmin; l < lmax; l++)
		if(tmp_map[k * w + l] == -1)
		  tmp_map[k*w + l] = red_map[k*w + l];
	    density[i * w + j] = sum;
	    true_dens[i * w + j] = dens;
	  }
	else
	  {
	    density[i * w + j] = 0;
	    true_dens[i * w + j] = 0;
	  }
      }
  to_oct(density, out, n);
  h_map(out, mask, coul, n);
  sprintf(s, "density_%d.ppm", ind);
  ecrire_image_ppm(s, coul, h, w);
  
  to_oct(true_dens, out, n);
  h_map(out, mask, coul, n);
  sprintf(s, "true_dens_%d.ppm", ind);
  ecrire_image_ppm(s, coul, h, w);
}

	    
//  void crit_ratio(int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);
