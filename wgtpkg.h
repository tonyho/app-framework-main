/*
 Copyright 2015 IoT.bzh

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/


#include <libxml/tree.h>
#include "config.h"

struct filedesc;

/**************************************************************/
/* from wgtpkg-base64 */

extern char *base64encw(const char *buffer, int length, int width);
extern char *base64enc(const char *buffer, int length);
extern int base64dec(const char *buffer, char **output);
extern int base64eq(const char *buf1, const char *buf2);

/**************************************************************/
/* from wgtpkg-certs */

extern void clear_certificates();
extern int add_certificate_b64(const char *b64);

/**************************************************************/
/* from wgtpkg-digsig */

/* verify the digital signature in file */
extern int verify_digsig(struct filedesc *fdesc);

/* create a digital signature */
extern int create_digsig(int index, const char *key, const char **certs);

/* check the signatures of the current directory */
extern int check_all_signatures();

/**************************************************************/
/* from wgtpkg-files */

enum entrytype {
	type_unset = 0,
	type_file = 1,
	type_directory = 2
};

enum fileflag {
	flag_referenced = 1,
	flag_opened = 2,
	flag_author_signature = 4,
	flag_distributor_signature = 8,
	flag_signature = 12
};

struct filedesc {
	enum entrytype type;
	unsigned int flags;
	unsigned int signum;
	unsigned int zindex;
	char name[1];
};

extern void file_reset();
extern void file_clear_flags();
extern unsigned int file_count();
extern struct filedesc *file_of_index(unsigned int index);
extern struct filedesc *file_of_name(const char *name);
extern struct filedesc *file_add_directory(const char *name);
extern struct filedesc *file_add_file(const char *name);
extern int fill_files();

extern unsigned int signature_count();
extern struct filedesc *signature_of_index(unsigned int index);
extern struct filedesc *create_signature(unsigned int number);
extern struct filedesc *get_signature(unsigned int number);

extern int file_set_prop(struct filedesc *file, const char *name, const char *value);
extern const char *file_get_prop(struct filedesc *file, const char *name);

/**************************************************************/
/* from wgtpkg-verbose */
extern int verbosity;
#define warning(...) do{if(verbosity)syslog(LOG_WARNING,__VA_ARGS__);}while(0)
#define notice(...)  do{if(verbosity)syslog(LOG_NOTICE,__VA_ARGS__);}while(0)
#define info(...)    do{if(verbosity)syslog(LOG_INFO,__VA_ARGS__);}while(0)
#define debug(...)   do{if(verbosity>1)syslog(LOG_DEBUG,__VA_ARGS__);}while(0)
extern int verbose_scan_args(int argc, char **argv);

/**************************************************************/
/* from wgtpkg-workdir */

extern int enter_workdir(int clean);
extern void remove_workdir();
extern int make_workdir(int reuse);
extern int set_workdir(const char *name, int create);

/**************************************************************/
/* from wgtpkg-xmlsec */

extern int xmlsec_init();
extern void xmlsec_shutdown();
extern int xmlsec_verify(xmlNodePtr node);
extern xmlDocPtr xmlsec_create(int index, const char *key, const char **certs);

/**************************************************************/
/* from wgtpkg-zip */

/* read (extract) 'zipfile' in current directory */
extern int zread(const char *zipfile, unsigned long long maxsize);

/* write (pack) content of the current directory in 'zipfile' */
extern int zwrite(const char *zipfile);


