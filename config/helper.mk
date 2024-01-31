COMMA :=,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

resep = $(subst $(SPACE),$(1),$(2))
remove = $(shell echo '$(subst ','\'',$(2))' | sed 's/$(call resep,\|,$(1))//g')
escape = $(subst |,\|,$(subst $$,\$$,$(subst &,\&,$(subst *,\*,$(subst ?,\?,$(subst #,\#,$(subst {,\{,$(subst },\},$(subst ",\",$(subst ',\',$(subst [,\[,$(subst ],\],$(subst ;,\;,$(subst <,\<,$(subst >,\>,$(subst `,\`,$(subst $(SPACE),\$(SPACE),${subst (,\(,${subst ),\),$(subst \,\\,$(1))}})))))))))))))))))

positive_values = ON on On 1 YES yes Yes Y y
negative_values = OFF off Off 0 NO no No N n

is_2_newer_than_1 = $(shell latest_mtime=9999999999; \
this_mtime=0; \
tmp_mtime=$$(stat -c %Y $(2) 2>/dev/null) && \
latest_mtime=$$(echo "$$tmp_mtime" | sort -n | tail -n 1); \
tmp_mtime=$$(stat -c %Y $(1) 2>/dev/null) && \
this_mtime=$$(echo "$$tmp_mtime" | sort -n | tail -n 1); \
if [ "$$latest_mtime" -gt "$$this_mtime" ]; then echo 1; else echo 0; fi; \
)

define start_build_abs_target
echo "CHK         $(this_dir)"; \
latest_mtime=9999999999; \
this_mtime=0; \
files=$$(printf \
"$$(find -L $(this_dir) -maxdepth 2 -type f -regextype posix-extended -regex '.*\.h$$|.*\.c$$|.*\.so$$|.*\.a$$|.*\.o$$|.*\.h$$|.*\.idl$$')\
\n$$(find $(this_dir) -mindepth 2 -maxdepth 2 -name tmp.make)") && \
tmp_mtime=$$(stat -c %Y $${files} 2>/dev/null) && \
latest_mtime=$$(echo "$$tmp_mtime" | sort -n | tail -n 1); \
tmp_mtime=$$(stat -c %Y $@ 2>/dev/null) && \
this_mtime=$$(echo "$$tmp_mtime" | sort -n | tail -n 1); \
if [ "$$latest_mtime" -gt "$$this_mtime" ]; then
endef

define end_build_abs_target
fi;
endef
