#!/bin/bash

set -e
set -u

version=`cat version.txt`

c_comment="//"
php_comment="//"
cs_comment="//"
wix_comment="<!--"

c_filename="client/version.h"
php_filename="server/www/application/config/version.php"
cs_filename="client/Windows/ConfigurationGUI/Properties/version.cs"
wix_filename="client/Windows/ForestInstaller/ForestInstaller/Version.wxi"

message_1="This file is automatically generated by update-version.sh."
message_2="Any updates will be overwritten."

# C version
echo "${c_comment} ${message_1}" > ${c_filename}
echo "${c_comment} ${message_2}" >> ${c_filename}
echo "#ifndef FOREST_VERSION" >> ${c_filename}
echo "#define FOREST_VERSION ${version}" >> ${c_filename}
echo "#endif" >> ${c_filename}

# PHP version
echo "<?php" > ${php_filename}
echo "${php_comment} ${message_1}" >> ${php_filename}
echo "${php_comment} ${message_2}" >> ${php_filename}
echo "\$config['forest_version'] = \"${version}\";" >> ${php_filename}

# C# version
echo "${cs_comment} ${message_1}" > ${cs_filename}
echo "${cs_comment} ${message_2}" >> ${cs_filename}
echo "[assembly: AssemblyVersion(\"${version}.0\")]" >> ${cs_filename}
echo "[assembly: AssemblyFileVersion(\"${version}.0\")]" >> ${cs_filename}

# Wix version
echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>" > ${wix_filename}
echo "${wix_comment} ${message_1} -->" >> ${wix_filename}
echo "${wix_comment} ${message_2} -->" >> ${wix_filename}
echo "<Include>" >> ${wix_filename}
echo "  <?define ProductVersion = \"${version}\"?>" >> ${wix_filename}
echo "</Include>" >> ${wix_filename}
