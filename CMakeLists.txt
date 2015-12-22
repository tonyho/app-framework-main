###########################################################################
# Copyright 2015 IoT.bzh
#
# author: José Bollo <jose.bollo@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

cmake_minimum_required(VERSION 2.8)

project("afm-main" LANGUAGES "C")

include(GNUInstallDirs)

macro(setc name value)
	if(NOT DEFINED ${name})
		set(${name} "${value}")
	endif(NOT DEFINED ${name})
endmacro(setc)

setc(afm_name                "aglfwk")
setc(afm_confdir             "${CMAKE_INSTALL_FULL_SYSCONFDIR}/${afm_name}")
setc(afm_datadir             "${CMAKE_INSTALL_FULL_DATADIR}/${afm_name}")
setc(afm_appdir              "${afm_datadir}/applications")
setc(afm_icondir             "${afm_datadir}/icons")
setc(afm_prefix              "urn:agl:")
setc(afm_prefix_permission   "${afm_prefix}perm:")
setc(afm_prefix_plugin       "${afm_prefix}plugin:")
setc(afm_user_appdir         "app-data")
setc(wgtpkg_trusted_cert_dir "${afm_confdir}/certs")

macro(defstr name value)
	add_definitions(-D${name}="${value}")
endmacro(defstr)

defstr(FWK_CONFIG_DIR          "${afm_confdir}")
defstr(FWK_PREFIX_PERMISSION   "${afm_prefix_permission}")
defstr(FWK_PREFIX_PLUGIN       "${afm_prefix_plugin}")
defstr(FWK_ICON_DIR            "${afm_icondir}")
defstr(FWK_APP_DIR             "${afm_appdir}")
defstr(FWK_USER_APP_DIR        "${afm_user_appdir}")
defstr(WGTPKG_TRUSTED_CERT_DIR "${wgtpkg_trusted_cert_dir}")

add_subdirectory(src)

