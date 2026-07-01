FROM gcc:13

WORKDIR /app
COPY solucoes/ ./solucoes/

RUN g++ -std=c++20 -pthread -O2 -Wall \
        -o solucoes/produtor_consumidor/produtor_consumidor \
        solucoes/produtor_consumidor/produtor_consumidor.cpp && \
    g++ -std=c++20 -pthread -O2 -Wall \
        -o solucoes/leitores_escritores/leitores_escritores \
        solucoes/leitores_escritores/leitores_escritores.cpp && \
    g++ -std=c++20 -pthread -O2 -Wall \
        -o solucoes/filosofos/jantar_filosofos \
        solucoes/filosofos/jantar_filosofos.cpp