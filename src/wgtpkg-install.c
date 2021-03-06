/*
 Copyright 2015, 2016 IoT.bzh

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

#define _GNU_SOURCE

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "verbose.h"
#include "wgt.h"
#include "wgt-info.h"
#include "wgtpkg-files.h"
#include "wgtpkg-workdir.h"
#include "wgtpkg-zip.h"
#include "wgtpkg-permissions.h"
#include "wgtpkg-digsig.h"
#include "wgtpkg-install.h"
#include "secmgr-wrap.h"
#include "utils-dir.h"

static const char permission_required[] = "required";
static const char permission_optional[] = "optional";
static const char feature_required_permissions[] = FWK_PREFIX "required-permissions";

static int check_defined(const void *data, const char *name)
{
	if (data)
		return 0;
	ERROR("widget has no defined '%s' (temporary constraints)", name);
	errno = EINVAL;
	return -1;
}

static int check_valid_string(const char *value, const char *name)
{
	int pos;
	char c;

	if (check_defined(value, name))
		return -1;
	pos = 0;
	c = value[pos];
	if (c == 0) {
		ERROR("empty string forbidden in '%s' (temporary constraints)", name);
		errno = EINVAL;
		return -1;			
	}
	do {
		if (!isalnum(c) && !strchr(".-_", c)) {
			ERROR("forbidden char %c in '%s' -> '%s' (temporary constraints)", c, name, value);
			errno = EINVAL;
			return -1;			
		}
		c = value[++pos];
	} while(c);
	return 0;
}

static int check_temporary_constraints(const struct wgt_desc *desc)
{
	int result = check_valid_string(desc->id, "id");
	result |= check_valid_string(desc->version, "version");
	result |= check_valid_string(desc->ver, "ver");
	result |= check_defined(desc->icons, "icon");
	result |= check_defined(desc->content_src, "content");
	if (result)
		return result;
	if (desc->icons->next) {
		ERROR("widget has more than one icon defined (temporary constraints)");
		errno = EINVAL;
		result = -1;
	}
	return 0;
}

static int set_required_permissions(struct wgt_desc_param *params, int required)
{
	int optional;

	while (params) {
		/* check the value */
		if (!strcmp(params->value, permission_required))
			optional = !required;
		else if (!strcmp(params->value, permission_optional))
			optional = 1;
		else {
			ERROR("unexpected parameter value: %s found for %s", params->value, params->name);
			errno = EPERM;
			return -1;
		}
		/* set the permission */
		if (request_permission(params->name)) {
			DEBUG("granted permission: %s", params->name);
		} else if (optional) {
			INFO("optional permission ungranted: %s", params->name);
		} else {
			ERROR("ungranted permission required: %s", params->name);
			errno = EPERM;
			return -1;
		}
		params = params->next;
	}
	return 0;
}

static int check_widget(const struct wgt_desc *desc)
{
	int result;
	const struct wgt_desc_feature *feature;

	result = check_temporary_constraints(desc);
	feature = desc->features;
	while(result >= 0 && feature) {
		if (!strcmp(feature->name, feature_required_permissions))
			result = set_required_permissions(feature->params, feature->required);
		feature = feature->next;
	}
	return result;
}

static int move_widget(const char *root, const struct wgt_desc *desc, int force)
{
	char newdir[PATH_MAX];
	int rc;

	rc = snprintf(newdir, sizeof newdir, "%s/%s/%s", root, desc->id, desc->ver);
	if (rc >= (int)sizeof newdir) {
		ERROR("path too long in move_widget");
		errno = EINVAL;
		return -1;
	}

	return move_workdir(newdir, 1, force);
}

static int install_icon(const struct wgt_desc *desc)
{
	char link[PATH_MAX];
	char target[PATH_MAX];
	int rc;

	create_directory(FWK_ICON_DIR, 0755, 1);
	rc = snprintf(link, sizeof link, "%s/%s", FWK_ICON_DIR, desc->idaver);
	if (rc >= (int)sizeof link) {
		ERROR("link too long in install_icon");
		errno = EINVAL;
		return -1;
	}

	rc = snprintf(target, sizeof target, "%s/%s", workdir, desc->icons->src);
	if (rc >= (int)sizeof target) {
		ERROR("target too long in install_icon");
		errno = EINVAL;
		return -1;
	}

	unlink(link);
	rc = symlink(target, link);
	if (rc)
		ERROR("can't create link %s -> %s", link, target);
	return rc;
}

static int install_security(const struct wgt_desc *desc)
{
	char path[PATH_MAX], *head;
	const char *icon, *perm;
	int rc;
	unsigned int i, n, len, lic, lf;
	struct filedesc *f;

	rc = secmgr_init(desc->id);
	if (rc)
		goto error;

	rc = secmgr_path_public_read_only(workdir);
	if (rc)
		goto error2;

	/* instal the files */
	head = stpcpy(path, workdir);
	assert(head < path + sizeof path);
	len = (unsigned)((path + sizeof path) - head);
	if (!len) {
		ERROR("root path too long in install_security");
		errno = ENAMETOOLONG;
		goto error2;
	}
	len--;
	*head++ = '/';
	icon = desc->icons->src;
	lic = (unsigned)strlen(icon);
	n = file_count();
	i = 0;
	while(i < n) {
		f = file_of_index(i++);
		lf = (unsigned)strlen(f->name);
		if (lf >= len) {
			ERROR("path too long in install_security");
			errno = ENAMETOOLONG;
			goto error2;
		}
		strcpy(head, f->name);
		if (lf <= lic && !memcmp(f->name, icon, lf) && (!f->name[lf] || f->name[lf] == '/'))
			rc = secmgr_path_public_read_only(path);
		else
			rc = secmgr_path_read_only(path);
		if (rc)
			goto error2;
	}

	/* install the permissions */
	perm = first_usable_permission();
	while(perm) {
		rc = secmgr_permit(perm);
		if (rc)
			goto error2;
		perm = next_usable_permission();
	}

	rc = secmgr_install();
	return rc;
error2:
	secmgr_cancel();
error:
	return -1;
}

/* install the widget of the file */
struct wgt_info *install_widget(const char *wgtfile, const char *root, int force)
{
	struct wgt_info *ifo;
	const struct wgt_desc *desc;

	NOTICE("-- INSTALLING widget %s to %s --", wgtfile, root);

	/* workdir */
	create_directory(root, 0755, 1);
	if (make_workdir(root, "TMP", 0)) {
		ERROR("failed to create a working directory");
		goto error1;
	}

	if (zread(wgtfile, 0))
		goto error2;

	if (check_all_signatures())
		goto error2;

	ifo = wgt_info_createat(workdirfd, NULL, 1, 1, 1);
	if (!ifo)
		goto error2;

	reset_requested_permissions();
	desc = wgt_info_desc(ifo);
	if (check_widget(desc))
		goto error3;

	if (move_widget(root, desc, force))
		goto error3;

	if (install_icon(desc))
		goto error3;

	if (install_security(desc))
		goto error3;

	file_reset();
	return ifo;

error3:
	wgt_info_unref(ifo);

error2:
	remove_workdir();

error1:
	file_reset();
	return NULL;
}

