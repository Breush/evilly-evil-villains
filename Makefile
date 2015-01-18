# Makefile for internationalization

#===== Edit below

LANGUAGES_SUPPORTED=fr

TRANSLATABLE_FILES=					\
	states/menu/main.cpp			\
	states/menu/selectworld.cpp
	
#===== Do not touch below
	
TRANSLATIONS_FILES=$(LANGUAGES_SUPPORTED:%=res/po/%/LC_MESSAGES/eev.mo)

pot:
	xgettext -Dsrc/ -k_ --c++ --msgid-bugs-address=alexis.breust@gmail.com --package-name=evily-evil-villains --foreign-user -i --no-location -o res/po/eev.pot $(TRANSLATABLE_FILES)

translations: $(TRANSLATIONS_FILES)
	
%.mo: %.po
	msgfmt -o $*.mo $*.po

clean:
	rm -rf $(TRANSLATIONS_FILES)

