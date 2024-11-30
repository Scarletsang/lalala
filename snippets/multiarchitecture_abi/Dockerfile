FROM ubuntu:latest
ARG OUTPUT_SUFFIX

RUN apt-get update && apt-get upgrade -y
RUN apt-get install clang -y
COPY main.c /test/main.c
WORKDIR test
RUN mkdir output && \
    clang main.c -S && \
    cp main.s ./output/main_${OUTPUT_SUFFIX}.s && \
    clang main.s -o ./output/main_${OUTPUT_SUFFIX}.out
CMD cp output/* /output && tail -f /dev/null
