#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void run_spoon();
void run_exif();

void gui_write(char *);

void update_inFile();
void update_outDir();

int main(int, char **);
