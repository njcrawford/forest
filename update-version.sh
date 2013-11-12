#!/bin/bash
set -e
set -u

# Run this script after changing version.txt to propigate the version
# to all the proper places.

version=`cat version.txt`
uuid=`uuidgen`

c_filename="client/version.h"
php_filename="server/www/application/config/version.php"
cs_filename="client/Windows/ConfigurationGUI/Properties/version.cs"
wix_filename="client/Windows/ForestInstaller/Version.wxi"
rc1_filename="client/Windows/version1.rc"
rc2_filename="client/Windows/version2.rc"

message_1="This file is automatically generated by update-version script."
message_2="Any changes will be overwritten next time the script is run."

# C version
echo "// ${message_1} " > ${c_filename}
echo "// ${message_2} " >> ${c_filename}
echo "#define FOREST_VERSION \"${version}\" " >> ${c_filename}

# C Resource version 1
special_version=`echo "${version}" | sed 's/\./,/g'`
echo "// ${message_1} " > ${rc1_filename}
echo "// ${message_2} " >> ${rc1_filename}
echo "FILEVERSION ${special_version},0 " >> ${rc1_filename}
echo "PRODUCTVERSION ${special_version},0 " >> ${rc1_filename}

# C Resource version 2
echo "// ${message_1} " > ${rc2_filename}
echo "// ${message_2} " >> ${rc2_filename}
echo "VALUE \"FileVersion\", \"${version}.0\" " >> ${rc2_filename}
echo "VALUE \"ProductVersion\", \"${version}.0\" " >> ${rc2_filename}

# PHP version
echo "<?php" > ${php_filename}
echo "// ${message_1}" >> ${php_filename}
echo "// ${message_2}" >> ${php_filename}
echo "define('FOREST_VERSION', '${version}'); " >> ${php_filename}

# C# version
echo "// ${message_1}" > ${cs_filename}
echo "// ${message_2}" >> ${cs_filename}
echo "using System.Reflection; " >> ${cs_filename}
echo "[assembly: AssemblyVersion(\"${version}.0\")] " >> ${cs_filename}
echo "[assembly: AssemblyFileVersion(\"${version}.0\")] " >> ${cs_filename}

# Wix version
echo "<?xml version=\"1.0\" encoding=\"utf-8\"?> " > ${wix_filename}
echo "<!-- ${message_1} --> " >> ${wix_filename}
echo "<!-- ${message_2} --> " >> ${wix_filename}
echo "<Include> " >> ${wix_filename}
echo "  <?define ProductVersion = \"${version}\"?> " >> ${wix_filename}
echo "  <?define ProductGuid = \"${uuid}\"?> " >> ${wix_filename}
echo "</Include> " >> ${wix_filename}


