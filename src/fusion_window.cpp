void get_window(OCTET* img, OCTET* mask, int size_max, int size_min, int wh, int ww, int h, int w, OCTET s)
{
  int* red_map, *map;
  vector<int> sizes;
  vector<int> red_size;
  vector<int> red_tab;
  vector<int> regions;

  allocation_tableau(red_map, int, h*w);
  allocation_tableau(map, int, h*w);
  
  cout << "label" << endl;
  label(img, mask, map, sizes, s, h, w);
  write_label(map, img, mask, h, w, sizes.size());
  cout << "red zone" << endl;
  red_zone(img, mask, red_map, red_size, s, h, w);
  cout << "filter" << endl;
  red_filter(red_map, red_size, red_tab, h, w);
  cout << "fusion" << endl;
  fusion(map, red_map, sizes, regions, red_tab, size_max, size_min, wh, ww, h, w);
}  
  

void red_filter(int* red_map, vector<int> red_sizes, vector<int> red_tab, int h, int w)
{
  int i, j, *center, min_i, max_i, min_j, max_j;

  cout << "  red_size : " << (int)red_sizes.size() << "   red_tab : " << (int)red_tab.size() << endl;
  
  i = red_sizes.size();

  allocation_tableau(center, int, 4 * i); // mini maxi minj maxj
 
  for(i = 0; i < red_sizes.size(); i++)
    {
      center[4 * (i)] = h;
      center[4 * (i) + 1] = 0;
      center[4 * (i) + 2] = w;
      center[4 * (i) + 3] = 0;
    }

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	min_i = center[4 * (red_map[i * w + j])];
	max_i = center[4 * (red_map[i * w + j]) + 1];
	min_j = center[4 * (red_map[i * w + j]) + 2];
	max_j = center[4 * (red_map[i * w + j]) + 3];

	if(i < min_i)
	  center[4 * (red_map[i * w + j])] = i;
	if(i > max_i)
	  center[4 * (red_map[i * w + j]) + 1] = i;
	if(j < min_j)
	  center[4 * (red_map[i * w + j]) + 2] = j;
	if(j > max_j)
	  center[4 * (red_map[i * w + j]) + 3] = j;
      }

  for(i = 0; i < red_sizes.size(); i++)
    {
      // red_tab : size bi bj
      red_tab.push_back(red_sizes[i]);
      red_tab.push_back((center[4 * i + 1] - center[4 * i])/2);
      red_tab.push_back((center[4 * i + 3] - center[4 * i + 2])/2);
    }
}

void barycentre(int* red_map, vector<int> red_tab, int h, int w);

void fusion(int* map, int* red_map, vector<int> sizes, vector<int> regions, vector<int> red_tab, int size_max, int size_min, int wh, int ww, int h, int w)
{
  int i, j, k, l, kmin, kmax, lmin, lmax,  sum, ind, correct, maxs = size_max, *tmp_map;
  cout << "begin" << endl;
  allocation_tableau(tmp_map, int, h*w);

  for(i = 0; i < h*w; i++)
    tmp_map[i] = red_map[i];
  
  while(maxs > 60) //*******************  seuil red_zone
    {
      cout <<"loop" << endl;
      maxs = 0;
      ind = 0;
      for(i = 0; i < red_tab.size(); i++)
	{
	  if(red_tab[3 * i] > maxs)
	    {
	      maxs = red_tab[3 * i];
	      ind = i;
	      cout << "red_tab = " << red_tab[3*i] << endl;
	    }
	}
      cout << "gitmax = " << maxs << endl;
      
      kmin = min(0, red_tab[3 * ind + 1] - wh);
      kmax = max(h, red_tab[3 * ind + 1] + wh);
      lmin = min(0, red_tab[3 * ind + 2] - ww);
      lmax = max(w, red_tab[3 * ind + 2] + ww);

      sum = 0;
      
      for(k = kmin; k < kmax; k++)
	for(l = lmin; l < lmax; l++)
	  {
	    cout << "truelopp" << endl;
	    if(tmp_map[k * (lmax - lmin + 1) + l])
	      {
		correct = tmp_map[k * w + l];
		sum += sizes[map[k * w + l]];
		cout << "git sum = " << sum << endl;
		for(i = kmin; i < kmax; i++)
		  for(j = lmin; j < lmax; j++)
		    if(tmp_map[k * w + l] == correct)
		      tmp_map[k*w + l] = -1;
	      }
	  }

      cout << "endtrue loop" << endl;

      if(sum > size_min && sum < size_max)
	{
	  regions.push_back(red_tab[3 * ind + 1]);
	  regions.push_back(red_tab[3 * ind + 2]);
	  red_tab[3 * ind] = 0;
	}
      else
	for(i = kmin; i < kmax; i++)
	  for(j = lmin; j < lmax; j++)
	    if(tmp_map[k * w + l] == -1)
	      tmp_map[k*w + l] = red_map[k*w + l];
    }
}

     

void draw_window(OCTET* img, vector<int> regions, int wh, int ww, int h, int w)
{
  int i, k, l, kmin, kmax, lmin, lmax, bi, bj, pad = 5;

  for(i = 0; i < regions.size(); i++)
    {
      bj = regions[2 * i];
      bi = regions[2 * i + 1];
      
      kmin = min(0, bi - wh);
      kmax = max(h, bi + wh);
      lmin = min(0, bj - ww);
      lmax = max(w, bj + ww);

      for(k = kmin; k < kmin + pad; k++)//top
	for(l = lmin; l < lmax; l++)
	  img[k * w + l] = 255;
      for(k = kmax - pad; k < kmax; k++)//bot
	for(l = lmin; l < lmax; l++)
	  img[k * w + l] = 255;
      for(k = kmin; k < kmax; k++)//left
	for(l = lmin; l < lmin + pad; l++)
	  img[k * w + l] = 255;
      for(k = kmin; k < kmax; k++)//right
	for(l = lmax - pad; l < lmax; l++)
	  img[k * w + l] = 255;
    }
  ecrire_image_pgm("fenetre.pgm", img, h, w);
}
      

