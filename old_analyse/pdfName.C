int pdfName(char *filename)
{
  int n = strlen(filename)-5;
  char pdfname[n];

  for (int i = 0; i<n; i ++)
  {
    pdfname[i] = filename[i];
  }
cout << pdfname << endl;
  return 0;
}
