#include <stdio.h>
#include "arg_parse.c"

// Succinct overview I referenced to figure out how exif data works. http://stackoverflow.com/questions/7555842/questions-about-exif-in-hexadecimal-form

struct ifd_field {
   short tag_id;
   short type;
   unsigned short int count;
   unsigned short int value;
   char * data;
};
struct ifd {
   short count;
   struct ifd_field * fields;
   unsigned long int offset;
   struct ifd *exif;
   struct ifd *gps;
   //char * data;
};

// http://exif35.pcfire.net/docs.php?doc=fields
// http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/EXIF.html
int read_ifd_id(int value, int collision) {
   //this function is where all the magic happens
   /*
   collisions:
   0 -> 0th ifd
   1 -> exif ifd
   2 -> gps ifd
   */
switch(value % 0xffff0000) { // % is a random hack because there's a ghost in my program and computers are hard. :(
      case 0x0000:
         printf("GPS Version ID:");
         break;
      case 0x0001:
         printf("GPS Latitude Ref:");
         break;
      case 0x0002:
         printf("GPS Latitude:");
         break;
      case 0x0003:
         printf("GPS Longitude Ref:");
         break;
      case 0x0004:
         printf("GPS Longitude:");
         break;
      case 0x0005:
         printf("GPS Altitude Ref:");
         break;
      case 0x0006:
         printf("GPS Altitude:");
         break;
      case 0x0007:
         printf("GPS Time Stamp:");
         break;
      case 0x0008:
         printf("GPS Satellites:");
         break;
      case 0x0009:
         printf("GPS Status:");
         break;
      case 0x000a:
         printf("GPS Measure Mode:");
         break;
      case 0x000b:
         printf("GPS DOP:");
         break;
      case 0x000c:
         printf("GPS Speed Ref:");
         break;
      case 0x000d:
         printf("GPS Speed:");
         break;
      case 0x000e:
         printf("GPS Track Ref:");
         break;
      case 0x000f:
         printf("GPS Track:");
         break;
      case 0x0010:
         printf("GPS Img Direction Ref:");
         break;
      case 0x0011:
         printf("GPS Img Direction:");
         break;
      case 0x0012:
         printf("GPS Map Datum:");
         break;
      case 0x0013:
         printf("GPS Dest Latitude Ref:");
         break;
      case 0x0014:
         printf("GPS Dest Latitude:");
         break;
      case 0x0015:
         printf("GPS Dest Longitude Ref:");
         break;
      case 0x0016:
         printf("GPS Dest Longitude:");
         break;
      case 0x0017:
         printf("GPS Dest Bearing Ref:");
         break;
      case 0x0018:
         printf("GPS Dest Bearing:");
         break;
      case 0x0019:
         printf("GPS Dest Distance Ref:");
         break;
      case 0x001a:
         printf("GPS Dest Distance:");
         break;
      case 0x001b:
         printf("GPS Processing Method:");
         break;
      case 0x001c:
         printf("GPS Area Information:");
         break;
      case 0x001d:
         printf("GPS Date Stamp:");
         break;
      case 0x001e:
         printf("GPS Differential:");
         break;
      case 0x001f:
         printf("GPSH Positioning Error:");
         break;
      case 0x0100:
         printf("Image Width:");
         break;
      case 0x0101:
         printf("Image Height:");
         break;
      case 0x0214:
         printf("Reference Black White:");
         break;
      case 0x9286:
         printf("User Comment:");
         break;
      case 0xa402:
         printf("Exposure Mode:");
         break;
      case 0xa403:
         printf("White Balance:");
         break;
      case 0x011a:
         printf("X Resolution:");
         break;
      case 0x011b:
         printf("Y Resolution:");
         break;
      case 0x0128:
         printf("Resolution Unit (1 = none, 2 = in, 3 = cm):");
         break;
      case 0x10f:
         printf("Camera Make:");
         break;
      case 0x110:
         printf("Camera Model:");
         break;
      case 0x131:
         printf("Software:");
         break;
      case 0x0213:
         printf("Y Cb Cr Positioning (1 = centered, 2 = co-sited):");
         break;
      case 0xa215:
         printf("Exposure Index:");
         break;
      case 0x13b:
         printf("Author:");
         break;
      case 0x132:
      case 0x9003:
         printf("Date Time Taken:");
         break;
      case 0xa406:
         printf("Scene Capture Type:");
         break;
      case 0x9207:
         printf("Metering Mode:");
         break;
      case 0x9206:
         printf("Subject distance:");
         break;
      case 0x9208:
         printf("Light Source:");
         break;
      case 0xa40c:
         printf("Subject Distance Range:");
         break;
      case 0x0112:
         printf("Orientation:");
         break;
      case 0x8298:
         printf("Copyright:");
         break;
      case 0x8769:
         printf("Exif Offset (program will explore new idf):");
         break;
      case 0x8822:
         printf("Exposure Program (0=undefined, 1=manual, 2=program ae, 3=aperture-priority ae, 4=shutter speed priority ae, 5=creative (slow), 6=action (fast), 7=portrait, 8=landscape, 9=bulb):");
         break;
      case 0x9209:
         printf("Flash:");
         break;
      case 0x9204:
         if(collision ==0) {
            printf("Exposure Bias:");
         } else if(collision ==1) {
            printf("Exposure Compensation:");
         }
         break;
      case 0x9290:
         printf("Sub Sec Time:");
         break;
      case 0x9291:
         printf("Sub Sec Time Original:");
         break;
      case 0x9292:
         printf("Sub Sec Time Digitized:");
         break;
      case 0x920a:
      case 0xa405:
         printf("Focal Length:");
         break;
      case 0x829a:
         printf("Exposure Time:");
         break;
      case 0x320:
      case 0x10d:
         printf("Title:");
         break;
      case 0x10e:
         printf("Description:");
         break;
      case 0x9205:
         if(collision ==0) {
            printf("Lens Maximum Aperture:");
         } else if(collision ==1) {
            printf("Max Aperture Value:");
         }
         break;
      case 0x8825:
         printf("GPS Data Detected, following offset:");
         break;
      case 0x829d:
         printf("FNumber:");
         break;
      case 0x8827:
         printf("ISO:");
         break;
      case 0x9000:
         printf("Exif Version:");
         break;
      case 0x9004:
         printf("Create Date:");
         break;
      case 0x9101:
         printf("Components Configuration:");
         break;
      case 0x9102:
         printf("Compressed Bits Per Pixel:");
         break;
      case 0x9201:
         printf("Shutter Speed Value:");
         break;
      case 0x9202:
         printf("Aperture Value:");
         break;
      case 0x9203:
         printf("Brightness Value:");
         break;
      case 0x927c:
         printf("Maker Notes (offset to new idf):");
         break;
      case 0xa000:
         printf("Flashpix Version:");
         break;
      case 0xa001:
         printf("Color Space (0x1=sRGB, 0x2=adobe RGB, 0xfffd = wide gamut RGB, 0xfffe= ICC profile):");
         break;
      case 0xa002:
         printf("Exif Image Width:");
         break;
      case 0xa003:
         printf("Exif Image Height:");
         break;
      case 0xa005:
         printf("Interop Offset:");
         break;
      case 0xa20e:
         printf("Focal Plane X Resolution:");
         break;
      case 0xa20f:
         printf("Focal Plane Y Resolution:");
         break;
      case 0xa210:
         printf("Focal Plane Resolution Unit (1=none, 2=in, 3=cm, 4=mm, 5=um):");
         break;
      case 0xa217:
         printf("Sensing Method (1=undefined, 2 = one-chip, 3 = two-chip, 4 = three-chip, 5 = color sequential area, 7 = trilinear, 8 = color sequential linear):");
         break;
      case 0xa300:
         printf("File Source (1 = film scanner, 2 = reflection print scanner, 3 = digital camera, x3x0x0x0 = Sigma digital Camera):");
         break;
      case 0xa301:
         printf("Scene Type (1 = directly photographed):");
         break;
      case 0xa004:
         printf("Related Dound File:");
         break;
      case 0xa302:
         printf("CFA Pattern:");
         break;
      case 0xa401:
         printf("Custom Rendered:");
         break;
      case 0xa407:
         printf("Gain Control:");
         break;
      case 0xa408:
         printf("Contrast:");
         break;
      case 0xa404:
         printf("Digital Zoom Ratio:");
         break;
      case 0xa409:
         printf("Saturation:");
         break;
      case 0xa40a:
         printf("Sharpness:");
         break;
      case 0xa420:
         printf("Image Unique ID:");
         break;
      case 0xa430:
         printf("Owner Name:");
         break;
      default:
         printf("%04x is an unknown exif id. -->",value);
         return 0;
   }
   printf(" "); 
   return 1;
}
/* The assumption in this function is that we're not trying to grab exif data from excessively large files.
If the image is bigger than ~100mb, the program will throw an error. */

int match(char *buf, int buf_len, char *str, int str_len, int *pos, int *start) {
   int index = 0;
   while((*pos)<buf_len) {
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
   }
}

void read_bytes(char buf[],char * out, int pos, int num_bytes, int endian) {
   int i;
   int j = 0; // index of new output pointer
   if(endian) {
      for(i=num_bytes; i>=0; i--) {
         memcpy((out+j),&buf[pos+i],1);
         j++;
      }
   } else {
      memcpy(out, &buf[pos],num_bytes+1);
   }
}

struct ifd * read_ifd(char buf[], int pos, int start, int endian, int mode) {
   //mode tracks whether we are in root idf or exif idf or gps idf. tag IDs conflict so this is necessary.
   struct ifd * cur_p = malloc(sizeof(struct ifd));
   struct ifd cur = *cur_p;
   int i;
   char * data;
   read_bytes(buf, (char * ) &cur.count,pos, 1, endian);
   pos+=2;
   printf("->New IFD Fields Count:%08x\n",cur.count);
   cur.fields = (struct ifd_field *) malloc(sizeof(struct ifd_field)*cur.count);
   for(i=0;i<cur.count;i++) {
         read_bytes(buf, (char *) &cur.fields[i].tag_id, start+2+i*12,1,endian);
         read_bytes(buf, (char *) &cur.fields[i].type, start+2+i*12+2,1,endian);
         read_bytes(buf, (char *) &cur.fields[i].count, start+2+i*12+4,3,endian);
         read_bytes(buf, (char *) &cur.fields[i].value, start+2+i*12+8,3,endian);
         if(!read_ifd_id(cur.fields[i].tag_id,mode)) {
            printf("fields count:%d,fields type:%08x,fields id:%08x,fields value:%08x\n",cur.fields[i].count,cur.fields[i].type,cur.fields[i].tag_id,cur.fields[i].value);
         }
         if(cur.fields[i].type==2) {
            if(cur.fields[i].count >4) {
               data = malloc(cur.fields[i].count);
               memcpy(data, &buf[start+cur.fields[i].value-8],cur.fields[i].count);//offset is not consistent throughout exif data. -.-
               printf("%s\n",data);
               cur.fields[i].data = data; 
            } else {
               printf("field %d reads a value of: %08x\n",i,cur.fields[i].value);
            }
         } else if(cur.fields[i].type==3) {
            printf("%u\n", (unsigned short)cur.fields[i].value);
         } else if(cur.fields[i].type==4) {
            printf("%lu\n",(unsigned long int) cur.fields[i].value); 
         } else if(cur.fields[i].type==5 || cur.fields[i].type==7 || cur.fields[i].type==10) {
            if(cur.fields[i].count >4) {
               data = malloc(cur.fields[i].count);
               memcpy(data, &buf[start+cur.fields[i].value-8],cur.fields[i].count);
               printf("%08x\n",(unsigned int) data);
               cur.fields[i].data = data;
            } else {
               if(cur.fields[i].type==10) {
                  printf("%i\n",cur.fields[i].value);
               } else {
                  printf("%lu\n",(unsigned long int) cur.fields[i].value);
               }
            }
         } else if(cur.fields[i].type==9) {
           printf("%i\n",cur.fields[i].value);
         } else {
           printf("unknown type (%i) data: %08x\n",cur.fields[i].type,cur.fields[i].value);  
         }
         if((cur.fields[i].tag_id % 0xffff0000) == 0x8769) {
            cur.exif = read_ifd(buf, start+cur.fields[i].value-8, start+cur.fields[i].value-8,endian,1);            
         } else if((cur.fields[i].tag_id % 0xffff0000) == 0x8825) {
            cur.gps = read_ifd(buf, start+cur.fields[i].value-8, start+cur.fields[i].value-8,endian,2);            
         }

     }
   pos += cur.count*12;
   read_bytes(buf, (char * ) &cur.offset, pos, 3,endian);
   pos += 4;
   printf("IFD offset: %08x\n", (unsigned int) cur.offset);
   return cur_p;
}

void parse_exif(char buf[], int start, int pos, int endian) {
    // the graphic on exif organization was very helpful: http://www.codeproject.com/Articles/43665/ExifLibrary-for-NET
    if(!((buf[pos]==0x00 && buf[pos+1]==0x2a && endian) || (buf[pos]==0x2a && buf[pos+1]==0x00 && !endian))) printf("expected 002A but got something else. exif data may be corrupt."); 
    pos +=2;
    // I referenced this to see how pointers to functions work: http://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work
    unsigned long int ifd0_offset = 0;
    read_bytes(buf, (char *) &ifd0_offset, pos,3,endian); 
    printf("\nifd0_offset: %lu\n",ifd0_offset);
    pos+=4;
    struct ifd * ifd0 = read_ifd(buf, pos, (ifd0_offset+start),endian,0);
    if(ifd0->offset != 0) {
      struct ifd * ifd1 = read_ifd(buf, pos, (ifd0_offset+start+ifd0->offset),endian,0);
    } 
}

void find_preamble(FILE *fp,int size) {
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

//look for the initial exif header
   if(match((char *) &buf,sizeof(buf),(char *) &exif_intro, sizeof(exif_intro), &pos,&start)==1) {
      printf("Exif preamble identified at:%d\n",pos);
      //now determine endianness
      if(((pos+1)<sizeof(buf))) {
         if(buf[pos]=='\x49' && buf[pos+1]=='\x49') {
            printf("little endian intel syntax");
            parse_exif((char *)&buf,pos,pos+2,0);
         } else if(buf[pos]=='\x4D' && buf[pos+1]=='\x4D') {
            printf("big endian att syntax");
            parse_exif((char *)&buf,pos,pos+2,1);
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
   find_preamble(fp,-1);
}
