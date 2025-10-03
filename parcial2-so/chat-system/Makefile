CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Archivos fuente
SERVIDOR_SRC = $(SRCDIR)/servidor.c
CLIENTE_SRC = $(SRCDIR)/cliente.c

# Archivos ejecutables
SERVIDOR = $(BUILDDIR)/servidor
CLIENTE = $(BUILDDIR)/cliente

# Regla por defecto
all: $(BUILDDIR) $(SERVIDOR) $(CLIENTE)

# Crear directorio build
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Compilar servidor
$(SERVIDOR): $(SERVIDOR_SRC)
	$(CC) $(CFLAGS) -I$(INCDIR) $(SERVIDOR_SRC) -o $(SERVIDOR)

# Compilar cliente
$(CLIENTE): $(CLIENTE_SRC)
	$(CC) $(CFLAGS) -I$(INCDIR) $(CLIENTE_SRC) -o $(CLIENTE)

# Limpiar archivos compilados
clean:
	rm -rf $(BUILDDIR)
	rm -f sala_*.log

# Limpiar colas de mensajes
cleanup:
	./scripts/cleanup.sh

# Compilar y ejecutar servidor
run-server: $(SERVIDOR)
	./$(SERVIDOR)

# Compilar y ejecutar cliente (requiere nombre como parámetro)
run-client: $(CLIENTE)
	@echo "Uso: make run-client NAME=<nombre>"
	@if [ -z "$(NAME)" ]; then \
		echo "Error: Especifica un nombre con NAME=<nombre>"; \
		exit 1; \
	fi
	./$(CLIENTE) $(NAME)

# Instalar dependencias (si es necesario)
install-deps:
	@echo "No se requieren dependencias adicionales para este proyecto"

# Ejecutar tests
test: all
	@echo "Ejecutando tests básicos..."
	@echo "Por favor, ejecute el servidor en una terminal y clientes en otras terminales"

.PHONY: all clean cleanup run-server run-client install-deps test