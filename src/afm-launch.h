/*
 Copyright 2015 IoT.bzh

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

struct afm_launch_desc {
	const char *path;
	const char *appid;
	const char *content;
	const char *type;
	const char *name;
	const char *home;
	const char **plugins;
	int width;
	int height;
	enum afm_launch_mode mode;
};

int afm_launch_initialize();
int afm_launch(struct afm_launch_desc *desc, pid_t children[2], char **uri);
