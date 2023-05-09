all: binary-tree.tex
	@mkdir -p build/
	tectonic -X compile \
		--synctex \
		--keep-logs \
		--keep-intermediates \
		--outdir build/ \
		binary-tree.tex

clean:
	rm -rf build/
