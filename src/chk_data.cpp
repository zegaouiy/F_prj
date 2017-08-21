void read_map(int* map, char* nom, int n)
{
  FILE *f_image;
  int  nb_colonnes, nb_lignes, max_grey_val;
  taille_image=3*taille_image;
   
  if( (f_image = fopen(nom, "rb")) == NULL)
    {
      printf("\nPas d'acces en ecriture sur l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }
   
  if( (fread((int*)map, sizeof(int), n, f_image))
      != (int)(n))
    {
      printf("\nErreur d'ecriture de l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }
  fclose(f_image);
}

void count_window(vector<int> win, ofstream count, int i)
{
  if (count.is_open())
    {
      count << i << " " << win.size() << endl;
    }
}

