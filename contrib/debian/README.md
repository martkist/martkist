
Debian
====================
This directory contains files used to package martkistd/martkist-qt
for Debian-based Linux systems. If you compile martkistd/martkist-qt yourself, there are some useful files here.

## martkist: URI support ##


martkist-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install martkist-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your martkist-qt binary to `/usr/bin`
and the `../../share/pixmaps/martkist128.png` to `/usr/share/pixmaps`

martkist-qt.protocol (KDE)

