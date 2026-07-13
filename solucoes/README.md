**Neste diretório estão sendo armazenadas as soluções em código para os seguintes problemas clássicos de concorrência:**
- Produtor/Consumidor
- Leitores/Escritores
- Jantar dos filósofos

## Rodar

```bash
> docker build -t atividade2 .

# Produtor/Consumidor
> docker run --rm -it atividade2 ./solucoes/produtor_consumidor/produtor_consumidor

# Leitores/Escritores
> docker run --rm -it atividade2 ./solucoes/leitores_escritores/leitores_escritores

# Jantar dos filósofos
> docker run --rm -it atividade2 ./solucoes/filosofos/jantar_filosofos
```