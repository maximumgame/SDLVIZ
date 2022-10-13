FROM emscripten/emsdk AS embuild
COPY . /src

RUN mkdir embuild && \
    cd embuild && \
    emcmake cmake .. && \
    make -j

FROM httpd:2.4
COPY --from=embuild /src/embuild /usr/local/apache2/htdocs
