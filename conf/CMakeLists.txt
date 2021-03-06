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

configure_file(afm-launch.conf.in afm-launch.conf)

if(NOT USE_SDK)
	set(SYSCONFDIR_DBUS_SYSTEM ${CMAKE_INSTALL_SYSCONFDIR}/dbus-1/system.d CACHE STRING "Path to dbus system configuration files")
	set(SYSCONFDIR_DBUS_USER   ${CMAKE_INSTALL_SYSCONFDIR}/dbus-1/session.d CACHE STRING "Path to dbus session configuration files")
	set(UNITDIR_SYSTEM         ${CMAKE_INSTALL_LIBDIR}/systemd/system CACHE STRING "Path to systemd system unit files")
	set(UNITDIR_USER           ${CMAKE_INSTALL_LIBDIR}/systemd/user CACHE STRING "Path to systemd system unit files")

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/afm-system-daemon.conf    DESTINATION ${SYSCONFDIR_DBUS_SYSTEM})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/afm-system-daemon.service DESTINATION ${UNITDIR_SYSTEM})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/afm-user-daemon.conf      DESTINATION ${SYSCONFDIR_DBUS_USER})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/afm-user-daemon.service   DESTINATION ${UNITDIR_USER})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/afm-launch.conf	    DESTINATION ${afm_confdir})
endif()

