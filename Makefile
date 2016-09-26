ROOT_DIR := $(shell pwd)

COQC        := /home/keiko/opt/coq-8.5pl2/bin/coqc
COQDEP      := /home/keiko/opt/coq-8.5pl2/bin/coqdep
COQ_LOADS := -Q $(ROOT_DIR)/coq/common common -Q $(ROOT_DIR)/coq/examples examples

OCAMLC   := /usr/bin/ocamlc
OCAMLDEP := /usr/bin/ocamldep

COQ_FILES = $(shell find $(ROOT_DIR) -name "*.v")
COQ_DEP_FILES = $(COQ_FILES:.v=.dep)

depend : $(COQ_DEP_FILES)

%.vo : %.v
	$(COQC) $(COQ_LOADS) $<

%.dep : %.v
	cd $(dir $@);\
	$(COQDEP) $(COQ_LOADS) $< > $@


