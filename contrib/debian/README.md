
Debian
====================
This directory contains files used to package deftchaind/deftchain-qt
for Debian-based Linux systems. If you compile deftchaind/deftchain-qt yourself, there are some useful files here.

## deftchain: URI support ##


deftchain-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install deftchain-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your deftchain-qt binary to `/usr/bin`
and the `../../share/pixmaps/deftchain128.png` to `/usr/share/pixmaps`

deftchain-qt.protocol (KDE)

