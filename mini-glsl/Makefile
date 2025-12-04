# Makefile for mini-glsl + SPIR-V workflow
#
# Layout:
#   shader.c               -> harness (FF + MiniGLSL + SPIR-V)
#   shaders/*.frag         -> MiniGLSL fragment *bodies* (flat/lambert/phong/rings/grid)
#   spirv/*.spvasm         -> reference SPIR-V assembly for those shaders
#   spirv/*.spv            -> assembled SPIR-V binaries (built artifacts)
#   assets/teapot.obj      -> mesh (auto-downloaded)

CC       ?= cc
CFLAGS   ?= -O2 -g -std=c11 -Wall -Wextra
CPPFLAGS ?= -I/usr/include/libpng16
LIBS     ?= -lOSMesa -lpng16 -lm

BIN      := shader

# MiniGLSL shader names (without extension)
SHADERS  := flat lambert phong rings grid

# SPIR-V tools (must be installed for SPIR-V workflow)
SPIRV_AS ?= spirv-as

SHADER_DIR := shaders
SPIRV_DIR  := spirv

ASSETS_DIR := assets
OBJ        := $(ASSETS_DIR)/teapot.obj
TEAPOT_URL ?= https://raw.githubusercontent.com/alecjacobson/common-3d-test-models/master/data/teapot.obj

.PHONY: all images spirv-images ff clean obj spirv-binaries

# Default: build harness + FF reference + MiniGLSL PNGs + SPIR-V PNGs
all: $(BIN) ff.png images spirv-images

# ------------------------------------------------------------
# Harness
# ------------------------------------------------------------
$(BIN): shader.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LIBS)

# ------------------------------------------------------------
# Assets
# ------------------------------------------------------------
$(OBJ):
	@mkdir -p $(ASSETS_DIR)
	@echo "Fetching teapot OBJ to $(OBJ)"
	curl -L "$(TEAPOT_URL)" -o "$(OBJ)"

obj: $(OBJ)

# ------------------------------------------------------------
# Fixed-function reference
# ------------------------------------------------------------
ff.png: $(BIN) $(OBJ)
	./$(BIN) --mode ff --obj $(OBJ) --png $@

# ------------------------------------------------------------
# MiniGLSL (GLSL 1.20 via wrapper) ? PNG
# Each shaders/<name>.frag is a MiniGLSL *body* that the harness wraps
# into a full GLSL 120 fragment shader.
# ------------------------------------------------------------
%.png: shaders/%.frag $(BIN) $(OBJ)
	./$(BIN) --mode glsl --frag $< --obj $(OBJ) --png $@

images: $(SHADERS:%=%.png)

# ------------------------------------------------------------
# SPIR-V: .spvasm (checked in) ? .spv (built) ? PNG
# ------------------------------------------------------------
HEADER     := $(SPIRV_DIR)/miniglsl_header.glsl
TOOLS_DIR := tools

# GLSL derived from MiniGLSL:
#   shaders/flat.frag  -> spirv/flat_450.frag
$(SPIRV_DIR)/%_450.frag: shaders/%.frag $(SPIRV_DIR)/miniglsl_header.glsl $(TOOLS_DIR)/miniglsl_rewrite.awk
	@mkdir -p $(SPIRV_DIR)
	@echo "[GEN] $@"
	@cat $(SPIRV_DIR)/miniglsl_header.glsl > $@
	@echo "" >> $@
	@echo "void main() {" >> $@
	@$(TOOLS_DIR)/miniglsl_rewrite.awk $< >> $@
	@echo "}" >> $@

# SPIR-V from that GLSL
$(SPIRV_DIR)/%.spv: $(SPIRV_DIR)/%_450.frag
	@echo "[SPIRV] $< -> $@"
	@glslangValidator -G -S frag -o $@ $< > spirv/$*.log 2>&1

# spirv/<name>.spvasm: textual SPIR-V disassembly
#
#   spirv-dis spirv/flat.spv -o spirv/flat.spvasm

$(SPIRV_DIR)/%.spvasm: $(SPIRV_DIR)/%.spv
	@echo "[SPIR-V→Text] $< -> $@"
	spirv-dis $< -o $@


# Build all SPIR-V binaries from MiniGLSL
spv-all: $(SHADERS:%=$(SPIRV_DIR)/%.spv)

# Build all SPIR-V binaries *and* .spvasm text
spv-asm-all: $(SHADERS:%=$(SPIRV_DIR)/%.spvasm)

.PHONY: spv-all spv-asm-all

# ------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------
clean:
	rm -f $(BIN) *.png
	# SPIR-V derivations (keep miniglsl_header.glsl)
	rm -f $(SPIRV_DIR)/*_450.frag \
	      $(SPIRV_DIR)/*.spv \
	      $(SPIRV_DIR)/*.spvasm \
	      $(SPIRV_DIR)/*.log

# ------------------------------------------------------------
# Docker helpers (optional)
# ------------------------------------------------------------

DOCKER_IMAGE ?= mini-glsl

.PHONY: docker-build docker-shell docker-all

docker-build:
	docker build -t $(DOCKER_IMAGE) .

docker-shell: docker-build
	docker run --rm -it -v "$(PWD)":/work -w /work $(DOCKER_IMAGE) /bin/bash

docker-all: docker-build
	docker run --rm -it -v "$(PWD)":/work -w /work $(DOCKER_IMAGE) \
	  make all CPPFLAGS="$$(pkg-config --cflags libpng)" \
	           LIBS="$$(pkg-config --libs libpng) -lOSMesa -lm"
