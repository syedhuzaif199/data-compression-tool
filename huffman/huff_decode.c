#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node
{
    struct node *left, *right;
    int freq;
    char c;
} *Node;

int nodeCount = 0, qEnd = 1;
struct node pool[256] = {{0}};
Node queue[255], *q = queue - 1;
char buf[1024];

Node newNode(int freq, char c, Node left, Node right)
{
    Node n = pool + nodeCount++;
    if (freq != 0)
    {
        n->c = c;
        n->freq = freq;
    }
    else
    {
        n->left = left, n->right = right;
        n->freq = left->freq + right->freq;
    }
    return n;
}

void enqueue(Node n)
{
    int j, i = qEnd++;
    while ((j = i / 2))
    {
        if (q[j]->freq <= n->freq)
            break;
        q[i] = q[j], i = j;
    }
    q[i] = n;
}

Node dequeue()
{
    int i, l;
    Node n = q[i = 1];

    if (qEnd < 2)
        return 0;
    qEnd--;
    while ((l = i * 2) < qEnd)
    {
        if (l + 1 < qEnd && q[l + 1]->freq < q[l]->freq)
            l++;
        q[i] = q[l], i = l;
    }
    q[i] = q[qEnd];
    return n;
}

void setFrequencies(FILE *fpFreq, unsigned int *freq)
{
    char c;
    int i = 0;

    while ((c = fgetc(fpFreq)) != EOF)
    {
        freq[i++] = (unsigned char)c;
    }
    for (i = 0; i < 128; i++)
        if (freq[i])
            enqueue(newNode(freq[i], i, 0, 0));
    while (qEnd > 2)
        enqueue(newNode(0, 0, dequeue(), dequeue()));
}

void decode(FILE *fpHuff, FILE *fpOut)
{
    int i = 0, lim = 0, j = 0;
    char c;
    Node n = q[1];

    fscanf(fpHuff, "%d", &lim);
    fseek(fpHuff, 1, SEEK_CUR);

    for (i = 0; i < lim; i++)
    {
        if (j == 0)
            c = fgetc(fpHuff);

        if (c & 128)
            n = n->right;
        else
            n = n->left;
        if (n->c)
        {
            fputc(n->c, fpOut);
            n = q[1];
        }
        c = c << 1;
        if (++j > 7)
            j = 0;
    }
}

int main(int argc, char *argv[])
{
    FILE *fpFreq, *fpHuff, *fpOut;
    char fileName[50] = {0}, temp[50] = {0};
    unsigned int freq[128] = {0};

    system("clear");

    if (argc == 2)
    {
        strcpy(fileName, argv[1]);
        if (strstr(fileName, ".huff") == NULL)
        {
            printf("\nERROR:wrong file format!\n");
            return 0;
        }
    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
    }
    else
    {
        printf("Please enter the name of the compressed file\t: ");
        scanf("%s", fileName);
        if (strstr(fileName, ".huff") == NULL)
        {
            printf("\nERROR:wrong file format!\n");
            return 0;
        }
    }

    if ((fpHuff = fopen(fileName, "r")) == NULL)
    {
        printf("\nERROR: No such file\n");
        return 0;
    }
    *strstr(fileName, ".huff") = '\0';
    strcat(fileName, ".freq");
    if ((fpFreq = fopen(fileName, "r")) == NULL)
    {
        printf("\nERROR: Frequency table cannot be found\n");
        return 0;
    }
    setFrequencies(fpFreq, freq);

    *strstr(fileName, ".freq") = '\0';
    strcpy(temp, "mkdir ");
    strcat(temp, fileName);
    system(strcat(temp, ".decoded"));

    strcpy(temp, "./");
    strcat(temp, fileName);
    strcat(temp, ".decoded/");
    if ((fpOut = fopen(strcat(temp, fileName), "w")) == NULL)
    {
        printf("ERROR:Creating decoded file failed\n");
        return 0;
    }
    decode(fpHuff, fpOut);

    fclose(fpHuff);
    fclose(fpFreq);
    fclose(fpOut);
    return 0;
}