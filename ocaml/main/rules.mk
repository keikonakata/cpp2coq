OCAML_OBJECT_FILES :=\
	$(ROOT_DIR)/ocaml/extraction/common/values.cmo \
	$(ROOT_DIR)/ocaml/extraction/common/compt.cmo \
	$(ROOT_DIR)/ocaml/common/pervasives_base.cmo \
	$(ROOT_DIR)/ocaml/common/print.cmo \
	$(ROOT_DIR)/ocaml/common/pervasives_impl.cmo \
	$(ROOT_DIR)/ocaml/extraction/common/pervasives_decl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/example1_stub.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/example1_decl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/example1_impl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/example1_quirk.cmo

$(ROOT_DIR)/ocaml/main/main : $(ROOT_DIR)/ocaml/main/main.ml $(OCAML_OBJECT_FILES)
	$(OCAMLC) $(OCAML_INCLUDE) -o $@ $(OCAML_OBJECT_FILES) $<
