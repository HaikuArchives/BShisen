#include <TranslationUtils.h>
#include <Bitmap.h>
#include <unistd.h>
#include <stdio.h>
#include <TranslatorRoster.h>



void main(int argc, char **argv)
{

argv++;
argc--;

BBitmap *bm;
void *bits;
int len;


BTranslatorRoster *roster = BTranslatorRoster::Default();

if (argc)
{
	bm = BTranslationUtils::GetBitmapFile(*argv);//, roster);
	if (bm)
	{
		len = bm->BitsLength();
		bits = bm->Bits();
		write(STDOUT_FILENO, bits, len);
	}
	else
		fprintf(stderr, "couldn't open file %s\n", *argv);

}


}
