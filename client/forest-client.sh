#!/bin/bash

###############################################################################
# Forest - a web-based multi-system update manager
#
# Copyright (C) 2010 Nathan Crawford
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# A copy of the full GPL 2 license can be found in the docs directory.
# You can contact me at http://www.njcrawford.com/contact
###############################################################################

# set environment stuff
# PATH should contain at least /usr/local/sbin, /usr/sbin and /sbin
PATH=$PATH:/usr/local/sbin:/usr/sbin:/sbin
# some parts of apt-get seem to need term set to something
#TERM=dumb

# global variables
rpc_version="2"
current_distro=""
# detect which distro we're on so we know what commands to run
if [ -f /etc/lsb-release ] ; then
	current_distro="ubuntu"
elif [ -f /etc/redhat-release ] ; then
	current_distro="centos"
fi

# This function does not output anything if there are no packages to update or
# if the distribution cannot be detected.
get_available_updates ()
{
	if [ "x$current_distro" = "xubuntu" ] ; then
		#for ubuntu
		updates=`apt-get dist-upgrade -Vs 2>&1`
		updates_return=$?
		if [ "$updates_return" -eq "0" ] ; then
			updates=`echo "$updates" | grep ^Inst | cut -d " " -f 2`
		else
			updates="error: $updates"
		fi
		echo $updates
	elif [ "x$current_distro" = "xcentos" ] ; then
		#for centos
		updates=`yum check-update -q -C 2>&1`
		updates_return=$?
		if [ "$updates_return" -eq "0" ] ; then
			updates=""
		elif [ "$updates_return" -eq "100" ] ; then
			updates=`echo "$updates" | grep -v " \* \|^$" | cut -d " " -f 1 | sed 's/\(.*\)\..*/\1/' | uniq`
		else
			updates="error: $updates"
		fi
		echo $updates
	fi
}

# Replaces spaces with commas
spaces_to_commas ()
{
	echo $1 | sed 's/ \+/,/g'
}

# Encodes special HTML characters
encode_for_html ()
{
	echo $1 | sed 's/+/%2b/g'
}

# Returns true if a reboot is needed, false otherwise
is_reboot_needed()
{
	if [ "x$current_distro" = "xubuntu" ]; then
		if [ -f /var/run/reboot-required ]; then
			echo "true"
		else
			echo "false"
		fi
	elif [ "x$current_distro" = "xcentos" ]; then
		# I assume that a kernel is the only thing that would require a
		# reboot. There could be other things that need it as well.
		running_kernel=`uname -a | cut -d " " -f 3`
		newest_kernel=`yum -q list kernel | grep "installed" | awk -F" " '{ print $2 }' | sort -n -s | sed '$!d'`
		if [ "x$running_kernel" != "x$newest_kernel"  ]; then
			echo "true"
		else
			echo "false"
		fi
	else
		echo "unknown"
	fi
}

# Invokes package manager to do updates on selected packages.
# Takes a space separated list of packages.
do_updates()
{
	if [ "x$current_distro" = "xubuntu" ]; then
		results=`apt-get -y -o DPkg::Options::\=--force-confold install $1 2>&1`
		if [ $? -ne 0 ]; then
			echo "apt-get install did not execute successfully"
			echo "$results"
			exit 1
		fi
	elif [ "x$current_distro" = "xcentos" ]; then
		results=`yum -y update $1 2>&1`
		if [ $? -ne 0 ]; then
			echo "yum update did not execute successfully"
			echo "$results"
			exit 1
		fi
	fi
}

# Trims spaces from beginning and end
trim_spaces()
{
	echo $1 | sed 's/^ *\(.*\) *$/\1/'
}

# default server url in case config file is missing
server_url="http://forest/forest"
# include config file
. /etc/forest-client.conf

# build urls to use for posting and getting data
post_url="$server_url/collect.php"
accepted_url="$server_url/getaccepted.php?rpc_version=$rpc_version&system=`hostname`"

# check for accepted updates and apply them
accepted_updates=`curl --silent --show-error $accepted_url`
# quit here silently if curl can't connect
if [ $? -ne 0 ]; then
	exit 0
fi

error_check=`echo $accepted_updates | cut -d ":" -f 1`
# quit here if there was an error getting accepted updates
if [ "x$error_check" != "xdata_ok" ]; then
	echo "error getting accepted packages: $accepted_updates"
	error_check=`echo $accepted_updates | cut -d ":" -f 2`
	error_check=`trim_spaces "$error_check"`
	if [ "x$error_check" = "xSystem not found in database" ]; then
		accepted_updates="data_ok: reboot-false: "
	else
		exit 1
	fi
fi

reboot_check=`echo $accepted_updates | cut -d ":" -f 2`
reboot_check=`trim_spaces $reboot_check`
do_reboot="false"
# quit here if there was an error getting accepted reboot
if [ "x$reboot_check" = "xreboot-true" ]; then
	do_reboot="true"
	echo "shutdown -r now" | at now + 10 minutes 2>&1 | grep -v "warning: commands will be executed using\|job [0-9]* at"
elif [ "x$reboot_check" != "xreboot-false" ]; then
	echo "invalid reboot request value: $reboot_check"
	exit 1
fi

# filter out error status and trim spaces from remainder
accepted_updates=`echo "$accepted_updates" | cut -d " " -f 3-`
accepted_updates=`trim_spaces "$accepted_updates"`

# only try to apply updates if the list isn't empty
if [ "x$accepted_updates" != "x" ]; then
	do_updates "$accepted_updates"
fi

# collect packages available for update and report them
formatted_system_name="system_name=`hostname`"
updates=`get_available_updates`
if [ "x$updates" != "x" ]; then
	error_check=`echo $updates | cut -d ":" -f 1`
	if [ "x$error_check" = "xerror" ]; then
		echo "Error getting available updates"
		echo "$updates"
		exit 1
	else
		updates=`spaces_to_commas "$updates"`
		updates=`encode_for_html "$updates"`
		formatted_updates="available_updates=$updates"
	fi
else
	formatted_updates="no_updates_available=true"
fi

formatted_reboot="reboot_required=`is_reboot_needed`"
formatted_reboot_attempted="reboot_attempted=$do_reboot"
formatted_rpc_version="rpc_version=$rpc_version"

curldata=`curl --silent --show-error --data "$formatted_rpc_version" --data "$formatted_system_name" --data "$formatted_updates" --data "$formatted_reboot" --data "$formatted_reboot_attempted" $post_url`

# filter out success messege and return value of grep to avoid cron warning messages
if [ "x$1" = "x--cron" ]; then
	curldata=`echo "$curldata" | grep -v "data_ok"`
fi

# output anything from curl that hasn't been filtered
if [ "x" != "x$curldata" ]; then
	echo "$curldata"
fi
