/*
   minizip.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/


#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif

#if defined(__APPLE__) || defined(__HAIKU__) || defined(MINIZIP_FOPEN_NO_64)
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

#include "zip.h"

#ifdef _WIN32
        #define USEWIN32IOAPI
        #include "iowin32.h"
#endif



#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

#ifdef _WIN32
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATAA ff32;

      hFind = FindFirstFileA(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}
#else
#if defined(unix) || defined(__APPLE__)
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
  (void)dt;
  int ret=0;
  struct stat s;        /* results of stat() */
  struct tm* filedate;
  time_t tm_t=0;

  if (strcmp(f,"-")!=0)
  {
    char name[MAXFILENAME+1];
    size_t len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f,MAXFILENAME-1);
    /* strncpy doesn't append the trailing NULL, of the string is too long. */
    name[ MAXFILENAME ] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name,&s)==0)
    {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec  = filedate->tm_sec;
  tmzip->tm_min  = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon  = filedate->tm_mon ;
  tmzip->tm_year = filedate->tm_year;

  return ret;
}
#else
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
    (void)f;
    (void)tmzip;
    (void)dt;
    return 0;
}
#endif
#endif




static int check_exist_file(const char* filename) {
    FILE* ftestexist;
    int ret = 1;
    ftestexist = FOPEN_FUNC(filename,"rb");
    if (ftestexist==NULL)
        ret = 0;
    else
        fclose(ftestexist);
    return ret;
}

static void do_banner(void) {
    printf("MiniZip 1.1, demo of zLib + MiniZip64 package, written by Gilles Vollant\n");
    printf("more info on MiniZip at http://www.winimage.com/zLibDll/minizip.html\n\n");
}

static void do_help(void) {
    printf("Usage : minizip [-o] [-a] [-0 to -9] [-p password] [-j] file.zip [files_to_add]\n\n" \
           "  -o  Overwrite existing file.zip\n" \
           "  -a  Append to existing file.zip\n" \
           "  -0  Store only\n" \
           "  -1  Compress faster\n" \
           "  -9  Compress better\n\n" \
           "  -j  exclude path. store only the file name.\n\n");
}

/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
static int getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc) {
   unsigned long calculate_crc=0;
   int err=ZIP_OK;
   FILE * fin = FOPEN_FUNC(filenameinzip,"rb");

   unsigned long size_read = 0;
   /* unsigned long total_read = 0; */
   if (fin==NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_read = fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                printf("error in reading %s\n",filenameinzip);
                err = ZIP_ERRNO;
            }

            if (size_read>0)
                calculate_crc = crc32_z(calculate_crc,buf,size_read);
            /* total_read += size_read; */

        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    *result_crc=calculate_crc;
    printf("file %s crc %lx\n", filenameinzip, calculate_crc);
    return err;
}

static int isLargeFile(const char* filename) {
  int largeFile = 0;
  ZPOS64_T pos = 0;
  FILE* pFile = FOPEN_FUNC(filename, "rb");

  if(pFile != NULL)
  {
    FSEEKO_FUNC(pFile, 0, SEEK_END);
    pos = (ZPOS64_T)FTELLO_FUNC(pFile);

                printf("File : %s is %llu bytes\n", filename, pos);

    if(pos >= 0xffffffff)
     largeFile = 1;

                fclose(pFile);
  }

 return largeFile;
}
