#!/bin/bash

# Variables (modify these as needed)
DB_VERSION="15"                # PostgreSQL version (change if needed)
DB_NAME="catbio_db"          # Database name
DB_USER="catbio"              # Database user
DB_PASSWORD="C@tbio227"      # Database password

# Update package index and install required dependencies
echo "Updating package lists and installing dependencies..."
apt-get update
apt-get install -y wget gnupg2 lsb-release ca-certificates

# Add PostgreSQL APT repository and install PostgreSQL
echo "Adding PostgreSQL APT repository and installing PostgreSQL..."
wget -q https://www.postgresql.org/media/keys/ACCC4CF8.asc -O - | apt-key add -
sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt/ $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
apt-get update
apt-get install -y postgresql-$DB_VERSION postgresql-contrib-$DB_VERSION

# Start PostgreSQL service
echo "Starting PostgreSQL service..."
systemctl start postgresql
systemctl enable postgresql

# Create database and user
echo "Creating PostgreSQL database and user..."
sudo -u postgres psql <<-EOSQL
  CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';
  CREATE DATABASE $DB_NAME OWNER $DB_USER;
  ALTER USER $DB_USER WITH SUPERUSER;
EOSQL

# Adjust PostgreSQL settings for remote connections (optional)
echo "Configuring PostgreSQL to allow remote connections..."
sed -i "s/#listen_addresses = 'localhost'/listen_addresses = '*'/g" /etc/postgresql/$DB_VERSION/main/postgresql.conf
echo "host all all 0.0.0.0/0 md5" >> /etc/postgresql/$DB_VERSION/main/pg_hba.conf

# Restart PostgreSQL to apply changes
echo "Restarting PostgreSQL service to apply changes..."
systemctl restart postgresql

echo "PostgreSQL installation and configuration complete!"
