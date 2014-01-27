/*
   Sienar Fleet Systems is a free game of space combat.
   Copyright (C) 2013 carabobz@gmail.com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "lfd.h"

/* Returns the full content of a file or NULL if error.
The returned buffer must be freed by caller */
char * lfd_read(const char * file_name, ssize_t * size)
{
        int fd;
        int ret;
        struct stat stat_buf;
        char * buf;

        ret  = stat(file_name,&stat_buf);
        if( ret == -1 ) {
                return NULL;
        };

        buf = (char*)malloc(stat_buf.st_size);
        if( buf == NULL ) {
                return NULL;
        }

        fd = open(file_name,O_RDONLY);
        if( fd == -1 ) {
                free(buf);
                return NULL;
        }

        *size = read(fd,buf,stat_buf.st_size);
        close(fd);
        if (*size == -1 ) {
                free(buf);
                return NULL;
        }

        return buf;
}

void get_resource_type(char * data,char * buf)
{
	strncpy(buf,data,4);
	buf[4]=0;
	return;
}

void get_resource_name(char * data,char * buf)
{
	strncpy(buf,data+4,8);
	buf[8]=0;
	return;
}

int get_resource_size(char * data)
{
	return (*(int*)(data+12)) + LFD_TAG_HEAD;
}

/* Print data block header */
void print_header(char *data,char * buf)
{
	char tmp[512];
	int size;

	if(buf) buf[0]=0;

	get_resource_type(data,tmp);
	printf("Type: %s - ",tmp); 
	if(buf) strcat(buf,tmp);

	get_resource_name(data,tmp);
	printf("Name: %s - ",tmp); 
	if(buf) strcat(buf,tmp);

	size = get_resource_size(data);
	printf("Size: %d (0x%x)\n",size,size); 

}

/* Write data of a blocks to a file */
int lfd_write(char * data)
{
        int fd;
	ssize_t wsize;
	char buf[512];
	char buf2[512];
	ssize_t size;

	print_header(data,buf);
	sprintf(buf2,"/tmp/%s",buf);

	size = get_resource_size(data);

        fd = open(buf2,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
        if( fd == -1 ) {
                return -1;
        }

        wsize = write(fd,data+LFD_TAG_HEAD,size-LFD_TAG_HEAD);
        close(fd);

        return wsize+LFD_TAG_HEAD;
}

/* Called when reading a data blocks of type RMAP */
void dec_rmap(char * raw_data)
{
	int num_res;
	int tot_size;

	tot_size = get_resource_size(raw_data);
	num_res = ( tot_size / LFD_TAG_HEAD );
	printf("%d resources\n",num_res);

	return ;
}

/* Called when reading a data blocks of type TEXT */
void dec_text(char * raw_data)
{
	char * cur_data;
	int i = 0;
	int size;

	size = get_resource_size(raw_data);
	cur_data = raw_data;
	while( cur_data - raw_data < size) {
		printf("%d: %s\n",i,cur_data);
		i++;
		cur_data+= strlen(cur_data) + 1;
	}

	return;
}

/* Called when reading a data blocks of type CRFT */
void dec_crft(char * data)
{

	uint16_t size;
	uint16_t * cur_data;
	int num_val = 0;

	/* Skip header */
	data += LFD_TAG_HEAD;

	size = *(uint16_t*)data;
	printf("size = %d (0x%x) - ",size,size);
	data+=sizeof(uint16_t);

	cur_data = (uint16_t*)data;
	do {
		num_val++;
		cur_data++;
	} while( *cur_data != 0xffff );

	if( *(cur_data+1) != 0x7fff ) {
		printf("Error\n");
		exit(-1);
	}

	printf("num val = %d\n",num_val);

	return;
}

void lfd_decode(const char * file_name)
{
	char * raw_data;
	int i;
	char * cur_data;
	ssize_t size;
	int num_res = 0;
	ssize_t offset;

	raw_data = lfd_read(file_name,&size);
	if(raw_data == NULL) {
		printf("Cannot read %s\n",file_name);
		exit(-1);
        }

	cur_data=raw_data;
	while( cur_data - raw_data < size ) {
		printf("%d- ",num_res++);
		/* Write all data blocks in separate files*/
		offset = lfd_write(cur_data);

		/* Print information on the data block depending its type */
		for(i=0;i<LFD_TAG_NUM;i++) {
			if(strncmp(cur_data,lfd_tag[i],strlen(lfd_tag[i]))==0) {
				lfd_tag_dec[i](cur_data);
				break;
			}
		}

		cur_data += offset;
	}
}

int main(int argc, char ** argv)
{
	lfd_decode(argv[1]);

	return 0;
}
