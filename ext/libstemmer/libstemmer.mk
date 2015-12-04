
snowball_sources= \
  src_c/stem_ISO_8859_1_danish.c \
  src_c/stem_UTF_8_danish.c \
  src_c/stem_ISO_8859_1_dutch.c \
  src_c/stem_UTF_8_dutch.c \
  src_c/stem_ISO_8859_1_english.c \
  src_c/stem_UTF_8_english.c \
  src_c/stem_ISO_8859_1_finnish.c \
  src_c/stem_UTF_8_finnish.c \
  src_c/stem_ISO_8859_1_french.c \
  src_c/stem_UTF_8_french.c \
  src_c/stem_ISO_8859_1_german.c \
  src_c/stem_UTF_8_german.c \
  src_c/stem_ISO_8859_2_hungarian.c \
  src_c/stem_UTF_8_hungarian.c \
  src_c/stem_ISO_8859_1_italian.c \
  src_c/stem_UTF_8_italian.c \
  src_c/stem_ISO_8859_1_norwegian.c \
  src_c/stem_UTF_8_norwegian.c \
  src_c/stem_ISO_8859_1_porter.c \
  src_c/stem_UTF_8_porter.c \
  src_c/stem_ISO_8859_1_portuguese.c \
  src_c/stem_UTF_8_portuguese.c \
  src_c/stem_ISO_8859_2_romanian.c \
  src_c/stem_UTF_8_romanian.c \
  src_c/stem_KOI8_R_russian.c \
  src_c/stem_UTF_8_russian.c \
  src_c/stem_ISO_8859_1_spanish.c \
  src_c/stem_UTF_8_spanish.c \
  src_c/stem_ISO_8859_1_swedish.c \
  src_c/stem_UTF_8_swedish.c \
  src_c/stem_UTF_8_turkish.c \
  runtime/api.c \
  runtime/utilities.c \
  libstemmer/libstemmer.c

snowball_headers= \
  src_c/stem_ISO_8859_1_danish.h \
  src_c/stem_UTF_8_danish.h \
  src_c/stem_ISO_8859_1_dutch.h \
  src_c/stem_UTF_8_dutch.h \
  src_c/stem_ISO_8859_1_english.h \
  src_c/stem_UTF_8_english.h \
  src_c/stem_ISO_8859_1_finnish.h \
  src_c/stem_UTF_8_finnish.h \
  src_c/stem_ISO_8859_1_french.h \
  src_c/stem_UTF_8_french.h \
  src_c/stem_ISO_8859_1_german.h \
  src_c/stem_UTF_8_german.h \
  src_c/stem_ISO_8859_2_hungarian.h \
  src_c/stem_UTF_8_hungarian.h \
  src_c/stem_ISO_8859_1_italian.h \
  src_c/stem_UTF_8_italian.h \
  src_c/stem_ISO_8859_1_norwegian.h \
  src_c/stem_UTF_8_norwegian.h \
  src_c/stem_ISO_8859_1_porter.h \
  src_c/stem_UTF_8_porter.h \
  src_c/stem_ISO_8859_1_portuguese.h \
  src_c/stem_UTF_8_portuguese.h \
  src_c/stem_ISO_8859_2_romanian.h \
  src_c/stem_UTF_8_romanian.h \
  src_c/stem_KOI8_R_russian.h \
  src_c/stem_UTF_8_russian.h \
  src_c/stem_ISO_8859_1_spanish.h \
  src_c/stem_UTF_8_spanish.h \
  src_c/stem_ISO_8859_1_swedish.h \
  src_c/stem_UTF_8_swedish.h \
  src_c/stem_UTF_8_turkish.h \
  include/libstemmer.h \
  libstemmer/modules.h \
  runtime/api.h \
  runtime/header.h

$(shell mkdir -p $(OBJ)/mldb/ext/libstemmer/runtime/)
$(shell mkdir -p $(OBJ)/mldb/ext/libstemmer/src_c/)
$(shell mkdir -p $(OBJ)/mldb/ext/libstemmer/libstemmer/)

$(eval $(call library,libstemmer,$(snowball_sources),))
