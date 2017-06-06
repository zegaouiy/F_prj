// void get_window(OCTET* img, OCTET* mask, int size_max, int size_min, int wh, int ww, int h, int w, OCTET s)
// {
//   int i, *red_map, *map, *center, *tmp;
//   vector<int> sizes;
//   vector<int> red_size;
//   vector<int> red_tab;
//   vector<int> regions;
//   OCTET* out;
  
//   allocation_tableau(red_map, int, h*w);
//   allocation_tableau(map, int, h*w);
//   allocation_tableau(tmp, int, h*w);
//   allocation_tableau(out, OCTET, h*w);

//   for(i = 0; i < h*w; i++)
//     out[i] = img[i];
  
//   cout << "label" << endl;
//   label(img, mask, map, sizes, s, h, w);
//   write_label(map, img, mask, h, w, sizes.size());
//   cout << "red zone" << endl;
//   red_zone(img, mask, red_map, red_size, s, h, w);

//   i = red_size.size();
    
//   allocation_tableau(center, int, 4 * i); // mini maxi minj maxj
  
//   cout << "filter" << endl;
//   red_filter(red_map, center, red_size, red_tab, h, w);
//   cout << "fusion" << endl;
//   cout << " red_tap size : " << red_tab.size() << endl;
//   fusion(map, red_map, tmp, sizes, regions, red_tab, size_max, size_min, wh, ww, h, w);
//   cout << "draaw " << endl;
//   draw_window(out, regions, wh, ww, h, w);
// }  
  

void red_filter(int* red_map, int* center, vector<int> red_sizes, vector<int>& red_tab, int h, int w)
{
  int i, j, min_i, max_i, min_j, max_j;

  //cout << "  red_size : " << (int)red_sizes.size() << "   red_tab : " << (int)red_tab.size() << endl;

  red_tab.clear();
  
  for(i = 0; i < red_sizes.size(); i++)
    {
      center[4 * (i)] = h;
      center[4 * (i) + 1] = 0;
      center[4 * (i) + 2] = w;
      center[4 * (i) + 3] = 0;
    }

  cout << "begin" << endl;

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(red_map[i*w + j])
	  {
	    min_i = center[4 * (red_map[i * w + j] - 1)];
	    max_i = center[4 * (red_map[i * w + j] - 1) + 1];
	    min_j = center[4 * (red_map[i * w + j] - 1) + 2];
	    max_j = center[4 * (red_map[i * w + j] - 1) + 3];
	    
	    if(i < min_i)
	      center[4 * (red_map[i * w + j] - 1)] = i;
	    if(i > max_i)
	      center[4 * (red_map[i * w + j] - 1) + 1] = i;
	    if(j < min_j)
	      center[4 * (red_map[i * w + j] - 1) + 2] = j;
	    if(j > max_j)
	      center[4 * (red_map[i * w + j] - 1) + 3] = j;
	  }
      }

  for(i = 0; i < red_sizes.size(); i++)
    {
      // red_tab : size bi bj
      red_tab.push_back(red_sizes[i]);
      red_tab.push_back((center[4 * i + 1] + center[4 * i])/2);
      red_tab.push_back((center[4 * i + 3] + center[4 * i + 2])/2);
      //cout << " red = " << red_tab[3*i] << "  size = " << red_tab.size() << endl;
    }
}

void barycentre(int* red_map, vector<int> red_tab, int h, int w);

/**
 *crit_size : size_red map of img
 *crit_ratio : map ratio red_size/size
 *crit_dens : map density red regions
 *map : region label
 *red_map : red regions label
 *tmp_map : tmp copy red_map
 *sizes : sizes of regions
 *regions : map result window
 *red_tab : map barycenters of red_regions + sizes
 *size_max : window size max height
 *size_min : ratio min regions_size/window_size
 *wh : windows min height
 *ww : windows min width
 *step : delta window size
 * h * w = n
 **/
void a_fusion(OCTET* crit_size, OCTET* crit_ratio, OCTET* crit_dens, int* map, int* red_map, int *tmp_map, vector<int> sizes, vector<int>& regions, vector<int> red_tab, int size_max, float size_min, int wh, int ww, float step, int h, int w)
{
  int i, j, k, l, kmin, kmax, lmin, lmax, crit, sum, ind, correct, maxs = size_max, n = h*w;
  int clear, win_h = wh, win_w = ww;
  float win = 1, val_crit = 0.5;

  regions.clear();

  for(i = 0; i < h*w; i++)
    tmp_map[i] = red_map[i];

  while(maxs > 20) //*******************  seuil red_zone
    {
      //cout <<"loop" << endl;
      maxs = 0;
      ind = 0;
	
      for(i = 0; i < red_tab.size(); i+=3)
	{
	  if(red_tab[i] > maxs)
	    {
	      maxs = red_tab[i];
	      ind = i;
	      //cout << "red_tab = " << red_tab[i] << endl;
	    }
	}
      clear = 0;
      while(win_h < size_max && !clear)
	{
	  //cout << "gitmax = " << maxs << endl;
	  win_h *= win++ * step + 1.0;
	  win_w *= win++ * step + 1.0;
	  
	  kmin = max(0, red_tab[ind + 1] - win_h);
	  kmax = min(h, red_tab[ind + 1] + win_h);
	  lmin = max(0, red_tab[ind + 2] - win_w);
	  lmax = min(w, red_tab[ind + 2] + win_w);

	  //cout << kmin << " " << kmax << " " << lmin << " " << lmax << endl;
      
	  sum = 0;
	  crit = 0;
	  	  
	  for(k = kmin; k < kmax; k++)
	    for(l = lmin; l < lmax; l++)
	      {
		//cout << "truelopp" << endl;
		if(tmp_map[k * w + l] > 0)
		  {
		    crit += ((crit_size[k * w + l] == 255) || (crit_ratio[k * w + l] == 255) || (crit_dens[k * w + l] != 255));
			      
		    //cout << "begin" << endl;
		    correct = tmp_map[k * w + l];
		    sum += sizes[map[k * w + l] - 1];
		    //cout << "git sum = " << sizes[map[k * w + l] - 1] << endl;
		    for(i = kmin; i < kmax; i++)
		      for(j = lmin; j < lmax; j++)
			if(tmp_map[i * w + j] == correct)
			  tmp_map[i * w + j] = -1;
		  }
	      }

	  //cout << "endtrue loop" << endl;

	  // cout << "sum = " << sum << endl;
      
	  if(sum < size_min * win_h * win_w)
	    {
	      //Check crits
	      if(crit > 0)//***************************************
		{
		  regions.push_back(red_tab[ind + 1]);
		  regions.push_back(red_tab[ind + 2]);
		  //regions.push_back(win);

		  clear = 1;
		}
	    }
	  else
	    win += size_max;
	}
      if(clear)
	{
	  for(k = kmin; k < kmax; k++)
	    for(l = lmin; l < lmax; l++)
	      if(tmp_map[k * w + l] == -1)
		tmp_map[k*w + l] = red_map[k*w + l];
	}
      
      red_tab[ind] = 0;
      //   cout << "end fusion" << endl;
    }
}

void fusion(int* map, int* red_map, int *tmp_map, vector<int> sizes, vector<int>& regions, vector<int> red_tab, int size_max, int size_min, int wh, int ww, int h, int w)
{
  int i, j, k, l, kmin, kmax, lmin, lmax,  sum, ind, correct, maxs = size_max;
  cout << "begin" << endl;
  
  regions.clear();
  
  for(i = 0; i < h*w; i++)
    tmp_map[i] = red_map[i];

  for(i = 0; i < sizes.size(); i++)
    {
      if(red_tab[3 * i] > 20)
	cout << "bi = " << red_tab[3 * i + 1] << "  bj = " << red_tab[3 * i + 2] <<endl;
    }
  
  while(maxs > 20) //*******************  seuil red_zone
    {
      //cout <<"loop" << endl;
      maxs = 0;
      ind = 0;
      
      for(i = 0; i < red_tab.size(); i+=3)
	{
	  if(red_tab[i] > maxs)
	    {
	      maxs = red_tab[i];
	      ind = i;
	      //cout << "red_tab = " << red_tab[i] << endl;
	    }
	}
      
      //cout << "gitmax = " << maxs << endl;
      
      kmin = max(0, red_tab[ind + 1] - wh);
      kmax = min(h, red_tab[ind + 1] + wh);
      lmin = max(0, red_tab[ind + 2] - ww);
      lmax = min(w, red_tab[ind + 2] + ww);

      //cout << kmin << " " << kmax << " " << lmin << " " << lmax << endl;
      
      sum = 0;
      
      for(k = kmin; k < kmax; k++)
    	for(l = lmin; l < lmax; l++)
    	  {
    	    //cout << "truelopp" << endl;
    	    if(tmp_map[k * w + l] > 0)
    	      {
    		//cout << "begin" << endl;
    		correct = tmp_map[k * w + l];
    		sum += sizes[map[k * w + l] - 1];
		//cout << "git sum = " << sizes[map[k * w + l] - 1] << endl;
    		for(i = kmin; i < kmax; i++)
    		  for(j = lmin; j < lmax; j++)
    		    if(tmp_map[i * w + j] == correct)
    		      tmp_map[i * w + j] = -1;
    	      }
    	  }

      //cout << "endtrue loop" << endl;

      // cout << "sum = " << sum << endl;
      
      if(sum >= size_min && sum < size_max)
    	{
    	  regions.push_back(red_tab[ind + 1]);
    	  regions.push_back(red_tab[ind + 2]);
    	}
      else
    	for(k = kmin; k < kmax; k++)
    	  for(l = lmin; l < lmax; l++)
    	    if(tmp_map[k * w + l] == -1)
    	      tmp_map[k*w + l] = red_map[k*w + l];
            
      red_tab[ind] = 0;
      //   cout << "end fusion" << endl;
    }
  
  cout << "nb fenetre : " << regions.size() << endl;  
  
}

     

void draw_window(OCTET* img, vector<int> regions, int ind, int wh, int ww, int h, int w)
{
  int i, k, l, kmin, kmax, lmin, lmax, bi, bj, pad = 5;
  char name[250];
  
  for(i = 0; i < regions.size(); i+=2)
    {
      bi = regions[i];
      bj = regions[i + 1];
      
      kmin = max(0, bi - wh);
      kmax = min(h, bi + wh);
      lmin = max(0, bj - ww);
      lmax = min(w, bj + ww);

      for(k = kmin; k < kmin + pad; k++)//top
	for(l = lmin; l < lmax; l++)
	  {
	    img[3 * (k * w + l)] = 255;
	    img[3 * (k * w + l) + 1] = 25;
	    img[3 * (k * w + l) + 2] = 25;
	  }
      for(k = kmax - pad; k < kmax; k++)//bot
	for(l = lmin; l < lmax; l++)
	  {
	    img[3 * (k * w + l)] = 255;
	    img[3 * (k * w + l) + 1] = 25;
	    img[3 * (k * w + l) + 2] = 25;
	  }
      for(k = kmin; k < kmax; k++)//left
	for(l = lmin; l < lmin + pad; l++)
	  {
	    img[3 * (k * w + l)] = 255;
	    img[3 * (k * w + l) + 1] = 25;
	    img[3 * (k * w + l) + 2] = 25;
	  }
      for(k = kmin; k < kmax; k++)//right
	for(l = lmax - pad; l < lmax; l++)
	  {
	    img[3 * (k * w + l)] = 255;
	    img[3 * (k * w + l) + 1] = 25;
	    img[3 * (k * w + l) + 2] = 25;
	  }
    }
  sprintf(name, "adapt_result_%d.ppm", ind);
  ecrire_image_ppm(name, img, h, w);
}
      

