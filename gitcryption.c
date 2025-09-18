#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int arc, char* argv[]){
    FILE *infile = NULL, *outfile = NULL, *keyfile = NULL;
    int c;
    while ((c = getopt(arc, argv, "i:o:k:")) != -1){
        switch (c){
            case 'i':
                infile = fopen(optarg, "rb");
                if (infile == NULL){
                    perror("Error opening input file");
                    return 1;
                }
                printf("Input file: %s\n", optarg);
                break;
            case 'o':
                outfile = fopen(optarg, "wb");
                if (outfile == NULL){
                    perror("Error opening output file");
                    return 1;
                }
                printf("Output file: %s\n", optarg);
                break;
            case 'k':
                keyfile = fopen(optarg, "rb");
                if (keyfile == NULL){
                    perror("Error opening key file");
                    return 1;
                }
                printf("Key file: %s\n", optarg);
                break;
            case '?':
                if (optopt == 'i' || optopt == 'o' || optopt == 'k'){
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option `-%c`.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
        
    }
    // Close files if opened
    if (infile) fclose(infile);
    if (outfile) fclose(outfile);
    if (keyfile) fclose(keyfile);
}