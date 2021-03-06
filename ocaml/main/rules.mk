OCAML_OBJECT_FILES :=\
	$(ROOT_DIR)/ocaml/extraction/common/values.cmo \
	$(ROOT_DIR)/ocaml/extraction/common/compt.cmo \
	$(ROOT_DIR)/ocaml/common/pervasives_base.cmo \
	$(ROOT_DIR)/ocaml/common/print.cmo \
	$(ROOT_DIR)/ocaml/common/pervasives_impl.cmo \
	$(ROOT_DIR)/ocaml/extraction/common/pervasives_decl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/misc_math_stub.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/misc_math_decl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/misc_math_impl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/misc_math_quirk.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/avl_tree_stub.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/avl_tree_decl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/avl_tree_impl.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/avl_tree_quirk.cmo \
	$(ROOT_DIR)/ocaml/extraction/examples/main_1_impl.cmo \

$(ROOT_DIR)/ocaml/main/main : $(ROOT_DIR)/ocaml/main/main.ml $(OCAML_OBJECT_FILES)
	$(OCAMLC) $(OCAML_INCLUDE) -o $@ $(OCAML_OBJECT_FILES) $<
