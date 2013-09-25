#!/bin/bash
set -e

cd /usr/share/forest/www
php index.php cron summary_email
