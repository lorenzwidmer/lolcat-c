/* Copyright (C) 2014 jaseg <github@jaseg.net>
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * Version 2, December 2004
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>


#include "include/ansiescape.hh"
#include "include/ansiescape/Parser.hh"


static char helpstr[] = "\n"
"Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n"
"\n"
"Concatenate FILE(s), or standard input, to standard output.\n"
"With no FILE, or when FILE is -, read standard input.\n"
"\n"
"              -h <d>:   Horizontal rainbow frequency (default: 0.23)\n"
"              -v <d>:   Vertical rainbow frequency (default: 0.1)\n"
"                  -f:   Force color even when stdout is not a tty\n"
"           --version:   Print version and exit\n"
"              --help:   Show this message\n"
"\n"
"Examples:\n"
"  lolcat f - g      Output f's contents, then stdin, then g's contents.\n"
"  lolcat            Copy standard input to standard output.\n"
"  fortune | lolcat  Display a rainbow cookie.\n"
"\n"
"Report lolcat bugs to <http://www.github.org/jaseg/lolcat/issues>\n"
"lolcat home page: <http://www.github.org/jaseg/lolcat/>\n"
"Original idea: <http://www.github.org/busyloop/lolcat/>\n";

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))
const unsigned char codes[] = {39,38,44,43,49,48,84,83,119,118,154,148,184,178,214,208,209,203,204,198,199,163,164,128,129,93,99,63,69,33};

void usage(){
    printf("Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n");
    exit(1);
}

void version(){
    printf("lolcat version 0.1, (c) 2014 jaseg\n");
    exit(0);
}

inline void printColor(char c, int colors, int* i, int* l, int* s, int* cc, double* offset, double *freq_h, double *freq_v)
{
    if(c >= 32 && colors){
        int ncc = *offset + (*i+=wcwidth(c))*(*freq_h) + (*l + *s)*(*freq_v);
        if(*cc != ncc){
            cc = &ncc;
            printf("\033[38;5;%hhum", codes[ncc % ARRAY_SIZE(codes)]);
        }
    } else {
    	if(c == '\n'){
            *l++;
            *i = 0;
        }else if(c == '\b'){
            *i = *i - 1;
        }else if(c == '\r'){
            *i = 0;
        }
    }

    printf("%lc", c);
}

int main(int argc, char* argv[]){
    int c = 0, cc=-1, i, a, l=0, s=0;
    int colors=1;
    double freq_h = 0.23, freq_v = 0.1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    double offset = rand()%ARRAY_SIZE(codes);

    for(a=1;a<argc;a++){
        char *endptr;
        if(!strcmp(argv[a], "-h")){
            if((++a)<argc){
                freq_h = strtod(argv[a], &endptr);
                if(*endptr)
                    usage();
            }else{
                usage();
            }
        }else if(!strcmp(argv[a], "-v")){
            if((++a)<argc){
                freq_v = strtod(argv[a], &endptr);
                if(*endptr)
                    usage();
            }else{
                usage();
            }
        }else if(!strcmp(argv[a], "-f")){
            colors = 1;
        }else if(!strcmp(argv[a], "--version")){
            version();
        }else{
            if(!strcmp(argv[a], "--"))
                a++;
            break;
        }
    }

    setlocale(LC_ALL, "");
    ansi_escape_parser_reset();

    int lastAnsi = 1;
    int numAnsi = 0;
    int printAnsi = 0;
    int ansi[10] = {};

    if(a == argc){
    	a = 0;
    	argc = 1;
    	argv[0] = "-";
    }

    for(char n=a; n<argc; n++){
        FILE *f = stdin;

        if(!strcmp(argv[n], "--help")){
            for(char *holp=&helpstr[0]; *holp > 0; holp ++)
                printColor(*holp, colors, &i, &l, &s, &cc, &offset, &freq_h, &freq_v);

        }
        else{
            if(strcmp(argv[n], "-"))
                f = fopen(argv[n], "r");
        
            if(!f){
                fprintf(stderr, "Cannot open input file \"%s\": %s\n", argv[n], strerror(errno));
                return 2;
            }

            FILE* fu = fopen("log.log", "w");

            while((c = fgetwc(f)) > 0){
                if(lastAnsi == 3 || c == '\033'){
                    lastAnsi = ansi_escape_parser_feed(c);
                    ansi[numAnsi] = c;
                    numAnsi ++;

                    if(lastAnsi == 2){
                        ansi_escape_parser_reset();

                        printAnsi = 1;
                    }
                    else if(lastAnsi == 1){
                        ansi_sequence* bloblob = ansi_escape_get_last_sequence();

                        if(bloblob->mode == 'H'){
                            if(bloblob->values->size() == 1){
                                l = 0;
                                i = 0;
                            }
                            else{
                                l = bloblob->values->at(0) - 1;
                                i = bloblob->values->at(1) - 1;
                            }
                        }
                        else if (bloblob->mode == 'd'){
                            l = bloblob->values->at(0) - 1;
                        }
                        else if (bloblob->mode == 'G'){
                            i = bloblob->values->at(0) - 1;
                        }
                        else if (bloblob->mode == 'A'){
                            l--;
                        }
                        else if (bloblob->mode == 'C'){
                            i++;
                        }
                        else if (bloblob->mode == 'S'){
                            s += bloblob->values->at(0);
                        }
                        else if (bloblob->mode == 'T'){
                            s -= bloblob->values->at(0);
                        }if(bloblob->mode == 'm'){
                            /*fprintf(fu, "\r\n%c: ", bloblob->mode);
                            for (std::vector<int>::iterator it = bloblob->values->begin() ; it != bloblob->values->end(); ++it)
                                fprintf(fu, "%i ", *it);
                            fflush(fu);*/
                        }
                    }

                    if(lastAnsi < 3){
                    	for(int o = 0; o < numAnsi; o ++)
                    		printf("%lc", ansi[o]);
                    	numAnsi = 0;
                    	fflush(stdout);
                    }
                }
                else if(printAnsi > 0){
                    printf("%lc", c);
                    printAnsi --;
                }
                else{
                    //printf("%u", c);
                    printColor(c, colors, &i, &l, &s, &cc, &offset, &freq_h, &freq_v);
                    fflush(stdout);
                }
            }

            fclose(f);
        }

        printf("\n\033[0m");
        cc = -1;

        if(c != WEOF && c != 0){
            fprintf(stderr, "Error reading input file \"%s\": %s\n", argv[n], strerror(errno));
            return 2;
        }
    }
}
