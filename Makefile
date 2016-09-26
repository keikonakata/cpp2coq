ROOT_DIR := $(shell pwd)

COQC        := /home/keiko/opt/coq-8.5pl2/bin/coqc
COQDEP      := /home/keiko/opt/coq-8.5pl2/bin/coqdep
COQ_LOADS := -Q $(ROOT_DIR)/coq/common common -Q $(ROOT_DIR)/coq/examples examples

OCAMLC   := /usr/bin/ocamlc
OCAMLDEP := /usr/bin/ocamldep
OCAML_INCLUDE := -I $(ROOT_DIR)/ocaml/common \
	-I $(ROOT_DIR)/ocaml/extraction/common

COQ_FILES = $(shell find $(ROOT_DIR) -name "*.v")

OCAML_ML_FILES = $(shell find $(ROOT_DIR) -name "*.ml")
OCAML_MLI_FILES = $(shell find $(ROOT_DIR) -name "*.mli")

top:
	echo no target

.PHONY : depend

include $(shell find $(ROOT_DIR) -name "*.mk")

depend : $(COQ_FILES:=.dep) $(OCAML_ML_FILES:=.dep) $(OCAML_MLI_FILES:=.dep)

%.cmo : %.ml
	$(OCAMLC) -c $<

%.vo : %.v
	$(COQC) $(COQ_LOADS) $<

%.v.dep : %.v
	cd $(dir $@);\
	$(COQDEP) $(COQ_LOADS) $< > $@

%.ml.dep : %.ml
	cd $(dir $@);\
	$(OCAMLDEP) $(OCAML_INCLUDE) $< > $@

%.mli.dep : %.mli
	cd $(dir $@);\
	$(OCAMLDEP) $(OCAML_INCLUDE) $< > $@

-include $(COQ_FILES:=.dep)
-include $(OCAML_ML_FILES:=.dep)
-include $(OCAML_MLI_FILES:=.dep)
