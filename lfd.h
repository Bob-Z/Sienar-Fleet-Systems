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

#define LFD_TAG_NUM	3
#define LFD_TAG_HEAD	0x10

const char * lfd_tag[] = {"RMAP","TEXT","CRFT"};

/* return the tag data size */
typedef void (*dec_func)(char *);

void dec_rmap(char *);
void dec_text(char *);
void dec_crft(char *);

dec_func lfd_tag_dec[] = {
	dec_rmap,
	dec_text,
	dec_crft
};
