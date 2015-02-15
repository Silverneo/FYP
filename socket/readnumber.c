#include<stdio.h>
#include<stdlib.h>

int main(void)
{
    FILE *fp = fopen("test.wav", "rb");
    FILE *fr = fopen("test.txt", "wt");
    int i;
    int lSize;
    int nSamp;
    short *buffer;
    fseek(fp, 0, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    nSamp = lSize / 2;

    buffer = (short *) malloc(nSamp * sizeof(short));

    fread(buffer, sizeof(short), nSamp, fp);

    for (i = 0; i < nSamp; i++)
    {
        fprintf(fr, "%hd\n", buffer[i]);
    }
    printf("%d\n", sizeof(short));
    fclose(fp);
    fclose(fr);
    return 0;

}
