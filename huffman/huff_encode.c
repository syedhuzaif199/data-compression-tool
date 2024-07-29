#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct node
{
    struct node *left, *right;
    int freq;
    char c;
} *Node;

// global variables
int nodeCount = 0, qEnd = 1;
struct node pool[256] = {{0}};
Node queue[255], *q = (queue - 1);
char *code[128] = {0}, buf[1024];
int inputData = 0, outputData = 0;

Node newNode(int freq, char c, Node left, Node right)
{
    Node node = pool + nodeCount++;
    if (freq != 0)
    {
        node->c = c;
        node->freq = freq;
    }
    else
    {
        node->left = left;
        node->right = right;
        node->freq = left->freq + right->freq;
    }
    return node;
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
    Node node = q[i = 1];

    if (qEnd < 2)
    {
        return 0;
    }
    qEnd--;

    while ((l = i * 2) < qEnd)
    {
        if (l + 1 < qEnd && q[l + 1]->freq < q[l]->freq)
            l++;
        q[i] = q[l], i = l;
    }
    q[i] = q[qEnd];
    return node;
}

void assignCodes(Node n, char *s, int len)
{
    static char *out = buf;
    if (n->c)
    {
        s[len] = 0;
        strcpy(out, s);
        code[(int)n->c] = out;
        out += len + 1;
        return;
    }

    s[len] = '0';
    assignCodes(n->left, s, len + 1);
    s[len] = '1';
    assignCodes(n->right, s, len + 1);
}

void readInputFile(FILE *fpIn, unsigned int *freq)
{
    char c, s[16] = {0};
    int i = 0;
    while ((c = fgetc(fpIn)) != EOF)
    {
        freq[(int)c]++;
    }
    for (i = 0; i < 128; i++)
        if (freq[i])
            enqueue(newNode(freq[i], i, 0, 0));
    while (qEnd > 2)
        enqueue(newNode(0, 0, dequeue(), dequeue()));
    assignCodes(q[1], s, 0);
}

void encodeOutputFile(FILE *fpIn, FILE *fpOut, unsigned int *freq)
{

    char in, c, temp[20] = {0};
    int i, j = 0, k = 0, lim = 0;
    rewind(fpIn);
    for (i = 0; i < 128; i++)
    {
        if (freq[i])
            lim += (freq[i] * strlen(code[i]));
    }
    outputData = lim;
    fprintf(fpOut, "%04d\n", lim);
    for (i = 0; i < lim; i++)
    {
        if (temp[j] == '\0')
        {
            in = fgetc(fpIn);
            strcpy(temp, code[in]);
            j = 0;
        }
        if (temp[j] == '1')
            c = c | (1 << (7 - k));
        else if (temp[j] == '0')
            c = c | (0 << (7 - k));
        else
            printf("ERROR: Wrong input!\n");
        k++;
        j++;
        if (((i + 1) % 8 == 0) || (i == lim - 1))
        {
            k = 0;
            fputc(c, fpOut);
            c = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *fpIn, *fpOut;
    char fileName[50] = {0};
    unsigned int freq[128] = {0}, i;

    system("clear");

    if (argc == 2)
    {
        strcpy(fileName, argv[1]);
    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
    }
    else
    {
        printf("Please enter the file to be compressed\t: ");
        scanf("%s", fileName);
    }

    if (strlen(fileName) >= 50)
    {
        printf("ERROR: Enter a file name less than 50 characters long");
        return 0;
    }

    if ((fpIn = fopen(fileName, "r")) == NULL)
    {
        printf("\nERROR: No such file\n");
        return 0;
    }
    readInputFile(fpIn, freq);

    strcat(fileName, ".huff");
    fpOut = fopen(fileName, "w");
    encodeOutputFile(fpIn, fpOut, freq);

    fclose(fpIn);
    fclose(fpOut);
    *strstr(fileName, ".huff") = '\0';
    strcat(fileName, ".freq");
    fpOut = fopen(fileName, "w");
    for (i = 0; i < 128; i++)
    {
        fprintf(fpOut, "%c", (char)freq[i]);
    }

    for (i = 0; i < 128; i++)
        inputData += freq[i];

    fclose(fpOut);

    return 0;
}