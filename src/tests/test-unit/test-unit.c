/*
 Copyright 2016 IoT.bzh

 author: José Bollo <jose.bollo@iot.bzh>

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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <wgt.h>
#include <wgt-info.h>
#include <wgtpkg-unit.h>

// gcc -o tu utils-dir.c wgt-config.c wgt-info.c wgt-names.c wgt.c wgtpkg-unit.c verbose.c mustach.c $(pkg-config --libs --cflags libxml-2.0) '-DFWK_PREFIX="urn:AGL:"' -g


// gcc -o tu utils-dir.c wgt-config.c wgt-info.c wgt-names.c wgt.c wgtpkg-unit.c verbose.c mustach.c $(pkg-config --libs --cflags libxml-2.0) '-DFWK_PREFIX="urn:AGL:"' -g
int main(int ac, char **av)
{
	struct wgt_info *ifo;

	while(*++av) {
		ifo = wgt_info_createat(AT_FDCWD, *av, 1, 1, 1);
		fwrite_unit(wgt_info_desc(ifo), stdout);
		wgt_info_unref(ifo);
	}
	return 0;
}


