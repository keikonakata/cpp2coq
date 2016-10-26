ROOT_DIR := $(shell pwd)

COQC        := /home/keiko/opt/coq-8.5pl2/bin/coqc
COQDEP      := /home/keiko/opt/coq-8.5pl2/bin/coqdep
COQ_LOADS := -Q $(ROOT_DIR)/coq/common common -Q $(ROOT_DIR)/coq/examples examples

OCAMLC   := /usr/bin/ocamlc
OCAMLDEP := /usr/bin/ocamldep
OCAML_INCLUDE := -I $(ROOT_DIR)/ocaml/common \
	-I $(ROOT_DIR)/ocaml/extraction/common \
	-I $(ROOT_DIR)/ocaml/extraction/examples

COQ_FILES = $(shell find $(ROOT_DIR) -name "*.v")
COQ_FILES_EXTRACTION = $(shell find $(ROOT_DIR) -name "extraction.v")

OCAML_ML_FILES = $(shell find $(ROOT_DIR) -name "*.ml")
OCAML_MLI_FILES = $(shell find $(ROOT_DIR) -name "*.mli")

top:
	make extract
	make $(ROOT_DIR)/ocaml/main/main

.PHONY : depend extract

include $(shell find $(ROOT_DIR) -name "*.mk")

depend : $(COQ_FILES:=.dep) $(OCAML_ML_FILES:=.dep) $(OCAML_MLI_FILES:=.dep)

extract : $(COQ_FILES_EXTRACTION:.v=.vo)

%.cmo : %.ml
	$(OCAMLC) -c $(OCAML_INCLUDE) $<

%.cmi : %.mli
	$(OCAMLC) -c $(OCAML_INCLUDE) $<

%.vo : %.v
	$(COQC) $(COQ_LOADS) $<

%.v.dep : %.v
	$(COQDEP) $(COQ_LOADS) $< > $@

%.ml.dep : %.ml
	$(OCAMLDEP) $(OCAML_INCLUDE) $< > $@

%.mli.dep : %.mli
	$(OCAMLDEP) $(OCAML_INCLUDE) $< > $@

-include $(COQ_FILES:=.dep)
-include $(OCAML_ML_FILES:=.dep)
-include $(OCAML_MLI_FILES:=.dep)
