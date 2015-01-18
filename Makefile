# Makefile for internationalization

TRANSLATABLE_FILES=					\
	states/menu/main.cpp			\
	states/menu/selectworld.cpp

pot:
	xgettext -Dsrc/ -k_ --c++ --msgid-bugs-address=alexis.breust@gmail.com --package-name=evily-evil-villains --foreign-user -i --no-location -o res/po/eev.pot ${TRANSLATABLE_FILES}

