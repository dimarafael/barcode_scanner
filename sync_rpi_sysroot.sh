#!/bin/bash

# Change to the home directory
cd "$HOME"

# Define remote server and destination paths
REMOTE_USER="admin"
REMOTE_HOST="10.0.25.224"

# Rsync commands to synchronize directories
echo "Syncing /lib..."
rsync -avzS --rsync-path="rsync" --delete "${REMOTE_USER}@${REMOTE_HOST}:/lib/" rpi-sysroot/lib

echo "Syncing /usr/include..."
rsync -avzS --rsync-path="rsync" --delete "${REMOTE_USER}@${REMOTE_HOST}:/usr/include/" rpi-sysroot/usr/include

echo "Syncing /usr/lib..."
rsync -avzS --rsync-path="rsync" --delete "${REMOTE_USER}@${REMOTE_HOST}:/usr/lib/" rpi-sysroot/usr/lib

# Fix symbolic links in the synchronized directory
echo "Fixing symbolic links..."
symlinks -rc rpi-sysroot

echo "Synchronization and link fixing completed successfully!"
