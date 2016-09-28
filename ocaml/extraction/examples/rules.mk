$(ROOT_DIR)/ocaml/extraction/examples/extraction.vo :
	cd $(ROOT_DIR)/ocaml/extraction/examples/;\
	$(COQC) $(COQ_LOADS) $<
