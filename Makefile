# Utilisation de Bazelisk
BAZEL = bazelisk

init:
	$(BAZEL) run @hedron_compile_commands//:refresh_all

# Compilation client et serveur
build:
	$(BAZEL) build //src/client:client //src/server:server $(ARGS)

# Exécution
client1:
	$(BAZEL) run //src/client:client -- 127.0.0.1 client1 $(ARGS)

client2:
	$(BAZEL) run //src/client:client -- 127.0.0.1 client2 $(ARGS)

server:
	$(BAZEL) run //src/server:server -- $(ARGS)

awale:
	$(BAZEL) run //src/awale:awale -- $(ARGS)

# Nettoyage
clean:
	$(BAZEL) clean