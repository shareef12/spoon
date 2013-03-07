#include <stdio.h>
#include "arg_parse.c"

// Succinct overview I referenced to figure out how exif data works. http://stackoverflow.com/questions/7555842/questions-about-exif-in-hexadecimal-form

struct idf_field {
    short field_id;
    short tag_id;
    int count;
    int value;
};


/* The assumption in this function is that we're not trying to grab exif data from excessively large files.
If the image is bigger than ~100mb, the program will throw an error. */

int match(char *buf, int buf_len, char *str, int str_len, int *pos, int *start) {
    int index = 0;
    printf("buflen: %d, strlen: %d",buf_len,str_len);
    while((*pos)<buf_len) {
        printf("%d,%d",*pos,index);
        if(index>=str_len) {
            printf("index>=strlen...quitting");
            return 0;
        } else if(index==0) {
            if(buf[*pos]==str[index]) {
                index++;
            }
        } else {
            if(buf[*pos]!=str[index]) {
                printf("mismatch");
                index=0;
                *start =0;
            } else index++;
            if(index==str_len-1) {
                (*pos)++;
                *start= *pos;
                return 1;
            }
        }
        (*pos)++;
    printf("\n");
    }
}

int grab_little_endian(char buf[],int pos,int len) {
    int i = 0;
    int ret = 0;
    for(i =0; i<=len;i++) {
        ret += buf[pos+i] << (i*4);
    }
        return ret;
    }
    int grab_big_endian(char buf[], int pos,int len) {
    int i = 0;
    int ret = 0;
    for(i = len; i>=0; i--) {
        ret += buf[pos+i] << (len*4 - i*4);    
    }
    return ret;
}

void parse_exif(char buf[], int start, int pos, int big_endian) {
    // the graphic on exif organization was very helpful...didn't look at their code when I saw it was for .net: http://www.codeproject.com/Articles/43665/ExifLibrary-for-NET
    if(!(buf[pos++]==0x00 && buf[pos++]==0x2a)) printf("expected 002A but got something else. exif data may be corrupt."); 
    // I referenced this to see how pointers to functions work: http://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work
    int (*grab_endian)(char[],int,int);
    grab_endian = &grab_little_endian;
    if(big_endian) grab_endian = &grab_big_endian; // use of function pointer makes it so i only have to check this once

    int ifd0_offset = (*grab_endian)(buf, pos,3);
    pos+=4;
    int fields_count = (*grab_endian)(buf,pos,1);
    pos+=2;
    struct idf_field fields[fields_count];
    int i = 0;
    for(i=0;i<fields_count;i++) {
         fields[i].field_id = &buf[start+ifd0_offset+2+i*12];
         fields[i].tag_id = &buf[start+ifd0_offset+2+i*12+2];
// breakdown: start = beginning of exif data. idf0_offset -> self explainatory. 2 skips over the 2 bytes dedicated to field count, and the rest accounts for the fact that each field specifier is 12 bytes long.
         fields[i].value =  (*grab_endian)(buf, start+ifd0_offset+2+i*12+8,3);
         fields[i].count = (*grab_endian)(buf, start+ifd0_offset+2+i*12+4,3);
         printf("%d,%d,\n",fields[i].count,fields[i].value);
         // there should never be negative numbers in fields[i].value. problem is probably with offset calculation.
         // note to future self: thayer weeks are no excuse to procastinate on coding.
         // 2am coding while on cprc is a Bad Idea (TM).
    }

}

void find_exif(FILE *fp,int size) {
    int len;
    int pos=0;
    int start = 0; //tracks the address of the startpoint of the exif intro. necessary to calculate offsets for headers. 
    char exif_intro[] = "\x45\x78\x69\x66\x00\x00";

    if(size==-1) {  // just opened a file and need to find its size.
        fseek(fp,0L,SEEK_END);
        if(ftell(fp)>104857000) {
            perror("The image is too big. try <100mb");
            exit(EXIT_FAILURE); 
        }
        len = ftell(fp);
        fseek(fp,0L,SEEK_SET);
    } else len = size; // reading exif from the main image
    
    char buf[len];
    fread(&buf,1,len,fp);
    printf("before position:%d\n",pos);

//look for the initial exif header
    if(match((char *) &buf,sizeof(buf),(char *) &exif_intro, sizeof(exif_intro), &pos,&start)==1) {
        printf("after position:%d\n",pos);
        if(((pos+1)<sizeof(buf))) {
            if(buf[pos]=='\x49' && buf[pos+1]=='\x49') {
                printf("little endian intel syntax");
                parse_exif((char *)&buf,start,pos+2,0);
            } else if(buf[pos]=='\x4D' && buf[pos+1]=='\x4D') {
                printf("big endian att syntax");
                parse_exif((char *)&buf,start,pos+2,1);
            } else printf("malformed exif data. Assuming Intel syntax.");
        } else printf("incomplete exif data. early eof.\n");
   } else printf("There are no exif headers in this file.");
}



int main(int argc, char* argv[]) {
    FILE *fp;
    parse_args(argc,argv);
    if((fp = fopen(path,"rb"))==NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    find_exif(fp,-1);
}
