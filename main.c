

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

//structure used to define a node
typedef struct node_t {
    struct node_t *left, *right;
    int freq;
    char c;
} *node;

//global variables
int n_nodes = 0, qend = 1; 		//global variables for keep track of no.of nodes and end of the que
struct node_t pool[256] = {{0}};        //pool of nodes
node qqq[255], *q = qqq-1;      	//the priority que
char *code[128] = {0}, buf[1024];	//a string array of the codes for each letter
int input_data=0,output_data=0;

//function used to create a new node
node new_node(int freq, char c, node a, node b)
{
    node n = pool + n_nodes++;
    if (freq != 0){
        n->c = c;			//assign the character 'c' to the character of the node (eventually a leaf)
        n->freq = freq;			//assign frequency
    }
    else {
        n->left = a, n->right = b;	//if there is no frequency provided with the invoking
        n->freq = a->freq + b->freq;	//the removed nodes at the end of the que will be added to left and right
    }
    return n;
}

//function ued to insert a node into the priority que
void qinsert(node n)
{
    int j, i = qend++;
    while ((j = i / 2)) {
        if (q[j]->freq <= n->freq) break;
        q[i] = q[j], i = j;
    }
    q[i] = n;
}

node qremove()
{
    int i, l;
    node n = q[i = 1];

    if (qend < 2) return 0;
    qend--;
    while ((l = i * 2) < qend) {
        if (l + 1 < qend && q[l + 1]->freq < q[l]->freq) l++;
        q[i] = q[l], i = l;
    }
    q[i] = q[qend];
    return n;	//return the node
}

//go along the builded huffman tree and assign the code for each character
void build_code(node n, char *s, int len)
{
    static char *out = buf;
    if (n->c) {
        s[len] = 0;		//if the provided node is a leaf (end node)
        strcpy(out, s);		//it contains a character
        code[(int)n->c] = out;	//therefore the code is copied in to the relevant character.
        out += len + 1;		//out pointer is incremented
        return;
    }

    s[len] = '0'; build_code(n->left,  s, len + 1);	//recurring is used to write out the code
    s[len] = '1'; build_code(n->right, s, len + 1);	//if right add a 1 and if right add a 0
}

void import_file(FILE *fp_in, unsigned int *freq){
    char c,s[16]={0};		//temporary variables
    int i = 0;
    printf("File Read:\n");
    while((c=fgetc(fp_in))!=EOF){
        freq[(int)c]++;		//read the file character by character and increment the particular frequency
        putchar(c);
    }
    for (i = 0; i < 128; i++)
        if (freq[i]) qinsert(new_node(freq[i], i, 0, 0));//insert new nodes into the que if there is a frequency
    while (qend > 2)
        qinsert(new_node(0, 0, qremove(), qremove()));	//build the tree
    build_code(q[1], s, 0);		//build the code for the characters
}

void encode(FILE* fp_in, FILE* fp_out, unsigned int *freq ){

    char in,c,temp[20] = {0};
    int i,j=0,k=0,lim=0;
    rewind(fp_in);
    for(i=0; i<128; i++){
        if(freq[i])	lim += (freq[i]*strlen(code[i]));
    }
    output_data = lim;			//The output data is equal to the limit
    fprintf(fp_out,"%04d\n",lim);
    printf("\nEncoded:\n");
    for(i=0; i<lim; i++){
        if(temp[j] == '\0'){
            in = fgetc(fp_in);
            strcpy(temp,code[in]);
            printf("%s",code[in]);
            j = 0;
        }
        if(temp[j] == '1')
            c = c|(1<<(7-k));       //shifts 1 to relevant position and OR with the temporary char
        else if(temp[j] == '0')
            c = c|(0<<(7-k));       //shifts 0 to relevant position and OR with the temporary char
        else
            printf("ERROR: Wrong input!\n");
        k++;                            // k is used to divide the string into 8 bit chunks and save
        j++;
        if(((i+1)%8 == 0) || (i==lim-1)){
            k = 0;                  //reset k
            fputc(c,fp_out);        //save the character
            c = 0;                  //reset character
        }
    }
    putchar('\n');
}

void print_code(unsigned int *freq){
    int i;
    printf("\n---------CODE TABLE---------\n----------------------------\nCHAR  FREQ  CODE\n----------------------------\n");
    for(i=0; i<128; i++){
        if(isprint((char)i)&&code[i]!=NULL&&i!=' ')
            printf("%-4c  %-4d  %16s\n",i,freq[i],code[i]);
        else if(code[i]!=NULL){
            switch(i){
                case '\n':
                    printf("\\n  ");
                    break;
                case ' ':
                    printf("\' \' ");
                    break;
                case '\t':
                    printf("\\t  ");
                    break;
                default:
                    printf("%0X  ",(char)i);
                    break;
            }
            printf("  %-4d  %16s\n",freq[i],code[i]);
        }
    }
    printf("----------------------------\n");

}

//-------------------------------------------------------------------------------
int splitFile(char *fileIn, size_t maxSize)
{
    int result = 0;
    FILE *fIn;
    FILE *fOut;
    char buffer[1024 * 16];
    size_t size;
    size_t read;
    size_t written;


    if ((fileIn != NULL) && (maxSize > 0))
    {
        fIn = fopen(fileIn, "rb");
        if (fIn != NULL)
        {
            fOut = NULL;
            result = 1;   /* we have at least one part */

            while (!feof(fIn))
            {
                /* initialize (next) output file if no output file opened */
                if (fOut == NULL)
                {
                    sprintf(buffer, "%s.%03d", fileIn, result);
                    fOut = fopen(buffer, "wb");
                    if (fOut == NULL)
                    {
                        result *= -1;
                        break;
                    }

                    size = 0;
                }

                /* calculate size of data to be read from input file in order to not exceed maxSize */
                read = sizeof(buffer);
                if ((size + read) > maxSize)
                {
                    read = maxSize - size;
                }

                /* read data from input file */
                read = fread(buffer, 1, read, fIn);
                if (read == 0)
                {
                    result *= -1;
                    break;
                }

                /* write data to output file */
                written = fwrite(buffer, 1, read, fOut);
                if (written != read)
                {
                    result *= -1;
                    break;
                }

                /* update size counter of current output file */
                size += written;
                if (size >= maxSize)   /* next split? */
                {
                    fclose(fOut);
                    fOut = NULL;
                    result++;
                }
            }

            /* clean up */
            if (fOut != NULL)
            {
                fclose(fOut);
            }
            fclose(fIn);
        }
    }

    return (result);
}
//---------------------------------------------------------------------
size_t divideTamanoTotalEn8(const char* filename)
{
    size_t size;
    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL) return -1;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);

    return size/8;
}
//--------------------------------------------------------------------------
int main(int argc, char* argv[]){
    FILE *fp_in, *fp_out;				//FIFO pointers
    char file_name[50]={0};				//file name
    unsigned int freq[128] = {0},i;			//frequency of the letters -------> Hay que bloquear con semaforo




    if( argc == 2 ) {
        strcpy(file_name,argv[1]);		//commandline argument directly allows to compress the file
    }
    else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
    }
    else {
        printf("Please enter the file to be compressed\t: "); //else a prompt comes to enter the file name
        scanf("%s",file_name);
    }

    if(strlen(file_name)>=50){
        printf("ERROR: Enter a file name less than 50 chars");
        return 0;
    }

    //import the file into the program and update the huffman tree
    if((fp_in = fopen(file_name,"rb"))==NULL){	//open the file stream
        printf("\nERROR: No such file\n");
        return 0;
    }
    size_t tamano = divideTamanoTotalEn8("prueba.txt");
  //  printf("You entered: %d",tamano );
    splitFile("prueba.txt",tamano);



   /* struct stat sb;
    if (stat(file_name, &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    */
   /* char *file_contents1 = malloc(sb.st_size/8);  // campo en memoria
    char *file_contents2 = malloc(sb.st_size/8);
    char *file_contents3 = malloc(sb.st_size/8);
    char *file_contents4 = malloc(sb.st_size/8);
    char *file_contents5 = malloc(sb.st_size/8);
    char *file_contents6 = malloc(sb.st_size/8);
    char *file_contents7 = malloc(sb.st_size/8);
    char *file_contents8 = malloc(sb.st_size/8);
    int part1 =  sb.st_size / 8; //divide el tamaño del documento en 8 partes (para 8 hilos)
    int part2 =  (sb.st_size / 8)+1;
    int part3 =  (sb.st_size / 8)+1;
    int part4 =  (sb.st_size / 8)+1;
    int part5 =  (sb.st_size / 8)+1;
    int part6 =  (sb.st_size / 8)+1;
    int part7 =  (sb.st_size / 8)+1;
    int part8 =  (sb.st_size / 8)+1;


    fread(file_contents1, part1, 1, fp_in);
    fread(file_contents2, part2, 1, fp_in);
    fread(file_contents3, part3, 1, fp_in);
    fread(file_contents4, part4, 1, fp_in);
    fread(file_contents5, part5, 1, fp_in);
    fread(file_contents6, part6, 1, fp_in);
    fread(file_contents7, part7, 1, fp_in);
    fread(file_contents8, part8, 1, fp_in);

*/

   /* import_file(fp_in,freq);			//import the file and fills frequency array


    print_code(freq);				//print the code table

    //Encode and save the encoded file
    strcat(file_name,".huffman");			//encoded file is saved in .huffman extension
    fp_out = fopen(file_name,"w");
    encode(fp_in,fp_out,freq);

    fclose(fp_in);
    fclose(fp_out);

    strcat(file_name,".table");			//write the frequency table in .table extension
    fp_out = fopen(file_name,"w");
    for(i=0; i<128; i++){
        fprintf(fp_out,"%c",(char)freq[i]);	//write the frequency to .table file
    }

    for(i=0; i<128; i++)	input_data += freq[i];	//calculate input bytes

    fclose(fp_out);					//close the output file pointer
    printf("\nInput bytes:\t\t%d\n",input_data);
    output_data = (output_data%8)? (output_data/8)+1 : (output_data/8);
    printf("Output bytes:\t\t%d\n",output_data);

    printf("\nCompression ratio:\t%.2f%%\n\n\n",((double)(input_data-output_data)/input_data)*100);
    */
    return 0;
}